#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <sys/wait.h>

void perform_computation() {
    volatile long int i;
    volatile long int x;
    for (i = 0; i < 1000000000; i++){
        x += i;
    }
}


pid_t create_process(int priority, int policy) {
    pid_t pid = fork();
    if (pid == 0) {

        struct sched_param param;
        param.sched_priority = priority;

        if (sched_setscheduler(0, policy, &param) == -1) {
            perror("sched_setscheduler");
            exit(EXIT_FAILURE);
        }

//        printf("Child process %d started with priority %d and policy %d\n", getpid(), priority, policy);


        perform_computation();

        printf("Child process %d finished with priority %d and policy %d\n", getpid(), priority, policy);
        exit(EXIT_SUCCESS);
    }
    return pid;
}

int main() {
    volatile long int i;
    const int num_processes = 10;
    pid_t pids[num_processes];
    int priorities[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int policies[] = {SCHED_FIFO, SCHED_RR, SCHED_OTHER};

    printf("Creating child processes with different priorities...\n\n");

    for ( i = 0; i < num_processes; i++) {
        pids[i] = create_process(priorities[i], SCHED_FIFO);
    }

    for ( i = 0; i < num_processes; i++) {
        waitpid(pids[i], NULL, 0);
    }

    printf("\nCreating child processes with different scheduling algorithms...\n\n");

    for ( i = 0; i < num_processes; i++) {
        int policy = policies[i % 3];
        pids[i] = create_process(1, policy);
    }

    for ( i = 0; i < num_processes; i++) {
        waitpid(pids[i], NULL, 0);
    }

    printf("\nAll child processes with different scheduling algorithms finished.\n");

    return 0;
}