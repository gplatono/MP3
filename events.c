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

SYSCALL_DEFINE1(sys_doeventopen)
{
	printk("Test");
	return 0;
}

SYSCALL_DEFINE1(sys_doeventclose, int, eventID)
{
	return 0;
}

SYSCALL_DEFINE1(sys_doeventwait, int, eventID)
{
	return 0;
}

SYSCALL_DEFINE1(sys_doeventsig, int, eventID)
{
	return 0;
}

SYSCALL_DEFINE1(sys_doeventinfo, int, num, int *, eventIDs)
{
	return 0;
}

SYSCALL_DEFINE1(sys_doeventchown, int, eventID, uid_t, UID, gid_t, GID)
{
	return 0;
}

SYSCALL_DEFINE1(sys_doeventchmod, int, eventID, int, UIDFlag, int, GIDFlag)
{
	return 0;
}

SYSCALL_DEFINE1(sys_doeventstat, int, eventID, uid_t *, UID, gid_t *, GID, int *, UIDFlag, int *, GIDFlag)
{
	return 0;
}
