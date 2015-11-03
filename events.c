#include <linux/kernel.h>
#include <linux/events.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/syscalls.h>

/* Maximum number of eventIDs available. If nextID == MAX_EVENTS, it's impossible to create events anymore */
#define MAX_EVENTS 1000000

/* Roles defining operations associated with an event. 
 * ROLE_WAITSIG - waiting and signaling on an event
 * ROLE_CLOSE - closing an event
 * ROLE_MODIFY - modification of UID and GID of an event 
 */
#define ROLE_WAITSIG 1
#define ROLE_CLOSE 2
#define ROLE_MODIFY 3


static LIST_HEAD(events);
int nextID;
static DEFINE_SPINLOCK(event_lock);

int doeventinit()
{
	nextID = 0;	
	return 0;
}

struct event_struct* get_event_by_id(int eventID)
{
	struct event_struct *ret_val;
	list_for_each_entry(ret_val, &events, evtlist)
	{
		if(ret_val->eventID == eventID)
			return ret_val;	
	}
	return NULL;
}

int check_privileges(struct event_struct* event, int roleflag)
{
	uid_t EUID = sys_geteuid();//syscall(107);//geteuid();
	gid_t EGID = sys_getegid();//syscall(108);//getegid();
	
	/* Root can always do everything */ 
	if(EUID == 0)
		return 1;

	if(roleflag == ROLE_WAITSIG || roleflag == ROLE_CLOSE)
	{
		if((EUID == event->UID && event->UIDFlag == 1) || (EUID != event->UID && EGID == event->GID && event->GIDFlag == 1))
			return 1;
	}
	else if(roleflag == ROLE_MODIFY)
	{
		if(EUID == event->UID)
			return 1;
	}
	return 0;
}

asmlinkage long sys_doeventopen(void)
{	
	spin_lock(&event_lock);
	if(nextID == MAX_EVENTS)
		return -1;	//Too many events
	struct event_struct *tmp = (struct event_struct *)kmalloc(sizeof(struct event_struct), GFP_KERNEL);
	if(tmp == NULL)
		return -1;	//Not enough memory
	INIT_LIST_HEAD(&tmp->evtlist);
	tmp->eventID = nextID;
	nextID++;
	list_add(&(tmp->evtlist), &events);
	spin_unlock(&event_lock);	
	printk("%i\n", tmp->eventID);
	return tmp->eventID;
}

asmlinkage long sys_doeventclose(int eventID)
{
	int ret_val = -1;
	spin_lock(&event_lock);
	struct event_struct *tmp = get_event_by_id(eventID);
	if(tmp == NULL || check_privileges(tmp, ROLE_CLOSE == 0))
	{
		spin_unlock(&event_lock);
		return -1;
	}
	spin_unlock(&event_lock);
	if((ret_val = sys_doeventsig(eventID)) == -1)
		return -1;	
	spin_lock(&event_lock);
	list_del(&(tmp->evtlist));
	kfree(tmp);
	spin_unlock(&event_lock);
	return ret_val;
}

asmlinkage long sys_doeventwait(int eventID)
{
	return 0;
}

asmlinkage long sys_doeventsig(int eventID)
{
	int proc_num = -1;
	spin_lock(&event_lock);
	struct event_struct* tmp = get_event_by_id(eventID);
	
	if(tmp == NULL || check_privileges(tmp, ROLE_WAITSIG) == 0)
	{
		spin_unlock(&event_lock);
		return -1;
	}
	
	wake_up_interruptible(&(tmp->waitq));
	proc_num = tmp->counter;
	tmp->counter = 0; 	
	spin_unlock(&event_lock);
	return proc_num;
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
