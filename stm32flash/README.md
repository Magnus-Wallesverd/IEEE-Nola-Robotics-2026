# Debugger

- dont use square brackets or quotes... 

## Commands

- 'b' [function] | [address]
  - creates a break point at a function or address 
  - 'break' also works

- 's' or 'si'
  - s enters a function
  - si steps one assembly instruction
  - 'step' also works

- 'n' or 'ni'
  - n executes entire function
  - ni steps one assembly instruction & runs functions in one step
  -'next' also works 

- 'c'
  - continues until breakpoint or program end
  - continue also works

- 'r'
  - runs the program from beginning to break or end
  - 'run' also works

- 'l'  | [n] | [function] | [file-name:line#]
  - lists 10 lines of code
  - can be used alone with a number or a function or a filewith the line you want

- 'p' [variable name] | (type)address | $[register-name]
  - prints value of a variable
  - print a register value with leading $

- 'x'/ [size][format] [address]
  - examine memory
  - # is number of
  - size refers to bit length, i.e. (w)ord
  - format refers to representation i.e. he(x)

- 'bt'
  - backtrace shows function calls to the current point

- 'monitor reset halt'
  - resets and halts

- 'load' [path/to/file.elf]
  - loads a file to be debugged

- 'file' [path/to/file.elf]
  - select new file to be loaded and debugged

- 'q'
  - quit



