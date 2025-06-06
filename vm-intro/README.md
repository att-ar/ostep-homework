# Overview

Use the following comman to check memory usage before, during, and after running the memory-user program with a specified number of megabytes to consume.

```
top -l 1 | head -n 10 | grep PhysMem
```

-   Before: PhysMem: 7521M used (1206M wired, 2716M compressor), 113M unused.
-   During (100 MB allocated): PhysMem: 7564M used (1230M wired, 2706M compressor), 71M unused.
-   After: PhysMem: 7471M used (1229M wired, 2726M compressor), 164M unused.

Get the process ID running the program: (Or print `getpid()`)

```
ps aux | grep memory_user | grep -v grep
```

Check the different virtual memory regions allocated to the process

```
vmmap [PID]
```
