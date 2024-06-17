#ifndef QUEUE_H
#define QUEUE_H
#include "defs.h"

typedef struct name_node{
    char name[PATHNAME_SIZE];
    struct name_node* next;
}NAME_NODE;

typedef struct name_queue{
    NAME_NODE dummyhead;
    NAME_NODE* tail;
}NAME_QUEUE;


void name_queueInit(NAME_QUEUE* queue){
    queue->tail=&(queue->dummyhead);
}

void enname_queue(NAME_QUEUE* queue, char* name){
    NAME_NODE* tempNode=(NAME_NODE*)malloc(sizeof(NAME_NODE));
    strcpy(tempNode->name,name);
    tempNode->next=NULL;
    queue->tail->next=tempNode;
    queue->tail=tempNode;
}

char* dename_queue(NAME_QUEUE* queue){
    NAME_NODE* tempNode=queue->dummyhead.next;
    if(tempNode==NULL)
        return NULL;
    char* retName=(char*)malloc(PATHNAME_SIZE);
    strcpy(retName,tempNode->name);

    queue->dummyhead.next=queue->dummyhead.next->next;
    free(tempNode);
    

    queue->tail=&(queue->dummyhead);
    while(queue->tail->next!=NULL)
        queue->tail=queue->tail->next;
    return retName;
}

int isNameQueueEmpty(NAME_QUEUE* queue){
    if(queue->tail==&(queue->dummyhead))
        return 1;
    else
        return 0;
}

void clearname_queue(NAME_QUEUE* queue){
    while(dename_queue(queue)!=NULL);
}

#endif