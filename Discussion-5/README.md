# COMP SCI 537 DIS-313 Week 5
Contents:
- Shell Continued
  - Parsing: `strchr()`, `strtok()` and `strdup()`
- Exam Review
  - Scheduling
    - Performance Metrics
    - Scheduling Policies
  - Virtual Memory



References:
- Online Linux manual pages at [man7.org](https://man7.org/linux/man-pages/man1/man.1.html)
- C reference page at [https://en.cppreference.com/w/c](https://en.cppreference.com/w/c)

## Shell Continued
### Parsing: `strchr()`, `strtok()` and `strdup()`
[strchr()](https://en.cppreference.com/w/c/string/byte/strchr) finds the first occurrence of `ch` in the null-terminated byte string pointed to by str. The behavior is undefined if `str` is not a pointer to a null-terminated byte string.
~~~[c]
#include <string.h>

char *strchr( const char *str, int ch );
~~~ 
[strtok()](https://man7.org/linux/man-pages/man3/strtok.3.html) breaks a string into a sequence of zero or more noempty tokens. On the first call to strtok(), the string to be parsed should be specified in `str`. In each subsequent call that should parse the same string, `str` must be `NULL`. 
~~~[c]
#include <string.h>

char *strtok(char *str, const char *delim);
~~~

Note that strtok will modify the first argument. Duplicate it if you want the original copy.

[strdup()](https://man7.org/linux/man-pages/man3/strdup.3.html) returns a pointer to a new string which is a duplicate of the string s.  Memory for the new string is obtained with malloc, and need to be freed by the user in the future.

~~~[c]
#include <string.h>

char *strdup(const char *s);
char *strndup(const char *s, size_t n);
~~~


## Exam Review
### Scheduling
#### Performane Metrics
- Turnaround time: completion_time - arrival_time
- Response time: scheduled_time - arrival_time
- Throughput: jobs_completed per seconds
- Resource utilization: % time CPU busy
- unfairness: All jobs get same amount of CPU times.

#### Scheduling Policy
- FIFO (Fisrt In, First Out) / FCFS (First Come, First Served)
  - Problems: Long-running jobs could block short-running jobs which lead to turnaround time.
- SJF (Shortest Job First)
  - Optimal for minimizing average turnaround time if no preemption
  - Problems: No preemption could lead to bad decision.
- STCF (Shortest Time-to-Completion First)
  - Preemptive: Taking CPU away from currently running jo
  - Problems: Run-time is unknown in pratice
- RR (Round-Robin):
  - RR may increase turnaround time and decreases response time.
- MLFQ (Multi-Level Feedback Queue):
  - Mutiple level of round-robin
  - Process starts at the highest priority (decrease response time)
  - Decrease priority if time slice or budget used up.
  - Periodly boost all the process to avoid starvation
- Lottery Scheduling
  - Each process hold certain amount of lottery.
### Virtual Memory
See the questions
