# COMP SCI 537 DIS-313 Week 6
Contents:
- P4 Specs
  - Compensated Round-Robin (CRR) Scheduler
  - `schedtest()` application 
- xv6 Scheduling
  - Process
  - Contenxt Switch
  - Round-Robin (RR) Scheduling
  - `sleep` and `wakeup1`

References:
- Old discussion [video](https://www.youtube.com/watch?v=eYfeOT1QYmg) from Remzi
- Chapter 5 (Scheduling) of xv6 reference [book](https://pdos.csail.mit.edu/6.828/2014/xv6/book-rev8.pdf)

## P2 Specs
### Compensated Round-Robin (CRR) Scheduler
A very simple Round-Robin scheduler works like this
~~~[bash]
# If we have process A and B running for a long time, then we have
ABABABABAB
~~~
CRR Scheduler is a advanced version of RR Scheduler with following extra features:
- We can adjust the number of slices for each process by calling `setslice(int pid, int slice)`
- The same amount of compensated ticks will be granted if a process sleeps for certain amount of time

We will through the example in the spec to develop better understanding.

**Note** that we need a queue-like strucuture to maintain the order. The original `ptable` defined in the xv6 is not enough. Therefore, a linked list structure is recommended to use for implementing the queue structure.

### `schedtest()` application 
See the specs for more detail.



## xv6 Scheudling
### Process
In this section, we are going to answer the following questions
- How is the very first process created?
- How is the new process created?

The very first created after kernel booted up is called `init`. This process is created in a function named `userinit`.
~~~[c]
// Called by the main function in main.c
void
userinit(void)
{
    struct proc *p;

    // Linker will put the initcode (initcode.S) at _binary_initcode_start
    extern char _binary_initcode_start[], _binary_initcode_size[];

    // Allocate the proc structures
    p = allocproc();
    initproc = p;
    ...
    // Setup the virtual page table
    if((p->pgdir = setupkvm()) == 0)
        panic("userinit: out of memory?");
    // Copy the init code to the new pagetable at address 0
    inituvm(p->pgdir, _binary_initcode_start, (int)_binary_initcode_size);
    ...
    // Set the program pointer to address 0 (starts of the init code) 
    p->tf->eip = 0;  // beginning of initcode.S

    // Change the process state to RUNNABLE
    acquire(&ptable.lock);
    p->state = RUNNABLE;
    release(&ptable.lock);
}
~~~
The above piece of code will end up allocating our very first process `init` in the `ptable`. Note this process hasn't been runned yet. This `init` process will run the code defined in `initcode.S` which will basically execute a user application named `init`. This `init` application will fork a child process and run the `sh` application (`sh.c`).

The new process is created by  `fork` which will inherit parent's page table to the child process and allocate new PCB for the child process (`allocproc`).
### Context Switch
Context switching involves storing the `context` of currently running process so that it can be reloaded when required and execution can be resumed from the same point as earlier.
~~~[c]
struct context {
  uint edi;     // Destination Index
  uint esi;     // Source Index
  uint ebx;     // A general-purpose register
  uint ebp;     // Function frame pointer
  uint eip;     // Extended Instruction Pointer (Program Pointer)
};
~~~
The above `context` structure is defined in `proc.h` which includes all the necessary registers needed to be stored for future resumption. Obviously, frame pointer of current function stack and the instruction pointer are included.
### Round-Robin (RR) Scheduling
We have learned how the very first `init` process is allocated in the `ptable` and be ready to run. But we we don't know how it actually is scheduled to run. Before we divides into the detail about how the RR scheduling works in xv6, we need first to know how all the cpus are booted up.

In xv6, there will be only one Boot Processer which will run the `main` function. Within this `main` function, this Boot Process will boot up all the other cpus. All those cpus will end up running the codes defined in `entryothers.S` which will jump to `mpenter` in the end. 
~~~[c]
// BOOT CPU jump to main function from entry.S
int
main(void)
{
    ...
    startothers();   // start other processors
    kinit2(P2V(4*1024*1024), P2V(PHYSTOP)); // must come after startothers()
    userinit();      // first user process
    mpmain();        // finish this processor's setup
}

// Other CPUs jump here from entryother.S.
static void
mpenter(void)
{
    switchkvm();    // Switch to kernel page table
    seginit();      // Setup the Interrupt Descriptor Table (IDT) 
    lapicinit();    // Init interrupt control 
    mpmain();      
}

// All the cpu ends up here.
static void
mpmain(void)
{
    cprintf("cpu%d: starting %d\n", cpuid(), cpuid());
    idtinit();       // load IDT register which is setup before
    xchg(&(mycpu()->started), 1); // tell startothers() we're up
    scheduler();     // start running processes
}
~~~
All the cpus will end up in the function named `mpmain` within which it called a function named `scheduler` at end. This `scheduler` function is where all the scheduling happens. 
~~~[c]
// Per-CPU state
struct cpu {
  ...
  struct context *scheduler;   // swtch() here to enter scheduler
  ...
  struct proc *proc;           // The process running on this cpu or null
};

void
scheduler(void)
{
  struct proc *p;
  struct cpu *c = mycpu();
  c->proc = 0;
  
  for(;;){
    // Enable interrupts on this processor.
    sti();

    // Loop over process table looking for process to run.
    acquire(&ptable.lock);
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++){
      if(p->state != RUNNABLE)
        continue;

      // Switch to chosen process.  It is the process's job
      // to release ptable.lock and then reacquire it
      // before jumping back to us.
      c->proc = p;
      switchuvm(p);
      p->state = RUNNING;

      swtch(&(c->scheduler), p->context);
      switchkvm();

      // Process is done running for now.
      // It should have changed its p->state before coming back.
      c->proc = 0;
    }
    release(&ptable.lock);

  }
}
~~~
Each cpu has its own state struct called `cpu`. Two things need to know about this structure: 
- `struct context *scheduler`: context to resume the execution of scheduler function
- `struct proc *proc`: The PCB of the currently running process on this cpu.

`for(;;)` will loop forever. In this endless loop, it will keep finding the next available job to run and switch to that job. The original xv6 just looped through the process table. We will upgrade it to a more sophiscated version of RR scheduler. 


It's important to know that there are three events that will lead us back to this `scheduler`:
1. The currently running process exits (`exit`)
2. The currently running process calls sleep (`sleep`)
3. A time interrupt happens (`trap`)

**Note**: An important **locking invariant** you might want to know here. Notice that before we call the `swtch`, the `ptable` lock is stilled hold. Do you think it is a problem?

### `sleep()` and `wakeup1()`
The current xv6 implementation of the `sleep` syscall forces the sleeping process to wake on every timer tick to check whether it has slept for the requested number of timer ticks.
~~~[c]
int
sys_sleep(void)
{
    ...
    while(ticks - ticks0 < n)
    {
        if(myproc()->killed)
        {
            release(&tickslock);
            return -1;
        }
        // Wake up every time ticks
        sleep(&ticks, &tickslock);
    }
    release(&tickslock);
    return 0;
}

static void
wakeup1(void *chan)
{
    struct proc *p;
    for(p = ptable.proc; p < &ptable.proc[NPROC]; p++)
        if(p->state == SLEEPING && p->chan == chan)
            // Simply wakeup every sleeping process
            p->state = RUNNABLE;
}
~~~
`wakeup1` will be called every time ticks by `cpu 0` in the `trap` handler. Therefore, each sleeping process will be woken up once every tick to check the sleeping condition.