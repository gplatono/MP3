#include <linux/kernel.h>
#include <linux/events.h>
#include <linux/spinlock.h>
#include <linux/slab.h>

#define MAX_EVENTS 1000000

static struct event_struct *events;
int nextID;
DEFINE_SPINLOCK(event_lock);

int doeventinit()
{
	nextID = 0;
	events = NULL;
	return 0;
}

asmlinkage long sys_doeventopen(void)
{	
	spin_lock(&event_lock);
	if(nextID == MAX_EVENTS)
		return -1;
	struct event_struct *tmp = (struct event_struct *)kmalloc(sizeof(struct event_struct), GFP_KERNEL);
	INIT_LIST_HEAD(&tmp->evtlist);
	tmp->eventID = nextID;
	nextID++;
	if(events == NULL)	
	{		
		events = tmp;
	}
	else 
	{	
		list_add(&(tmp->evtlist), &(events->evtlist));
	}
	spin_unlock(&event_lock);	
	printk("%i\n", tmp->eventID);
	return tmp->eventID;
}

asmlinkage long sys_doeventclose(int eventID)
{
	spin_lock(&event_lock);
	
	spin_unlock(&event_lock);
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
