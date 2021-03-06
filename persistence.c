#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>

#include <persistence.h>


#define PORT_NUMBER 3661

int socketFile, newSocketFile;
int rc;
char setString[256];

void errorPersistence(const char *msg){
  perror(msg);
  exit(1);
}



void persistenceInitialize(){
  struct sockaddr_un serverAddress;

  // make socket file
  socketFile = socket(AF_UNIX, SOCK_STREAM, 0);
  if (socketFile < 0) {
    errorPersistence("ERROR opening socket");
  }

  // make server address
  bzero((char *) &serverAddress, sizeof(serverAddress));
  serverAddress.sun_family = AF_UNIX;
  strncpy(serverAddress.sun_path, "./data/sensors.sock", sizeof(serverAddress.sun_path) - 1);

  // bind to socket
  if( bind(socketFile, (struct sockaddr *) &serverAddress, sizeof(serverAddress)) < 0){
    errorPersistence("ERROR on binding");
  }

  // listen and bind once the client contacts
  if( listen(socketFile, 5) == -1 ){
    errorPersistence("ERROR on listen");
  }

  newSocketFile = accept(socketFile, NULL, NULL);
  if( newSocketFile < 0 ){
    errorPersistence("ERROR on accept");
  }
}

ssize_t persistSenseSet(struct SenseSet *set){
  sprintf(setString, "%lu|%f|%f|%d|%d|%d\n",
      set->timestamp, set->qOut, set->qDump,
      set->ppmOut, set->ppmIn, set->ppmRec );
  return write(newSocketFile, setString, strlen(setString));
}

void persistenceCleanup(){
  if( newSocketFile != 0 ){
    close(newSocketFile);
  }
  if( socketFile != 0 ){
    close(socketFile);
  }
  unlink("./data/sensors.sock");
}
