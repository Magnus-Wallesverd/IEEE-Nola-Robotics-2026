## function table 

- a list of pointers to functions
  - how do i load the functions in here? (manual first?)
  
## task queue

- after function table is populated ISR/SVC enqueues
  - how big is the queue? 
  - what happens when it gets full?
- queue should be filled with struct pointers
  - pointers to a struct that contain a pointer to a function

## worker function explanation

- each thread has a worker function 
- the worker pops a function off the queue

## Library function

- make a delete instance type function? (probably just for structs?)
