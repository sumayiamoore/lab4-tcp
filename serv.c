#include <netinet/in.h> //structure for storing address information 
#include <stdio.h> 
#include <string.h>
#include <stdlib.h> 
#include <sys/socket.h> //for socket APIs 
#include <sys/types.h> 
#include "list.h"
#include <signal.h>
#include <unistd.h>

#define PORT 9001
#define ACK "ACK"

static list_t *mylist = NULL;
static int servSockD = -1;
static int clientSocket = -1;

void handle_sigint(int sig) {
  if (mylist != NULL) list_free(mylist);
  if (clientSocket >= 0) close(clientSocket);
  if (servSockD >= 0) close(servSockD);
  _exit(0);
}

int main(int argc, char const* argv[]) 
{ 
    int n, val, idx;

    signal(SIGINT, handle_sigint);

    // create server socket
    servSockD = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(servSockD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // string store data to recv/send to/from client 
    char buf[1024];
    char sbuf[1024];
    char* token;

    // define server address 
    struct sockaddr_in servAddr; 

    servAddr.sin_family = AF_INET; 
    servAddr.sin_port = htons(PORT); 
    servAddr.sin_addr.s_addr = INADDR_ANY; 

    // bind socket to the specified IP and port 
    bind(servSockD, (struct sockaddr*)&servAddr, 
         sizeof(servAddr)); 

    // listen for connections 
    listen(servSockD, 1); 

    // accept the client connection
    clientSocket = accept(servSockD, NULL, NULL); 

    mylist = list_alloc();  // create the list

    while(1){
        // recvs messages from client socket 
        n = recv(clientSocket, buf, sizeof(buf) - 1, 0);
        if (n <= 0) break;
        buf[n] = '\0';
        memset(sbuf, '\0', sizeof(sbuf));

        if(n > 0){
            token = strtok(buf, " ");

            if(strcmp(token,"exit") == 0){
                break;
            }
            else if(strcmp(token,"get_length") == 0){
                val = list_length(mylist);
                sprintf(sbuf,"%s%d", "Length = ", val);
            }
            else if(strcmp(token,"add_front") == 0){
                token = strtok(NULL, " ");  // get next token (value)
                val = atoi(token);
                sprintf(sbuf,"%s%d", ACK, val);
                list_add_to_front(mylist,val);
            }
            else if(strcmp(token,"remove_position") == 0){
                token = strtok(NULL, " ");
                idx = atoi(token);
                val = list_remove_at_index(mylist,idx);
                sprintf(sbuf,"%s%d", ACK, val);
            }
            else if(strcmp(token,"print") == 0){
                char *s = listToString(mylist);
                snprintf(sbuf, sizeof(sbuf), "%s", s);
                free(s);
            }
            else if(strcmp(token,"add_back") == 0){
                token = strtok(NULL, " ");
                list_add_to_back(mylist, atoi(token));
                sprintf(sbuf, "%s", ACK);
            }
            else if(strcmp(token,"add_position") == 0){
                char *t1 = strtok(NULL, " ");   // index
                char *t2 = strtok(NULL, " ");   // value
                list_add_at_index(mylist, atoi(t2), atoi(t1));
                sprintf(sbuf, "%s", ACK);
            }
            else if(strcmp(token,"remove_back") == 0){
                sprintf(sbuf, "%d", list_remove_from_back(mylist));
            }
            else if(strcmp(token,"remove_front") == 0){
                sprintf(sbuf, "%d", list_remove_from_front(mylist));
            }
            else if(strcmp(token,"get") == 0){
                token = strtok(NULL, " ");
                sprintf(sbuf, "%d", list_get_elem_at(mylist, atoi(token)));
            }
            else if(strcmp(token,"menu") == 0){
                sprintf(sbuf, "%s", ACK);
            }

            // send's messages to client socket 
            send(clientSocket, sbuf, sizeof(sbuf), 0);
        }
        memset(buf, '\0', 1024);
    }

    list_free(mylist);
    close(clientSocket);
    close(servSockD);
    return 0; 
}
