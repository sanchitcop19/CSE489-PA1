/**
 * @sanchitb_assignment1
 * @author  Sanchit Batra <sanchitb@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <stdio.h>
#include <stdlib.h>
#include "../include/global.h"
#include "../include/logger.h"
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include "dict.h"

#define CLIENT 1
#define SERVER 0
#define BACKLOG 5
#define STDIN 0
#define TRUE 1
#define FALSE 0
#define CMD_SIZE 100
#define BUFFER_SIZE 256
#define IP_SIZE 16
#define HOSTNAME_SIZE 100
#define MAX_CLIENTS 4
#define PORT_SIZE 5

int g_server_fd = -1;
int g_server_port = -1;

void itoa(int n, char s[]);

struct client {
	char hostname[HOSTNAME_SIZE];
	char ip[IP_SIZE];
	int port;
	int logged_in;
	int num_sent;
	int num_recv;
		
} ;
struct client* clients[4] = {
		NULL,
		NULL,
		NULL,
		NULL
	};

/* https://www.geeksforgeeks.org/bubble-sort/ */
void swap(struct client **xp, struct client **yp) 
{ 
	struct client* temp = *xp; 
	*xp = *yp; 
	*yp = temp; 
} 

void edit_port(char ip[], char port[]){
	for(int i = 0; i < MAX_CLIENTS; ++i){
		if (clients[i] == NULL) continue;
		if (!strcmp(ip, clients[i]->ip)) clients[i]->port = atoi(port);
	}
};

void bubbleSort(struct client* arr[], int n) 
{ 
int i, j; 
if (arr[1] == NULL) return;
for (i = 0; i < n-1; i++){	 
	for (j = 0; j < n-i-1; j++){
		if (arr[j+1] == NULL) return; 	
		if (arr[j]->port > arr[j+1]->port) 
			swap(&arr[j], &arr[j+1]); 
	}
}
} 

/* https://beej.us/guide/bgnet/html/multi/getpeernameman.html */

char* socket_to_ip(int s){
	socklen_t len;
	struct sockaddr_storage addr;
	char* ipstr = malloc(IP_SIZE);
	int port;

	len = sizeof addr;
	if (getpeername(s, (struct sockaddr*)&addr, &len)) perror("getpeername threw an error: ");
	if (addr.ss_family == AF_INET) {
		struct sockaddr_in *s = (struct sockaddr_in *)&addr;
		port = ntohs(s->sin_port);
		inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);

	} else{	
		struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
		port = ntohs(s->sin6_port);
		inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
	}
return ipstr;
}

void logout(){

}

void login(){

}

void add_client(int clientfd, struct sockaddr_in * client_address, char ip_copy[]){
	/* Adds a client's information to the global array and logs them in */
	char ip[IP_SIZE];	
        if (inet_ntop(AF_INET, &(client_address->sin_addr), ip, sizeof ip) == NULL){
                perror("inettnop returned null\n");
	}
	strcpy(ip_copy, ip);
	/* Convert the sockaddr to the corresponding hostname, service can be thrown away */	
	char host[HOSTNAME_SIZE];
	char service[20];
	if (getnameinfo((struct sockaddr*)client_address, sizeof (*client_address), host, sizeof host, service , sizeof service, 0) != 0){
		perror("getnameinfo returned non-zero\n");
	}


	for(int c = 0; c < MAX_CLIENTS; ++c ){
		if(clients[c] == NULL) continue; 
		char temp[20];
		itoa(strcmp(clients[c]->hostname, host), temp);
		if (!strcmp(clients[c]->hostname, host)) {
			clients[c]->logged_in = TRUE;
			/* TODO: CHECK NETWORK BYTE ORDER */
			clients[c]->port = ntohs(client_address->sin_port);
			return;
		}
	}	
	
	int i = 0;
	while (clients[i] != NULL){
		++i;
	}
	struct client _client;
	struct client* ptr = malloc(sizeof (struct client)); 
	strcpy(ptr->ip, ip);
	strcpy(ptr->hostname, host);
	ptr->port = ntohs(client_address->sin_port);
	ptr->logged_in = TRUE;
	ptr->num_sent = 0;
	ptr->num_recv = 0;	

	clients[i] = ptr;

	}


void list_clients(){
	int id = 0;
	for (int list_id = 0; list_id < MAX_CLIENTS; ++list_id){
		if (clients[list_id] == NULL) continue;
		if (!clients[list_id]->logged_in) continue;
		++id;
		char hostname[HOSTNAME_SIZE];
		strcpy(hostname, clients[list_id]->hostname);
		char ip_addr[IP_SIZE];
		strcpy(ip_addr, clients[list_id]->ip);
		int port_num = clients[list_id]->port;
		cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", (id), hostname, ip_addr, port_num);
	}
}
const char* lookup_ip(int sock_index, Dict map){

				char temp[4];
				memset(temp, '\0', sizeof temp);
				itoa(sock_index, temp);
				
				const char* from = DictSearch(map, temp);
				if (from == NULL) from = "";
				return from;
}
void get_ip(){
        int dummy_socket;
        struct sockaddr_in addr;
        struct addrinfo hints, *res;
        memset(&hints, 0, sizeof hints);
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;
        int len = sizeof addr;
        getaddrinfo("www.beej.us", "80", &hints, &res);

        dummy_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

         if (connect(dummy_socket, res->ai_addr, res->ai_addrlen) < 0){
                perror("connect failed\n");
        }

         if (getsockname(dummy_socket,(struct sockaddr*) &addr,(socklen_t*)(&len))  < 0){
                ////printf("errno: %i\n", errno);
                perror("getsockname failed\n");
        }
        char ip[16]; 

        if (inet_ntop(AF_INET, &(addr.sin_addr), ip, sizeof ip) == NULL){
                perror("inettnop returned null\n");
	}
	cse4589_print_and_log("IP:%s\n", ip);


}
/* https://en.wikibooks.org/wiki/C_Programming/stdlib.h/itoa */
 /* reverse:  reverse string s in place */
 void reverse(char s[])
 {
     int i, j;
     char c;
 
     for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
 }

/* https://en.wikibooks.org/wiki/C_Programming/stdlib.h/itoa */
/* itoa:  convert n to characters in s */
 void itoa(int n, char s[])
 {
     int i, sign;
 
     if ((sign = n) < 0)  /* record sign */
         n = -n;          /* make n positive */
     i = 0;
     do {       /* generate digits in reverse order */
         s[i++] = n % 10 + '0';   /* get next digit */
     } while ((n /= 10) > 0);     /* delete it */
     if (sign < 0)
         s[i++] = '-';
     s[i] = '\0';
     reverse(s);
 }

void add_helper(struct client* client, char host[], char ip[], int port ){

        strcpy(client->ip, ip);
        client->port = port;
        strcpy(client->hostname, host);
}

void create_list(char buffer[]){

	for (int i = 0; i < 4; ++i){
		if (clients[i] == NULL || clients[i]->logged_in == 0) continue;
		char temp[BUFFER_SIZE];
		if(i!=0)strcat(buffer, " ");
		memset(temp, '\0', sizeof temp);
		strcpy(temp, clients[i]->hostname);
		strcat(buffer, strcat(temp, " "));
		memset(temp, '\0', sizeof temp);
		strcpy(temp, clients[i]->ip);
		strcat(buffer, strcat(temp, " "));
		memset(temp, '\0', sizeof temp);
		char port[PORT_SIZE+1];	
		itoa(clients[i]->port, port);			
		strcat(buffer, port);
	}
				
}

int connect_to_host(char *server_ip, int server_port, char* g_server_ip)
{   
    int fdsocket, len; 
    struct sockaddr_in remote_server_addr;
    
    fdsocket = socket(AF_INET, SOCK_STREAM, 0);
    //printf("%i\n", fdsocket);
    if(fdsocket < 0)
       perror("Failed to create socket");
    g_server_fd = fdsocket;
    g_server_port = server_port;
    strcpy(g_server_ip, server_ip);
    bzero(&remote_server_addr, sizeof(remote_server_addr));
    remote_server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, server_ip, &remote_server_addr.sin_addr);
    remote_server_addr.sin_port = htons(server_port);
         
        if(connect(fdsocket, (struct sockaddr*)&remote_server_addr, sizeof(remote_server_addr)) < 0)
                perror("Connect failed");
        return fdsocket;


}

void get_logged_in(char* info){
        int i = 0;

        info = strtok(info, " ");
        while (info!= NULL){
                struct client* ptr = malloc(sizeof (struct client));
                char host[HOSTNAME_SIZE];
                char ip[IP_SIZE];
                int port = -1;

                strcpy(host, info);
                info = strtok(NULL, " ");
                strcpy(ip, info);
                info = strtok(NULL, " \n");
                port = atoi(info);
                add_helper(ptr, host, ip, port);
                clients[i] = ptr;
                clients[i]->logged_in = TRUE;
		++i;
                info = strtok(NULL, " ");
        }
}

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
int main(int argc, char **argv)
{
	/*Init. Logger*/
	cse4589_init_log(argv[2]);

	/*Clear LOGFILE*/
	close( fileno(fopen(LOGFILE, "w")));

	char* g_server_ip = malloc(IP_SIZE);
 	int MACHINE;	
	if (!strcmp(argv[1], "c")){

				//client
		MACHINE = CLIENT;

	    int head_socket, selret, sock_index;
	    fd_set master_list, watch_list;
	    /* Zero select FD sets */
	    FD_ZERO(&master_list);
	    FD_ZERO(&watch_list);

	    /* Register STDIN */
	    FD_SET(STDIN, &master_list);

	    head_socket = STDIN;
	    fflush(stdout);
	    while(TRUE){
		memcpy(&watch_list, &master_list, sizeof(master_list));
		/* select() system call. This will BLOCK */
		selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL);
		if(selret < 0)
		    perror("select failed.");

		/* Check if we have sockets/STDIN to process */
		if(selret > 0){
		    /* Loop through socket descriptors to check which ones are ready */
		    for(sock_index=0; sock_index<=head_socket; sock_index+=1){
			/*//printf("sock_index: %i\n", sock_index);*/
			if(FD_ISSET(sock_index, &watch_list)){
			    /* Check if new command on STDIN */
			    if (sock_index == STDIN){
				char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);

				memset(cmd, '\0', CMD_SIZE);
							if(fgets(cmd, CMD_SIZE-1, stdin) == NULL) 
								exit(-1);

							/*//printf("\nI got: %s\n", cmd);*/
							char* delimiters = " \n";	
							char* command_str = strtok(cmd, delimiters);
							//printf("command_str: %s\n", command_str);
							if (!strcmp(command_str, "AUTHOR")){
							cse4589_print_and_log("[%s:SUCCESS]\n", command_str);
							cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n","sanchitb");		
							cse4589_print_and_log("[%s:END]\n", command_str);
							}
						else if (!strcmp(command_str, "IP")){
							cse4589_print_and_log("[%s:SUCCESS]\n", command_str);
							get_ip();
							cse4589_print_and_log("[%s:END]\n", command_str);
						}
						else if (!strcmp(command_str, "PORT")){
							cse4589_print_and_log("[%s:SUCCESS]\n", command_str);
							cse4589_print_and_log("PORT:%s\n", argv[2]);
							cse4589_print_and_log("[%s:END]\n", command_str);
						}
						else if (!strcmp(command_str, "LIST")){
							cse4589_print_and_log("[%s:SUCCESS]\n", command_str);
							list_clients();		
							cse4589_print_and_log("[%s:END]\n", command_str);
						}
						else if (!strcmp(command_str, "REFRESH")){
							cse4589_print_and_log("[%s:SUCCESS]\n", command_str);
							/* logout and log back in */

							FD_CLR(g_server_fd, &master_list);
							FD_CLR(g_server_fd, &watch_list);
							close(g_server_fd);
							char* ip;
							strcpy(ip, g_server_ip);
							int port = g_server_port;
							
							int server = connect_to_host(ip, port, g_server_ip);

							char *list = (char*) malloc(sizeof(char)*BUFFER_SIZE);
							memset(list, '\0', BUFFER_SIZE);
							if (send(server, argv[2], PORT_SIZE, 0) <= 0) perror("could not send port\n");

							if (recv(server, list, BUFFER_SIZE, 0) <= 0) perror("could not retreive list\n");
							//printf("list: %s\n", list);	
							get_logged_in(list);
							
							/* Register the listening socket */
							FD_SET(server, &master_list);
							head_socket = server;
							//printf("[%s:END]\n", command_str);
							list_clients();
							cse4589_print_and_log("[%s:END]\n", command_str);

						}
						else if (!strcmp(command_str, "LOGIN")){
							//printf("[%s:SUCCESS]\n", command_str);
							command_str = strtok(NULL, delimiters);
							char* ip = command_str;
							command_str = strtok(NULL, delimiters);
							int port = atoi(command_str);
							
							int server = connect_to_host(ip, port, g_server_ip);

							char *list = (char*) malloc(sizeof(char)*BUFFER_SIZE);
							memset(list, '\0', BUFFER_SIZE);
							if (send(server, argv[2], PORT_SIZE, 0) <= 0) perror("could not send port\n");

							if (recv(server, list, BUFFER_SIZE, 0) <= 0) perror("could not retreive list\n");
							//printf("list: %s\n", list);	
							get_logged_in(list);
							
							/* Register the listening socket */
							FD_SET(server, &master_list);
							head_socket = server;
							//printf("[%s:END]\n", command_str);
						}
						else if (!strcmp(command_str, "LOGOUT")){
							//printf("[%s:SUCCESS]\n", command_str);
							FD_CLR(g_server_fd, &master_list);
							FD_CLR(g_server_fd, &watch_list);
							close(g_server_fd);
							for (int free = 0; free < MAX_CLIENTS; ++free) clients[free] = NULL;
							//printf("[%s:END]\n", command_str);
						}
						else if (!strcmp(command_str, "SEND")){
							cse4589_print_and_log("[%s:SUCCESS]\n", command_str);
							command_str = strtok(NULL, delimiters);				
							char* ip = command_str;
							command_str = strtok(NULL, "\n");
							char msg[BUFFER_SIZE+IP_SIZE];
							memset(msg, '\0', sizeof msg);
							strcat(msg, ip);
							strcat(msg, ":");
							strcat(msg, command_str);
							int sent = send(g_server_fd, msg, strlen(msg), 0);
							if (sent != strlen(msg)) perror("could not send entire msg\n");
							fflush(stdout);				
							cse4589_print_and_log("[%s:END]\n", "SEND");
							}		
								
						free(cmd);
			    }
		
	else{
		//server
		MACHINE = SERVER;
	
	/* http://www.cs.yale.edu/homes/aspnes/pinewiki/C(2f)HashTables.html?highlight=%28CategoryAlgorithmNotes%29 */
	Dict map;
	map = DictCreate();
	
	int port, server_socket, head_socket, selret, sock_index, fdaccept=0, caddr_len;
        struct sockaddr_in server_addr, client_addr;
        fd_set master_list, watch_list;
			
	    /* Socket */
        server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket < 0)
                perror("Cannot create socket");

        /* Fill up sockaddr_in struct */
        port = atoi(argv[2]);
        bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    /* Bind */
    if(bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0 )
        perror("Bind failed");

    /* Listen */
    if(listen(server_socket, BACKLOG) < 0)
        perror("Unable to listen on port");

    /* ---------------------------------------------------------------------------- */

    /* Zero select FD sets */
    FD_ZERO(&master_list);
    FD_ZERO(&watch_list);

    /* Register the listening socket */
    FD_SET(server_socket, &master_list);
    /* Register STDIN */
    FD_SET(STDIN, &master_list);

    head_socket = server_socket;
    fflush(stdout);
    while(TRUE){
       
	fflush(stdout);fflush(stdin);
	 memcpy(&watch_list, &master_list, sizeof(master_list));
        /* select() system call. This will BLOCK */
        selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL);
        if(selret < 0)
            perror("select failed.");

        /* Check if we have sockets/STDIN to process */
        if(selret > 0){
            /* Loop through socket descriptors to check which ones are ready */
            for(sock_index=0; sock_index<=head_socket; sock_index+=1){
		/*//printf("sock_index: %i\n", sock_index);*/
                if(FD_ISSET(sock_index, &watch_list)){

                    /* Check if new command on STDIN */
                    if (sock_index == STDIN){
                    	char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);

                    	memset(cmd, '\0', CMD_SIZE);
						if(fgets(cmd, CMD_SIZE-1, stdin) == NULL) 
							exit(-1);

						/*//printf("\nI got: %s\n", cmd);*/
						
						char* command_str = strtok(cmd, "\n");
						/*//printf("command_str: %s\n", command_str);*/
						if (!strcmp(command_str, "AUTHOR")){
													
							cse4589_print_and_log("[%s:SUCCESS]\n", command_str);
							cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n","sanchitb");		
							cse4589_print_and_log("[%s:END]\n", command_str);
						}
						else if (!strcmp(command_str, "IP")){
							
							cse4589_print_and_log("[%s:SUCCESS]\n", command_str);
							get_ip();
							cse4589_print_and_log("[%s:END]\n", command_str);
						}
						else if (!strcmp(command_str, "PORT")){
							cse4589_print_and_log("[%s:SUCCESS]\n", command_str);
							cse4589_print_and_log("PORT:%s\n", argv[2]);
							cse4589_print_and_log("[%s:END]\n", command_str);
						}
						else if (!strcmp(command_str, "LIST")){
							cse4589_print_and_log("[%s:SUCCESS]\n", command_str);

							list_clients();		
							cse4589_print_and_log("[%s:END]\n", command_str);
						
						}
								
						free(cmd);
                    }
                    /* Check if new client is requesting connection */
                    else if(sock_index == server_socket){
                        
			caddr_len = sizeof(client_addr);
                        fdaccept = accept(server_socket, (struct sockaddr *)&client_addr, &caddr_len);
                        if(fdaccept < 0)
                            perror("Accept failed.");
			/*//printf("accept socket: %i\n", fdaccept);*/
			/*//printf("\nRemote Host connected!\n");*/                        
			
			char ip[IP_SIZE];
			memset(ip, '\0', sizeof ip);
			
			//printf("adding client\n");
			add_client(fdaccept, &client_addr, ip);	
			
			
			char client_port[PORT_SIZE+1];
			memset(client_port, '\0', sizeof client_port);
			if (recv(fdaccept, client_port, sizeof client_port, 0) <= 0) perror("error receiving port\n");
			//printf("port: %s\n", client_port);
			edit_port(ip, client_port);	
                        bubbleSort(clients, 4);

			char buffer[HOSTNAME_SIZE*4];
			memset(buffer, '\0', sizeof buffer);
			/* Create list of logged in clients to send to the client trying to login */
			create_list(buffer);
			//printf("created list sending\n");	
			if (send(fdaccept, buffer, strlen(buffer), 0) != strlen(buffer)) perror("list not sent \n");
		
			
			FD_SET(fdaccept, &master_list);
                        if(fdaccept > head_socket) head_socket = fdaccept;
                    	char temp[2];
			memset(temp, '\0', sizeof temp);
			itoa(fdaccept, temp);
			//printf("Mapping socket descriptor %s to ip: %s\n", temp, ip);
			DictInsert(map, temp, ip);
			}
                    /* Read from existing clients */
                    else{
                        /* Initialize buffer to receieve response */
                        char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
                        memset(buffer, '\0', BUFFER_SIZE);

                        if(recv(sock_index, buffer, BUFFER_SIZE, 0) <= 0){
                            	/* Remove from watched list */
                            	FD_CLR(sock_index, &master_list);
				char temp[2];
				memset(temp, '\0', sizeof temp);
				itoa(sock_index, temp);
				const char* ip = DictSearch(map, temp);
				//printf("ip: %s is mapped to socket descriptor: %s\n", ip, temp);
			    	for (int client = 0; client < MAX_CLIENTS; ++client){
			    		if (clients[client] == NULL) continue;
					if (!strcmp(clients[client]->ip, ip)){
						//printf("removing ip: %s, %s\n", clients[client]->ip, ip);
						clients[client]->logged_in = 0;
					}
				}
                            	close(sock_index);
				DictDelete(map, temp);
			}
                        else {
				cse4589_print_and_log("[%s:SUCCESS]\n", "RELAYED");
				char* token = strtok(buffer, ":");
				char to[IP_SIZE];	
				strcpy(to, token);
				token = strtok(NULL, "\0");
				const char* from = lookup_ip(sock_index, map); 
				cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n", from, to, token);
				int send_socket = -1;
				for (int i = 0; i <= head_socket; ++i){
					if (!strcmp(lookup_ip(i, map), to)) send_socket = i;
				}
				fflush(stdout);fflush(stdin);
				char msg[BUFFER_SIZE+IP_SIZE];
	                        memset(msg, '\0', sizeof msg); 
				strcat(msg, from);
				strcat(msg, ":");
				strcat(msg, token);
				if(send(send_socket, msg, strlen(msg), 0) != strlen(msg)) perror("could not send to recipient\n");
				cse4589_print_and_log("[%s:END]\n", "RELAYED");
                        }

                        free(buffer);
                    }
                }
            }
        }
    }
  }

return 0;

}
