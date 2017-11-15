#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <string.h>
#define EXIT 0
#define MAX_LINE 4096
using namespace std;

void private_message(){

}

void broadcast(){
  string message;
  
}

void login(int s, char* username){
  if (send(s, username, strlen(username), 0) == -1){
    perror("Send error\n");
    exit(1);
  }
  char buf[MAX_LINE];
  if (recv(s, buf, sizeof(buf), 0) == -1){
    perror("Receive error\n");
  }
  //existing user
  if (strncmp("N", buf, 1)){
    printf("Welcome back %s. Enter password >> ", username);
  }
  //new user 
  else {
    printf("New User? Create password >> ");
  }
  string password;
  cin >> password;
  if (send(s, password.c_str(), strlen(password.c_str()), 0) == -1){
    perror("Send error\n");
    exit(1);
  }
  bzero(buf, MAX_LINE);
  if (recv(s, buf, sizeof(buf), 0) == -1){
    perror("Receive error\n");
  }
  //while password is incorrect
  while (strncmp("Y", buf, 1)){
    printf("Incorrect password. Try again >> ");
    cin >> password;
    if (send(s, password.c_str(), strlen(password.c_str()), 0) == -1){
      perror("Send error\n");
      exit(1);
    }
    bzero(buf, MAX_LINE);
    if (recv(s, buf, sizeof(buf), 0) == -1){
      perror("Receive error\n");
    }
  }
  printf("Welcome %s. Login successful.", username);
}

int main(int argc, char * argv[]) {
  char *host, *port, *username;
  struct hostent *hp;
  struct sockaddr_in sin;
  int s;
  string op;
  
  if (argc == 4){
    host = argv[1];
    port = argv[2];
    username = argv[3];
  } else {
    fprintf(stderr, "usage: ftp-client host port\n");
    exit(1);
  }

  hp = gethostbyname(host);
  if (!hp){
    fprintf(stderr, "ftp-client: unknown host: %s\n", host);
    exit(1);
  }
  bzero((char*)&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  bcopy(hp->h_addr, (char*)&sin.sin_addr, hp->h_length);
  sin.sin_port = htons(atoi(port));

  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0){
    perror("ftp-client: socket\n");
    exit(1);
  }

  if (connect(s, (struct sockaddr*)&sin, sizeof(sin)) < 0){
    perror("ftp-client: connect\n");
    close(s);
    exit(1);
  }

	while(!EXIT) {
		login(s, username);

		/*pthread_t thread;
		int rc = pthread_create(&thread, NULL, handle_messages, NULL);
    */
		while(1) {
			/*if (rc) {
				cout << "Error: unable to create thread.\n";
				exit(-1);
			}*/

			cin >> op;
			
			if (op == "P") {
				private_message();
			} else if (op == "B") {
				broadcast();
			} else if (op == "E") {
				exit(-1);
			} else {
				printf("Invalid entry.\nEnter P for private, B for broadcast, or E to exit\n");
			}
		}
		
		close(s);
	}
	return 0;
}
