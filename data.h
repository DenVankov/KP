#ifndef DATA_H
#define DATA_H

#include <vector>
#include <string>

typedef enum {
    INIT, CLIENT_OFF,
    SEND_MESSAGE, SEND_TO_SUBS, SHOW_HISTORY
} RequestToken;

typedef struct {
    char Client[256];
} NamesOfClients;


typedef struct {

  RequestToken action;
  char clientName[256];
  NamesOfClients names[256];
  pid_t client_proc_id;
  char text[256];
  int sizeOfNames;
  long long status;
} Message ;



#endif
