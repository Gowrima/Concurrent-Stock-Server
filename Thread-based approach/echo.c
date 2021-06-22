/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *																 *
 * Sogang University											 *
 * Computer Science and Engineering								 *
 * System Programming											 *
 *																 *
 * Project name : Concurrent StockServer						 *
 * FIle name    : echo.c										 *
 * Author       : 20181603 kim minseon							 *
 * Date         : 2021 - 06 - 22								 *
 *																 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "csapp.h"

/* Convert number into string */
char *num_to_char(int num)
{
	static char str[10];
	memset(str, 0, sizeof(str));
	sprintf(str, "%d", num);

	return str;
}

/* Get the contents of stock_table 
 * - stock_table is binary tree  
 * - traverse the binary tree in-order */
void show(item *cur, char *result)
{
	if(cur != NULL){
		/* 1. traverse left child */
		show(cur->left, result);

		/* 2. get the contents of stock_table */
		strcat(result, num_to_char(cur->id));
		strcat(result, " ");
		strcat(result, num_to_char(cur->left_stock));
		strcat(result, " ");
		strcat(result, num_to_char(cur->price));
		strcat(result, "\n");

		/* 3. traverse right child */
		show(cur->right, result);
	}
}

/* Search for the node */
item *search(int id)
{
	item *cur = stock_table;	/* start from root node */

	while(cur != NULL){
		if(id < cur->id)		/* less than current node, go to the left child */
			cur = cur->left;
		else if(id > cur->id)	/* greater than current node, go to the right child */
			cur = cur->right;
		else					/* equal to current node, return it */
			break;
	}

	return cur;
}

int buy(int id, int num)
{
	/* Get the node from stock_table */
	item *cur = search(id);

	/* If left_stocks are not enough */
	if(cur->left_stock - num < 0)
		return -1;

	/* Update stock table */
	P(&w);					/* Block writers */
	cur->left_stock -= num; /* Critical section */
	V(&w);					/* Unblock writers */

	return 0;
}

void sell(int id, int num)
{
	/* Get the node from stock_table */
	item *cur = search(id);

	/* Update stock table */
	P(&w);					/* Block writers */
	cur->left_stock += num; /* Critical section */
	V(&w);					/* Unblock writers */
}

void echo(int connfd) 
{
    int n; 
    char buf[MAXLINE]; 
    rio_t rio;

    Rio_readinitb(&rio, connfd);
    while((n = Rio_readlineb(&rio, buf, MAXLINE)) != 0) {
		char result[MAXLINE];
		char *ptr;			/* temporary pointer for parsing */
		char cmd[10];		/* SHOW, BUY, SELL, EXIT */
		int id = 0;			/* stock id to BUY / SELL */
		int stock_num = 0;	/* # of stocks to BUY / SELL */

		printf("server received %d bytes\n", n); 
		memset(result, 0, sizeof(result));

		/* Parse buf */
		ptr = strtok(buf, " \n");
		strcpy(cmd, ptr);
		if((ptr = strtok(NULL, " \n")) != NULL)
			id = atoi(ptr);
		if((ptr = strtok(NULL, " \n")) != NULL)
			stock_num = atoi(ptr);

		/* Execute Command */
		if(strcmp(cmd, "show") == 0){	/* SHOW */
			P(&mutex);			/* Block readers */
			readcnt++;			/* Increase read count */
			if(readcnt == 1)	/* If readers exist, block writers */
				P(&w);			/* Block writers */
			V(&mutex);			/* Unblock readers */

			show(stock_table, result);

			P(&mutex);			/* Block readers */
			readcnt--;			/* Decrease read count */
			if(readcnt == 0)	/* If no readers, unblock writers */
				V(&w);			/* Unblock writers */
			V(&mutex);			/* Unblock readers */
		}

		else if(strcmp(cmd, "buy") == 0){	/* BUY */
			if(buy(id, stock_num) == -1)
				strcpy(result, "Not enough left stocks\n");
			else
				strcpy(result, "[buy] success\n");
		}

		else if(strcmp(cmd, "sell") == 0){	/* SELL */
			sell(id, stock_num);
			strcpy(result, "[sell] success\n");
		}

		else if(strcmp(cmd, "exit") == 0){	/* EXIT */
			strcpy(result, "exit\n");
			return;
		}

		else{
			strcpy(result, "invalid\n");
			return;
		}

		Rio_writen(connfd, result, MAXLINE);
    }
}

