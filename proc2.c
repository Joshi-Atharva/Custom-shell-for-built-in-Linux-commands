#include <stdio.h>
#include <unistd.h>
int main() {
	printf("process 2 with pid = %d\n", getpid());
	return 0;
}
