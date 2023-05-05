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
//magic number to use to modulo % our hash(key)
#define MAGICNUMBER 9999
#define MAXKVSIZE 129
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
int mapoffset=9;
int logfd;
int mapfd;
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

void hashInsert(const char* key, const char* value){
    //printf("inserting key: %s value: %s\n",key,value );
    //init the std:hash function for hashing strings
    string entry;
    string keyString(key);
    string valueString(value);
    entry= keyString+":"+valueString+"\r\n";
    hash<std::string> stringHash;
    //cout<< "hash%MAGICNUMBER: "<< stringHash(keyString)%MAGICNUMBER<<endl;
    int tempOffset=(stringHash(keyString)%MAGICNUMBER)*(MAXKVSIZE)+mapoffset;
    //printf("offset to find key: %d\n",tempOffset);
    pwrite(mapfd,entry.c_str(),strlen(entry.c_str()),tempOffset);
}

string hashGet(const char* key){
    //printf("retrieving key: %s\n", key);
    uint8_t tempBuffer[MAXKVSIZE];
    char value[MAXKVSIZE];
    string keyString(key);
    //init the std:hash function for hashing strings
    hash<std::string> stringHash;
    //cout<< "hash%MAGICNUMBER: "<< stringHash(keyString)%MAGICNUMBER<<endl;
    int tempOffset=(stringHash(keyString)%MAGICNUMBER)*(MAXKVSIZE)+mapoffset;
    //printf("offset to find key: %d\n",tempOffset);
    pread(mapfd, tempBuffer,sizeof(tempBuffer),tempOffset);
    //printf("%s with len: %d\n",tempBuffer,strlen((char*)tempBuffer) );
    //nothing in buffer meaning no key at the location it should be
    if (strlen((char*)tempBuffer)==0 )
    {   
        string res="";
        return res;
    }else{
        string format=keyString+":%s";
        sscanf((char *)tempBuffer,format.c_str(),value);
        //printf("retrieved value: %s\n", value);
        string res(value);
        //printf("length/size of value: %d\n",res.length() );
        return res;
    }
    
}
void processHeader(int clientfd)
{
    char requestType[MAX_BUFFER_SIZE];
    char resourceName[MAX_BUFFER_SIZE];
    char httpProtocol[MAX_BUFFER_SIZE];
    uint8_t contentBuffer[MAX_BUFFER_SIZE];
    //char logbuffer[MAX_BUFFER_SIZE];
    bool endOfHeader = false;
    bool contentLengthExists = false;
    char *contentLengthPtr;
    int contentLength;
    int bytesRead;
    //int loggingoffset;
    // begin
    if((bytesRead = read(clientfd, contentBuffer, sizeof(contentBuffer))) == 0) {
        // first read did not give us any data for header
        sendResponse(clientfd, 400, 0);
        close(clientfd);
    }
    //printf("%s\n",contentBuffer );
    sscanf((char *)contentBuffer,
      "%s %s %s\r\n",
      (char *)requestType,
      (char *)resourceName,
      (char *)httpProtocol);
    //printf("%s %s %s\n",requestType,resourceName,httpProtocol );
    while(endOfHeader == false) {
        // check for content-length if its PUT OR PATCH because GET doesnt have it
        if((strcmp(requestType, "PUT") == 0)||(strcmp(requestType, "PATCH") == 0)) {
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
    //can ignore checking resourcename if PATCH
    if(strlen(resourceName) != 27 && (strcmp(requestType, "PATCH") != 0)) {
        //printf("resource name is not 27 long\n");
        //might be a alias
        int lookUps=0;
        string alias = hashGet(resourceName);
        while(lookUps<10){
            if (strlen(alias.c_str())==27)
            {
                //printf("found httpname\n");
                validName=true;
                break;
            }else{
                lookUps++;
                alias=hashGet(alias.c_str());
            }
            if (strlen(alias.c_str())==0)
            {
                //printf("No corresponding httpname to this alias\n");
                //sendResponse(clientfd,404,0);
                //close(clientfd);
                break;
            }
        }
        //printf("r name before: %s\n",resourceName );
        strcpy(resourceName,alias.c_str());
        //printf("r name after%s\n",resourceName );
        if (strlen(resourceName)!=27)
        {
            printf("No corresponding httpname to this alias after 10 lookups\n");
            sendResponse(clientfd,404,0);
            close(clientfd);
        }
    } else {
        //printf("into else?\n");
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
    if(validName == false && (strcmp(requestType, "PATCH") != 0)) {
        // log
        // sem_wait(&offsetMutex);
        // loggingoffset = offset;
        // offset += 26 + strlen(requestType) + strlen(resourceName) + strlen(httpProtocol);
        // printf("%d %d %d\n", strlen(requestType),strlen(resourceName),strlen(httpProtocol));
        // printf("new global offset: %d\n", offset);
        // sem_post(&offsetMutex);
        // sprintf(logbuffer,
        //   "FAIL: %s %s %s --- response 400\n",
        //   (char *)requestType,
        //   (char *)resourceName,
        //   (char *)httpProtocol);
        // printf("%s\n", logbuffer);
        // printf("failog length%d\n",strlen(logbuffer));
        // if(pwrite(logfd, logbuffer, strlen(logbuffer), loggingoffset) <= 0) {
        //     fprintf(stderr, "Error writing to log: %s\n", strerror(errno));
        // }
        printf("invalid resourceName\n");
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
                printf("could not find file in GET\n");
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
        /*
        PATCH handling

        */
        else if (strcmp(requestType, "PATCH") == 0)
        {
            char keyValue[MAXKVSIZE];
            char key[MAXKVSIZE];
            char value[MAXKVSIZE];
            bool inHash=false;
            bool isFile=false;
            //printf("patch command found\n");
            //printf("%d\n", contentLength);
            memset(contentBuffer, 0, sizeof(contentBuffer));
            if((bytesRead = read(clientfd, contentBuffer, sizeof(contentBuffer))) == 0) {
                // first read did not give us any data for header
                sendResponse(clientfd, 400, 0);
                close(clientfd);
            }
            //printf("%s\n", contentBuffer);
            //take only what is behind ALIAS up to the new line
            sscanf((char*)contentBuffer, "ALIAS %[^\r\n]",keyValue);
            // printf("%s\n", keyValue);
            // count how many spaces just to make sure that there is no spaces inside the aliases
            int numSpaces=0;
            int numUnprintable=0;
            for (size_t i=0;i<strlen(keyValue);i++)
            {
                if (isspace(keyValue[i]))
                {
                    numSpaces++;
                }
                if (isprint(keyValue[i])==0)
                {
                    numUnprintable++;
                }
            }
            if (numSpaces!=1)
            {
                //printf("sending 400\n");
                sendResponse(clientfd, 400, 0);
                close(clientfd);
            }
            //printf("numspaces %d numUnprintable %d\n",numSpaces, numUnprintable );
            sscanf(keyValue, "%s %s\r\n",value,key);
            //printf("going to insert key: %s value: %s\n",key,value );
            //check if value is a file on the server.
            int tempFd;
            if((tempFd = open(value, O_RDONLY)) == -1) {
                // if file does not exist
                if(errno == ENOENT) {
                    isFile=false;
                }
            }else{
                isFile=true;
            }
            //check if value is in hash
            if (strlen(hashGet(value).c_str())==0)
            {
                inHash=false;
            }else{
                inHash=true;
            }

            //if both not a file on the server or existing alias in the hash table
            if ((inHash==false)&&(isFile==false))
            {
                printf("not alias or a file\n");
                sendResponse(clientfd, 404, 0);
                close(clientfd);
            }else{
                hashInsert(key,value);
                //send 200 OK and close clientfd
                sendResponse(clientfd,200,0);
                close(clientfd);
            }
            
            
        }

        // MISMATCHED HTTP REQUEST
        else {
            //printf("final else respond 400\n");
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
void *worker(void *ptr)
{
    //int workerNum = (int)(intptr_t)arg;
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
    return ptr;
}

int main(int argc, char *argv[])
{
    const char *hostname;
    const char *port;
    const char *logFileName;
    const char *mappingfile;
    // defaults to 4 threads
    int nThreads = DEFAULT_NUMTHREADS;
    int opt;
    bool requiredOpt=false;

    while((opt = getopt(argc, argv, "a:N:l:")) != -1) {
        switch(opt) {
            case 'a':
                mappingfile=optarg;
                requiredOpt=true;
                break;
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
                  stderr, "invalid/unknown option. supported: -a <mappingfile> -n <thread count> -l <destination>\n");
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
    //check if mapping file was given 
    if (mappingfile == NULL||requiredOpt==false)
    {
        fprintf(stderr, "Must be run with -a <file>\n");
        exit(errno);
    }else{
        //if given, we need check if the file exists, if not create it.
        //if it exists, need to check the magic number at the start of the file.
        // if open fails
        string magnum =to_string(MAGICNUMBER)+"\r\n\r\n";
        //printf("MAGICNUMBER:%s size: %d\n",magnum.c_str(), strlen(magnum.c_str()));
        if((mapfd = open(mappingfile, O_RDWR)) == -1) {
            // if file does not exist
            if(errno == ENOENT) {
                // create file using O_CREAT and permission 0644 for rw
                mapfd = open(mappingfile, O_RDWR | O_CREAT, 0644);
                if(mapfd == -1) {
                    fprintf(stderr, "%s\n", strerror(errno));
                    exit(errno);
                }
                //add magic number to the top of the file
                write(mapfd, magnum.c_str(), strlen(magnum.c_str()));

            }

        }else{
            //mapfd=open(mappingfile,O_RDONLY);
            //was able to open the file
            //printf("file exists\n");
            uint8_t magicNumBuffer[MAX_BUFFER_SIZE];
            read(mapfd, magicNumBuffer, sizeof(magicNumBuffer));
            //printf("%s\n",magicNumBuffer);
            int magicnumfile;
            sscanf((char *)magicNumBuffer,"%d",&magicnumfile);
            //printf("%d\n",magicnumfile);
            //check if the number at the top of the map file is the same as our set MAGICNUMBER
            //exit if theyre not the same.
            if (magicnumfile!=MAGICNUMBER)
            {
                fprintf(stderr, "Magic number is %d but magic number at the top of the file: %s is %d\n",MAGICNUMBER,mappingfile,magicnumfile);
                exit(errno);
            }

        }


    }
    // initialize all the threads and semaphores
    pthread_t *workers = (pthread_t *)calloc(nThreads, sizeof(pthread_t));
    for(int i = 0; i < nThreads; i++) {
        // printf("attempting to make thread %d\n", i);
        if((pthread_create(&workers[i], NULL, worker, NULL)) != 0) {
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
