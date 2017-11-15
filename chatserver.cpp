#include <iostream>
#include <iomanip>
#include <fstream>
#include <istream>
#include <string.h>
#include <unordered_map>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#define MAX_LINE 4096
using namespace std;

void login(int s, char username[], unordered_map<string, string> &userPasswordKey) {
	string userstring(username);
	auto search = userPasswordKey.find(userstring);
	char buf[496];
	if (search != userPasswordKey.end()) { // in map
		if (send(s, "E", sizeof("N"), 0) == -1) {
			perror("Send error\n");
			exit(1);
		}
		if (recv(s, buf, sizeof(buf), 0) == -1) {
      perror("Recieve error\n");
    }
    string bufPass(buf);
    if (userPasswordKey[userstring] != bufPass) { // incorrect password
	    while (userPasswordKey[userstring] != bufPass) {
		    if (send(s, "N", sizeof("N"), 0) == -1) {
          perror("Send error\n");
          exit(1);
        }
		    if (recv(s, buf, sizeof(buf), 0) == -1) {
          perror("Recieve error\n");
        }
        string bufPass(buf);
	    }	
		}
		if (send(s, "Y", sizeof("N"), 0) == -1) {
      perror("Send error\n");
      exit(1);
    }
	}
	else { // not in map
		if (send(s, "N", sizeof("N"), 0) == -1) {
      perror("Send error\n");
      exit(1);
    }
		if (recv(s, buf, sizeof(buf), 0) == -1) {
      perror("Recieve error\n");
      exit(1);
    }
    string bufPass(buf);
		userPasswordKey[userstring] = bufPass;	
	ofstream ofs;
	ofs.open("user_passwords.txt");
	ofs << userstring << " " << bufPass << "\n";
	}	
	
	
		
}



int main(int argc, char * argv[]) {
	string line;
	string user;
	string password;
	unordered_map<string, string> userPasswordKey;
	ifstream ifs("user_passwords.txt");
	if (ifs.is_open()) {
		while (ifs >> user) {
			ifs >> password;
			userPasswordKey.insert(make_pair(user, password));
		}
	}
	ifs.close();
  struct sockaddr_in sin, client_addr;
  char buf[MAX_LINE], outBuf[MAX_LINE];
  int addr_len, s, client_sock, opt = 1;
  struct timeval t1;
  socklen_t len;

  if (argc != 2){
    fprintf(stderr, "usage: chatserver port\n");
  }

  bzero((char*)&sin, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons((atoi(argv[1])));
  if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0){
    perror("ftp-server: socket\n");
    exit(1);
  }

  if ((setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(int))) < 0){
    perror("ftp-server: setsockopt\n");
    exit(1);
  }

  if ((bind(s, (struct sockaddr*)&sin, sizeof(sin))) < 0){
    perror("ftp-server: bind\n");
    exit(1);
  }
  
  if ((listen(s, 1)) < 0){
    perror("ftp-server: listen\n");
    exit(1);
  }
  
	while((client_sock = accept(s, (struct sockaddr *)&sin, &len)) > 0) {
		if (NUM_THREADS == 10) {
			cout << "Connection refused: max clients online.\n";
			continue;
		}
		cout << "Connection accepted.\n";
		pthread_t thread;
		struct thread_args args;
		NUM_THREADS++;

		if (pthread_create(&thread, NULL, clientinteraction, (void*)&args) < 0) {
			perror("Error creating thread");
			return 1;
		}
	}

	//s.close_socket(0);
}
