/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *																 
 * Sogang University											 
 * Computer Science and Engineering								 
 * System Programming											 
 *																 
 * Project name : Concurrent StockServer						 
 * FIle name    : echo.c										 
 * Author       : 20181603 kim minseon							 
 * Date         : 2021 - 06 - 22								 
 *																 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "csapp.h"

/* *******************************************
 * Convert number into string 
 *********************************************/
char *num_to_str(int num){
	static char str[10];
	memset(str, 0, sizeof(str));
	sprintf(str, "%d", num);

	return str;
}

/* *******************************************
 * Get the contents of stock_table
 * - stock_table is binary tree
 * - traverse the binary tree in-order
 **********************************************/
void show(item* cur, char* result){
	if (cur != NULL) {
		/* 1. traverse left child */
		show(cur->left, result);

		/* 2. get the contents of stock_table */
		strcat(result, num_to_str(cur->id));
		strcat(result, " ");
		strcat(result, num_to_str(cur->left_stock));
		strcat(result, " ");
		strcat(result, num_to_str(cur->price));
		strcat(result, "\n");

		/* 3. traverse right child */
		show(cur->right, result);
	}
}

/* *******************************************
 * Search for the node 
 *********************************************/
item* search(int id)
{
	item* cur = stock_table;	/* start from root node */

	while (cur != NULL) {
		if (id < cur->id) cur = cur->left;	 /* less than current node, go to the left child */			
		else if (id > cur->id) cur = cur->right; /* greater than current node, go to the right child */			
		else break;				 /* equal to current node, return it */			
	}

	return cur;
}

int buy(int id, int num)
{
	item* cur = search(id); 		/* Get the node from stock_table */
	
	if (cur->left_stock - num < 0) 		/* If left_stocks are not enough */
		return -1;

	/* Update stock table */
	P(&w);					/* Block writers */
	cur->left_stock -= num; 		/* Critical section */
	V(&w);					/* Unblock writers */

	return 0;
}

void sell(int id, int num)
{	
	item* cur = search(id);			/* Get the node from stock_table */

	/* Update stock table */
	P(&w);					/* Block writers */
	cur->left_stock += num; 		/* Critical section */
	V(&w);					/* Unblock writers */
}

char *echo(char *buf) 
{
	static char result[MAXLINE];
	char *ptr;
	char cmd[10];
	int id = 0;
	int stock_num = 0;

	memset(result, 0, sizeof(result));
	
	/* Parse buf */
	ptr = strtok(buf, " \n");
	strcpy(cmd, ptr);
	if((ptr = strtok(NULL, " \n")) != NULL)
		id = atoi(ptr);
	if((ptr = strtok(NULL, " \n")) != NULL)
		stock_num = atoi(ptr);

	/* Execute Command */
	if (strcmp(cmd, "show") == 0) {	 /* SHOW */
		P(&mutex);		 /* Block readers */
		readcnt++;		 /* Increase read count */
		if (readcnt == 1) P(&w); /* If readers exist, block writers */			
		V(&mutex);		 /* Unblock readers */

		show(stock_table, result);

		P(&mutex);		 /* Block readers */
		readcnt--;		 /* Decrease read count */
		if (readcnt == 0) V(&w); /* If no readers, unblock writers */			
		V(&mutex);		 /* Unblock writers */
	}

	else if (strcmp(cmd, "buy") == 0) { /* BUY */
		if (buy(id, stock_num) == -1) strcpy(result, "Not enough left stocks\n");
		else strcpy(result, "[buy] success\n");
	}

	else if (strcmp(cmd, "sell") == 0) { /* SELL */
		sell(id, stock_num);
		strcpy(result, "[sell] success\n");
	}

	else if (strcmp(cmd, "exit") == 0) { /* EXIT */
		strcpy(result, "exit");
	}

	else {
		strcpy(result, "invalid");
	}

	return result;
}
