# E20 16-Bit Architecture Assembler & Simulator
Attached is a C++ program that reads a file containing assembly language instructions (with a .s suffix) and then processes and assembles them into machine code. Attached are also sample E20 instructions.

The credit for the E20 processor goes to Professor Jeffrey Epstein.

## Overview
This project implements an E20 processor and assembler for practicing computer architecture concepts. It includes an E20 assembler written in C++ that translates assembly language instructions into machine code compatible with the E20 processor.

## Features
Reads assembly language instructions from a file with a .s extension.
Translates assembly instructions into machine code.
Supports various instruction types and addressing modes.

## Usage
Commands to run the project, I used Anubis LMS:
cd custom-processor-assembler
g++ assembler.cpp -o assembler
./assembler input.s

Replace input.s with the filename containing your assembly code.

