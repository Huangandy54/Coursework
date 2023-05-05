# CSE 130
## asg2
### Andy Huang
### ahuang44@ucsc.edu

This program is a multi-threaded http server able to handle GET and PUT requests.
port is optional and will be run at port 80 if not supplied
program will default to 4 threads if -n option is not provided
Provide the -l option to enable logging


Run on command line: 

basic execution with default # of threads and no logging
```bash
./httpserver <hostname> <port>
```

The following will start the httpserver at localhost:8888 with 8 worker threads
```bash
./httpserver -N 8 localhost 8888
```

The following will start the httpserver at localhost:8888 with 8 worker threads and logging to my_log.txt
```bash
./httpserver -N 8 -l my_log.txt localhost 8888
``` 


Target Resource must be 27 characters long and cannot be anything other than upper and lower case letters, a dash(-), or a underscore(_). If there is a slash(/) in the front, we can ignore it and make everything after it is a valid resource name.
