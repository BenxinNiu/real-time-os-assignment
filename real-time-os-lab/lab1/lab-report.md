# Lab report 1 - System calls
ssh ec2-user@54.208.4.49 -i /Users/benxinniu/batcave/keys/http-server-instance.pem

## System call tracing

### Question 1
List of command run for question 1:
```bash
strace echo "hello" &> trace1.txt
strace echo "hello" &> trace2.txt
diff trace1.txt trace2.txt
```
Up on examination of difference between two output files, we noticed that the same system calls are executed in the same order. However, the value returned by 
these system calls are different. For instance, system call `mmap` which creates a mapping in the virtual address pace of the calling process, the very first invocation of `mmap` returns different values, as shown below:
The first *trace*
```$xslt
mmap(NULL, 23476, PROT_READ, MAP_PRIVATE, 3, 0) = 0x7fa6ac565000
```
The second *trace*
```$xslt
mmap(NULL, 23476, PROT_READ, MAP_PRIVATE, 3, 0) = 0x7f6254962000
```
In both case `NULL` were passed into the `mmap` which lets the kernel to assign the mapping, but the returned pointer value are different (0x7fa6ac565000 and 0x7f6254962000)       
Similar patterns are shown in the diff, same system calls are executed in the same order, but with different arguments value, and different returned pointer value.

### Question 2
List of command run for question 2
```bash
strace /etc/fstab &> trace3.txt
```

The major difference is the latter command exited with `1` while both of the `echo` exited with `0` as shown below:
```$xslt
# cat command
exit_group(1)                           = ?
+++ exited with 1 +++

# echo command
exit_group(0)                           = ?
+++ exited with 0 +++
```

In addition to this, `cat` let shell executed a list of different system calls, with `fstat`, `close`, and `open` being called. At the last, `wirte` was invoked which caused *Permission denied* error, and lead the process to exit with `1`      
Also, one interesting discovery was the `getpid` was invoked which return the calling process id (25655)

## System call assembly code

### Question 1 
List of command run for Question 1

```bash
gcc -c stuff.s -o stuff.o
man objdump
objdump -D stuff.o
```
Output of objdump:
```$xslt
stuff.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <do_stuff>:
   0:	b8 00 00 00 00       	mov    $0x0,%eax
   5:	cd 80                	int    $0x80
   7:	c3                   	retq   

Disassembly of section .data:

0000000000000000 <message>:
   0:	48                   	rex.W
   1:	65 6c                	gs insb (%dx),%es:(%rdi)
   3:	6c                   	insb   (%dx),%es:(%rdi)
   4:	6f                   	outsl  %ds:(%rsi),(%dx)
   5:	2c 20                	sub    $0x20,%al
   7:	77 6f                	ja     78 <message+0x78>
   9:	72 6c                	jb     77 <message+0x77>
   b:	64 21 0a             	and    %ecx,%fs:(%rdx)
```
After disassembly, instructions in function `do_stuff` are the same, with `mov`, `int`, and `ret` are displayed, but more instructions are displayed for `message`. For instance, `insb`, `sub`, as shown above     
One of the difference we discovered was instructions in function `do_stuff`, memory offset are displayed (0, 5 ,7), the offset starts from zero and increase by the size of each instruction sequentially. For instance, the instruction `mov` has a size of 5 bytes, so the next offset value is 5.        
memory offset in `message` also follows this. 


### Question 2
List of command run for Question 2
```$xslt
sudo yum install clang -y
make
objdump -d syscalls > syscall_dis_small.txt
scp -i /Users/benxinniu/batcave/keys/http-server-instance.pem ec2-user@54.208.4.49:syscall_dis_small.txt .
cat syscall_dis_small.txt | grep do_stuff -A 8
```
Below shows the output of running `cat syscall_dis_small.txt | grep do_stuff -A 8` command      
```$xslt
 4005e7:       e8 4b 00 00 00          callq  400637 <do_stuff>
  4005ec:       bf 02 00 00 00          mov    $0x2,%edi
  4005f1:       48 8d 75 d0             lea    -0x30(%rbp),%rsi
  4005f5:       e8 96 fe ff ff          callq  400490 <clock_gettime@plt>
  4005fa:       83 f8 00                cmp    $0x0,%eax
  4005fd:       0f 84 16 00 00 00       je     400619 <main+0x89>
  400603:       bf ff ff ff ff          mov    $0xffffffff,%edi
  400608:       48 be ee 06 40 00 00    movabs $0x4006ee,%rsi
  40060f:       00 00 00 
--
0000000000400637 <do_stuff>:
  400637:       b8 34 10 60 00          mov    $0x601034,%eax
  40063c:       cd 80                   int    $0x80
  40063e:       c3                      retq   
  40063f:       90                      nop

0000000000400640 <__libc_csu_init>:
  400640:       41 57                   push   %r15
  400642:       41 56                   push   %r14
```

By comparing output of two disassembly, we discovered that the disassembly of function `do_stuff` of the complete program included on extra instruction `nop` (no operation), also memory offset starts at a different value with 400637 and 0, respectively. As shown above.       

The difference in memory offset is further investigated by using `nm` command. The snippet of outputs are shown below:
```$xslt
nm syscalls.o
...

0000000000400560 t __do_global_dtors_aux
0000000000600e18 t __do_global_dtors_aux_fini_array_entry
0000000000400637 T do_stuff
00000000004006b8 R __dso_handle
0000000000600e20 d _DYNAMIC

...

nm stuff.o

0000000000000000 T do_stuff
0000000000000000 d message
```

As shown above, both outputs show function `do_stuff` is global text symbol but with different virtual address, 400637 and 0, respectively. This also aligns with the output of disassembly, which shows the offset starts at 400637 and 0.

### Question 3
By comparing traces of two program, we discovered that a group of system calls were executed by both program, as shown in the output until line 25.         
After which, the `syscalls` invoked different system calls than `echo`. For instance, `clock_gettime`, as shown in `main`. In addition, line `syscall_0x601034(0x2, 0x7ffd92b0b980, 0, 0, 0, 0) = -1 (errno 38)` indicates a system call was invoked by the instruction `int $0x80` in the stuff.s.

### Question 4

list of command used in Question 4
```$xslt
sudo yum install gdb -y
gdb syscalls
break 36
run
si
si
info register
```
snippet of `info register` command in `gdb`:

```$xslt
rax            0x601034	6295604
rbx            0x0	0
rcx            0x7ffff7ffab52	140737354115922
rdx            0x0	0
rsi            0x7fffffffe3a0	140737488348064
rdi            0x2	2
rbp            0x7fffffffe3c0	0x7fffffffe3c0
rsp            0x7fffffffe378	0x7fffffffe378
r8             0x3	3
r9             0x77	119
r10            0x7fffffffe360	140737488348000
r11            0x297	663
r12            0x4004c0	4195520
r13            0x7fffffffe4a0	140737488348320
r14            0x0	0
r15            0x0	0

...

```
By setting the breakpoint on line 36, then step into instruction `int $0x80` and printing content of all registers, we found the content of register `rax` has the value of `0x601034` which is the same as `syscall_0x601034(0x2, 0x7ffd92b0b980, 0, 0, 0, 0) = -1 (errno 38)` shown in the output by `strace`     

## Invoking system calls

### Question 1
Modified code       
``` 
do_stuff:
    mov $0x4, %eax
    mov $0x1, %ebx
    mov $message, %ecx
    mov $0x12, %edx
    int	$0x80
    ret
```
