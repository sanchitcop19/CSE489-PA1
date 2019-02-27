/**
 * @client
 * @author  Swetank Kumar Saha <swetankk@buffalo.edu>
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
 * This file contains the client.
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <strings.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#define TRUE 1
#define MSG_SIZE 256
#define BUFFER_SIZE 256
#define STDIN 0
#define CMD_SIZE 100
#define HOSTNAME_SIZE 100
#define IP_SIZE 16
#define MAX_CLIENTS 4
#define PORT_SIZE 5


void get_logged_in(char* info);


int connect_to_host(char *server_ip, int server_port, char* self_port);

 /**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
int g_server_fd = -1;

int client_main(int argc, char **argv)

{
	if(argc != 3) {
		printf("Usage:%s [c/s] [port]\n", argv[0]);
		exit(-1);
	}

    /* ---------------------------------------------------------------------------- */
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
		/*printf("sock_index: %i\n", sock_index);*/
                if(FD_ISSET(sock_index, &watch_list)){
                    /* Check if new command on STDIN */
                    if (sock_index == STDIN){
                    	char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);

                    	memset(cmd, '\0', CMD_SIZE);
						if(fgets(cmd, CMD_SIZE-1, stdin) == NULL) 
							exit(-1);

						/*printf("\nI got: %s\n", cmd);*/
						char* delimiters = " \n";	
						char* command_str = strtok(cmd, delimiters);
						printf("command_str: %s\n", command_str);
						if (!strcmp(command_str, "AUTHOR")){
													
							printf("[%s:SUCCESS]\n", command_str);
							printf("I, %s, have read and understood the course academic integrity policy.\n","sanchitb");		
							printf("[%s:END]\n", command_str);
						}
						else if (!strcmp(command_str, "IP")){
							
							printf("[%s:SUCCESS]\n", command_str);
							printf("[%s:END]\n", command_str);
						}
						else if (!strcmp(command_str, "PORT")){
							printf("[%s:SUCCESS]\n", command_str);
							printf("[%s:END]\n", command_str);
						}
						else if (!strcmp(command_str, "LIST")){
							printf("[%s:SUCCESS]\n", command_str);

							printf("[%s:END]\n", command_str);
						
						}
						else if (!strcmp(command_str, "LOGIN")){
							printf("[%s:SUCCESS]\n", command_str);
							command_str = strtok(NULL, delimiters);
							char* ip = command_str;
							command_str = strtok(NULL, delimiters);
							int port = atoi(command_str);
							
							int server = connect_to_host(ip, port, argv[2]);

    							char *list = (char*) malloc(sizeof(char)*BUFFER_SIZE);
   							memset(list, '\0', BUFFER_SIZE);
							if (send(server, argv[2], PORT_SIZE, 0) <= 0) perror("could not send port\n");

							if (recv(server, list, BUFFER_SIZE, 0) <= 0) perror("could not retreive list\n");
							printf("list: %s\n", list);	
							get_logged_in(list);
    							
							/* Register the listening socket */
    							FD_SET(server, &master_list);
							head_socket = server;
							printf("[%s:END]\n", command_str);
						}
						else if (!strcmp(command_str, "LOGOUT")){
							printf("[%s:SUCCESS]\n", command_str);
			                                FD_CLR(g_server_fd, &master_list);
			                             	FD_CLR(g_server_fd, &watch_list);
							close(g_server_fd);
							for (int free = 0; free < MAX_CLIENTS; ++free) clients[free] = NULL;
							printf("[%s:END]\n", command_str);
						}		
								
						free(cmd);
                    }
                    else{
                        /* Initialize buffer to receieve response */
                        char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
                        memset(buffer, '\0', BUFFER_SIZE);
			
                        if(recv(sock_index, buffer, BUFFER_SIZE, 0) <= 0){
                            close(sock_index);

                            /* Remove from watched list */
                            FD_CLR(sock_index, &master_list);
                        }
                        else {
				if(send(sock_index, buffer, strlen(buffer), 0) == strlen(buffer)){
					printf("Done!\n");
				}
				fflush(stdout);
                        }

                        free(buffer);
                    }
                }
            }
        }
    }

}

void add_helper(struct client* client, char host[], char ip[], int port ){

        strcpy(client->ip, ip);
        client->port = port;
        strcpy(client->hostname, host);
}



int connect_to_host(char *server_ip, int server_port, char* self_port)
{
    int fdsocket, len;
    struct sockaddr_in remote_server_addr;

    fdsocket = socket(AF_INET, SOCK_STREAM, 0);
    printf("%i\n", fdsocket); 
    if(fdsocket < 0)
       perror("Failed to create socket");
    g_server_fd = fdsocket;
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
		++i;
		info = strtok(NULL, " ");
	}
}
