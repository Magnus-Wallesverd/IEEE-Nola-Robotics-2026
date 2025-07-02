function table 

- a list of pointers to functions
  - how do i load the functions in here? (manual first?)
  
task queue

- after function table is populated ISR/SVC enqueues
  - how big is the queue? 
  - what happens when it gets full?

worker function explanation

- each thread has a worker function 
- the worker pops a function off the queue
