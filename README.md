# Web-Monitoring
Web monitoring system, developed in C, to count number of visits from different countries using an input log file.

## About the project:


## How to use:
1. Usage:
```
./program <Begin date> <End date> <input>
```
> Input: If input is empty or '-', it will use the standard input. Otherwise, it will use the input file.
> Begin and End date: The dates we want to evaluate log file between.

2. To run the program:
``` 
$ make tldmonitor  
$ ./tldmonitor 01/01/2000 01/09/2020 <small.txt
```

3. To debug:
```
$ make <name>
$ gdb <name>.o 01/01/2000 01/09/2020 <small.txt
```
> For 'name', can use:
> 1. cktldmonitor: checks for addresses usage.
> 2. ckt2dmonitor: checks for memoty leaks.
> 3. ckt3dmonitor: checks for undefined program behaviour.
> 4. dtldmonitor: normal program but with debugging symbols (to be able to use a debugger). 

4. To run the tests:
```
$ make <test>
$ ./<test>.o
```
> For 'test', can use:
> 1. datetests: To test date.c.
> 2. tldtests: To test tldlist.c.

5. To delete all .o files:
```
$ make clean
```
