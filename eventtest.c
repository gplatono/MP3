#include <sys/syscall.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
	int eventIDs[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
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
		{
			pid_t pid = fork();
			if(pid == 0)
				printf("%i\n", syscall (183, param));
			else printf("Child created...\n");
		}
		else if(input == 184)
			printf("%i\n", syscall (184, param));
		else if(input == 185)
		{	
			printf("result of doeventinfo: %i\n", syscall(185, 10, &eventIDs[0]));
			for(int i=0; i<10; i++)
				printf("%i ", eventIDs[i]);
			printf("\n");
		}	
		else if(input == 205)
		{
			uid_t UID;
			gid_t GID;
			printf("Enter UID:\n");
			scanf("%ld", &UID);
			printf("Enter GID:\n");
			scanf("%ld", &GID);
			printf("result of doeventchown: %i\n", syscall(205, param, UID, GID));
		}		
		else if(input == 211)
		{
			int UIDFlag;
			int GIDFlag;
			printf("Enter UIDFlag:\n");
			scanf("%ld", &UIDFlag);
			printf("Enter GIDFlag:\n");
			scanf("%ld", &GIDFlag);
			printf("result of doeventchmod: %i\n", syscall(211, param, UIDFlag, GIDFlag));
		}
		else if(input == 214)
		{
			uid_t UID;
			gid_t GID;
			int UIDFlag;
			int GIDFlag;
			printf("result of doeventstat: %i\n", syscall(214, param, &UID, &GID, &UIDFlag, &GIDFlag));
			printf("event stats: ID = %ld, UID = %ld, GID = %ld, UIDFlag = %i, GIDFlag = %i\n", UID, GID, UIDFlag, GIDFlag);
		}		
	}	
	return 0;
}
