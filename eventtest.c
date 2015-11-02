#include <sys/syscall.h>


int main(int argc, char **argv)
{
	printf("Before syscall\n");
	syscall (181);
	syscall (182);
	syscall (183);
	syscall (184);
	syscall (185);
	syscall (205);
	syscall (211);
	syscall (214);
	printf("After syscall\n"); 
	return 0;
}
