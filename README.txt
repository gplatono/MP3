Files modified:

1) linux256/linux-3.18.20/kernel/events.c
Code corresponding to the functions added to implement all of the system calls.

2) linux256/linux-3.18.20/include/linux/events.h
Declaration of the structure for the events.

3) linux256/linux-3.18.20/init/main.c
doevent_init() was inserted into "asmlinkage void __init start_kernel(void)" to initialize variables at boot time:

/* Do the rest non-__init'ed, we're now alive */
doevent_init();   
rest_init();

doevent_init() is declared in the file doevent_init(). The steps to complete this process were completed by following the instructions in the "Boot Time" section at http://web.cs.wpi.edu/~claypool/courses/3013-A02/projects/proj2/index.html#boot

4) linux256/floppy/test.c
User-level program to test the kernel functions, including all of the systems calls that were created in the file linux256/linux-3.18.20/kernel/events.c.

5) linux256/runqemu.sh
Include the configuration to ensure that the source code for the test file can be compiled from the "floppy" directory and then make executable file available to run using QEMU.

6) linux256/linux-3.18.20/arch/x86/syscalls/syscall_64.tbl
Declaration of the system call numbers and entry vectors to include the new system calls that were created.

7) linux256/linux-3.18.20/include/linux/syscalls.h
Modified to add the syscall interfaces corresponding to the new system calls that were created for this project.



Jaime implemented the system calls 1, 3, 4, 7 and Georgiy implemented 2, 4, 6, 8.
We coded test program together.



Note: While we added the compiled test program to the patch it doesn’t seem to work, so after applying patch to fresh kernel you might need to recompile our test.c.
