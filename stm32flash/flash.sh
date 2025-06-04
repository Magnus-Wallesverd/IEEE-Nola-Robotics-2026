#! /bin/zsh

# Simulation compiler 
# -o is output 

gcc -Iinclude src/main.c src/gpio.c -o build/sim.elf

# run 
./build/sim.elf
