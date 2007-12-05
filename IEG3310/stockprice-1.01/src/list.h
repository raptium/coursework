#ifndef LIST_H_
#define LIST_H_
typedef struct listCDT *listADT;

typedef void *listElementT;

listADT EmptyList();
listADT ListCons(listElementT head, listADT tail);
listElementT ListHead(listADT list);
listADT ListTail(listADT list);
int ListIsEmpty(listADT list);

#endif /*LIST_H_*/
