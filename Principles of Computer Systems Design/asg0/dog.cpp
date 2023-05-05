#include <stdio.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/stat.h> 
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char const *argv[])
{
	int fd;
	ssize_t bytesRead;
	char buf[4096];
	//if there are no args
	if (argc==1){
		while(true){
			bytesRead=read(STDIN_FILENO, buf, 4096);
			if (bytesRead==0)
			{
				//free(buf);
				break;
			}
			write(STDOUT_FILENO, buf, bytesRead);
			
		}
	}
	else{
		//loops through file args
		for (int i=1; i<argc; i++){
			fd= open(argv[i], O_RDONLY);
			//error if file cant be opened
			if (fd == -1){
				perror(argv[i]);
				exit(errno);
			}
			bytesRead= read(fd, buf, 4096);
			//error for byts read
			if (bytesRead == -1){
				perror(argv[i]);
				exit(errno);
			}
			write(STDOUT_FILENO, buf, bytesRead);
			//closes the current fd
			close(fd);
		}
	}
	return 0;
}