#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <termios.h>

#define PORT 8080
#define SIZE 1 
volatile char buffer[SIZE] = {0};
volatile int flag_in ;

int getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

void *get_cmd(void * arg){					///function to get arrow keys from terminal

while(1)	
{	
	fflush(stdin);
	buffer[0] = getch();
	flag_in = 1;
	//printf("got key press :%c\n",buffer[0]);
	//usleep(5000);
}
}
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread,ret;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    pthread_t output;
      
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, 
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    //valread = read( new_socket , buffer, 1024);
    //printf("%s\n",buffer );

	ret = pthread_create(&output, NULL, get_cmd, NULL);
	if(ret != 0)
		printf("Error creating output thread");


	while(1){
		if(flag_in){
    		printf("%d",send(new_socket , buffer , SIZE , 0 ));
		}
		flag_in = 0;
		usleep(500);
	}
    printf("Hello message sent\n");
    return 0;
}
