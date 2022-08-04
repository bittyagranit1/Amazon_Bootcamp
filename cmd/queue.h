#ifndef QUEUE_H
#define QUEUE_H
#include<semaphore.h>

typedef struct node{
    void* data;
    struct node* next;
}Node;
typedef struct queue{
    Node*head;
    Node*tail;
    int capacity;
    int counter;
    sem_t sema;
}Queue,*p_queue;
int enqueue(Queue* queue,Node* node);
Node* dequeue(Queue*);
Node* createNode(void*);
Queue*createQueue(int);
void freeNode(Node*);
int isEmpty(Queue* queue);
void freeQueue(Queue*);
Node* top(Queue* queue);
#endif // QUEUE_H
