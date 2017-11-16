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

unordered_map<string, string> userPasswordKey;
unordered_map<string, int> online_users;
int NUM_THREADS = 0;

string login(int s) {
  char username[MAX_LINE];
  if (recv(s, username, sizeof(username), 0) == -1){
    perror("Receive error\n");
    exit(1);
  } 
	string userstring(username);
	auto search = userPasswordKey.find(userstring);
	char buf[MAX_LINE];
  //if user is in map and thus is an existing user
	if (search != userPasswordKey.end()) {
		if (send(s, "E", sizeof("N"), 0) == -1) {
			perror("Send error\n");
			exit(1);
		}
    //receive password
		if (recv(s, buf, sizeof(buf), 0) == -1) {
      perror("Receive error\n");
      exit(1);
    }
    string bufPass(buf);
    //if incorrect password, continue to prompt user for password
    if (strcmp(userPasswordKey[userstring].c_str(), bufPass.c_str())) {
	    while (strcmp(userPasswordKey[userstring].c_str(), bufPass.c_str())) {
		    if (send(s, "N", sizeof("N"), 0) == -1) {
          perror("Send error\n");
          exit(1);
        }
        bzero(buf, MAX_LINE);
		    if (recv(s, buf, sizeof(buf), 0) == -1) {
          perror("Receive error\n");
          exit(1);
        }
        string temp(buf);
        bufPass = temp;
	    }	
		}
    //send confirmation that password was correct
		if (send(s, "Y", sizeof("N"), 0) == -1) {
      perror("Send error\n");
      exit(1);
    }
	} else {
    //user not in map, so new user
		if (send(s, "N", strlen("N"), 0) == -1) {
      perror("Send error\n");
      exit(1);
    }
    bzero(buf, MAX_LINE);
    //receive password
		if (recv(s, buf, sizeof(buf), 0) == -1) {
      perror("Receive error\n");
      exit(1);
    }
    string bufPass(buf);
    //add username and password to map
		userPasswordKey[userstring] = bufPass;
    //write username and password to file  
	  ofstream ofs;
	  ofs.open("user_passwords.txt", std::ios::app);
	  ofs << userstring << " " << bufPass << "\n";
    ofs.close();
	}
  //add username and socket descriptor to online users map
  online_users[userstring] = s;
  return userstring;
}

void broadcast_message(string username, int s){
  string m = "CEnter Message: ";
  if (send(s, m.c_str(), strlen(m.c_str()), 0) == -1){
    perror("Send error\n");
    exit(1);
  }
  char buf[MAX_LINE];
  //receive message
  if (recv(s, buf, sizeof(buf), 0) == -1){
    perror("Send error\n");
    exit(1);
  }
  string message(buf);
  message = "D####\tMessage received from " + username + ": " + message + "\t####";
  //send message to all online users other than the sending user
  for (auto user : online_users){
    if (user.first != username){
      if (send(user.second, message.c_str(), strlen(message.c_str()), 0) == -1){
        perror("Send error\n");
        exit(1);
      }
    }
  }
  if (send(s, "DMessage sent\n", strlen("DMessage sent\n"), 0) == -1){
    perror("Send error\n");
    exit(1);
  }
}

void private_message(string username, int s){
  string m = "CEnter message: ";
  string n = "CUsers online:\n";
  //add online users to string to send to sending user
  for (auto u : online_users){
    if (u.first != username)
      n = n + u.first + "\n";
  }
  n += "Enter user: ";
  //send list of users and prompt for user
  if (send(s, n.c_str(), strlen(n.c_str()), 0) == -1){
    perror("Send error\n");
    exit(1);
  }
  char buf[MAX_LINE];
  bzero(buf, MAX_LINE);
  //receive user
  if (recv(s, buf, sizeof(buf), 0) == -1){
    perror("Receive error\n");
    exit(1);
  }
  string user(buf);
  //continue to prompt for user if user not in online users map
  while (online_users.find(user) == online_users.end()){
    if (send(s, n.c_str(), strlen(n.c_str()), 0) == -1){
      perror("Send error\n");
      exit(1);
    }
    bzero(buf, MAX_LINE);
    if (recv(s, buf, sizeof(buf), 0) == -1){
      perror("Receive error\n");
      exit(1);
    }
    string name(buf);
    user = name;
  }
  //send confirmation that user is correct
  if (send(s, "CUser is online\n", strlen("CUser is online\n"), 0) == -1){
    perror("Send error\n");
    exit(1);
  }
  bzero(buf, MAX_LINE);
  //receive confirmation of confirmation
  if (recv(s, buf, sizeof(buf), 0) == -1){
    perror("Receive error\n");
    exit(1);
  }
  string in(buf);
  //send message prompt
  if (send(s, m.c_str(), strlen(m.c_str()), 0) == -1){
    perror("Send error\n");
    exit(1);
  }
  bzero(buf, MAX_LINE);
  //receive message
  if (recv(s, buf, sizeof(buf), 0) == -1){
    perror("Receive error\n");
    exit(1);
  }
  string message(buf);
  message = "D####\tMessage received from " + username + ": " + message + "\t####";
  //send message to specified user
  if (send(online_users[user], message.c_str(), strlen(message.c_str()), 0) == -1){
    perror("Send error\n");
    exit(1);
  }
  if (send(s, "DMessage sent\n", strlen("DMessage sent\n"), 0) == -1){
    perror("Send error\n");
    exit(1);
  }
}

void *clientinteraction(void *s){
  int new_s = *(int*)s;
  string user = login(new_s);
  while (1){
    string op = "CEnter P for private conversation\nEnter B for message broadcasting\nEnter E for Exit\n";
    //prompt user for operation
    if (send(new_s, op.c_str(), strlen(op.c_str()), 0) == -1){
      perror("Send error\n");
      exit(1);
    }
    char buf[MAX_LINE];
    if (recv(new_s, buf, sizeof(buf), 0) == -1){
      perror("Receive error\n");
      exit(1);
    }
    if (!strncmp("E", buf, 1)){
      cout << "EXIT" << endl;
      NUM_THREADS--;
      close(new_s);
      online_users.erase(user);
      pthread_exit(NULL);
    } else if (!strncmp("B", buf, 1)){
      broadcast_message(user, new_s);
    } else {
      private_message(user, new_s);
    }
  }
}


int main(int argc, char * argv[]) {
	string user;
	string password;
  //read in user names and password from file at beginning of program execution and add to map
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
		NUM_THREADS++;

		if (pthread_create(&thread, NULL, clientinteraction, (void*)&client_sock) < 0) {
			perror("Error creating thread");
			return 1;
		}
	}

	close(s);
}
