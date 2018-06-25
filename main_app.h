#include<stdio.h>	
#include<sys/ioctl.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdlib.h>
#include<termios.h>
#include<time.h>
#include<signal.h>
#include<pthread.h>
#include<string.h>

//////******************struct for snake ***************************///////
struct snake{					/// stuct snake for linked list
	int x,y;				//var x and y for x and y cordinates
 	struct snake *next;			// pointer to point list to next element
};

//////******************function declaration***************************///////
void print_menu();
void setup();
void restart_game();
void start_game();
void create_food();
void *get_cmd(void *);
void gotoxy(int ,int );
void  get_time(struct tm *, time_t ,char *); 

void update_screen();
void *move_snake(void *);
void resume_game(struct snake *);
void save_game(struct snake *);
void game_over();
void print_snake(struct snake *);
void follow_head(struct snake *);
void print_msg();
struct snake * create_node(struct snake*,int ,int);

