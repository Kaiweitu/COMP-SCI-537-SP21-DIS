# COMP SCI 537 DIS-313 Week 10
Contents:
- P6 Spec Overview
  - Clock Algorithm
  - Tips
- P6 Background
  - Exec Function

References:
- Chapter 2 and 3 of xv6 reference [book](https://pdos.csail.mit.edu/6.828/2014/xv6/book-rev8.pdf)



## P6 Spec Overview
Remembering in P5, the user takes the responsibility to encrypt those pages. This time in P6, we will handle over the responsibility to the kernel. The core idea is to have as much as possible pages encrypted to mimize the chance being attacked. But making all the pages encrypted will largely harm the performance. Therefore, a better idea is to only maintain a working set of pages in clear text for each process. A few important points here:
1. Maintain the working set separately for each process not globally.
2. The size of the working set is predefined as a constant CLOCKSIZE. You should add this constant to the `param.h` following the instruction in the spec.

The general idea could be summaried as folllowing:
1. All the user pages should be encrypted when allocated. You don't need to care about kernel pages. This includes the text page which contains the program code and also the stack pages where the program stack resides. 
2. And once a user page is accessed, the page fault will be triggered (as we did in P5). This time we will not only decrypte but also add it into a queue which maintains all the decrypted pages. If this queue is full, then we will evict one page by running a so called clock algorithm.
3. This page will only be encrypted again until it is evicted from the queue.

### Clock Algorithm 
The clock algorithm is also named FIFO second-chance algorithm. So simply speaking, it's an FIFO-based algorithm that is used to approximate LRU in practice. In order to implement this algorithm, we need to have one extra reference bit maybe named PTE_R to keep track whether this page has recently been accessed. The idea is as following:
1. When a page is decrypted, it will be added to the tail of the queue. The reference bit is set to one at this time. 
2. When we want to find a victim page to evict, we pop the head of the queue.
     - If the reference bit of this page is 0, then evict this page.
     - Otherwise, we set its reference bit to 0 and push it to the back of the queue(that's why we call it second-chance).
3. Repeat step 2 until we find the victim pages to evict.

Check the spec for more example. Note that a page might be deallocated by the user, when it's on the queue, in this case, simply remove it from the queue.

### Tips
Hopefully, through P5, we have already known how to encrypt/decrypt a page and also how to manipulate the page table (read or modify pte). In addition, we also learn how to handle the page fault in trap handler. You can reuse part of the P5 code, if you want. Therefore, in P6, we will focuse on implementing the clock algorithm mechanism. A few thing you might need to modify:
1. `struch proc` is used to maintain per-process data. We don't want to restrict you to specific implementation. The following sample code is ONLY one way of allocating the clock queue.
  ```
  typedef struct node {
    struct node *next;
    struct node *prev;
    pde_t *v; // Maybe vaddr here
  } node_t;

  struct proc {
    ...
    node_t clockQ[CLOCKSIZE];       // Locally allocated Clock Queue 
    node_t *head;                  // Double-linked List Queue
  }
  ```
2. `growproc` in `vm.c` and `exec` in `exec.c` might be a good place to encrypt all the USER pages when they are allocated. Altough there are many ways you can achieve this, you need to be aware that some other functions (e.g. `allocuvm` or  `mappages`) will also be used to allocate kernel pages.
3. Now we have three possible states for a page table entry. Below is a figure might help you to better understand this. You might notice that there is a state we have all the bits as 0. This saying that we couldn't distinguish it between an invalid page without looking into other information. There are many ways to solve this problem. Adding an extra PTE_V is one of the option. 
      
      ![State Diagram](state-diagram.jpeg)
4. When copying a linked list. Don't simply copy the pointer (shallow copy) when you copy the clock queue from the parent process. Copy the pointer will make your memory look as following at the end.
      ![Copy Figure](copy.jpeg)
5. Be sure to modify the Makefile before running the test:
   1. Modify `-O2` to `-O0`. Any other flag except `-O0` will let the compiler do some amount of optimization which will make the test not work.
   2. Modify CPU to 1 
## P6 Background
### Exec
Exec is a function which do all the setup works before running a user program. All the user program includes `init` and `sh` are run by the `exec`. These setup works includes
1. Copy the code text from the file system into the memory. The text and data for the progam is placed normally starting from virtual address 0.
2. Allocate one stack page two pages above the text and data pages. Copy all the argument into the stack.

The layout looks as following:

  ![Memory Figure](memory.jpeg)
  
All the pages above are heap-allocated page which could be allocated by calling `sbrk` syscall (used in `malloc`).

An intuitive way to encrypt those text and stack pages are right after they are set-up which should be somewhere at the end of the `exec` function.

~~~[c]
int
exec(char *path, char **argv)
{
  ...
  // Load program into memory.
  sz = 0;
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    ...
    // Allocate page for the program text pages
    if((sz = allocuvm(pgdir, sz, ph.vaddr + ph.memsz)) == 0)
      goto bad;
    ...
    // Load the program into the memory
    if(loaduvm(pgdir, (char*)ph.vaddr, ip, ph.off, ph.filesz) < 0)
      goto bad;
  }
  ...

  // Pad the text and data page to a full page
  sz = PGROUNDUP(sz);

  // Allocate two pages at the next page boundary.
  // Make the first inaccessible.  Use the second as the user stack.
  if((sz = allocuvm(pgdir, sz, sz + 2*PGSIZE)) == 0)
    goto bad;
  clearpteu(pgdir, (char*)(sz - 2*PGSIZE));
  sp = sz;

  // Push argument strings, prepare rest of stack in ustack.
  ...

  // Commit to the user image.
  oldpgdir = curproc->pgdir;
  curproc->pgdir = pgdir;
  curproc->sz = sz;
  curproc->tf->eip = elf.entry;  // main
  curproc->tf->esp = sp;
  switchuvm(curproc);
  freevm(oldpgdir);
  return 0;

 bad:
  ...
}
~~~