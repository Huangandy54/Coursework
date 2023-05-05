struct hostent *hent = gethostbyname(SERVER_NAME_STIRNG /* eg "localhost" */);
struct sockaddr_in addr;
memcpy(&addr.sin_addr.s_addr, hent->h_addr, hent->h_length);
addr.sin_port =htons(PORT_NUMBER);
addr.sin_family = AF_INET;

// Creating a socket
int sockfd = socket(AF_INET, SOCK_STREAM, 0);

//Socket Setup for Server
int enable = 1;
setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable));
bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
listen(sockfd, 0);
int cl = accept(sockfd, NULL, NULL);

//Socket Setup for Client
connect(sockfd, (struct sockaddr *)&addr, sizeof(addr));
