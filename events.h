#ifndef _EVENTS_H
#define _EVENTS_H

#include <linux/types.h>
#include <linux/list.h>
#include <linux/wait.h>

/* Structure implementing our event abstraction */
struct event_struct 
{
	int eventID;			// Unique ID of an event
	uid_t UID;			// Effective UID associated with an event
	gid_t GID;			// Effective GID associated with an event
	int UIDFlag;			// User Signal Enable Bit
	int GIDFlag;			// Group Signal Enable Bit

	int sig_flag;			// Signal flag 
					// 0 - No signal, some processes are currently sleeping on this event
					// 1 - Signal is set
					
	int counter;			// The number of processes currently sleeping on this event
	struct list_head evtlist;	// Linux list structure
	wait_queue_head_t waitq;	// Waiting queue for this event
};

typedef struct event_struct event_t;

/* Init function for events */
void __init doevent_init(void);

#endif
