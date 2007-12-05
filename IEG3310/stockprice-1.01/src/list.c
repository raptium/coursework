/*******************************************************************************
 *
 * list.c
 * CSC2100 Data Struture
 * Assignment 2
 *
 * Guan Hao
 * 05569511
 * raptium[AT]gmail.com
 *
 * 3 Feb, 2007
 *
 * Implementation for listADT.
 * Some codes are copied from the lecture and tutorial notes with permission.
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "list.h"

struct listCDT {
    listElementT head;
    listADT tail;
};

// EmptyList creates an empty list.
listADT EmptyList() {
    return NULL;
}

// ListCons constructs list form head and tail provided.
listADT ListCons(listElementT head, listADT tail) {
    listADT list;

    list = (listADT) malloc(sizeof(struct listCDT));
    list->head = head;
    list->tail = tail;
    return list;
}

// ListHead returns the head of the list.
listElementT ListHead(listADT list) {
    if (ListIsEmpty(list))
        exit(0); // Call ListHead on an empty list is not allowed!

    return list->head;
}

// ListTail returns the tail of the list.
listADT ListTail(listADT list) {
    if (ListIsEmpty(list))
        exit(0); // Call ListTail on an empty list is not allowed!

    return list->tail;
}

// ListIsEmpty returns boolean value wheather the list is an empty list.
int ListIsEmpty(listADT list) {
    return (list == NULL);
}
