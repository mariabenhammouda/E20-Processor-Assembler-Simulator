/*
Maria Benhammouda
sim.cpp
*/

#include <bitset>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <math.h>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <regex>
#include <cstdlib>

using namespace std;

// Some helpful constant values that we'll be using.
size_t const static NUM_REGS = 8; 
size_t const static MEM_SIZE = 1<<13;
size_t const static REG_SIZE = 1<<16;

/*
    Loads an E20 machine code file into the list
    provided by mem. We assume that mem is
    large enough to hold the values in the machine
    code file.

    @param f Open file to read from
    @param mem Array represetnting memory into which to read program
*/
uint16_t signExtend7(bitset<7> x) {
    uint16_t result = static_cast<uint16_t>(x.to_ulong()); // Convert to uint16_t
    if (x[6] == 1) { // If the 7th bit is 1, sign extend
        result |= 0xFF80;
    }
    return result;
}

uint16_t signExtend13(bitset<13> x) {
    if (x[12] == 1) { // Check if 13th bit is 1
        x |= (0xFFFF << 13); // Set bits 13-15 to 1 (sign extension)
    }
    return static_cast<uint16_t>(x.to_ulong());
}

uint16_t zeroExtend13(bitset<13> x) {
    x &= 0x1FFF; // Bitwise AND with 0x1FFF (binary 0001111111111111)
    return static_cast<uint16_t>(x.to_ulong());
}


// responsible for executing all opcodes of 0
// imm4 holds the last 4 bits of the instruction
void executeopcode0(uint16_t &pc, vector<uint16_t> &RgSrcA, vector<uint16_t> &RgSrcB, vector<uint16_t> &RgDst, vector<uint16_t> &imm4, uint16_t (&reg)[8]) {
     if(imm4[pc&8191]==0){ //add
        if(RgDst[pc&8191]==0){
            reg[0]= 0;
        }else{
        reg[RgDst[pc&8191]]= (reg[RgSrcA[pc&8191]]+reg[RgSrcB[pc&8191]]);
         
        }
           pc=pc+1;
        }
  
     else if(imm4[pc&8191]==1){ // sub
        if(RgDst[pc&8191]==0){
            reg[0]= 0;
        }else { 
         reg[RgDst[pc&8191]]= reg[RgSrcA[pc&8191]]-reg[RgSrcB[pc&8191]];
            
        }
           pc=pc+1;
        }else if(imm4[pc&8191]==2){   // or
  
        if(RgDst[pc&8191]==0){
          reg[0]= 0;
        }else{

          reg[RgDst[pc&8191]]= (reg[RgSrcA[pc&8191]]|reg[RgSrcB[pc&8191]]);

        }
           pc=pc+1;
         }else if(imm4[pc&8191]==3){ //and

        if(RgDst[pc&8191]==0){
             reg[0]= 0;
        }else {
          reg[RgDst[pc&8191]]= (reg[RgSrcA[pc&8191]]&reg[RgSrcB[pc&8191]]);
        }
           pc=pc+1;
         }else if(imm4[pc&8191]==4){ // slt
      
        if(RgDst[pc&8191]==0){
             reg[0]= 0;
        }
        else {
           reg[RgDst[pc&8191]]= (reg[RgSrcA[pc&8191]]<reg[RgSrcB[pc&8191]]);
        }
           pc=pc+1;
        }
     else if(imm4[pc&8191]==8){ // jr
            //value of pc
          pc=reg[RgSrcA[pc&8191]];
        
        }
     else{ //if it is not any of the above, then it is a .fill, therefore we must increment coutner
         pc=pc+1;
        }
}

// addi opcode
void executeopcode1(uint16_t &pc, vector<uint16_t> &RgSrcA, vector<uint16_t> &RgSrcB, vector<uint16_t> &RgDst, vector<bitset<7>> &imm7, uint16_t (&reg)[8]) {
        if(RgSrcB[pc&8191]==0){
            reg[0]= 0;
        }else{
        
        reg[RgSrcB[pc&8191]]= (reg[RgSrcA[pc&8191]]+ signExtend7( imm7[pc&8191]));
           
        }
        
        pc=pc+1;
}

//jump and halt 
void executeopcode2(uint16_t &pc, bool &halt, const vector<bitset<13>> &imm13) {
    if (zeroExtend13(imm13[pc & 8191]) == pc) {
        halt = true; // if the pc is the smae as the immediate then this is a halt 
    }
    pc = zeroExtend13(imm13[pc & 8191]);
}

//jal
void executeopcode3(uint16_t &pc, uint16_t &reg7, const vector<bitset<13>> &imm13) {
    reg7 = pc + 1; //set the next pc in reg7 
    pc = zeroExtend13(imm13[pc & 8191]);
}

//lw
void executeopcode4(uint16_t &pc, uint16_t (&reg)[8], uint16_t (&memory)[MEM_SIZE], vector<uint16_t> &RgSrcA, vector<uint16_t> &RgSrcB, vector<bitset<7>> &imm7) {
    if(RgSrcB[pc & 8191] == 0){
        reg[0] = 0;
    }
    else {
        //only take the least significant 13 bits of the addition as the index of memory, since mem size is max 8191
        reg[RgSrcB[pc & 8191]] = memory[(signExtend7(imm7[pc & 8191]) + reg[RgSrcA[pc & 8191]]) & 8191];
    }
    pc = pc + 1;
}
// sw
void executeopcode5(uint16_t &pc, uint16_t (&reg)[8], uint16_t (&memory)[MEM_SIZE], vector<uint16_t> &RgSrcA, vector<uint16_t> &RgSrcB, vector<bitset<7>> &imm7) {
//store the value of the register in the memory
   memory[(reg[RgSrcA[pc&8191]]+signExtend7(imm7[pc&8191]))& 8191]= reg[RgSrcB[pc&8191]];
         pc=pc+1;
}

//jeq
 void executeopcode6(uint16_t &pc, uint16_t (&reg)[8], uint16_t (&memory)[MEM_SIZE], vector<uint16_t> &RgSrcA, vector<uint16_t> &RgSrcB, vector<bitset<7>> &imm7) {
        if(reg[RgSrcA[pc&8191]] == reg[RgSrcB[pc&8191]]){ // if the values in the registers are equal then jump to pc+imm+1 
            pc = pc+signExtend7(imm7[pc&8191])+1;
            }
        else {
          pc=pc+1;
        }
 }
 //slti
 void executeopcode7(uint16_t &pc, uint16_t (&reg)[8], uint16_t (&memory)[MEM_SIZE], vector<uint16_t> &RgSrcA, vector<uint16_t> &RgSrcB, vector<bitset<7>> &imm7) {

        if(RgSrcB[pc&8191]==0){
           reg[0]= 0;
        }
        else {
           reg[RgSrcB[pc&8191]]= reg[RgSrcA[pc&8191]] < signExtend7(imm7[pc&8191]);
        }
           pc=pc+1;
 }
/*
    Prints the current state of the simulator, including
    the current program counter, the current register values,
    and the first memquantity elements of memory.

    @param pc The final value of the program counter
    @param regs Final value of all registers
    @param memory Final value of memory
    @param memquantity How many words of memory to dump
*/
void print_state(uint16_t pc, uint16_t regs[], uint16_t memory[], size_t memquantity) {
    cout << setfill(' ');
    cout << "Final state:" << endl;
    cout << "\tpc=" <<setw(5)<< pc << endl;

    for (size_t reg=0; reg<NUM_REGS; reg++)
        cout << "\t$" << reg << "="<<setw(5)<<regs[reg]<<endl;

    cout << setfill('0');
    bool cr = false;
    for (size_t count=0; count<memquantity; count++) {
        cout << hex << setw(4) << memory[count] << " ";
        cr = true;
        if (count % 8 == 7) {
            cout << endl;
            cr = false;
        }
    }
    if (cr)
        cout << endl;
}

/**
    Main function
    Takes command-line args as documented below
*/




void load_data_from_file( ifstream &f, char *filename, uint16_t memory[], uint16_t &pc ,vector<uint16_t> &opcode,vector<uint16_t> &imm4,vector<bitset<7>> & imm7,  
vector<bitset<13>> &imm13,  vector<uint16_t> & RgSrcA,vector<uint16_t> & RgSrcB,vector<uint16_t> &RgDst  ){
    if (!f.is_open()) {
        cerr << "Can't open file "<<filename<<endl;
        exit (1);
    }

    string line;    // define the standard format of the instructions 
    regex machine_code_re("^ram\\[(\\d+)\\] = 16'b(\\d+);.*$");
    size_t expectedaddr = 0;
    while (getline(f, line)) {
        smatch sm;
        if (!regex_match(line, sm, machine_code_re)) {
            cerr << "Can't parse line: " << line << endl;
            exit(1);
        }
        size_t addr = stoi(sm[1], nullptr, 10);
        unsigned instr = stoi(sm[2], nullptr, 2);
        if (addr != expectedaddr) {
            cerr << "Memory addresses encountered out of sequence: " << addr << endl;
            exit(1);
        }
        if (addr >= MEM_SIZE) {
            cerr << "Program too big for memory" << endl;
            exit(1);
        }
       //set the value of each memory cell as the value of the 16 bit instruction
        memory[addr] = instr;
        // define the opcode as the first 3 bits
        opcode.push_back((memory[addr]>> 13) & 7) ;
        // define the opcode as the last 4 bits
        imm4.push_back(memory[addr]& 15);
        // define the opcode as the last 13 bits
        imm13.push_back(memory[addr]& 8191);
        // define the opcode as the last 7 bits
        imm7.push_back(memory[addr]& 127);

        RgSrcA.push_back((memory[addr]>> 10)& 7);
        RgSrcB.push_back(((memory[addr]) >>7)& 7);
        RgDst.push_back(((memory[addr]) >> 4)& 7); 
        expectedaddr ++;
        

       }
}

int main(int argc, char *argv[]) {
    /*
        Parse the command-line arguments
    */
    char *filename = nullptr;
    bool do_help = false;
    bool arg_error = false;
    for (int i=1; i<argc; i++) {
        string arg(argv[i]);
        if (arg.rfind("-",0)==0) {
            if (arg== "-h" || arg == "--help")
                do_help = true;
            else
                arg_error = true;
        } else {
            if (filename == nullptr)
                filename = argv[i];
            else
                arg_error = true;
        }
    }
    /* Display error message if appropriate */
    if (arg_error || do_help || filename == nullptr) {
        cerr << "usage " << argv[0] << " [-h] filename" << endl << endl; 
        cerr << "Simulate E20 machine" << endl << endl;
        cerr << "positional arguments:" << endl;
        cerr << "  filename    The file containing machine code, typically with .bin suffix" << endl<<endl;
        cerr << "optional arguments:"<<endl;
        cerr << "  -h, --help  show this help message and exit"<<endl;
        return 1;
    }

    ifstream f(filename);
    if (!f.is_open()) {
        cerr << "Can't open file "<<filename<<endl;
        return 1;
    }
       //define the necessary variables and constants       
    uint16_t memory[MEM_SIZE]={0};
    uint16_t pc=0;
    bool halt=false;
    vector<uint16_t> opcode,imm4;
    vector<bitset<7>>  imm7;
    vector<bitset<13>> imm13;
    uint16_t reg[NUM_REGS]={0};
    vector<uint16_t> RgSrcA, RgSrcB, RgDst;

    load_data_from_file(f,filename,memory,pc,opcode,imm4,imm7,imm13,RgSrcA,RgSrcB,RgDst);


    while(!halt){
    
       switch (opcode[pc&8191]) {
        case 0:
        executeopcode0(pc, RgSrcA,RgSrcB,RgDst,imm4, reg);
        break;
        case 1:
        executeopcode1(pc, RgSrcA,RgSrcB,RgDst,imm7, reg);
        break;
        case 2: 
       executeopcode2(pc,halt,imm13);
        break;
        case 3:
       executeopcode3(pc, reg[7],imm13);
        break;
        case 4:
       executeopcode4(pc,reg,memory,RgSrcA,RgSrcB,imm7);
        break;
        case 5:
       executeopcode5(pc,reg,memory,RgSrcA,RgSrcB,imm7);
        break;
        case 6:
       executeopcode6(pc,reg,memory,RgSrcA,RgSrcB,imm7);
        break;
        case 7:
        executeopcode7(pc,reg,memory,RgSrcA,RgSrcB,imm7);
        break;
    }
    }
    
print_state(pc, reg, memory, 128);
/* iterates through the file and removes any comments, meanwhile it counts number of lines in file to be used later for arrays and while loops
*/
  
    return 0;
}
//ra0Eequ6ucie6Jei0koh6phishohm9
