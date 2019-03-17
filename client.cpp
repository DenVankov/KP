#include <zmq.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/file.h>
#include <unistd.h>
#include <csignal>
#include <sys/types.h>
#include <pthread.h>
#include <iostream>
#include <sstream>
#include "data.h"

Message message;
void *server;

void menu() {
    printf("****************************************\n");
    printf("1 - Send message                        \n");
    printf("2 - Show history                        \n");
    printf("0 - exit                                \n");
    printf("****************************************\n");
}

void send_recv()
{


    message.action = SEND_TO_SUBS;

    zmq_msg_t request;
    zmq_msg_init_size(&request, sizeof(Message));
    memcpy(zmq_msg_data(&request), &message, sizeof(Message));
    zmq_msg_send(&request, server, 0);
    zmq_msg_close(&request);

    char ans[256];
    zmq_msg_init(&request);
    zmq_msg_recv(&request, server, 0);
    strcpy(ans, (char *) zmq_msg_data(&request));
    zmq_msg_close(&request);

    printf("%s",ans);
    std::cout << std::endl;
    //printf("\n1\n");

}

void client_off()
{


    message.action = CLIENT_OFF;

    zmq_msg_t request;
    zmq_msg_init_size(&request, sizeof(Message));
    memcpy(zmq_msg_data(&request), &message, sizeof(Message));
    zmq_msg_send(&request, server, 0);
    zmq_msg_close(&request);

    char ans[256];
    zmq_msg_init(&request);
    zmq_msg_recv(&request, server, 0);
    strcpy(ans, (char *) zmq_msg_data(&request));
    zmq_msg_close(&request);

    printf("%s\n",ans);
}


int main(int argc, char *argv[]) {


    std::signal(SIGUSR1,send_recv);
    std::signal(SIGHUP,client_off);

    int act = 0;
    char ans[256];

    void *context = zmq_ctx_new();
    server = zmq_socket(context, ZMQ_REQ);

    ans[0] = '\0';
    strcat(ans, "tcp://localhost:");
    if (argc == 2)
        strcat(ans, argv[1]);
    else
        strcat(ans, "4040");
    int rc = zmq_connect(server, ans);
    if (rc != 0) {
        perror("zmq_connect");
        zmq_close(server);
        zmq_ctx_destroy(context);
        exit(1);
    }

    printf("Enter client name:\n");
    scanf("%s", message.clientName);
    message.client_proc_id = getpid();
    std::cout << "client pid = " << message.client_proc_id << std::endl;
    message.action = INIT;

    zmq_msg_t clientReq;
    zmq_msg_init_size(&clientReq, sizeof(Message));
    memcpy(zmq_msg_data(&clientReq), &message, sizeof(Message));
    zmq_msg_send(&clientReq, server, ZMQ_DONTWAIT);
    zmq_msg_close(&clientReq);

    zmq_msg_t reply;
    zmq_msg_init(&reply);
    zmq_msg_recv(&reply, server, 0);
    strcpy(ans, (char *) zmq_msg_data(&reply));

    if (strcmp(ans, "OK") == 0) {
        printf("\nWelcome~\n");
    } else if (strcmp(ans, "ERROR") == 0) {
        printf("Sorry! Server returned error, try again\n");
        exit(1);
    } else {
        printf("\nWelcome~\n");
        printf("%s\n", ans);
    }

    zmq_msg_close(&reply);

    for (;;) {
        menu();
        if (scanf("%d", &act) == EOF) {
            act = 0;
        }


        if (act == 1) {
            message.action = SEND_MESSAGE;
            printf("Type users name <name name ...>:\n");
            std::string str;
            getchar();
            std::getline(std::cin, str);
//            std::cout << str << std::endl;
            std::istringstream iss(str);
            int count = 0;
            for(std::string s; iss >> s; ) {
                strcpy(message.names[count].Client, s.c_str());
                ++count;
            }

            message.sizeOfNames = count;
            printf("Type message:\n");
            std::getline(std::cin, str);
//            std::cout << str << std::endl;
            strcpy(message.text, str.c_str());
            zmq_msg_init_size(&clientReq, sizeof(Message));
            memcpy(zmq_msg_data(&clientReq), &message, sizeof(Message));

        } else if (act == 2) {
            message.action = SHOW_HISTORY;

            zmq_msg_init_size(&clientReq, sizeof(Message));
            memcpy(zmq_msg_data(&clientReq), &message, sizeof(Message));

        }  else if (act == 0) {
            message.action = CLIENT_OFF;
            zmq_msg_init_size(&clientReq, sizeof(Message));
            memcpy(zmq_msg_data(&clientReq), &message, sizeof(Message));

            printf("Sending...\n");
            zmq_msg_send(&clientReq, server, 0);
            zmq_msg_close(&clientReq);

            break;

        } else {
            printf("Try more...\n");
            continue;
        }

        printf("Answer:\n");
        zmq_msg_init_size(&clientReq, sizeof(Message));
        memcpy(zmq_msg_data(&clientReq), &message, sizeof(Message));
        zmq_msg_send(&clientReq, server, 0);
        zmq_msg_close(&clientReq);

        zmq_msg_init(&reply);
        zmq_msg_recv(&reply, server, 0);
        strcpy(ans, (char *) zmq_msg_data(&reply));

        if (strcmp(ans, "ERROR") == 0) {
            printf("Error occured\n");
        } else {
            printf("%s\n", ans);
        }

        zmq_msg_close(&reply);
    }

    zmq_close(server);
    zmq_ctx_destroy(context);
    return 0;
}
