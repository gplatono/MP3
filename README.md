# MP3
<h1>CSC456 Machine Problem 3</h1>

<h1>Files modified</h1>

linux256/linux-3.18.20/kernel/events.c<br />
linux256/linux-3.18.20/include/linux/events.h<br />
linux256/linux-3.18.20/init/main.c<br />
linux256/floppy/eventtest.c<br />
linux256/runqemu.sh<br />
linux256/linux-3.18.20/arch/x86/syscalls/syscall_64.tbl<br />
linux256/linux-3.18.20/include/linux/syscalls.h

<h1>Test program</h1>

<h2>Creating events</h2>
Create three new events. The return value is the event ID of the event that was created successfully.

Enter syscall number and  param:<br />
181 0<br />
[  383.720087] ID - 10<br />
[  383.720087] UID - 0<br />
[  383.720087] GID - 0<br />
[  383.720087] UIDFlag - 1<br />
[  383.720087] GIDFlag - 1<br />
[  383.720087] Counter - 0<br />
[  383.720087] sig_flag - 1<br />
Enter syscall number and  param:<br />
181 0<br />
[  392.594469] ID - 11<br />
[  392.594469] UID - 0<br />
[  392.594469] GID - 0<br />
[  392.594469] UIDFlag - 1<br />
[  392.594469] GIDFlag - 1<br />
[  392.594469] Counter - 0<br />
[  392.594469] sig_flag - 1<br />
Enter syscall number and  param:<br />
181 0<br />
[  404.345504] ID - 12<br />
[  404.345504] UID - 0<br />
[  404.345504] GID - 0<br />
[  404.345504] UIDFlag - 1<br />
[  404.345504] GIDFlag - 1<br />
[  404.345504] Counter - 0<br />
[  404.345504] sig_flag - 1<br />

Three events were created with the following eventIDs: 10, 11, 12. Notice that the eventID of the first event was 10, not 0. This is because the function void __init doevent_init(void) was called in the file linux256/linux-3.18.20/init/main.c, and the function created this initialization: "nextID = 10;".

<h2>Blocking a process until the event is signaled</h2>

Create three processes that will block on the event with eventID 11 (previously created).

Enter syscall number and  param:<br />
183 11<br />
Child created...<br />
Enter syscall number and  param:<br />
183 11<br />
Child created...<br />
Enter syscall number and  param:<br />
183 11<br />
Child created...<br />

<h2>Unblocking all waiting processes</h2>

Enter syscall number and  param:<br />
184 123<br />
-1<br />

The return value is -1 because an event with eventID 123 does not exist.

Enter syscall number and  param:<br />
184 10<br />
0<br />

The return value is 0 because the event with eventID 10 does not have any processes blocked.

Enter syscall number and  param:<br />
184 11<br />
3<br />

The return value is 3 because the event with eventID 11 had 3 processes blocked. Signaling eventID 11 unblocked all of the waiting processes.

<h2>Filling in the array of integers pointed to by eventIDs</h2>

result of doeventinfo: 3<br />
12 11 10 -1 -1 -1 -1 -1 -1 -1<br />

The return value is 3 because three events were created. "12 11 10" correspond to the eventIDs 10, 11, and 12 assigned to the events that were created.
