# E20 16-Bit Architecture Assembler & Simulator

## Overview
Attached is a C++ program that reads a file containing assembly language instructions (with a .s suffix) and then processes and assembles them into machine code. The simulator is a program that will execute E20 machine language. Normally, machine language would be executed by a processor, but for simplicity, we will reproduce the behavior of an E20 processor in software. The final values of the registers, the program counter, and the memory cells will be displayed at the end of the simulation. 

The credit for the E20 processor architecture goes to Professor Jeffrey Epstein.

## Features
Reads assembly language instructions from a file with a .s extension.
Translates assembly instructions into machine code.
Supports various instruction types and addressing modes.

## Usage
Commands to run the project, I used Anubis LMS:
g++ -Wall -o assembler ass.cpp
./assembler input.s

Replace input.s with the filename containing your assembly code.
The terminal would print out the machine language, save this output to a file, and name it output.s.

g++ -Wall -o simulator sim.cpp
./simulator output.s

Attached are also sample E20 instructions to experiment with. 
