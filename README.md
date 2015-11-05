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

Enter syscall number and  param:
181 0
[  383.720087] ID - 10
[  383.720087] UID - 0
[  383.720087] GID - 0
[  383.720087] UIDFlag - 1
[  383.720087] GIDFlag - 1
[  383.720087] Counter - 0
[  383.720087] sig_flag - 1
Enter syscall number and  param:
181 0
[  392.594469] ID - 11
[  392.594469] UID - 0
[  392.594469] GID - 0
[  392.594469] UIDFlag - 1
[  392.594469] GIDFlag - 1
[  392.594469] Counter - 0
[  392.594469] sig_flag - 1
Enter syscall number and  param:
181 0
[  404.345504] ID - 12
[  404.345504] UID - 0
[  404.345504] GID - 0
[  404.345504] UIDFlag - 1
[  404.345504] GIDFlag - 1
[  404.345504] Counter - 0
[  404.345504] sig_flag - 1

