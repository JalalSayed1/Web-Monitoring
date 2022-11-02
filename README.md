# Web Monitoring
- Web monitoring system to count the number of visits from different domains using an input log file.
- Using a binary search tree (BST) as the basis of the list.

## Requirements:
- Each Web server routinely logs accesses from other Web servers and browsers. The log is a text file in which each line contains a date and a hostname. Each date is logged in the format dd/mm/yyyy.
- Each hostname ends with a 2-letter country code such as uk or fr (or a 3-letter code such as com) preceded by a dot (‘.’).
- The final token in a hostname is usually called the “top level domain”.
 
## Specification:
- Given a start date, an end date, and one or more log files, the program is to determine the percentage of access from each TLD during that period, outputting the final percentages on standard output.
- Hostnames, and therefore, top level domain names, are case-insensitive. Therefore, accesses by X.Y.UK and a.b.uk are both accesses from the same TLD (i.e. uk).

## date.h: 
- Constructor for this ADT is date_create(); it converts a datestring in the format “dd/mm/yyyy” to a Date structure.
- date_duplicate() is known as a copy constructor; it duplicates the Date argument on the heap (using malloc()) and returns it to the user.
- date_compare() compares two Date structures, returning <0, 0, >0 if date1<date2, date1==date2, date1>date2, respectively.
- date_destroy() returns the heap storage associated with the Date structure.

## tldlist.h:
- LDList, TLDIterator, and TLDNode are opaque data structures that you can only manipulate using methods in this class.
- tldlist_create() creates a TLDList which can be used to store the counts of log entries against TLD strings; the begin and end date arguments enable filtering of added entries to be in the preferred date range.
- tldlist_destroy() returns the heap storage associated with the TLDList structure.
- tldlist_add() will count the log entry if the associated date is within the preferred data range.
- tldlist_count() returns the number of log entries that have been counted in the list. tldlist_iter_create() creates an iterator to enable you to iterate over the entries, independent of the data structure chosen for representing the list.
- tldlist_iter_next() returns the next TLDNode in the list, or NULL if there are no more entries.
- tldnode_tldname() returns the string for the TLD represented by this node.
- tldnode_count() returns the number of log entries that were counted for that TLD.

## tldmonitor.c:
- The mainline functionality of tldmonitor.c consists of the following pseudocode:
```
process the arguments
create a TLD list
if no file args are provided
    process stdin
else for each file in the argument list
    open the file
    process the file
    close the file
create an iterator
while there is another entry in the iterator
    print out the percentage associated with that TLD
destroy the iterator
destroy the Date structures
destroy the TLDList
```
- A static function process is provided to process all of the log entries in a particular log file.

## How to use:
1. Usage:
```
./program <Begin date> <End date> <input>
```
> Input: If input is empty or '-', it will use the standard input. Otherwise, it will use the input file. E.g. '< small.txt'
> Begin and End date: The dates we want to evaluate log file between. E.g 01/01/2000 01/09/2020

2. To run the program:
``` 
$ make tldmonitor  
$ ./tldmonitor <Begin date> <End date> <input>
```

3. To compare results in Linux terminal:
```
$ make tldmonitor
$ ./tldmonitor <Begin date> <End date> <input> | sort -n | diff - <input>.out
```
For instance: 
```
./tldmonitor 01/01/2000 01/09/2020 <small.txt | sort -n | diff - small.out
```

4. To debug:
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
