#include <iostream>
#include <iomanip>
#include <pthread.h>
using namespace std;

int main(int argc, char * argv[]) {
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
}
