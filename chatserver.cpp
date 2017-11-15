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

/*

	while((client_sock = accept(s.get_s(), (struct sockaddr *)&s.s_in, &s_inlen)) > 0) {
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
*/
}
