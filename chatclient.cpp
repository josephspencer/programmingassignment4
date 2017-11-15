#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <string.h>
#include <string>
#define MAX_LINE 4096
using namespace std;

int EXIT = 0;
int ACTIVE = 1;
int COUNT = 0;
string LAST_COMMAND = "";
string CURR_COMMAND = "";

void private_message(int s){
  if (send(s, "P", strlen("P"), 0) == -1){
    perror("Send error\n");
    exit(1);
  }
  /*char buf[MAX_LINE];
  if (recv(s, buf, sizeof(buf), 0) == -1){
    perror("Receive error\n");
    exit(1);
  }
  string output(buf);
  cout << output.substr(1);
  LAST_COMMAND = output.substr(1);*/
  while (strncmp("Users", LAST_COMMAND.c_str(), 5)){

  }
  string name;
  cin >> name;
  int c = COUNT;
  if (send(s, name.c_str(), strlen(name.c_str()), 0) == -1){
    perror("Send error\n");
    exit(1);
  }
  /*bzero(buf, MAX_LINE);
  if (recv(s, buf, sizeof(buf), 0) == -1){
    perror("Receive error\n");
    exit(1);
  }*/
  //while user not online
  while (strncmp("User ", LAST_COMMAND.c_str(), 5)){
    if (c < COUNT && strncmp("User ", LAST_COMMAND.c_str(), 5)){
      c++;
    /*string prompt(buf);
    cout << prompt.substr(1);
    LAST_COMMAND = prompt.substr(1);*/
      name.clear();
      cin >> name;
      if (send(s, name.c_str(), strlen(name.c_str()), 0) == -1){
        perror("Send error\n");
        exit(1); 
      }
    }
    /*bzero(buf, MAX_LINE);
    if (recv(s, buf, sizeof(buf), 0) == -1){
      perror("Receive error\n");
      exit(1);
    }*/
  }
  //send confirmation that confirmation was received
  if (send(s, "Y", strlen("Y"), 0) == -1){
    perror("Send error\n");
    exit(1);
  }
  /*bzero(buf, MAX_LINE);
  if (recv(s, buf, sizeof(buf), 0) == -1){
    perror("Receive error\n");
    exit(1);
  }
  string message(buf);
  cout << message.substr(1);
  LAST_COMMAND = message.substr(1);*/
  while (strncmp("Enter", LAST_COMMAND.c_str(), 5)){

  }
  string m;
  cin >> m;
  if (send(s, m.c_str(), strlen(m.c_str()), 0) == -1){
    perror("Send error\n");
    exit(1);
  }
}

void broadcast(int s){
  char buf[MAX_LINE];
  if (send(s, "B", strlen("B"), 0) == -1){
    perror("Send error\n");
    exit(1);
  }
  /*if (recv(s, buf, sizeof(buf), 0) == -1){
    perror("Receive error\n");
    exit(1);
  }
  string message(buf);
  cout << message.substr(1);
  LAST_COMMAND = message.substr(1);*/
  string m;
  cin >> m;
  if (send(s, m.c_str(), strlen(m.c_str()), 0) == -1){
    perror("Send error\n");
    exit(1);
  }
}

void *handle_messages(void *s){
  int new_s = *(int*)s;
  while (ACTIVE){
    char buf[MAX_LINE];
    bzero(buf, MAX_LINE);
    if (recv(new_s, buf, sizeof(buf), 0) == -1){
      perror("Receive error\n");
      exit(1);
    }
    string output(buf);
    if (!strncmp(buf, "D", 1)){
      cout << "Incoming message\n" << output.substr(1) << endl << LAST_COMMAND << std::flush << endl;
      //data message
      //display?
    } else {
      cout << output.substr(1) << std::flush;
      LAST_COMMAND.clear();
      LAST_COMMAND = output.substr(1);
      COUNT++;
      //command message
      //thread join?
    }
  }
  return 0;
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
  if (strncmp("N", buf, 1)){
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
  }
  printf("Welcome %s. Login successful.\n", username);
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

		pthread_t thread;
		int rc = pthread_create(&thread, NULL, handle_messages, (void*)&s);
    
		while(1) {
			if (rc) {
				cout << "Error: unable to create thread.\n";
				exit(-1);
			}
      op.clear();
			cin >> op;
			
			if (op == "P") {
				private_message(s);
			} else if (op == "B") {
				broadcast(s);
			} else if (op == "E") {
        ACTIVE = 0;
        
        if (send(s, "E", strlen("E"), 0) == -1){
          perror("Send error\n");
          exit(1);
        }
				exit(-1);
			} else {
				printf("Invalid entry.\nEnter P for private, B for broadcast, or E to exit\n");
			}
		}
		
		close(s);
	}
	return 0;
}
