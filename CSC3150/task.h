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
typedef struct tasksCDT *tasksADT;

taskADT taskNew(const char *cmd);
tasksADT tasksNew(char *head,char *tail,int append);
tasksADT tasksInsert(tasksADT tasks, taskADT task);
int tasksExec(tasksADT tasks);
pid_t taskExec(taskADT task);
int taskSusspend(taskADT task);
int taskKill(taskADT task);


