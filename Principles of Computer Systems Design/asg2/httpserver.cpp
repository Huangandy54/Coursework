#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <iostream>
#include <netdb.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;
#define MAX_BUFFER_SIZE 16384
#define DEFAULT_PORT "80"
#define DEFAULT_NUMTHREADS 4
#define WORKBUFFERSIZE 10
/*
    semaphores:
    (full)tasks=init at 0. post() when dispatcher accepts a job and wait() when worker accepts a job
    (empty)avail=init at nthreads. dispacher will wait() and worker will post() after work is done
    mutex=binary semephore to allow access to workbuffer
    offsetMutex= binary semaphore to allow access to offset for logging
*/
sem_t tasks, avail, mutex, offsetMutex;
int workbuffer[WORKBUFFERSIZE];
int disPos = 0;
int workerPos = 0;
// offset to determine where to write in the logfile using pwrite()
int offset = 0;
int logfd;
bool logging = false;

// given code to create and bind the main socket, and start listening on that socket
int startServer(const char *hostname, const char *port)
{
    struct addrinfo *addrs, hints = {};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    getaddrinfo(hostname, port, &hints, &addrs);
    int main_socket = socket(addrs->ai_family, addrs->ai_socktype, addrs->ai_protocol);
    int enable = 1;
    if(setsockopt(main_socket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }
    if(bind(main_socket, addrs->ai_addr, addrs->ai_addrlen) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    // N is the maximum number of "waiting" connections on the socket.
    // We suggest something like 16.
    if(listen(main_socket, 16) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    return main_socket;
}
// helper function to get file size (content-length: )for when we have to respond to a GET
size_t getFileSize(const char *filename)
{
    struct stat st;
    if(stat(filename, &st) != 0) {
        return 0;
    }
    return st.st_size;
}

void sendResponse(int destination, int code, int length)
{
    string httpCode;
    switch(code) {
        case 200:
            httpCode = "200 OK";
            break;
        case 201:
            httpCode = "201 Created";
            break;
        case 400:
            httpCode = "400 Bad Request";
            break;
        case 403:
            httpCode = "403 Forbidden";
            break;
        case 404:
            httpCode = "404 Not Found";
            break;
        default:
            httpCode = "500 Internal Server Error";
            break;
    }
    string response =
      "HTTP/1.1 " + httpCode + "\r\nContent-Length: " + to_string(length) + "\r\n\r\n";
    write(destination, response.c_str(), strlen(response.c_str()));
}

void processHeader(int clientfd)
{
    char requestType[MAX_BUFFER_SIZE];
    char resourceName[MAX_BUFFER_SIZE];
    char httpProtocol[MAX_BUFFER_SIZE];
    uint8_t contentBuffer[MAX_BUFFER_SIZE];
    char logbuffer[MAX_BUFFER_SIZE];
    bool endOfHeader = false;
    bool contentLengthExists = false;
    char *contentLengthPtr;
    int contentLength;
    int bytesRead;
    int loggingoffset;
    // begin
    if((bytesRead = read(clientfd, contentBuffer, sizeof(contentBuffer))) == 0) {
        // first read did not give us any data for header
        sendResponse(clientfd, 400, 0);
        close(clientfd);
    }

    sscanf((char *)contentBuffer,
      "%s %s %s\r\n",
      (char *)requestType,
      (char *)resourceName,
      (char *)httpProtocol);
    // printf("%s %s %s\n",requestType,resourceName,httpProtocol );
    while(endOfHeader == false) {
        // check for content-length only if its PUT because GET doesnt have it
        if(strcmp(requestType, "PUT") == 0) {
            // looks for "Content-Length:" and puts a pointer on it
            // keeps looking for it until the end of the header was found above
            contentLengthPtr = strstr((char *)contentBuffer, "Content-Length: ");
            if(contentLengthPtr == NULL) {
                // printf("not found\n");
            } else {
                sscanf(contentLengthPtr, "Content-Length: %d\r\n", &contentLength);
                contentLengthExists = true;
            }
        }
        // checks for the first \r\n\r\n to detect if it is the end of the header
        char *endptr = strstr((char *)contentBuffer, "\r\n\r\n");
        // if it was not found, we have to continue reading from the client socket
        if(endptr == NULL) {
            // current does not contain end stuff keep reading
            // clear buffer and contiue reading
            memset(contentBuffer, 0, sizeof(contentBuffer));
            read(clientfd, contentBuffer, MAX_BUFFER_SIZE);

        } else {
            // header ends bc we found \r\n\r\n
            endOfHeader = true;
        }

    } // end while

    /*
    ERROR CHECK FOR RESOURCE NAME

    */
    // code from piazza
    if(resourceName[0] == '/') {
        memmove(resourceName, resourceName + 1, strlen(resourceName));
    }
    bool validName = false;
    if(strlen(resourceName) != 27) {
        validName = false;
    } else {
        for(int i = 0; i < 27; i++) {
            // if is a num or alphabet
            // printf("comparing %c\n", resourceName[i]);
            if(isalnum(resourceName[i]) != 0) {
                validName = true;
            }
            // if "-" or "_"
            else if(resourceName[i] == '-' || resourceName[i] == '_') {
                validName = true;
            } else {
                // anything that isnt num || alpha || - || _
                validName = false;
                break;
            }
        }
    }
    // reply 400 error code when bad header
    if(validName == false) {
        // log
        sem_wait(&offsetMutex);
        loggingoffset = offset;
        offset += 26 + strlen(requestType) + strlen(resourceName) + strlen(httpProtocol);
        // printf("%d %d %d\n", strlen(requestType),strlen(resourceName),strlen(httpProtocol));
        // printf("new global offset: %d\n", offset);
        sem_post(&offsetMutex);
        sprintf(logbuffer,
          "FAIL: %s %s %s --- response 400\n",
          (char *)requestType,
          (char *)resourceName,
          (char *)httpProtocol);
        // printf("%s\n", logbuffer);
        // printf("failog length%d\n",strlen(logbuffer));
        if(pwrite(logfd, logbuffer, strlen(logbuffer), loggingoffset) <= 0) {
            fprintf(stderr, "Error writing to log: %s\n", strerror(errno));
        }
        sendResponse(clientfd, 400, 0);
    } else {
        /*
        PUT COMMAND HANDLING

        */
        if(strcmp(requestType, "PUT") == 0) {
            // clear content buffer
            memset(contentBuffer, 0, sizeof(contentBuffer));
            // for the file we are creating or overwriting
            int fd;
            // if open fails
            if((fd = open(resourceName, O_WRONLY | O_TRUNC)) == -1) {
                // if file does not exist
                if(errno == ENOENT) {
                    // create file using O_CREAT and permission 0644 for rw
                    fd = open(resourceName, O_WRONLY | O_CREAT, 0644);
                    if(fd == -1) {
                        fprintf(stderr, "%s\n", strerror(errno));
                        // probably means they have no permission to create the file?
                        sendResponse(clientfd, 403, 0);
                    }
                }

            } else {
                // set file to 0 anyways error handling in case for some reason it couldnt
                // overwrite?
                ftruncate(fd, 0);
            }

            // if content length was specified, read only that many bytes
            if(contentLengthExists == true) {
                uint32_t remaining = contentLength;
                // intitial read and write
                int x = read(clientfd, contentBuffer, sizeof(contentBuffer));
                if(x < 0) {
                    fprintf(stderr, "Error reading from client socket: %s\n", strerror(errno));
                } else {
                    remaining -= x;
                    if((write(fd, contentBuffer, x)) < 0) {
                        fprintf(stderr, "Error writing: %s\n", strerror(errno));
                        sendResponse(clientfd, 403, 0);
                    }
                }

                // check for while havent read to EOF
                while(x != 0) {
                    // edge case to check if we have to read less than the max size buffer
                    // in case there is extra stuff we dont want to read
                    if(remaining < sizeof(contentBuffer)) {
                        x = read(clientfd, contentBuffer, remaining);
                    } else {
                        x = read(clientfd, contentBuffer, sizeof(contentBuffer));
                    }
                    remaining -= x;
                    if((write(fd, contentBuffer, x)) < 0) {
                        fprintf(stderr, "Error writing: %s\n", strerror(errno));
                        sendResponse(clientfd, 403, 0);
                    }
                }

            }
            // else read until read() reads EOF
            else {
                while(true) {
                    bytesRead = read(clientfd, contentBuffer, sizeof(contentBuffer));
                    if(bytesRead == 0) {
                        break;
                    }
                    if((write(fd, contentBuffer, bytesRead)) < 0) {
                        fprintf(stderr, "Error writing: %s\n", strerror(errno));
                        sendResponse(clientfd, 403, 0);
                    }
                    memset(contentBuffer, 0, sizeof(contentBuffer));
                }
            }
            // closes fd after finished writing
            // closes fd after finished writing
            if(close(fd) == -1) {
                fprintf(stderr, "%s\n", strerror(errno));
            }
            // always responds with created bc piazza @157
            // we are creating a new file to replace the old one anyways
            sendResponse(clientfd, 201, 0);
        }
        /*
        GET COMMAND HANDLING

        */
        else if(strcmp(requestType, "GET") == 0) {
            int fd;
            int totalBytesRead = 0;
            memset(contentBuffer, 0, sizeof(contentBuffer));
            if((fd = open(resourceName, O_RDONLY)) == -1) {
                // file doesnt exit thus send 404 header with content length of 0
                sendResponse(clientfd, 404, 0);
            } else {
                // file does exist and can be opened thus reply with 200 OK along with the correct
                // content length

                totalBytesRead = getFileSize(resourceName);
                sendResponse(clientfd, 200, totalBytesRead);
                while(true) {
                    bytesRead = read(fd, contentBuffer, sizeof(contentBuffer));
                    if(bytesRead == 0) {
                        break;
                    }

                    if((write(clientfd, contentBuffer, bytesRead)) < 0) {
                        fprintf(stderr, "Error writing: %s\n", strerror(errno));
                    }
                    memset(contentBuffer, 0, sizeof(contentBuffer));
                }
            }
        }
        // MISMATCHED HTTP REQUEST
        else {
            // bad header
            sendResponse(clientfd, 400, 0);
        }
    }

    // check if there is any more data on client fd and close fd
    if((read(clientfd, contentBuffer, contentLength)) == 0) {
        // printf("client disconnected, closing client fd\n");
        close(clientfd);
    } else {
        processHeader(clientfd);
    }
}
// helper function to make into hex format
void toHex(uint8_t *inputBuffer, int size, char *result)
{
    for(int i = 0; i < size; i++) {
        result += sprintf(result, "%02x", inputBuffer[i]);
    }
}

void dispatcher(const char *hostname, const char *port)
{
    int new_socket;
    int main_socket;
    main_socket = startServer(hostname, port);

    // Your code, starting with accept(), goes here
    // continue listening and accept requests
    while(true) {
        if((new_socket = accept(main_socket, NULL, NULL)) < 0) {
            fprintf(stderr, "could not accept request: %s\n", strerror(errno));
            // exit(EXIT_FAILURE);
        } else {
            /*
            SEMAPHORE STUFF AND SHARING OF NEW_SOCKET HERE
            INSTEAD OF READING AND PROCESSING BELOW
            */
            // wait until there is at least 1 worker
            sem_wait(&avail);
            // aquire the mutex
            sem_wait(&mutex);
            // critical region we can access workbuffer
            workbuffer[disPos] = new_socket;
            disPos = (disPos + 1) % WORKBUFFERSIZE;
            // reincrements the mutex. out of critical region
            sem_post(&mutex);
            // tell workers waiting on tasks there is one
            sem_post(&tasks);
        }
    } // end while
}
// wait decreases and waits until non 9
// post incrememnts
void *worker(void *arg)
{
    int workerNum = (int)(intptr_t)arg;
    // int processid=syscall(SYS_gettid);
    // printf("This is thread #%d with processid: %d\n", workerNum, processid);
    while(true) {
        // uint8_t buffer[MAX_BUFFER_SIZE];
        // int bytesRead;
        int clientfd;
        // sleep until there is task
        sem_wait(&tasks);
        // woke up but wait until mutex can be aquired
        sem_wait(&mutex);
        // inside critical region
        clientfd = workbuffer[workerPos];
        workbuffer[workerPos] = -1;
        workerPos = (workerPos + 1) % WORKBUFFERSIZE;
        // leaving critical region
        sem_post(&mutex);
        // say youre done

        processHeader(clientfd);
        // worker done
        sem_post(&avail);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    const char *hostname;
    const char *port;
    const char *logFileName;

    // defaults to 4 threads
    int nThreads = DEFAULT_NUMTHREADS;
    int opt;

    while((opt = getopt(argc, argv, "N:l:")) != -1) {
        switch(opt) {
            case 'N':
                nThreads = atoi(optarg);
                break;
            case 'l':
                // set the global variable to true. default is false.
                logging = true;
                logFileName = optarg;
                break;
            case '?':
                fprintf(
                  stderr, "invalid/unknown option. supported: -n<thread count> -l<destination>\n");
                exit(errno);
        }
    }

    // if optind is less than argc there is at least one extra arg
    if(optind < argc) {
        hostname = argv[optind];
        int portopt = optind + 1;
        if(portopt
           == argc) // if the arg after the hostname is equal to total then port is not found
        {
            port = DEFAULT_PORT;
        } else {
            port = argv[portopt];
        }

    } else { // else optind was greater or equal to total args meaning there wasnt any non options
        fprintf(stderr, "hostname not found. Usage: httpserver <hostname> <port(optional)>\n");
        exit(errno);
    }
    // testing for getopt()
    // printf("hostname: %s | port: %s | nThreads: %d\n",hostname,port,nThreads);

    if(logging) {
        if((logfd = open(logFileName, O_WRONLY)) == -1) {
            // if file does not exist
            if(errno == ENOENT) {
                // create file using O_CREAT and permission 0644 for rw
                logfd = open(logFileName, O_WRONLY | O_CREAT, 0644);
                if(logfd == -1) {
                    fprintf(stderr, "%s\n", strerror(errno));
                    exit(errno);
                }
            }
        }
        // printf("fd for logfile: %d\n", logfd);
    }
    // initialize all the threads and semaphores
    pthread_t *workers = (pthread_t *)calloc(nThreads, sizeof(pthread_t));
    for(int i = 0; i < nThreads; i++) {
        // printf("attempting to make thread %d\n", i);
        if((pthread_create(&workers[i], NULL, worker, (void *)(intptr_t)i)) != 0) {
            fprintf(stderr, "error creating worker thread #%d\n", i);
            exit(errno);
        }
    }
    // init semaphores
    if(sem_init(&tasks, 0, 0) != 0) {
        fprintf(stderr, "error creating semaphore\n");
        exit(errno);
    }
    if(sem_init(&avail, 0, nThreads) != 0) {
        fprintf(stderr, "error creating semaphore\n");
        exit(errno);
    }
    if(sem_init(&mutex, 0, 1) != 0) {
        fprintf(stderr, "error creating semaphore\n");
        exit(errno);
    }
    if(sem_init(&offsetMutex, 0, 1) != 0) {
        fprintf(stderr, "error creating semaphore\n");
        exit(errno);
    }
    // calls dispatcher subroutine to "become" the dispatcher
    dispatcher(hostname, port);
    return 0;
} // end main
