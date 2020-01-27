# Prelab 1
Question1		
 A. The `man` pages are a user manual that is by default built into most  Linux  distributions and most other Unix like Operating System during installation. The `man` command is used to format and display this `man` page

B. The Section 2 refers to *System calls and error numbers* while Section 3 refers to *Functions in the C libraries.*

C. `write` - write to a file descriptor, the integers *1* and *2* stands for `standard output` & `standard error`, respectively

Question 2

trace system calls and signals

Question 3
A. On 64 bit Linux system in AWS EC2 instance, we found there are five `unistd` header files, as listed below:
``` bash 
locate -A unistd
/usr/lib64/perl5/asm/unistd.ph
/usr/lib64/perl5/asm/unistd_32.ph
/usr/lib64/perl5/asm/unistd_64.ph
/usr/lib64/perl5/asm/unistd_x32.ph
/usr/share/man/man0p/unistd.h.0p.gz
```

B. The third listed file (*/usr/lib64/perl5/asm/unistd_64.ph*) is the most appropriate file to use because the system is Linux 64bits and command `uname` also tells us that the system is indeed `Linux` in the EC2 instance.

C. Knowing that `unistd_64.ph` file contains the system call number. Upon examine the content, we found the following call number:
1. `open(2)` : 2
2. `read(2)` : 0 
3. `write(2)` : 1
4. `close(2)`: 3

Question 4      

|                    | SYSCALL | SYSENTER | LEGACY |
|--------------------|---------|----------|--------|
| System call number | rax     | eax      | eax    |
| Argument 1         | rdi     | ebx      | ebx    |
| Argument 2         | rsi     | ecx      | ecx    |
| Argument 3         | rdx     | edx      | edx    |
| Argument 4         | r10     | esi      | esi    |
| Argument 5         | r8      | edi      | edi    |
| Argument 6         | r9      | 0(%ebp)  | ebp    |
