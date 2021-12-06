#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// =======================      process struct     =================================
typedef struct process{
    int n;
    int stage;
    int runTime;
    bool isBlocked;
    int blockedTime;
    int r1, r2, b1, b2;
} PROC;


// =======================      queue variables & methods     ==================================
int size = 0;
int finished = 0;
PROC **b; //blocked array
PROC **q; //queued array
PROC **ready; //ready array
int front = 0;
int end = -1;
int qCount = 0;
int bCount = 0;

bool isEmpty(){
    if(qCount == 0) return true;
    return false;
}

void enqueue(PROC *p){
    if(end == size-1){
        end = -1;
    }
    qCount++;
    q[++end] = p;
}

PROC * dequeue(){
    if(isEmpty()) return NULL;
    PROC *process = q[front++];
    if(front == size){
        front = 0;
    }
    qCount--;
    return process;
}

//for increasing size (if lists were not the same size as #of processes)
void increaseSize(int newSize){
    size = newSize;
    q = (PROC **)realloc(q, size*(sizeof(PROC*)));
}


// =======================      scheduling alg methods     ==================================
void insert(PROC *p, int n){ //insert the process into the tentative ready list; n is how many processes are in the list
    int i = 0;
    PROC *temp = ready[i];

    while(temp != NULL){
        if (p->n > temp->n){
            temp = ready[++i];
            continue;
        }
        //then we know either the process should go before i because p's n < arr[i]'s n
        //thus insert into i by moving everything else up by 1
        for (int index = n-1; index >= i; index--){
            ready[index+1] = ready[index];
        }
        //finally insert into i
        ready[i] = p;
        return;
    }
    //otherwise current i is null and p's n is the largest so far so insert into current i
    ready[i] = p;
}

//check for any blocked processes; if there exists a blocked process, decrement its blockedtimer and
//handle any necessary movement to the ready list
void handleBlockedProcess(){
    if (finished >= size){ //base
        printf("\n\n");
        return;
    }

    //keep a record of processes that are unblocked this cycle
    int unblockCount = 0;
    if (ready[0] != NULL) unblockCount++; //if the ready list already has an item from (round robin) running

    if (bCount > 0){ //if there are blocked processes, decrement their blocked timer
        for (int i = 0; i < size; i++){
            if (!b[i]->isBlocked) continue;

            if ((b[i]->blockedTime -= 1) == 0){
                printf("\nP%d, time left: %d cycles (about to be ready)", b[i]->n, b[i]->blockedTime);
                insert(b[i], unblockCount);
                
                b[i]->stage += 1; //go to the next stage
                
                //unblock the process that is about to be queued
                b[i]->isBlocked = false; 
                unblockCount++;
                bCount--;
            }
            else if (b[i]->blockedTime >0){
                printf("\nP%d, time left: %d cycles", b[i]->n, b[i]->blockedTime);
            }
        }
    }
    //finally enqueue the processes in order
    for(int i = 0; i < unblockCount; i++){
        enqueue(ready[i]);
        ready[i] = NULL;
    }
    printf("\n\n");
}

void getRuntime(PROC *p){
    if (p->runTime > 0) return; //if this method is called w/ rr process thats not done running, return

    if(p->stage == 0){
        p->runTime = p->r1;
    }
    else if(p->stage == 1){
        p->runTime = p->r2;
    }
}

void rrAlg(int quantum){
    //local variables
    int cycle = 1;
    int clock = quantum; //backwards clock
    if (quantum == 0) clock = -1;
    PROC *running;
    PROC *temp;

    //set running to first process
    running = dequeue();
    getRuntime(running);

    //loop
    while(true){
        while(!isEmpty() || bCount != 0 || running->runTime >= 0){
            if (finished == size){ //base
                printf("\n---------------\nScheduling terminated.\n---------------\n");
                return;
            }

            //reset bools
            bool blocked = false;
            bool preempted = false;

            //see if there is a new process, if not then handle blocked and continue
            if (running == NULL && temp == NULL) {
                if (quantum != 0) clock = quantum; //if there is no running process and no new process, reset the quantum
                temp = dequeue();
                if (temp == NULL){
                    printf("\n\nCycle %d\n---------------\nRunning: None.\n\nBlocked: ", cycle++);
                    handleBlockedProcess();
                    if (qCount == 1) running = dequeue();
                    else continue;
                }
                else running = temp;
            }
            temp = NULL; //reset temp

            //step 1: decrement clock and run the process if need be (or just run process if its fifo)
            if (quantum == 0 || clock > 0){
                running->runTime -= 1;
                if (quantum != 0) clock--;
            }
            if (clock == 0) clock = quantum;

            //step 2: set a new process if the previous process is now done running or if clock has been reset
            if (running->runTime <= 0 || clock == quantum){
                //if process is not done running, this is rr, and we add it to the tentative ready list
                if (running->runTime > 0){
                    preempted = true;
                }
                //otherwise adjust process's timers if its actually done running
                else if (running->runTime <= 0){
                    blocked = true;
                    if (quantum != 0) clock = quantum; //reset the clock if need be
                    // printf("on cycle %d P%d", cycle, running->n);
                    if (running->stage == 0) {
                        running->runTime = running->r2; //set up timer for after next block
                        running->blockedTime = running->b1; //block previous running process
                    }
                    else if(running->stage == 1){
                        running->runTime = 1; //termination ready after next block
                        running->blockedTime = running->b2; //block previous running process
                    }
                    else if(running->stage == 2){
                        //this process is finished
                        finished++;
                        blocked = false;
                        // printf("\n\non cycle %d process %d finished w/ stage %d\nThe number of finished processes is %d\n", cycle, running->n, running->stage, finished);
                    }
                }
            }
                            
            //step 3: print + calculate blocked times
            if (running->stage >2){
                printf("\n\nCycle %d\n---------------\nRunning: None.\n\nBlocked: ", cycle++);
            }
            else if (running->stage ==2){
                running->stage +=1;
                printf("\n\nCycle %d\n---------------\nRunning: P%d (Termination) \n\nBlocked: ", cycle++, running->n);
            }
            else printf("\n\nCycle %d\n---------------\nRunning: P%d\n\nBlocked: ", cycle++, running->n);
            
            //step 4: do any final readying/blocking for the process that ran this cycle + find the next process
            if(!blocked && preempted){ //round robin; process w/ time left
                ready[0] = running;
                if ((temp = dequeue()) != NULL){
                    getRuntime(temp);
                }
                running = temp;
            }
            //readying should happen before handling any blocked (since we may add to the ready list here);
            //blocking the running process will happen at end of cycle
            handleBlockedProcess();
            
            if(blocked){//block process if necessary
                running->isBlocked = true;
                bCount++;
                if ((temp = dequeue()) != NULL){ //get the next process
                    getRuntime(temp);
                }
                running = temp;
            }
        }
    }
}

// ================    main    ======================
int main(int argc, char* argv[])
{
    if(argc==1){
        printf("\nPlease pass input with ./lab2 <input here>\n\n");
        return 0;
    }

    int n = atoi(argv[1]);
    size = n;

    int sa = atoi(argv[2]);
    int counter = 0;
    int quant = 0;

    if (sa == 2){ //if scheduling algorithm is rr, then there is a quantum provided
        quant = atoi(argv[3]);
        counter = 4;
    }
    else counter = 3; //for moving through argv

    //create array/"queue"
    q = (PROC **)malloc(size * sizeof(PROC*));
    //"blocked list" (b basically holds all the processes)
    b = (PROC **)malloc(size * sizeof(PROC*));
    //ready list for tentative ready (for "sorting")
    ready = (PROC **)malloc(size * sizeof(PROC*));

    //get the values of the processes && queue all processes
    
    for (int pNum = 0; pNum < n; pNum++){
        //initialize process
        PROC *process = (PROC *)malloc(sizeof(PROC));
        process->n = pNum+1;
        process->blockedTime = 0;
        process->runTime = 0;
        process->r1 = atoi(argv[counter++]);
        process->b1 = atoi(argv[counter++]);
        process->r2 = atoi(argv[counter++]);
        process->b2 = atoi(argv[counter++]);
        enqueue(process);
        b[pNum] = q[pNum]; //processes are currently in order -> we never dequeue from blocked list
    }

    rrAlg(quant);

    free(ready);
    free(b);
    free(q);

    return 0;
}
