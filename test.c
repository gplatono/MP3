#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	int eventIDs[10] = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
	
	int ret_val;

	printf("Calling doeventclose and doeventsig on events with IDs 1 and 0 respectively...\n");

	/* Try to destroy an event */
	ret_val = syscall(182, 1);
	if(ret_val == -1)
		printf("doeventclose returned %i since there are no events yet\n", ret_val);
	else printf("Unknown error in doeventclose\n");

	printf("\n\n");

	/* Try to signal an event */
	ret_val = syscall(184, 0);
	if(ret_val == -1)
		printf("doeventsig returned %i since there are no events yet\n", ret_val);
	else printf("Unknown error in doeventsig\n");

	printf("\n\n");

	/* Create a bunch of events */	
	for(int i = 0; i < 100; i++)
	{
		ret_val = syscall(181);
		if(ret_val >= 0)
			printf("The event with ID = %i was created\n", ret_val);
		else printf("Event creation failed\n");
	}

	printf("\n\n");

	/* Output stats of event 0 */
	uid_t UID = -1;
	gid_t GID = -1;
	int UIDFlag = -1;
	int GIDFlag = -1;
	printf("Calling doeventstat on event 0. Result: %i\n", syscall(214, 0, &UID, &GID, &UIDFlag, &GIDFlag));
	printf("Event stats: ID = 0, UID = %i, GID = %i, UIDFlag = %i, GIDFlag = %i\n", UID, GID, UIDFlag, GIDFlag);
			
	printf("\n\n");
	
	/* Try to destroy an event with ID 0 again*/
	ret_val = syscall(182, 0);
	if(ret_val >= -1)
		printf("doeventclose returned %i."
		" Event closing successful, since event with ID 0 existed\n", ret_val);
	else printf("Error in doeventclose\n");

	printf("\n\n");

	/* Try to get stats of event 0 */
	printf("Calling doeventstat on event ID 0: %i\n", syscall(214, 0, &UID, &GID, &UIDFlag, &GIDFlag));

	printf("\n\n");

	/* Try to signal an event again*/
	ret_val = syscall(184, 99);
	if(ret_val >= -1)
		printf("doeventsig returned %i. Signaling is successful,"
 		"since event with ID 99 existed and 0 processes are waiting on this event\n", ret_val);
	else printf("Error in doeventsig\n");

	printf("\n\n");

	/* Testing signals. 
	*  Creating 4 children and sending them to sleep on event 99 and then signaling them twice 
	*/
	pid_t pid = fork();
	if(pid == 0)
	{		
		fork();
		fork();
		printf("Sending process %i to sleep on an event 99\n", getpid());
		ret_val = syscall(183, 99);
		if(ret_val != -1)
			printf("Child has been awakened\n");
		else printf("Error in doeventwait\n");		
		return 0;
	}
	else
	{
		sleep(1);
		printf("Trying to wake up processes waiting on event 99. Sending signal...\n");

		ret_val = syscall(184, 99);
		if(ret_val != -1)
			printf("Doeventsig successful. "
			"The number of processes that were waiting on event 99: %i\n", ret_val);
		else printf("Unknown error in doeventsig");
		
		
		printf("Trying to wake up processes again. Sending signal...\n");

		ret_val = syscall(184, 99);
		if(ret_val > 0)
			printf("There are %i process(es) that have not been awakened yet\n", ret_val);
		else if(ret_val == 0)
			printf("Doeventsig returned 0 since there are no processes waiting on event 98 anymore\n");
		else printf("Error in doeventsig\n");		
	}

	sleep(1);
	printf("\n\n");


	/* Trying to close event with several processes waiting on it */
	pid = fork();
	if(pid == 0)
	{		
		fork();
		fork();
		printf("Sending process %i to sleep on an event 99\n", getpid());

		ret_val = syscall(183, 99);
		if(ret_val != -1)
			printf("Child has been awakened\n");
		else printf("Error in doeventwait\n");
		return 0;		
	}
	else
	{
		sleep(1);
		printf("Trying to close event 99...\n");

		ret_val = syscall(182, 99);
		if(ret_val != -1)
			printf("Doeventclose successful."
			" The number of processes waited on event 99: %i\n", ret_val);
		else printf("Unknown error in doeventclose\n");
	}

	sleep(1);
	printf("\n\n");

	/* Testing access control mechanism */	

	printf("Change UID, GID of event 98 to 100 and UID and GID of current process to 200\n");
	ret_val = syscall(205, 98, 100, 100);	
	seteuid(200);
	setegid(200);

	/* try to change UIDFlag and GIDFlag */

	printf("Calling Doeventchmod on event 98...\n");
	ret_val = syscall(211, 98, 0, 0);
	if(ret_val == -1) 
		printf("Doeventchmod returned -1 because current process has effective UID %i\n", geteuid());
	else printf("Unknown error in doeventchmod\n");

	printf("\n\n");	

	printf("Changing UID, GID of event 98 to 200 and trying again:\n");
	ret_val = syscall(205, 98, 200, 200);	

	
	/* try to change UIDFlag and GIDFlag again */

	ret_val = syscall(211, 98, 0, 0);
	printf("Calling doeventstat on event ID 98: %i\n", syscall(214, 98, &UID, &GID, &UIDFlag, &GIDFlag));
	printf("Event stats: ID = 98, UID = %i, GID = %i, UIDFlag = %i, GIDFlag = %i\n", UID, GID, UIDFlag, GIDFlag);

	
	for(int i=0; i<100; i++)
	{
		syscall(182, i);
	}
	return 0;
}
