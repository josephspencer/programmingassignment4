#include <iostream>
#include <iostream>
#include <pthread.h>
using namespace std;

int main(int argc, char * argv[]) {
	char[] username = argv[3];
	while(!EXIT) {
		login(username);

		pthread_t thread;
		int rc = pthread_create(&thread, NULL, handle_messages, NULL);

		while(1) {
			if (rc) {
				cout << "Error: unable to create thread.\n";
				exit(-1);
			}

			cin >> op;
			
			if (op == "P") {
				private_message();
			} else if (op == "B") {
				broadcast();
			} else if (op == "E") {
				exit(-1);
			} else {
				cout << "Invalid entry\n" << OPTIONS;
			}
		}
		
		//s.close_socket();
	}
	return 0
}
