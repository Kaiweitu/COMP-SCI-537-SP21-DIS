# COMP SCI 537 DIS-313 Week 4
Contents:
- Introduction to the Shell
- Basic Linux Programming in C
  - fork() and wait()
  - exec() Family Functions
  - dup() and dup2()
- P3 Spec Overview


References:
- Online Linux manual pages at [man7.org](https://man7.org/linux/man-pages/man1/man.1.html)
- C reference page at [https://en.cppreference.com/w/c](https://en.cppreference.com/w/c)

## Introduction to the Shell
Shell is basically a **command line interpreter** that provides the user an interface to input command and execute the program. To be more specific, shell works in the following basic way:
1. Shows the shell prompt (e.g. `$` ) and wait for the input from the user
2. Parse user's command and execute them
3. Back to Step 1

Briefly speaking, shell is just a user application running in a `while` loop that keeps accepting user command and executing them. When the kernel bootup, it will create a very first process named `init` (as PID 1). This `init` process will fork a child process which is just the `shell`.  

In addition to its basic functionality, shell has some nice advanced features that we should know about. 
- Redirection: Redirect the output of a program to a file instead of the screen (`stdout`). Usually, `>` is used as the character to represent redirection.
~~~[bash]
cat * > cat.out
~~~
- Alias: Setup a short-cut for some complex command. 
~~~[bash]
# Check all the alias
alias
~~~
- Pipeline: the output of the first command will be used as the input of the second command
~~~[bash]
command_1 | command_2
~~~

There are still many other features that isn't covered here. Please refer to this [manual](https://man7.org/linux/man-pages/man1/bash.1.html) for more information. We will implement the first two features in project 3: **redirection** and **alias**.

## Basic Linux Progamming in C
### fork() and wait()
[fork()](https://man7.org/linux/man-pages/man2/fork.2.html) is a syscall that creates a new process by duplicating the calling process. The newly created process is referred as **child** process while the calling process is referred as **parent** process.

At the time `fork()` is called, parent and child process will have different virtual page tables but share the same memory content. The child process will be a exact duplicate of the parent process except that PID, parent PID ex are different.

The return value of the `fork()` is the PID of the child process, while for the child process the return value is zero.


[wait()](https://man7.org/linux/man-pages/man2/waitid.2.html) is a syscall that is used to wait for state changes in a child of the calling process. A state change includes termination. When the parent calls `wait()`, the behavior of the `wait()` can divided into two cases:
1. A child process has already changed state and then return immediately.
2. Block until a child changes state if no state change for any child process before.

Note that one call to the `wait()` only corresponds to one state change. Therefore, if we want to wait for the termination of multiple children processes, then we need to call `wait()` multiple times.

Combining `fork()` and `wait()`, a typical usage would looks as following:
~~~[c]
pid_t rnt = fork();

if (rnt == -1) {
    perror("parent process: fork failed");
    exit(-1);
}

if (rnt == 0) {
    // child process goes here

    return 0;
} else {
    // parent process goes here
}

// Wait for the child process to finish before return
int wstatus;
wait(&wstatus);
return 0;
~~~

### exec() Family Functions
exec() replaces the current process image with a new process image. The new process will have newly initialized stack, heap and data segments with replaced code segments. Note that on successful execution, `exec()` will **NEVER** returns; If it does return, it indicates a failure.

There are a family of `exec()` functions which has different suffix. Checkout the [manual](https://man7.org/linux/man-pages/man3/exec.3.html) pages for more information.
- `l` means that `exec` take arguments as a list of parameters just like printf.
```[c]
int execl(const char *pathname, const char *arg, ...
                       /* (char  *) NULL */);
```
- `v` means that `exec` take argument as an array of pointers to null-terminated strings.
```
int execv(const char *pathname, char *const argv[]);
```
### dup() and dup2()
Both [dup()](https://man7.org/linux/man-pages/man2/dup2.2.html) and [dup2()](https://man7.org/linux/man-pages/man2/dup2.2.html) performs same task that creates a copy of the file descriptor of a given fd. The difference is that `dup()` uses the lowest-numbered unused file descriptor while `dup2()` uses a given new fd. If this `newfd` was previously open, it is silently closed before being reused.
```[c]
#include <unistd.h>

int dup(int oldfd);
int dup2(int oldfd, int newfd);
```
Think about how we can implement redirection using `dup2()` or `dup()`.

### P3 Spec Overview
1. Three build-in commands:
   - alias
   - unalias
   - exit
2. Support two advanced features:
   - Alias
   - Redirection

Tips:
- Modularize your code instead of putting them all together
- Implement one functionality at one time and make sure its fully tested.