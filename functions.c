#include"main_app.h"
extern int score;				//game score
extern int max_x,max_y,tail_x,tail_y;		//max x and y pos. of screen cordiantes
extern int life;				//snake life
extern int food_x,food_y;			//food cordinates
extern int head_dir;				//dir of head 0 up, 1 right, 2 down, 3 left
extern int game_flag,first_print;
extern int user_found;			// flag for user entry in database
//volatile int new_user;				//flag for new and regular user
extern int sock;				//for socket return value (fd)
extern char cur_time[30],user_name[20];		//to store in string format time and user name
extern struct snake *head;				//head ppointer of snake linked list
extern struct tm *tm_ptr;
extern struct sockaddr_in address;			//struct for socket connection
extern struct sockaddr_in serv_addr;

extern char buffer[2];			//buffer to store incomeing data via socket
time_t the_time;				//to get time value in linux/unix env. style i.e. from 01/01/1970 00:00:00
extern char cmd[1024];

//////****************function definition***************************///////

void update_screen(){						//function to update screen later used as thread
		//system("clear");
		fflush(stdout);						//flush stdout data
		gotoxy(0,0);
		printf("Score:%3d",score);				//print score
		gotoxy(max_y/3,1);
		printf("\u2665 %d",life);				//
		gotoxy(max_y-10,1);
		get_time(tm_ptr,the_time,&cur_time);			//call fun and store time in cur_time
		//printf("%s",cur_time);					//print cur time in game board
		puts(cur_time);
		//gotoxy(tail_y, tail_x);
		//printf(" ");
		//gotoxy(food_y, food_x);
		//printf(" ");
		gotoxy(food_y,food_x);					//print food on game board
		printf("\u25CF");
		//gotoxy(max_x,max_y);
		print_snake(head);					//print snake body on board
		usleep(10000);
}

///////////***************  print game over when life = 0*********//////////////////////
void game_over(){

	int i =0;
	while(i<3){

	system("clear");				//display game over 3 times
	gotoxy(max_y/2, max_x/2);
	printf("Game Over !!");
	sleep(1);
	i++;
	}
}

///////********************* resume or restart logic******/////////////////////////////
void resume_game(struct snake *head){				//load snake from file
        FILE *game;
	char file_name[20];
	strcpy(file_name,user_name);
	strcat(file_name,".data");
	//puts(file_name);
	game = fopen(file_name,"r");
        if(game != NULL){		//open user file if present
	 
		fscanf(game, "%d\n",&score);			//load score in score var
		//printf("score %d\n",score);
               	fscanf(game, "%d %d\n",&food_x, &food_y);	//load food position variables
		//printf("food %d %d\n",food_x, food_y);
       	      	fscanf(game, "%d\n",&head_dir);
		//printf("dir %d\n",head_dir);
	        head = create_node(head, 0, 0);
		
		while(fscanf(game,"%d %d\n",&head->x, &head->y) != EOF){	//load snake from save file
			head->next = create_node(head, 0 ,0);
			//printf("POS %d %d\n",head->x, head->y);
			head = head->next;
		}
		head->next = NULL;
       		fclose(game);
	}
	else{							//if user file not present print error
		printf("no save game found !!");
		sleep(2);
		print_menu();
	}

}


///////******************   save game to file  *******/////////////////////////////////
void save_game(struct snake *head){				//save game
	FILE *game;
	char file_name[20];
        strcpy(file_name,user_name);
        strcat(file_name,".data");
        game = fopen(file_name, "w");

	fprintf(game, "%d\n",score);
	fprintf(game, "%d %d\n",food_x, food_y);
	fprintf(game, "%d\n",head_dir);
	while(head->next != NULL){
		fprintf(game,"%d %d\n",head->x, head->y);
		head = head->next;
	}
	fclose(game);

}

/////********************  print snake on game board ********///////////////////////////

void print_snake(struct snake *head){				//print snake on game board
	
/*	if(first_print == 0){	

		while(head != NULL){					//go to each node in ll and print 8 at respective position
		gotoxy(head->y,head->x);
		head = head->next;
		printf("8");
		}
		first_print = 1;
	}
	else{*/
			
		gotoxy(head->y, head->x);
		printf("\u25A0");
		gotoxy(tail_y, tail_x);					// go to last tail pos and eraze *
		printf(" ");
//	}

}

////************    move snake body accordig to current game state ****////////////////////
void *move_snake(void *arg){
while(game_flag){
	int x,y;
	struct snake *temp =head;				
	if(head->next != NULL){						// if only one body element tail is = head
		tail_y = head->y;
		tail_x = head->x;
		
		follow_head(head);					//call function follow head
	}
	switch(head_dir)
	{
		default:
		break;
		case 0:							//up dir. setting limits and reset on game board
			if(head->x != 0)
				head->x--;
			else
				head->x = max_x;
		break;
		case 1:							//right
			if(head->y != max_y)
				head->y++;
			else
				head->y = 0;
		break;
		case 2:							//down
			if(head->x != max_x)
				head->x++;
			else
				head->x = 0;
		break;
		case 3:							//left
			if(head->y != 0)
				head->y--;
			else
				head->y = max_y;
		break;
	}
	if(head->x == food_x && head->y == food_y)			// check if food and head is on same position then update socore and gen.new food
	{
		switch(head_dir)
		{
			default:
			break;
			case 0:
				create_node(head,head->x+2,head->y);	//if snake is on up dire the gen new body element on same dir.
			break;
			case 1:
				create_node(head,head->x,head->y-2);	//same for right
			break;
			case 2:
				create_node(head,head->x-2,head->y);	//same for down
			break;
			case 3:
				create_node(head,head->x,head->y+2);	//same for left
			break;
		}
		score++;						// increase score
		system("aplay onboard-key-feedback.au -q");		//play sound when food is eaten
		create_food();						//food is ate, gen. new
	}
	head = temp;
	if(temp->next != NULL){
		x = temp->x;					//temp store in x and y for comparison later on
		y = temp->y;

		while(temp->next != NULL){
			if((x == temp->next->x) && (y == temp->next->y)){	//check if head is coliding with any body element
				game_flag = 0;				//tem stop printing board layout
				life--;					//reduce life
				head_dir += 1 %3;			//change dir so life -- is not recursive
				head_dir += 1 %3;
				gotoxy(max_y/2, max_x/2);
				print_msg();				//print msg that life reduced
				//save_game(head);			//save game when life is reduced
				game_flag = 1;				///resume updating game board
			}
			temp = temp->next;
		}
	}
	usleep(90000);							// sleep to copup with user speed 
	}
}

////////////*************  to make other body elements to follow head /********//////////////
void follow_head(struct snake *head){					//fun to make rest of the body follow the head
	
	struct snake *temp = head;
	char x,y,z,a;
	x = head->x;							//copy cordinates of head to temp var.
	y = head->y;
	while(head->next != NULL){					//start swaping cordinates from head to back in loop till last body element
		z = head->next->x;
		a = head->next->y;
		head->next->x = x;
		head->next->y = y;
		x = z;
		y = a;
		head = head->next;
	}
	tail_y = y;							//record last body element pos. to remove at last
	tail_x = x;
}
//////************  print messege function definition****************/////////
void print_msg(){
	system("clear");
	game_flag = 0;	
	puts("<<<<<<<<<<<LIFE REDUCED>>>>>>>>>");
	usleep(50000);
	game_flag = 1;
}

