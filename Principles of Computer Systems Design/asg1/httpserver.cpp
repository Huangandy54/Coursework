#include <errno.h>
#include <fcntl.h>
#include <iostream>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
using namespace std;
#define MAX_BUFFER_SIZE 32768
#define DEFAULT_PORT "80"

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

void processHeader(uint8_t inputBuffer[MAX_BUFFER_SIZE], int clientfd)
{
    char requestType[MAX_BUFFER_SIZE];
    char resourceName[MAX_BUFFER_SIZE];
    uint8_t contentBuffer[MAX_BUFFER_SIZE];
    memcpy(contentBuffer, inputBuffer, MAX_BUFFER_SIZE);
    bool endOfHeader = false;
    bool contentLengthExists = false;
    char *contentLengthPtr;
    int contentLength;
    sscanf((char *)contentBuffer, "%s %s \r\n", (char *)requestType, (char *)resourceName);
    while(endOfHeader == false) {
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
            if((fd = open(resourceName, O_WRONLY)) == -1) {
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
                    int bytesRead = read(clientfd, contentBuffer, sizeof(contentBuffer));
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
                    int bytesRead = read(fd, contentBuffer, sizeof(contentBuffer));
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
        processHeader(contentBuffer, clientfd);
    }
}

int main(int argc, char const *argv[])
{
    const char *hostname;
    const char *port;
    uint8_t buffer[MAX_BUFFER_SIZE];
    int bytesRead;
    int new_socket;
    int main_socket;
    // if $httpserver x then set port to default 80
    if(argc == 2) {
        hostname = argv[1];
        port = DEFAULT_PORT;

    } else if(argc == 3) // all both hostname and port given through command line
    {
        hostname = argv[1];
        port = argv[2];
    } else { // else any other amount of args is incorrect
        fprintf(stderr, "hostname not found. Usage: httpserver <hostname> <port(optional)>\n");
        exit(errno);
    }

    main_socket = startServer(hostname, port);

    // Your code, starting with accept(), goes here
    // continue listening and accept requests
    while(true) {
        if((new_socket = accept(main_socket, NULL, NULL)) < 0) {
            fprintf(stderr, "could not accept request: %s\n", strerror(errno));
            // exit(EXIT_FAILURE);
        } else {
            bytesRead = read(new_socket, buffer, sizeof(buffer));
            if(bytesRead == 0) {
                // printf("end of file\n");
            }
            processHeader(buffer, new_socket);
            memset(buffer, 0, sizeof(buffer));
        }
    } // end while
    return 0;
} // end main
