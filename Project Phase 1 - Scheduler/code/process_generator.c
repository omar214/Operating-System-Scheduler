#include "headers.h"
#include "process_model.h"
void clearResources(int);
void initResources();
struct linked_list *readInputFile(char *);
int scheduler_msgq = -1;


int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // 1. Read the input files.
    struct linked_list *processes = readInputFile(argv[1]);

    //debug
    struct node *pp = processes->head;
    while (pp)
    {
        process *pr = (process *)pp->data;
        printf("%d\t%d\t%d\t%d\t\n", pr->pid, pr->arrival_time, pr->run_time, pr->priority);
        pp = pp->next;
    }

    // 2. Read the chosen scheduling algorithm and its parameters, if there are any from the argument list.
    enum Scheduling_Algorithms algo_num = atoi(argv[2]);
    int quanta = -1;
    if (algo_num == RR)
        quanta = atoi(argv[3]);
    
    // 3. Initiate and create the scheduler and clock processes.
    int scheduler_pid = fork();
    if (scheduler_pid == 0)
    {
        //converting args to strings
        char str_qua[2];
        sprintf(str_qua, "%d", quanta);
        execl("./scheduler.out", argv[2], str_qua, NULL);
    }
    
    int clk_pid = fork();
    if (clk_pid == 0)
        execl("./clk.out", NULL);
    
    //initialize clock.
    initClk();

    // To get time use this function: getClk()
    int curr_proc = 0;
    int curr_time = -1;

    initResources();

    //start simulation
    struct node *next_process = processes->head;
    while (1 /* or curr_proc < num of procs*/)
    {
        while (curr_time == getClk())
        {
            //wait
        }
        curr_time++;
        printf("Proc gen, Time: %d\n", curr_time);

        //TODO: implement this: (Sending of "arrived" procs)
    while (next_process && ((process *)next_process->data)->arrival_time == curr_time)
    {
        
        msgsnd(scheduler_msgq, ((process *)next_process->data), sizeof(process), !IPC_NOWAIT);
        printf("Proc gen, process with ID: %d has just arrived. \n\n ", ((process *)next_process->data)->pid);
        next_process = next_process->next;
    }
        /*
        while (PROCCESES[crr_proc].arrival_time >= curr_time)
            //send curr proc to scheduler
            curr_proc++;
        */
    }

    // 7. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    if (scheduler_msgq == -1) //not iniitaized yet
        msgctl(scheduler_msgq, IPC_RMID, NULL);
    //cascade the signal to group
    killpg(getpgrp(), signum);

    exit(0);
}

void initResources()
{
    key_t key = ftok("Makefile", 'p');

    scheduler_msgq = msgget(key, 0666 | IPC_CREAT);
}
struct linked_list *readInputFile(char *path)
{
    char *input_file_path = path;

    FILE *fptr;
    fptr = fopen(input_file_path, "r");
    if (fptr == NULL)
    {
        printf("Can't Open INPUT File!\n");
        return NULL;
    }

    /*{
        char *line = NULL;
        int len = 0;
        while (getline(&line, 0, fptr) != -1)
        {
            printf("Retrieved line: %s", line);
            fflush(stdout);
        }
    }
    */

    struct linked_list *processes = new_linked_list();
    read_all_processes(fptr, processes);
    fclose(fptr);
    return processes;
}

