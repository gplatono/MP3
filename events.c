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

/* Maximum number of eventIDs available.
*  If nextID == MAX_EVENTS, it's impossible to create events anymore 
*/
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

/*
 * Function: doevent_init()
 *
 * Description:
 *  This function is called when the system is booted. It was added to 
 *  this file: linux-3.18.20/init/main.c, inside 
 *  asmlinkage __visible void __init start_kernel(void){}.
 *
 * Inputs:
 *  No inputs.
 * 
 * Outputs: 
 *  No outputs.
 *
 * Return value:
 *  No return.
 */

void __init doevent_init(void)
{
	nextID = 0;
	event_count = 0;
	printk("Event init ok...\n");
}

/*
 * Function: get_event_by_id()
 *
 * Description:
 *  This function finds an event in the event list using the eventID received
 *  as a parameter.
 *
 * Inputs:
 *  eventID - An integer that corresponds to the event that the function will
 *  try to find. 
 * 
 * Outputs: 
 *  No outputs.
 *
 * Return value:
 *  A pointer to the element in the event list with the eventID 
 *  received as a parameter. Returns NULL if the event was not found in the event list.
 */
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

/*
 * Function: check_privileges()
 *
 * Description:
 *  This function is used to check the privileges based on Event Access Control
 *  rules created for the system calls on events.
 *
 * Inputs:
 *  event - A pointer to a memory location in which to store the event that
 *  will be evaluated for the corresponding privileges.
 *  roleflag - An integer that will determine the roles defining operations 
 *  associated with an event. The possible values are defined above in this 
 *  file.
 *
 * Outputs: 
 *  No outputs.
 *
 * Return value:
 *  0 - Insufficient privileges, do not allow access.
 *  1 - Sufficient privileges, allow access.
 */
int check_privileges(event_t* event, int roleflag)
{
	/* Getting effective UID and GID of the current process */
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

/*
 * Function: sys_doeventopen()
 *
 * Description:
 *  This function creates a new event.
 *
 * Inputs:
 *  void - The function takes zero arguments.
 * 
 * Outputs: 
 *  No outputs.
 *
 * Return value:
 *  eventID - The event ID of the event that was created successfully.
 *  -1 - Creation of the event failed.
 */
asmlinkage long sys_doeventopen(void)
{	
	spin_lock(&event_lock);

	/* Too many events */
	if(nextID == MAX_EVENTS)	
	{
		spin_unlock(&event_lock);
		return -1;	
	}

	event_t *tmp = (event_t *)kmalloc(sizeof(event_t), GFP_KERNEL);

	/* Not enough memory to create event */
	if(tmp == NULL)			
	{
		spin_unlock(&event_lock);
		return -1;	
	}

	/* Init new event */
	INIT_LIST_HEAD(&tmp->evtlist);
	tmp->eventID = nextID;
	tmp->UID = sys_geteuid();
	tmp->GID = sys_getegid();
	tmp->UIDFlag = 1;
	tmp->GIDFlag = 1;
	tmp->counter = 0;
	tmp->sig_flag = 1;
	init_waitqueue_head(&(tmp->waitq));

	list_add(&(tmp->evtlist), &events);
	nextID++;
	event_count++;
	spin_unlock(&event_lock);	
	return tmp->eventID;
}

/*
 * Function: sys_doeventclose()
 *
 * Description:
 *  This function destroys an event with the given event ID and signals any 
 *  processes waiting on the event to leave the event.
 *
 * Inputs:
 *  eventID - An integer that corresponds to the event that the function will
 *  be applied to.
 * 
 * Outputs: 
 *  No outputs.
 *
 * Return value:
 *  proc_num - Number of processes signaled.
 */
asmlinkage long sys_doeventclose(int eventID)
{
	int proc_num = -1; 	//Number of processes waiting on an event

	spin_lock(&event_lock);

	event_t *tmp = get_event_by_id(eventID);
	
	/* If there is no event with provided eventID or we don't have enough privileges then fail */
	if(tmp == NULL || check_privileges(tmp, ROLE_CLOSE) == 0 || check_privileges(tmp, ROLE_WAITSIG) == 0)
	{
		spin_unlock(&event_lock);
		return -1;
	}
	
	/* Signal and wake up all the waiting processes before destroying the event */
	tmp->sig_flag = 1;
	wake_up_interruptible(&(tmp->waitq));

	/* Save the amount of waiting processes,
	* delete the event from the list and free memory
	*/
	proc_num = tmp->counter;
	list_del(&(tmp->evtlist));
	event_count--;	
	kfree(tmp);
	spin_unlock(&event_lock);
	return proc_num;
}

/*
 * Function: sys_doeventwait()
 *
 * Description:
 *  This function blocks process until the event is signaled.
 *
 * Inputs:
 *  eventID - An integer that corresponds to the event that the function will
 *  be applied to.
 *
 * Outputs: 
 *  No outputs.
 *
 * Return value:
 *  1 - Process blocking succeeded.
 *  -1 - Process blocking failed.
 */
asmlinkage long sys_doeventwait(int eventID)
{
	spin_lock(&event_lock);

	event_t *tmp = get_event_by_id(eventID);

	/* If there is no event with provided eventID then fail */
	if(tmp == NULL)
	{
		spin_unlock(&event_lock);
		return -1;
	}

	/* Increment the counter of waiting processes and set signal flag to 0 (No signals) */
	tmp->counter++;
	tmp->sig_flag = 0;
	spin_unlock(&event_lock);
	wait_event_interruptible(tmp->waitq, tmp->sig_flag == 1);	
	return 1;
}

/*
 * Function: sys_doeventsig()
 *
 * Description:
 *  This function unblocks all waiting processes, or it is ignored if no
 *  processes are blocked.
 *
 * Inputs:
 *  eventID - An integer that corresponds to the event that the function will
 *  be applied to.
 *
 * Outputs: 
 *  No outputs. 
 *
 * Return value:
 *  proc_num - Number of processes signaled on success.
 *  -1 - Unblocking the waiting processes failed.
 */
asmlinkage long sys_doeventsig(int eventID)
{
	int proc_num = -1;

	spin_lock(&event_lock);

	event_t *tmp = get_event_by_id(eventID);

	/* If there is no event with provided eventID 
	*  or we don't have enough privileges then fail 
	*/
	if(tmp == NULL || check_privileges(tmp, ROLE_WAITSIG) == 0)
	{
		spin_unlock(&event_lock);
		return -1;
	}
	
	/* Set signal flag to 1 (send a signal),
	*  wake up all the waiting processes,
	*  reset the waiting counter
	*/
	tmp->sig_flag = 1;
	wake_up_interruptible(&(tmp->waitq));
	proc_num = tmp->counter;
	tmp->counter = 0; 	
	spin_unlock(&event_lock);
	return proc_num;
}

/*
 * Function: sys_doeventinfo()
 *
 * Description:
 *  This function fills in the array of integers pointed to by eventIDs with
 *  the current set of active event IDs.
 *
 * Inputs:
 *  num - The number of integers which the memory pointed to by eventIDs can hold.
 *  eventIDs - A pointer to a memory location in which to store the current set of 
 *  active eventIDs.
 *
 * Outputs: 
 *  eventIDs - the array in user space to store existing event IDs.
 *
 * Return value:
 *  index - Value corresponding to the number of active events.
 *  -1 - The function failed to return the number of active events.
 */
asmlinkage long sys_doeventinfo(int num, int *eventIDs)
{	
	/* Safety checks */
	if(eventIDs == NULL)
		return event_count;
	if(num < event_count)
		return -1;

	spin_lock(&event_lock);		

	/* Try to create a temporary buffer to store event IDs */
	int *IDs = (int*)kmalloc(event_count*sizeof(int), GFP_KERNEL);

	if(IDs == NULL)
	{
		spin_unlock(&event_lock);
		return -1;
	}

	/* Scan through the list of events and fill IDs array */
	event_t *tmp;
	int index = 0;
	list_for_each_entry(tmp, &events, evtlist)
	{
		IDs[index] = tmp->eventID;
		index++;
	}
	
	/* Copy IDs to eventIDs and check if the copying succeded */
	if(copy_to_user(eventIDs, IDs, event_count*sizeof(int)) != 0)
	{
		spin_unlock(&event_lock);
		return -1;
	}
	spin_unlock(&event_lock);

	/* We return index because event_count may
	*  change between this and the previous instruction
	*/
	return index;	
}

/*
 * Function: sys_doeventchown()
 *
 * Description:
 *  This function changes the UID and GID of the event to the specified values.
 *
 * Inputs:
 *  eventID - An integer that corresponds to the event that the function will
 *  be applied to.
 *  UID - new effective UID value for this event.
 *  GID - new effective GID value for this event.
 *
 * Outputs: 
 *  No outputs.
 *
 * Return value:
 *  0 - Succeeded changing the UID and GID of the event.
 *  -1 - Failed changing the UID and GID of the event.
 */
asmlinkage long sys_doeventchown(int eventID, uid_t UID, gid_t GID)
{
	spin_lock(&event_lock);

	event_t *tmp = get_event_by_id(eventID);
	
	/* If there is no event with provided eventID 
	*  or we don't have enough privileges then fail 
	*/
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

/*
 * Function: sys_doeventchmod()
 *
 * Description:
 *  This function changes the User Signal Enable Bit (UIDFlag) and the Group Signal 
 *  Enable Bit (GIDFlag) to provided values.
 *
 * Inputs:
 *  eventID - An integer that corresponds to the event that the function will
 *  be applied to.
 *  UIDFlag - New value for the event's UIDFlag
 *  GIDFlag - New value for the event's GIDFlag
 *
 * Outputs: 
 *  No outputs.
 *
 * Return value:
 *  0 - Succeeded changing the User Signal Enable Bit and Group Signal Enable Bit.
 *  -1 - Failed changing the User Signal Enable Bit or Group Signal Enable Bit.
 */
asmlinkage long sys_doeventchmod(int eventID, int UIDFlag, int GIDFlag)
{
	spin_lock(&event_lock);

	event_t *tmp = get_event_by_id(eventID);

	/* If there is no event with provided eventID 
	*  or we don't have enough privileges then fail 
	*/
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
/*
 * Function: sys_doeventstat()
 *
 * Description:
 *  This function places the UID, GID, User Signal Enable Bit, and Group Signal Enable
 *  Bit into the memory pointed to by UID, GID, UIDFlag, and GIDFlag, respectively.
 *
 * Inputs:
 *  eventID - An integer that corresponds to the event that the function will
 *  be applied to.
 *  UID - A pointer to a memory location in which to store the UID.
 *  GID - A pointer to a memory location in which to store the GID.
 *  UIDFlag - A pointer to a memory location in which to store the UIDFlag.
 *  GIDFlag - A pointer to a memory location in which to store the GIDFlag.
 *
 * Outputs: 
 *  No outputs.
 *
 * Return value:
 *  0 - Succeeded placing values into memory corresponding to UID, GID, UIDFlag, and 
 *  GIDFlag.
 *  -1 - Failed placing values into memory corresponding to UID, GID, UIDFlag, and 
 *  GIDFlag.
 */
asmlinkage long sys_doeventstat(int eventID, uid_t __user *UID, gid_t __user *GID, int __user *UIDFlag, int __user *GIDFlag)
{
	/* Pointer safety check */
	if(UID == NULL || GID == NULL || UIDFlag == NULL || GIDFlag == NULL)
		return -1;

	spin_lock(&event_lock);

	event_t *tmp = get_event_by_id(eventID);

	/* If no event with specified event ID then fail */
	if(tmp == NULL)
	{
		spin_unlock(&event_lock);
		return -1;	
	}

	/* Copy the event's UID, GID, UIDFlag, GIDFlag and check whether the operation succeded */
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
