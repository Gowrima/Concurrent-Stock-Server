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

/* Initializes the pool of active clients */
void init_pool(int listenfd, pool *p)
{
	/* Initially, there are no connected descriptors */
	p->maxi = -1;
	for(int i = 0 ; i < FD_SETSIZE; i++)
		p->clientfd[i] = -1;

	/* Initially, listenfd is only member of select read set */
	p->maxfd = listenfd;
	FD_ZERO(&p->read_set);
	FD_SET(listenfd, &p->read_set);
}

/* Adds a new client connection to the pool */
void add_client(int connfd, pool *p)
{
	p->nready--;
	for(int i=0; i<FD_SETSIZE; i++){ /* Find an available slot */
		if(p->clientfd[i] == -1){
			/* Add connected descriptor to the pool */
			p->clientfd[i] = connfd;
			Rio_readinitb(&p->clientrio[i], connfd);

			/* Add the descriptor to desciptor set */
			FD_SET(connfd, &p->read_set);

			/* Update max descriptor and pool high water mark */
			if(connfd > p->maxfd)
				p->maxfd = connfd;
			if(i > p->maxi)
				p->maxi = i;
			break;
		}
		if(i == FD_SETSIZE) /* No empty slot */
			fprintf(stderr, "add_client error: Too many clients \n");
	}
}

/* services ready client connections */
void check_clients(pool *p)
{
	int connfd, n;
	char buf[MAXLINE];
	char result[MAXLINE];
	rio_t rio;

	for(int i=0; (i<=p->maxi) && (p->nready > 0); i++){
		connfd = p->clientfd[i];
		rio = p->clientrio[i];

		if((connfd > 0) && (FD_ISSET(connfd, &p->ready_set))){
			p->nready--;

			if((n=Rio_readlineb(&rio, buf, MAXLINE)) != 0){
				printf("server received %d bytes on fd %d\n", n, connfd);
				strcpy(result, echo(buf));
				if(strcmp(result, "exit") == 0){
					Close(connfd);
					FD_CLR(connfd, &p->read_set);
					p->clientfd[i] = -1;

					P(&w);
					write_file();
					V(&w);
				}
				else
					Rio_writen(connfd, result, MAXLINE);
			}
			else{
				Close(connfd);
				FD_CLR(connfd, &p->read_set);
				p->clientfd[i] = -1;

				P(&w);
				write_file();
				V(&w);
			}
		}
	}
}

int main(int argc, char **argv) 
{
	int listenfd, connfd;
	socklen_t clientlen;
	struct sockaddr_storage clientaddr;  /* Enough space for any address */
	char client_hostname[MAXLINE], client_port[MAXLINE];
	static pool pool;

	if (argc != 2) {
		fprintf(stderr, "usage: %s <port>\n", argv[0]);
		exit(0);
	}

	read_file();			/* Get stock table from stock.txt */
	Sem_init(&w, 0, 1);		/* Initialize w = 1 */
	Sem_init(&mutex, 0, 1); 	/* Initialize mutex = 1 */
	readcnt = 0;			/* Initialize readcount */

	listenfd = Open_listenfd(argv[1]);
	init_pool(listenfd, &pool);

	while (1) {
		/* Wait for listening/connected descriptors to become ready */
		pool.ready_set = pool.read_set;
		pool.nready = select(pool.maxfd+1, &pool.ready_set, NULL, NULL, NULL);

		/* If listening descriptor ready, add new client to pool */
		if(FD_ISSET(listenfd, &pool.ready_set)){
			clientlen = sizeof(struct sockaddr_storage); 
			connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
			Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
			printf("Connected to (%s, %s)\n", client_hostname, client_port);
			add_client(connfd, &pool);
		}

		/* Echo a text line from each ready connected descriptor */
		check_clients(&pool);
	}
    	exit(0);
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

	/* Write stock_table into stock.txt */
	update(stock_table, fp);

	fclose(fp);
}

void update(item *cur, FILE *fp){
	char buf[MAXLINE];
	memset(buf, 0, sizeof(buf));

	if(cur != NULL){
		update(cur->left, fp);

		strcat(buf, num_to_str(cur->id));
		strcat(buf, " ");
		strcat(buf, num_to_str(cur->left_stock));
		strcat(buf, " ");
		strcat(buf, num_to_str(cur->price));
		strcat(buf, "\n");

		Fputs(buf, fp);
		update(cur->right, fp);
	}
}
