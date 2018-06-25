
////////*********************system libraries******************** /////////////

#include "main_app.h"

////////*********************macro definition*************************/////////
#define PORT 8080

////////*********************Global variables****************************//////
	
int score;				//game score
int max_x,max_y,tail_x,tail_y;		//max x and y pos. of screen cordiantes
int life;				//snake life
int food_x,food_y;			//food cordinates
int head_dir;				//dir of head 0 up, 1 right, 2 down, 3 left
int game_flag,first_print;
int user_found;			// flag for user entry in database
//volatile int new_user;				//flag for new and regular user
int sock;				//for socket return value (fd)
char cur_time[30],user_name[20];		//to store in string format time and user name
struct snake *head;				//head ppointer of snake linked list
struct tm *tm_ptr;
struct sockaddr_in address;			//struct for socket connection
struct sockaddr_in serv_addr;

char buffer[2];			//buffer to store incomeing data via socket
time_t the_time;				//to get time value in linux/unix env. style i.e. from 01/01/1970 00:00:00
char cmd[1024];



/////****************  function definition ***********************////////////

void  get_time(struct tm *tm_ptr, time_t the_time,char *cur_time){
	(void) time(&the_time);
	tm_ptr = localtime(&the_time);
	sprintf(cur_time,"%02d:%02d:%02d",tm_ptr->tm_hour, tm_ptr->tm_min, tm_ptr->tm_sec);
}

void gotoxy(int x,int y)
{
printf("%c[%d;%df",0x1B,y,x);
}

int main(int argc, char *argv[]){
	int opt,ret;					//variables used for command line argument count
	char user[40];

	//system("img2txt maxresdefault.jpg -g 0.9 -H 25 -W 70"); display ascii image
	
		
	sleep(2);
	system("clear");
	
	ret = fork();
	if(ret ==0){
		//execl("/bin/sh", "sh", "-c", "pwd",(char *) 0);
		char *new_window_val = getenv("IN_NEW_WINDOW");
    		if (!new_window_val || new_window_val[0] != '1') {
        		snprintf(cmd, sizeof(cmd), "gnome-terminal -e ./server");
	        return system(cmd);
	    }//if ends
	}
	else if(ret < 0){
		printf("\nError creating process....EC 1");
	}
	else{
	
	while((opt = getopt(argc, argv, ":pu:rh")) != -1) {		//getopt for command line args break up
	switch(opt) {
	case 'p':
	case 'h':
	case 'r':
		printf("option: %c\n", opt);
	break;
	case 'u':
		//printf("user: %s\n", optarg);
		strcpy(user_name,optarg);		//copy username to global char array
	//	if(check_user(optarg)){puts(user_name);}
	///	else{puts("user not found");}
	break;
	case ':':
		printf("option needs a value i.e. -u user\n");
	exit(0);
	break;
	case '?':
		printf(	"unknown option: %c\n", optopt);
		printf("type ./snake -u user\n");
	exit(0);
	break;
    }
	}
	setup();
	print_menu();
}///else close
	return 0;
}


void print_menu(){
	int user_input,pid;					//var used to store user input and process id of child
	user_input = 0;						//set to 0 initially
	while(user_input != 'q'){				//print menu

		system("clear");
		user_input = 0;					//reset user input for safety
		gotoxy(max_y/2,max_x/2);
		printf("Please enter command\n");		//print menu
		gotoxy(max_y/2,max_x/2+1);
		printf("[p]  Start game\n");
		gotoxy(max_y/2,max_x/2+2);
		printf("[a]  Play again\n");
		gotoxy(max_y/2,max_x/2+3);
		printf("[v]  Visit us\n");
		gotoxy(max_y/2,max_x/2+4);
		printf("[s]  Save game\n");
		gotoxy(max_y/2,max_x/2+5);
		printf("[r]  Resume game\n");
		gotoxy(max_y/2,max_x/2+6);
		printf("[q] Exit\n");
		gotoxy(max_y/2,max_x/2+7);
		//fflush(stdin);
		//scanf("%d",&user_input);
		//fflush(stdin);
		user_input = getchar();	
		switch(user_input){
			default:
				printf("Provide valid input\n");
			break;
			case 'p':
				start_game();
			break;
			case 'a':
				restart_game();
			break;
			case 's':
				game_flag = 0;
				save_game(head);
			break;
			case 'r':
				resume_game(head);
				start_game();
			break;
			case 'v':	
				pid = fork();
				
				if(pid == 0) {
					execl("/usr/bin/firefox", "/usr/bin/firefox", "ekalin.ga", (char*)NULL);
				}
				else if(pid<0){
					printf("Execl failed\n");				
				}	
				else{
					
				}
			break;
			case 'q':
				exit(0);	
			break;
		}
	}

}


void start_game(){
	game_flag = 1;					//make game flag 1 to start threads work
	system("clear");				//clear screen
	while(game_flag)				//while game_flag = 1 update screen
	{
		if(life <0 ){
			gotoxy(max_y/2, max_x/2);
			printf("Game over Do you wanna restart? y/n");
			sleep(5);
			fflush(stdin);
			if(getchar() == 'y'){		// if yes reset all the variables
				game_flag = 0;
				game_over();
        			score = 0;
			        //free(head->next);// = NULL;
			        head = create_node(head,max_y/2,max_y/2);
			        head_dir = 0;
			        life = 3;
			        game_flag = 1;

			}
		}
		//gotoxy(0,0);					//were used when meethod was linear
		//system("clear");
		//get_cmd(&head_dir);
		else{
			update_screen(NULL);
		}
		//usleep(90000);
	}
}
void restart_game(){
	game_flag = 0;
	score = 0;
	//head->next = NULL;
	head = create_node(head,max_y/2,max_y/2);
	head_dir = 0;
	life = 3;
	game_flag = 1;
	start_game();	
}


void setup(){
	int ret;							//for return value of thread creation
	int count = 0;	
	pthread_t input,output;						//to store thread object
	struct winsize w;						//to get window size
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);				//ioctl to get resolution of window
	
		if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)		//create TCP socket and check foe errors
    	{
        	printf("\n Socket creation error \n");
    	}

	memset(&serv_addr, '0', sizeof(serv_addr));			//fill server add var with '0's, for safety
	
	serv_addr.sin_family = AF_INET;					//set socket for IPV4 and PORT to defined port
	serv_addr.sin_port = htons(PORT);

									// Convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    	{
       		 printf("\nInvalid address/ Address not supported \n");
    	}
	
									//initiate a connection on a socket
	retry:if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    	{
		gotoxy(0,0);
       		 printf("\nFailed to connect to server....\n");
		sleep(1);
		printf("\nretrying....\n");
		sleep(1);
		system("clear");
		if(count <3){
		count++;
		goto retry;
		}
		printf("\nExiting now...\n");
		sleep(2);
		exit(0);
		
    	}


	score = 0;							//set score to 0 initially
	game_flag = 1;							//game flag set to start the game
	max_x = w.ws_row-10;						//set board width and height
	max_y = w.ws_col-10;
	life = 3;
	head_dir = 3;
	first_print = 1;
	signal(SIGINT, print_menu);						///setting signal for SIGINT
	create_food();							//create initial food
	head = NULL;
	head = create_node(head,max_x/2,max_y/2);			//create head of snake and give pos. of 50x,50y
	head = create_node(head,max_x/2+1,max_y/2);
	system("clear");
	
	ret = pthread_create(&input, NULL,move_snake ,NULL);
	if(ret != 0)
		printf("Error creating input thread");
	ret = pthread_create(&output, NULL, get_cmd, NULL);
	if(ret != 0)
		printf("Error creating output thread");






}



void create_food(){					//generate food 
	srand(time(NULL));
	printf(" ");
	food_x = rand()%(max_x-2);
	food_y = rand()%(max_y-2);
	if( food_x <= 1)				//to make sure food in gen on postion of display infoo like score
	{
		//food_y = 10;
		food_x = 10;
	}
}
void *get_cmd(void * arg){					///function to get arrow keys from terminal

while(game_flag)	
{
	
		read( sock , buffer, 2);
		
               // printf("%d",buffer[0]);
//                write(STDIN_FILENO,buffer,1);
	/*if (getch() == '\033') {			// if the first value is esc
	 getch(); 					// skip the [
		
*/
    switch(buffer[0]) { 					// the real value
        case 'A':
		if(head_dir != 2)
            		head_dir = 0;				//directin up
            break;
        case 'B':
		if(head_dir != 0)
            		head_dir = 2;				//direction down
            break;
        case 'C':
		if(head_dir != 3)
            		head_dir = 1;				//dir right
            break;
        case 'D':	
		if(head_dir != 1)			//dir left
            		head_dir = 3;
            break;
   	 }
//	}
	//usleep(5000);
}
}


