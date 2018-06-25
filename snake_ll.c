#include"main_app.h"
/////************  function for adding new node in list ********///////////
struct snake* create_node(struct snake *head,int x,int y){
	struct snake *temp;					// temp variable
	temp = (struct snake *) malloc(sizeof(struct snake));	//allocate memory
	temp->x=x;						//assigning x and y data to struct members respectively
	temp->y=y;
	temp->next = NULL;					// point to null, cause last element

	if(head==NULL){						// check is head is first element
		head = temp;					//point head to temp
	}
	else{							//else go to last element and merge temp there
	while(head->next != NULL)
		head = head->next;

		head->next = temp;
	}
	return head;						//return head
}
