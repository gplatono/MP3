#ifndef _EVENTS_H
#define _EVENTS_H

#include <linux/types.h>
#include <linux/list.h>
#include <linux/wait.h>

struct event_struct 
{
	int eventID;
	uid_t UID;
	gid_t GID;
	int UIDFlag;
	int GIDFlag;
	int sig_flag;
	int counter;
	struct list_head evtlist;
	wait_queue_head_t waitq;
};

typedef struct event_struct event_t;

void __init doevent_init(void);

#endif




