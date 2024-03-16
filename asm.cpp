/*
Maria Benhammouda
asm.cpp
*/
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <bitset>
#include <algorithm>
#include <iomanip>
using namespace std;
 
 //function to return the last 4 bit immediate for the opcodes add,sub,or,and,slt,jr 
inline int GetImmediate(string &opcodestring ){
   return opcodestring=="add"?  0
   : opcodestring=="sub"?  1
   : opcodestring=="or"?  2
   : opcodestring=="and"?  3
   : opcodestring=="slt"?  4
   : opcodestring=="jr"?  8
   :-1; 
}
/*
function takes the immediate label string and retunrs the address of the label
*/
int FindLabelIndex(vector<string>&label,vector<string>&opcode, string &wordfind) {
    //remove all spaces and tabs around the string and make it lower case for comparision
    std::transform(wordfind.begin(), wordfind.end(), wordfind.begin(), ::tolower);
    wordfind.erase(std::remove_if(wordfind.begin(), wordfind.end(),[](char c) { return (c == '\t')|| (c == ' ') ; }),wordfind.end());
    int index=0, count=-1, labelsize=label.size();
    for (int y = 0; y < labelsize; y++) {
        if (label[y].find(wordfind+":") != std::string::npos) { //if the immediate label is matched to label definition
             index=y; //assign the position of the label definition to the index
            for (int i = 0; i <= index; i++) { // search through up all the lines before the definition,
                if (opcode[i].find_first_not_of(' ') != std::string::npos) { 
                count++;                
                }
            } // if the label definition is on a seperate line as instruction, then its value is the label index +1
            if(opcode[y].find_first_not_of(' ') == std::string::npos){ 
                return count+1;
             }
            else {
                return count;
             }// if there is no instruction on the line as the label definition, then the address is the index where the label is defined
         }
        }
          return 0; 
     }
/**
    print_line(address, num)
    Print a line of machine code in the required format.
    Parameters:
        address = RAM address of the instructions
        num = numeric value of machine instruction 
    */
void print_machine_code(unsigned address, unsigned num) {
    bitset<16> instruction_in_binary(num);
    cout << "ram[" << address << "] = 16'b" << instruction_in_binary <<";"<<endl;
}
/**
    Main function
    Takes command-line args as documented below
*/
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
        cerr << "Assemble E20 files into machine code" << endl << endl;
        cerr << "positional arguments:" << endl;
        cerr << "  filename    The file containing assembly language, typically with .s suffix" << endl<<endl;
        cerr << "optional arguments:"<<endl;
        cerr << "  -h, --help  show this help message and exit"<<endl;
        return 1;
    }
    /* iterate through the line in the file, construct a list
       of numeric values representing machine code */
    ifstream f(filename);
    if (!f.is_open()) {
        cerr << "Can't open file "<<filename<<endl;
        return 1;
    }
    /* our final output is a list of ints values representing
       machine code instructions */
    vector<unsigned> instructions;
    string comments; 
    int num_of_lines_in_file = 0; 
/* iterates through the file and removes any comments, meanwhile it counts number of lines in file to be used later for arrays and while loops
*/
   while (getline(f, comments)) {
   
       size_t pos = comments.find("#");
        if (pos != string::npos){
            comments = comments.substr(0, pos);
        }
        if(comments.empty()){ // if the line is empty, skip and dont increment line counter 
            continue; 
        }
    num_of_lines_in_file++;
  }

//bring the file pointer up to iterate through file 
  f.clear();
  f.seekg(0);
  vector <string> opcodetext(num_of_lines_in_file);
  vector <int> opcodeBinary (num_of_lines_in_file);
  vector <string > labels(num_of_lines_in_file);
  string line, trimmed_Label= "";
  int LineCounter=0;
  int16_t imm=0;
  string RgSrcA[num_of_lines_in_file], RgSrcB[num_of_lines_in_file],RgDst[num_of_lines_in_file], imm_string[num_of_lines_in_file];
     while (getline(f, line)) {
        size_t pos = line.find("#");
        if (pos != string::npos){
            line = line.substr(0, pos);
        } // removes comments
         
        if(line.empty()){
            continue;
        }
        //remove any leading spaces from file line
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(0, line.find_first_not_of("  "));
    // if the line contains a label definition, parse the line with respect to ":"
     if(line.find(':')){ 
     size_t pos = line.rfind(':');
     if (pos != string::npos) {
        // Extract the label from the line
        trimmed_Label= line.substr(0, pos + 1);
        transform(trimmed_Label.begin(), trimmed_Label.end(), trimmed_Label.begin(),[](unsigned char c){ return std::tolower(c); });
        labels[LineCounter]= trimmed_Label; // add the label definition to the array of labels, this keeps count of all definitions of array and is used in finding the label address
        // Remove the label from the line
        line.erase(0, pos + 1);
         }
         //removes any additional space before the opcode if there was a label before the opcode on the same line
         line.erase(0, line.find_first_not_of(" "));
         line.erase(0, line.find_first_not_of("\t"));
        }
        //facilitates breaking apart the string 
         stringstream ss(line); 
        //store the first word as the opcode and transform it by removing spaces and making all characters lower case
         getline(ss, opcodetext[LineCounter], ' ');
         transform(opcodetext[LineCounter].begin(), opcodetext[LineCounter].end(), opcodetext[LineCounter].begin(), ::tolower);
         opcodetext[LineCounter].erase(std::remove_if(opcodetext[LineCounter].begin(), opcodetext[LineCounter].end(), [](char c) { return (c == '\t')|| (c == ' ') ; }),opcodetext[LineCounter].end());
      // set register & imm to 0, will be overwritten if the value is needed for that opcode
         imm_string[LineCounter]="0";
         RgSrcA[LineCounter]="0";
         RgSrcB[LineCounter]="0";
         RgDst[LineCounter]="0";
         opcodeBinary[LineCounter]=-5; //intialize opcode to invalue opcode, -5 cannot be represented in 3 bits
    /* if statements, based on the opcode, the line is parsed  and diffrent registers are used*/
      
        if (opcodetext[LineCounter] == "add" || opcodetext[LineCounter] == "sub" ||
         opcodetext[LineCounter] == "or" || opcodetext[LineCounter] == "and" ||
         opcodetext[LineCounter] == "slt" ){
         opcodeBinary[LineCounter] =0; 
         getline(ss, RgDst[LineCounter], ',');            
         getline(ss, RgSrcA[LineCounter], ',');     
         getline(ss, RgSrcB[LineCounter], '\n');
        }
        else if( opcodetext[LineCounter] == "slti"){
        opcodeBinary[LineCounter]=7;
        getline(ss, RgDst[LineCounter], ',');
        getline(ss, RgSrcA[LineCounter], ',');
        getline(ss, imm_string[LineCounter], '\n');            
        }
        else if( opcodetext[LineCounter] == "lw"){
        opcodeBinary[LineCounter] = 4;
        getline(ss, RgDst[LineCounter], ',');
        getline(ss, imm_string[LineCounter], '(');
        getline(ss, RgSrcA[LineCounter], ')');
           }
        else if( opcodetext[LineCounter] == "sw"){
        opcodeBinary[LineCounter] = 5;
        getline(ss, RgDst[LineCounter], ',');
        getline(ss, imm_string[LineCounter], '(');
        getline(ss, RgSrcA[LineCounter], ')');
        }
        else if ( opcodetext[LineCounter] == "jeq"){
        opcodeBinary[LineCounter] = 6;
        getline(ss, RgSrcA[LineCounter], ',');    
        getline(ss, RgSrcB[LineCounter], ',');
        getline(ss, imm_string[LineCounter], '\n');
        }
        else if (opcodetext[LineCounter]== "addi"){
        opcodeBinary[LineCounter] = 1;
        getline(ss, RgDst[LineCounter], ',');
        getline(ss, RgSrcA[LineCounter], ',');
        getline(ss, imm_string[LineCounter], '\n');
        }
        else if (opcodetext[LineCounter]=="j"){
        opcodeBinary[LineCounter] = 2; 
        getline(ss, imm_string[LineCounter], '\n');
        }
        else if (opcodetext[LineCounter]=="jr"){
        opcodeBinary[LineCounter] = 0; 
        getline(ss, RgSrcA[LineCounter], '\n');
        }
        else if (opcodetext[LineCounter]=="jal"){
        opcodeBinary[LineCounter] = 3; 
        getline(ss, imm_string[LineCounter], '\n');
        }
        else if (opcodetext[LineCounter]=="movi"){
        opcodeBinary[LineCounter] = 1; 
        getline(ss, RgDst[LineCounter], ',');        
        getline(ss, imm_string[LineCounter], '\n');
        }
        else if (opcodetext[LineCounter]==".fill"){ 
        getline(ss, imm_string[LineCounter], '\n');   
        }
        else if (opcodetext[LineCounter]=="halt"){
        opcodeBinary[LineCounter] = 2; 
        }
        LineCounter++;
    } 
    f.close();
    //reset the counter to go through the file lines 
    LineCounter=0;
    int RgD=0, RgA=0, RgB=0, pc=0;
    while (LineCounter<num_of_lines_in_file) {
        //remove unnecessary characters and keep only the integer value of the register, then convert string to int
    RgDst[LineCounter].erase(std::remove_if(RgDst[LineCounter].begin(), RgDst[LineCounter].end(),[](char c) { return (c == '\t') || (c == '$')|| (c == ' ')  || (c == '(') || (c == ')') || (c == ','); }),RgDst[LineCounter].end());
    RgSrcA[LineCounter].erase(std::remove_if(RgSrcA[LineCounter].begin(), RgSrcA[LineCounter].end(),[](char c) { return (c == '\t') || (c == '$') || (c == ' ') || (c == '(') || (c == ')') || (c == ','); }),RgSrcA[LineCounter].end());
    RgSrcB[LineCounter].erase(std::remove_if(RgSrcB[LineCounter].begin(), RgSrcB[LineCounter].end(),[](char c) { return (c == '\t') || (c == '$')|| (c == ' ')  || (c == '(') || (c == ')') || (c == ','); }),RgSrcB[LineCounter].end());
    //get the value of the register as integer
    RgD=stoi(RgDst[LineCounter]);
    RgA=stoi(RgSrcA[LineCounter]);
    RgB=stoi(RgSrcB[LineCounter]); 
       
       //transfrom the immediate to lower case and remove space and tabs
    transform(imm_string[LineCounter].begin(), imm_string[LineCounter].end(), imm_string[LineCounter].begin(), ::tolower);
    imm_string[LineCounter].erase(std::remove_if(imm_string[LineCounter].begin(), imm_string[LineCounter].end(),[](char c) { return (c == '\t')|| (c == ' ') ; }),imm_string[LineCounter].end());
   
   //check if the immediate is a label or an integer value
      if(!isdigit(imm_string[LineCounter][0]) && !(imm_string[LineCounter][0]== '-')) { //label
        imm= FindLabelIndex(labels,opcodetext,imm_string[LineCounter]);
      }
      else { //integer
        imm= stoi(imm_string[LineCounter]);
      }
      // compose the instruction 
        if(opcodetext[pc]== " "){
            continue;
        }
        else if (opcodetext[LineCounter]=="nop"){
            //add $0 $0 $0   
            instructions.push_back(0 );
             pc++;
        }
        else if (opcodetext[LineCounter]==".fill"){
            instructions.push_back(imm) ;
             pc++;
        }
        else if(opcodeBinary[LineCounter]!= -5) { //make sure the opcode is valid, if not its an empty line 
            switch(opcodeBinary[LineCounter]){
                case 0: //add,sub,or,and,slt,jr
                instructions.push_back((((((((opcodeBinary[LineCounter]<<3) | RgA)<<3)|RgB )<< 3 ) | RgD )<<4 )| GetImmediate(opcodetext[LineCounter]));
                pc++; // pc is the counter for the number of instructions called whereas Linecounter is the number of lines in file 
                break;
                case 4: //lw
                case 5: // sw
                case 7: //slti
                case 1: //addi // movi
                instructions.push_back((((((opcodeBinary[LineCounter]<<3) | RgA)<<3)|  RgD )<<7 ) | (imm & 127));
                pc++;
                break;
                case 6: //jeq
                 imm= imm-pc-1;
                instructions.push_back((((((opcodeBinary[LineCounter]<<3) | RgA)<<3)|  RgB )<<7 ) | (imm & 127));
                 pc++;
                 break;
                 case 2: // j //halt
                 case 3: //jal
                 if(opcodetext[LineCounter] == "halt" ){
                    instructions.push_back((opcodeBinary[LineCounter]<<13)| (pc & 127));
                    pc++;
                    break;
                 }
                 else{
                    instructions.push_back((opcodeBinary[LineCounter]<<13) | (imm & 8191));
                    pc++;
                    break;
                 }
        
            }
        }
         LineCounter++;   
    }  
    /* print out each instruction in the required format */
    unsigned address = 0;
    for (unsigned instruction : instructions) {     
        print_machine_code(address, instruction); 
        address ++;
    }
    return 0;
}
