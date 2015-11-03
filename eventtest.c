#include <sys/syscall.h>


int main(int argc, char **argv)
{
	int input = 0;
	int param;
	while(input != -1)
	{
		printf("Enter syscall number and  param:\n");
		scanf("%i %i", &input, &param);
		if(input == 181)
			syscall (181);
		else if(input == 182)
			printf("%i\n", syscall (182, param));
		else if(input == 183)
			printf("%i\n", syscall (183, param));
		else if(input == 184)
			printf("%i\n", syscall (184, param));
//		syscall (184);
//		syscall (185);
//		syscall (205);
//		syscall (211);
//		syscall (214);
	}	
	return 0;
}
