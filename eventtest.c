#include <sys/syscall.h>
#include <sys/types.h>

int main(int argc, char **argv)
{
	int eventIDs[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
	int input = 0;
	int param;
	/*	while(input != -1)
	{
		printf("Enter syscall number and  param:\n");
		scanf("%i %i", &input, &param);
		// System call to create a new event.
		if(input == 181)
			syscall (181);
		// System call to destroy an event.
		else if(input == 182)
			printf("%i\n", syscall (182, param));
		// System call to block a process until the event is signaled.
		else if(input == 183)
		{
			pid_t pid = fork();
			if(pid == 0) 
			{
				printf("%i\n", syscall (183, param));
				return 0;
			}
			else printf("Child created...\n");
		}
		// System call to unblock all waiting processes.
		else if(input == 184)
			printf("%i\n", syscall (184, param));
		// System call to fill in the array of integers.
		else if(input == 185)
		{	
			printf("result of doeventinfo: %i\n", syscall(185, 10, &eventIDs[0]));
			for(int i=0; i<10; i++)
				printf("%i ", eventIDs[i]);
			printf("\n");
		}	
		// System call to change the UID and GID of the event to the specified values.
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
		// System call to change the User Signal Enable Bit to UIDFlag and the Group 
		// Signal Enable Bit to GIDFlag.
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
		// System call to place the UID, GID, User Signal Enable Bit, and Group Signal Enable Bit 
		// into the memory pointed to by UID, GID, UIDFlag, and GIDFlag, respectively.
		else if(input == 214)
		{
			uid_t UID;
			gid_t GID;
			int UIDFlag;
			int GIDFlag;
			printf("result of doeventstat: %i\n", syscall(214, param, &UID, &GID, &UIDFlag, &GIDFlag));
			printf("event stats: ID = %i, UID = %i, GID = %i, UIDFlag = %i, GIDFlag = %i\n", param, UID, GID, UIDFlag, GIDFlag);
		}		
	}	*/
	return 0;
}
