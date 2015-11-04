/*
* CSC456 Machine Problem 3
*
* Authors:
* Jaime Montoya
* Georgiy Platonov
*/

#include <linux/kernel.h>
#include <linux/events.h>
#include <linux/spinlock.h>
#include <linux/slab.h>
#include <linux/syscalls.h>
#include <linux/types.h>
#include <linux/wait.h>

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
static int nextID;
static DEFINE_SPINLOCK(event_lock);
static int event_count;

void __init doevent_init(void)
{
	nextID = 10;
	event_count = 0;
	printk("Event init ok...\n");
	return 0;
}

event_t* get_event_by_id(int eventID)
{
	event_t *ret_val;
	list_for_each_entry(ret_val, &events, evtlist)
	{
		if(ret_val->eventID == eventID)
			return ret_val;	
	}
	return NULL;
}

int check_privileges(event_t* event, int roleflag)
{
	uid_t EUID = sys_geteuid();
	gid_t EGID = sys_getegid();
	
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
	if(nextID == MAX_EVENTS)	//Too many events
	{
		spin_unlock(&event_lock);
		return -1;	
	}
	event_t *tmp = (event_t *)kmalloc(sizeof(event_t), GFP_KERNEL);
	if(tmp == NULL)			//Not enough memory to create event
	{
		spin_unlock(&event_lock);
		return -1;	
	}
	INIT_LIST_HEAD(&tmp->evtlist);
	tmp->eventID = nextID;
	nextID++;
	tmp->UID = sys_geteuid();
	tmp->GID = sys_getegid();
	tmp->UIDFlag = 1;
	tmp->GIDFlag = 1;
	tmp->counter = 0;
	tmp->sig_flag = 1;
	init_waitqueue_head(&(tmp->waitq));
	list_add(&(tmp->evtlist), &events);
	event_count++;
	spin_unlock(&event_lock);	
	printk("ID - %i\nUID - %i\nGID - %i\nUIDFlag - %i\nGIDFlag - %i\nCounter - %i\nsig_flag - %i\n", tmp->eventID, tmp->UID, tmp->GID, tmp->UIDFlag, tmp->GIDFlag, tmp->counter, tmp->sig_flag);
	return tmp->eventID;
}

asmlinkage long sys_doeventclose(int eventID)
{
	int proc_num = -1;
	spin_lock(&event_lock);
	event_t *tmp = get_event_by_id(eventID);
	if(tmp == NULL || check_privileges(tmp, ROLE_CLOSE) == 0 || check_privileges(tmp, ROLE_WAITSIG) == 0)
	{
		spin_unlock(&event_lock);
		return -1;
	}
	wake_up_interruptible(&(tmp->waitq));
	proc_num = tmp->counter;	
	list_del(&(tmp->evtlist));
	event_count--;	
	kfree(tmp);
	spin_unlock(&event_lock);
	return proc_num;
}

asmlinkage long sys_doeventwait(int eventID)
{
	spin_lock(&event_lock);
	event_t *tmp = get_event_by_id(eventID);
	if(tmp == NULL)
	{
		spin_unlock(&event_lock);
		return -1;
	}
	tmp->counter++;
	tmp->sig_flag = 0;
	spin_unlock(&event_lock);
	wait_event_interruptible(tmp->waitq, tmp->sig_flag == 1);	
	return 1;
}

asmlinkage long sys_doeventsig(int eventID)
{
	int proc_num = -1;

	spin_lock(&event_lock);
	event_t *tmp = get_event_by_id(eventID);
	
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
	if(eventIDs == NULL)
		return event_count;
	if(num < event_count)
		return -1;
	spin_lock(&event_lock);		
	int *IDs = (int*)kmalloc(event_count*sizeof(int), GFP_KERNEL);
	if(IDs == NULL)
	{
		spin_unlock(&event_lock);
		return -1;
	}
	event_t *tmp;
	int index = 0;
	list_for_each_entry(tmp, &events, evtlist)
	{
		IDs[index] = tmp->eventID;
		index++;
	}
	if(copy_to_user(eventIDs, IDs, event_count*sizeof(int)) != 0)
	{
		spin_unlock(&event_lock);
		return -1;
	}
	spin_unlock(&event_lock);
	return index;	//We return index because event_count may change between this and the previous instruction
}

asmlinkage long sys_doeventchown(int eventID, uid_t UID, gid_t GID)
{
	spin_lock(&event_lock);
	event_t *tmp = get_event_by_id(eventID);
	if(tmp == NULL || check_privileges(tmp, ROLE_MODIFY) == 0)
	{
		spin_unlock(&event_lock);
		return -1;		
	}
	tmp->UID = UID;
	tmp->GID = GID;
	spin_unlock(&event_lock);
	return 0;
}

asmlinkage long sys_doeventchmod(int eventID, int UIDFlag, int GIDFlag)
{
	spin_lock(&event_lock);
	event_t *tmp = get_event_by_id(eventID);
	if(tmp == NULL || check_privileges(tmp, ROLE_MODIFY) == 0)
	{
		spin_unlock(&event_lock);
		return -1;		
	}
	tmp->UIDFlag = UIDFlag;
	tmp->GIDFlag = GIDFlag;
	spin_unlock(&event_lock);
	return 0;
}

asmlinkage long sys_doeventstat(int eventID, uid_t __user *UID, gid_t __user *GID, int __user *UIDFlag, int __user *GIDFlag)
{
	if(UID == NULL || GID == NULL || UIDFlag == NULL || GIDFlag == NULL)
		return -1;
	spin_lock(&event_lock);
	event_t *tmp = get_event_by_id(eventID);
	if(tmp == NULL)
	{
		spin_unlock(&event_lock);
		return -1;	
	}
	if(copy_to_user(UID, &(tmp->UID), sizeof(uid_t)) != 0 
	|| copy_to_user(GID, &(tmp->GID), sizeof(gid_t)) != 0
	|| copy_to_user(UIDFlag, &(tmp->UIDFlag), sizeof(int)) != 0 
	|| copy_to_user(GIDFlag, &(tmp->GIDFlag), sizeof(int)) != 0)
	{	
		spin_unlock(&event_lock);		
		return -1;
	}
	spin_unlock(&event_lock);
	return 0;
}
