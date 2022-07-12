================================================
ABOUT
================================================
This lab simulates the scheduling of n processes, Pi, 0 < i <= n.
There are two versions, FIFO, and Round Robin. Each simulation includes two running and two blocking sessions for each process, then a final termination run for 1 cycle.


================================================
How TO RUN
================================================
Please create a new executable with the following:

gcc -std=c99 -o lab2 scheduling.c

Input:
n sa q r1_1 b1_1 r2_1 b2_1 ... r1_n b1_n r2_n b2_n

where
n 	<- number of processes
sa 	<- the scheduling algorithm to use; 1 signifies FIFO; 2 signifies RR.
q 	<- quantum for RR (this only exists if sa is 2)
r1_i 	<- the first running time for process i
b1_i 	<- the first blocking time for process i
r2_i 	<- the second running time for process i
b2_i 	<- the second blocking time for process i


To run the program use the following:
./lab2 <input here>

example: 
for FIFO  ./lab2 2 1 3 2 3 2 3 2 3 2
for RR    ./lab2 2 2 2 3 2 3 2 3 2 3 2
