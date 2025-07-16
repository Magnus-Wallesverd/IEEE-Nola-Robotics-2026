#include <stdio.h>
#include <stdint.h>
#define SIZE 3

void test1(void* args){
    
    printf("hello world 1\n");
}

void test2(void* args){
    printf("hello world 2\n");
}

void test3(void* args){
    printf("hello world 3\n");
}

typedef void (*func_t)(void*);

    // tasks table
func_t fn_table[SIZE]={
    test1,
    test2,
    test3
};

typedef struct{
    uint32_t size;
    uint32_t count;
    void**   front;
    void**   end;
    void**   array;
}queue_t;

typedef struct{
    func_t fn;
    void* args;
}data_t;

void* arr[SIZE];
data_t task_pool[SIZE];
queue_t queue;
queue_t* q = &queue;

void enqueue(queue_t* q, void* args){
    if(q->count == SIZE){
        return;
    }else if(q->end <= q->array+q->size-1){
        *(q->end) = args;
        q->end++;
        q->count++;
    } else {
        q->end = q->array;
        *(q->end) = args;
        q->end++;
        q->count++;
    }
}

void* dequeue(queue_t* q){
    // item gets set first
    //data_t* item = *front;
    void* args = *(q->front);
    if(q->count == 0){
        return (void*)0;
    }else if(q->front<=q->array+q->size-1){
        *(q->front) = (void*)0;
        q->front++;
        q->count--;
        return args;
    } else {
        q->front = q->array;
        *(q->front) = (void*)0;
        q->front++;
        q->count--;
        return args;
    }
}

void queue_print(queue_t* q){
    printf("array ptr: %p\n",q->array);
    printf("front ptr: %p\n",q->front);
    printf("end ptr:   %p\n",q->end);
    printf("size:  %x\ncount: %x\n",q->size,q->count);
    for(int i=0; i<q->size;i++){
        printf("queue[%d] = %p -> %p\n",i,q->array+i,q->array[i]);
    }
}

// initializes tasks into work items into a work pool
void queue_init(queue_t* q){
    q->array = arr;
    q->size  = SIZE;
    q->count = 0;
    q->front = q->array;
    q->end   = q->array;
    
    for(int i=0;i<SIZE;i++){
        task_pool[i].fn=fn_table[i];
    }
    /*
    for(int i=0; i<SIZE; i++){
        enqueue(q, fn_table[i]);
    }
    */
    for(int i=0; i<SIZE; i++){
        enqueue(q, &task_pool[i]);
    }
}

int main() {
    queue_print(q);
    queue_init(q);
    queue_print(q);
    data_t* temp_ptr;
    for(int i=0;i<SIZE;i++){
        temp_ptr = (data_t*)dequeue(q);
        temp_ptr->fn(temp_ptr->args);
    }
    queue_print(q);
    // ((data_t*)dequeue(q))->fn(args);
    /*
    for(int i=0;i<q->size;i++){
        (*(func_t)dequeue(q))(args);
        queue_print(q);
    }
    */
    // printf("%p\n",task_pool[0]);
    return 0;
}
