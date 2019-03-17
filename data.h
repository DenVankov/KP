#ifndef DATA_H
#define DATA_H

#include <vector>
#include <string>

typedef enum {
    INIT, CLIENT_OFF,
    SEND_MESSAGE, SEND_TO_SUBS, SHOW_HISTORY
} RequestToken;

/*typedef struct {
    char *Client;
} NamesOfClients;
*/
typedef struct {
  RequestToken action;
  char clientName[256];
  std::vector<std::string> namesOfClients;
  pid_t client_proc_id;
  char text[256];
  long long status;
} Message ;



#endif
