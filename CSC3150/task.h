/*
 * CSC3150 Operating System
 * Assignment 1
 * Simple Unix Shell
 * 
 *
 * task.h
 * prototype
 *
 *
 */
#include <sys/types.h>
typedef struct taskCDT *taskADT;
typedef struct taskCDT *tasksADT;

taskADT taskNew(const char *cmd);
tasksADT tasksInsert(tasksADT tasks);
int tasksExec(tasksADT tasks);
pid_t taskExec(taskADT task,int input,int output);
int taskSusspend(taskADT task);
int taskKill(taskADT task);


