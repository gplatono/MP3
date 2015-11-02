#include "linux/kernel.h"
#include "linux/events.h"

struct event_struct *events;
int nextID;

int doeventinit()
{
	nextID = 0;
	events = NULL;
	return 0;
}

asmlinkage long sys_doeventopen(void)
{
	return 0;
}

asmlinkage long sys_doeventclose(int eventID)
{
	return 0;
}

asmlinkage long sys_doeventwait(int eventID)
{
	return 0;
}

asmlinkage long sys_doeventsig(int eventID)
{
	return 0;
}

asmlinkage long sys_doeventinfo(int num, int *eventIDs)
{
	return 0;
}

asmlinkage long sys_doeventchown(int eventID, uid_t UID, gid_t GID)
{
	return 0;
}

asmlinkage long sys_doeventchmod(int eventID, int UIDFlag, int GIDFlag)
{
	return 0;
}

asmlinkage long sys_doeventstat(int eventID, uid_t *UID, gid_t *GID, int *UIDFlag, int *GIDFlag)
{
	return 0;
}
