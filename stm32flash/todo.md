## function table  🙌 

### test

- a list of pointers to functions
  - how do i load the functions in here? (manual first?)
  
## task queue 🙌

### test 🙅

- after function table is populated ISR/SVC enqueues
  - how big is the queue? 
  - what happens when it gets full?
- queue should be filled with struct pointers
  - pointers to a struct that contain a pointer to a function

## worker function explanation 🙌

### test

- each thread has a worker function 
- the worker pops a function off the queue

## thread states

### test

- threads state should be set before worker accesses the queue
  - think of some clever way to disable threads if queue is empty
  - alternatively just do some system calls if idle ? (could mess with timing)

## Library function

- maybe make enqueue/dequeue reusable?

## pretest realizations 😱

- race conditions exist if a thread accesses the queue right before an interrupt
- thread states wont help here because i could end up starving my threads

## cleanup system init

- it looks awful
