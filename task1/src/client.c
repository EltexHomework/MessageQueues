#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <mqueue.h>
#include <fcntl.h>

#define run_client() \
  do {if (STANDARD == 0) {client_sysv();} \
    else {client_posix();} } while (0)

#define MESSAGE_LEN 16
#define SERVER_QUEUE "/server"
#define CLIENT_QUEUE "/client"
#define MAX_MESSAGES 10
#define MESSAGE_BUFFER_SIZE MESSAGE_LEN + 10

struct msgbuf {
  long mtype;
  char mtext[MESSAGE_LEN];
}; 

struct msgbuf* create_message(long type, char* text);

void client_sysv();

void client_posix();

int main(void) {
  run_client();
  return 0;
}

struct msgbuf* create_message(long type, char* text) {
  struct msgbuf* message = (struct msgbuf*) malloc(sizeof(struct msgbuf));
  
  message->mtype = type;
  strncpy(message->mtext, text, MESSAGE_LEN - 1);
  message->mtext[MESSAGE_LEN - 1] = '\0';
  
  return message;
}

void client_sysv() {
  key_t key;
  int msqid;

  if ((key = ftok("./server", 1)) == -1) {
    perror("ftok error"); 
    exit(EXIT_FAILURE);
  }
  
  if ((msqid = msgget(key, 0666)) == -1) {
    perror("msgget error");
    exit(EXIT_FAILURE);
  }

  struct msgbuf* send_message = create_message(3, "Hello!"); 
  struct msgbuf* receive_message = (struct msgbuf*) malloc(sizeof(struct msgbuf));
     
  if (msgrcv(msqid, receive_message, sizeof(receive_message->mtext), 2, 0) == -1) {
    perror("msgrcv error");
    exit(EXIT_FAILURE);
  }

  if (msgsnd(msqid, send_message, sizeof(send_message->mtext), 0) == -1) {
    perror("msgsnd error");
    exit(EXIT_FAILURE);
  }
  
  printf("Data received (SysV): %s\n", receive_message->mtext);

  free(send_message);
  free(receive_message);

}

void client_posix() {
  mqd_t queue0, queue1;
  char* message = "Hello!";
  char buffer[MESSAGE_LEN];

  if ((queue0 = mq_open(SERVER_QUEUE, O_RDONLY)) == -1) {
    perror("mq_open error");
    exit(EXIT_FAILURE);
  }
  
  if (mq_receive(queue0, buffer, MESSAGE_LEN, NULL) == -1) {
    perror("mq_receive error");
    exit(EXIT_FAILURE);
  }
  
  printf("Message read (POSIX): %s\n", buffer);

  if ((queue1 = mq_open(CLIENT_QUEUE, O_WRONLY)) == -1) {
    perror("mq_open error");
    exit(EXIT_FAILURE);
  }
  
  if (mq_send(queue1, message, strlen(message), 3) == -1) {
    perror("mq_send error");
    exit(EXIT_FAILURE);
  }

  mq_close(queue0);
  mq_unlink(SERVER_QUEUE);
  mq_close(queue1);
  mq_unlink(CLIENT_QUEUE);
}
