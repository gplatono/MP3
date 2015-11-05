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

