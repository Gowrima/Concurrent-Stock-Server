/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *																 
 * Sogang University											 
 * Computer Science and Engineering								 
 * System Programming											 
 *																 
 * Project name : Concurrent StockServer						 
 * FIle name    : stockserver.c									 
 * Author       : 20181603 kim minseon							 
 * Date         : 2021 - 06 - 22								 
 *																 
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "csapp.h"

void sbuf_init(sbuf_t *sp, int n){
	/* Create an empty bounded, shared FIFO buffer with n slots */
	sp->buf = Calloc(n, sizeof(int));
	sp->n = n;			/* Buffer holds max of n items */
	sp->front = sp->rear = 0;	/* Empty buffer iff front == rear */
	Sem_init(&sp->mutex, 0, 1);	/* Binary semaphore for locking */
	Sem_init(&sp->slots, 0, n);	/* Initially, buf has n empty slots */
	Sem_init(&sp->items, 0, 0);	/* Initially, buf has 0 items */
}

void sbuf_deinit(sbuf_t *sp){	
	Free(sp->buf); 	/* Clean up buffer sp */
}

void sbuf_insert(sbuf_t *sp, int item){
	/* Insert item onto the rear of shared buffer sp */
	P(&sp->slots);				/* Wait for available slot */
	P(&sp->mutex);				/* Lock the buffer */
	sp->buf[(++sp->rear)%(sp->n)] = item;	/* Insert the item */
	V(&sp->mutex);				/* Unlock the buffer */
	V(&sp->items);				/* Announce available item */
}

int sbuf_remove(sbuf_t *sp){
	/* Remove and return the first item from buffer sp */
	int item;
	P(&sp->items);				/* Wait for available item */
	P(&sp->mutex);				/* Lock the buffer */
	item = sp->buf[(++sp->front)%(sp->n)];	/* Remove the item */
	V(&sp->mutex);				/* Unlock the buffer */
	V(&sp->slots);				/* Announce available slot */
	return item;
}

int main(int argc, char **argv) 
{
	int listenfd, connfd;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;  /* Enough space for any address */ 
	char client_hostname[MAXLINE], client_port[MAXLINE];
	pthread_t tid;
		
	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
	}

	read_file();		/* Get stock table from stock.txt */
	Sem_init(&w, 0, 1);	/* Initialize w = 1 */
	Sem_init(&mutex, 0, 1); /* Initialize mutex = 1 */
	readcnt = 0;		/* Initialize readcount */

    	listenfd = Open_listenfd(argv[1]); /* Listen */
	sbuf_init(&sbuf, SBUFSIZE);		
	for(int i=0; i<NTHREADS; i++) 	   /* Pooling threads */
		Pthread_create(&tid, NULL, thread, NULL);

	while (1) {
		clientlen = sizeof(struct sockaddr_storage); 
		connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen); 					/* Wait for client connection */
		Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0); 	/* Get info of the connected client */
		printf("Connected to (%s, %s)\n", client_hostname, client_port); 				/* Print info of the connected client */
		sbuf_insert(&sbuf, connfd); 									/* Run on thread */
	}

	sbuf_deinit(&sbuf);
	//free(stock_table);
   	exit(0);
}

void *thread(void *vargp){
	Pthread_detach(pthread_self());

	while(1){
		int connfd = sbuf_remove(&sbuf); /* Remove connfd from buf */
		echo(connfd);			 /* Service client */
		Close(connfd);			 /* Close connection */

		P(&w);		/* Block writers */
		write_file();	/* Update stock.txt */
		V(&w);		/* Unblock writers */
	}
}

void read_file(){
	FILE *fp = fopen("stock.txt", "r");
	char line[MAXLINE];
	item_num = 0;

	if(fp == NULL){
		fprintf(stderr, "*** File open error ***\n");
		exit(0);
	}

	while(fgets(line, MAXLINE, fp) != NULL){
		int id;
		int num = 0;
		int price = 0;
		char *ptr;
		item *new;
		item *cur = stock_table;

		ptr = strtok(line, " \n");
		id = atoi(ptr);
		if((ptr = strtok(NULL, " \n")) != NULL)
			num = atoi(ptr);
		if((ptr = strtok(NULL, " \n")) != NULL)
			price = atoi(ptr);

		/* Input to stock_table */
		new = malloc(sizeof(struct item));
		new->id = id;
		new->left_stock = num;
		new->price = price;
		new->left = NULL;
		new->right = NULL;

		if(item_num == 0){
			stock_table = new;
		}
		else{
			while(1){
				if(new->id < cur->id){
					if(cur->left == NULL){
						cur->left = new;
						break;
					}
					cur = cur->left;
				}
				else{
					if(cur->right == NULL){
						cur->right = new;
						break;
					}
					cur = cur->right;
				}
			}
		}
		item_num++;
	}

	fclose(fp);
}

void write_file(){
	FILE *fp = fopen("stock.txt", "w");
	
	if(fp == NULL){
		fprintf(stderr, "*** File open error ***\n");
		exit(0);
	}
	
	update(stock_table, fp); /* Write stock_table into stock.txt */
	fclose(fp);
}

void update(item *cur, FILE *fp){
	char buf[MAXLINE];
	memset(buf, 0, sizeof(buf));

	if(cur != NULL){
		update(cur->left, fp);

		strcat(buf, num_to_char(cur->id));
		strcat(buf, " ");
		strcat(buf, num_to_char(cur->left_stock));
		strcat(buf, " ");
		strcat(buf, num_to_char(cur->price));
		strcat(buf, "\n");

		Fputs(buf, fp);
		update(cur->right, fp);
	}
}
