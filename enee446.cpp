 /*****************************************************************************/
 /*                     VESP COMPUTER-2.0-RISC-V EXTENSION                     *
 /*                            Author: A. Yavuz Oruc                           *
 /*                            Copyright  2021.                                *
 /*                      Sabatech Corporation (www.sabatech.com)               *
 /*                                All rights reserved.                        *
 /*Copying and compiling this program for personal use is permitted. However,  *
 /*no part of this program may be reproduced or distributed in any form or     *
 /*by any means, or stored in a database or  retrieval system, without the     *
 /*prior written permission of the Author and AlgoritiX Corporation.           *
 /*Neither Algoritix nor the Author makes any direct or implied claims         *
 /*that this program will work accurately and/or generate correct results,     *
 /*In addition, they provide no warranties direct or implied that using        *
 /*this program on any computer will be safe in so far as  guarding against    *
 /*destroying and/or deleting any information, data, code, program or file     *
 /*that may be stored in the memory of user's computer.                        *
 /*****************************************************************************/


/*      Please communicate all your questions and comments to                 */
/*                    A. Yavuz Oruc at yavuz@sabatech.com                        */


/*--------------------Changes and additions in this version-------------------*/
/*                                                                            */
/* -This edition of VESP provides an extension to RISC-V.                     */
/* Warning!!!! This program overwrites any data in the filename  which is     */
/* specified at the console in response to the prompt for saving a vesp       */
/* program.  Save at your own risk!!!!!                                       */
/******************************************************************************/

/* This  computer program simulates the fetch,decode
/* and execute cycles of RISC-V processor,by extending Vesp 2.0
/* Revised to reduce the address field to 12 bits. 2/13/03
 /* VESP-RISC-V  has the following registers:
 /* R0,R1,...,R31:     32 bits
 /* IR:    32 bits
 /* PC:    32 bits
 /* MAR:   16 bits
 /* Its instruction repertoire consists of the following instructions:
 /* Here Define RISC-V instructions
 /* LUI:
 /* AUIPC:
 /* JAL:
 /* JALR:
 /* BEQ
 /* BNE:
 /* BLT:
 /* BGE:
 /* BLTU:
 /* BGEU:
 /* LB:
 /* LH:
 /* LW:
 /* LBU:
 /* LHU:
 /* SB
 /* Complete
 /* .
 /* .
 /*         Programs are entered and displayed in hex code. */
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <limits.h>
#include <unistd.h>

void initialize(void); int readprogram(void);
void displayprogram(int progend);
void displayregisters(void); void displaymemory(void);
void maincycle(int trace);   void fetch(int trace);
void decode(void);           void execute(void);

//AYO: Define the registers of the architecture.
typedef struct
{int REGISTERS[32];
 short INST_BUFFER[65536];
 long PC,IR,MAR;
 long clock;
 int  MEMORY[65536],reset;
} architecture;  architecture   vespRISCV;

using namespace std;
int j=1;

int main(void)

{int action,action2,progend;
 initialize();//AYO: Initialize all the registers.
 cout << "\nWelcome to Vesp 2.0\n\n";
 char * dir = getcwd(NULL, 0); // Platform-dependent, see reference link below
 printf("Current dir: %s", dir);

while(1) {vespRISCV.reset = false; //AYO: vesp keeps running unless it is reset.
//AYO: Input program, diplay registers or memory
     cin.clear();
     cout << "\nType \n 0 to enter a program\n "
          << "1 to display registers\n 2 to display memory: ";
     cin >> action2;
     cin.clear();
     cout << "\n";
//AYO: Read the program, display it, and execute it.
if(action2 == 0)
{progend = readprogram(); displayprogram(progend);
//AYO: Step through or execute the program.

while(vespRISCV.reset == 0)
{cout << "\nEnter 0 if you wish to execute the program, 1 to step it, ";
 cout <<  "2 to exit to the main menu: ";
 cin.clear(); cin >> action2;
 switch(action2)
 {case 0: cout << "Enter 0 for a brief trace 1 for long trace: "; cin.clear(); cin >> action;
          cout << "\n";
          while(vespRISCV.reset == 0) maincycle(action); break;
  case 1: maincycle(1);    break;
  case 2: vespRISCV.reset = true; break;
 }
}

//AYO: Display the number of instructions and clock cycles executed.
if(action2 != 2)
{cout << "The number of instructions executed  = " << dec << j-1  << "\n";
 cout << "The number of clock cycles used = " << dec << vespRISCV.clock << "\n";
j = 1; vespRISCV.clock = 0;
}
}
if (action2 == 1) displayregisters();
if (action2 == 2) displaymemory();
//if (action == 3) readprogram();
}
}

void initialize(void)
{vespRISCV.PC = vespRISCV.MEMORY[0] = vespRISCV.MEMORY[1] = vespRISCV.IR = 0;
 vespRISCV.reset = 0; vespRISCV.clock = 0;
}

int readprogram(void)
{int address,instruction,progend,action; //,i;
//char longfilename[100] ="/UMD-Web-Dreamweaver/teaching/courses/enee350/vesp-source-code/vesp2.0X/",
  char filename[25];
 FILE *file;
 do{ cout << "Enter your program's starting "
      << "address ( >= 3) as a 3-digit hex number: ";
  cin >> hex >> vespRISCV.PC;
 } while (vespRISCV.PC < 3);

 address =  vespRISCV.PC; action = -1;
 cout << "\n";
 do { cout << "Enter 0 to type in your program or 1 to read it from a file: ";
 cin >> action;
 } while (action != 0 && action != 1);

 if(action == 1)
 {cout << "\nEnter the file name: "; cin >> filename;
 file = fopen(filename,"r");
 if( file != NULL)
 {
 while (fscanf(file,"%x",&instruction) != EOF  &&  address < 8192 )
  {
   vespRISCV.MEMORY[address] = instruction; address = address + 1;
   }
 fclose(file);
 }
  else
  {cout << "The file is not found. Check if file to be opened is in the program directory... \n"; exit(1);}

 }
 else
 do {cin.clear();
     cout << "Enter instruction  "
          << (address -vespRISCV.PC)
          << " using a 4-digit hex number" << "\n";
     cout << "Or type -1 to end your program: ";

     cin >> hex >> instruction;   //AYO: read it in hex.
      vespRISCV.MEMORY[address] = instruction;
      address = address + 1;
    }
 while ( ( vespRISCV.MEMORY[address-1] !=  -1 ) && (address < 8192)); //AYO: -1 marks the end.

if (address >= 4096)
{cout << "Memory overflow,"
      << "Please quit from the file menu and restart me!";
return address-1;}
progend = address - 1;


//save the program into a file
 cout << "Enter 0 to continue, 1 to save your program into a file: ";
 cin >> action;

 if(action == 1)
 {cout << "\nEnter the file name: "; cin >> filename;

 cout << hex;
 file = fopen(filename,"w");
 if( file != NULL)
 {address = vespRISCV.PC;
 while (address < progend )
  {
 fprintf(file,"%04X\n",vespRISCV.MEMORY[address]);
  address = address + 1;
   }
 fclose(file);
 }
 else
  {cout << "The file is not found. Check if file to be opened is in the program directory... \n"; exit(1);}

 }

return progend;

}

void displayprogram(int progend)
{int i;
cout << "\nHere is your program: \n\n";
for (i = vespRISCV.PC; i<= progend; i++)
{
cout << "Location "; cout.fill('0');  cout.width(4); cout.setf(ios::uppercase);
cout <<  hex << i << ": ";  //AYO: display it in uppercase hex.

cout.fill('0');  cout.width(4); cout.setf(ios::uppercase);
cout << hex << (0x0000FFFF & vespRISCV.MEMORY[i]) << "\n";
}
}
void displayregisters(void)
{
cout << "A = ";  cout.fill('0'); cout.width(8); cout.setf(ios::uppercase);
cout << hex << (0x0000FFFF & vespRISCV.MEMORY[0])<< ", ";
cout << "B = ";  cout.fill('0'); cout.width(8); cout.setf(ios::uppercase);
cout << hex << (0x0000FFFF & vespRISCV.MEMORY[1])<< ", ";
cout << "MAR = "; cout.fill('0'); cout.width(8); cout.setf(ios::uppercase);
cout << hex << vespRISCV.MAR << ", ";
cout << "PC = ";  cout.fill('0'); cout.width(8); cout.setf(ios::uppercase);
cout << hex << vespRISCV.PC << ", ";
cout << "IR = "; cout.fill('0');  cout.width(8); cout.setf(ios::uppercase);
cout << hex << vespRISCV.IR << ", ";
cout << "reset = " << vespRISCV.reset << "\n";
}

void displaymemory(void)
{int location1,location2,i;
 cout << "Enter the first address (3 hex digits): ";
 cin.clear();
 cin >> hex >> location1;
 cin.clear();
 cout << "Enter the last address: (3 hex digits): ";
 cin.clear();
 cin >> hex >> location2;
 cin.clear();
 for (i = location1; i <= location2; i++)
 {cout <<  "Location "; cout.fill('0'); cout.width(4); cout.setf(ios::uppercase);
 cout << hex << i << " : ";
 cout.fill('0'); cout.width(4); cout.setf(ios::uppercase);
 cout << hex << (0x0000FFFF & vespRISCV.MEMORY[i]); cout << "\n";
}
}

void maincycle(int trace)
{     cout << "Machine Cycle ";
      cout.fill('0'); cout.width(8); cout.setf(ios::uppercase);
      cout << hex << j << ":  ";
       j = j+1;
       //AYO: Fetch Step
        cout << "PC = ";  cout.fill('0'); cout.width(8); cout.setf(ios::uppercase);
        cout << hex << (vespRISCV.PC & 0x0FFFFFFF) << ", ";
        if(trace == 1) cout << "\nFETCH SUBCYCLE\n";
        fetch(trace);
        if(trace == 1) cout << "\nClock cycle = " << vespRISCV.clock << "\n";
        //AYO: Decode Step
        if(trace == 1) cout << "DECODE SUBCYCLE\n";
        decode();
        if(trace == 1) cout << "Clock cycle = " << vespRISCV.clock << "\n";
        //AYO: Execute Step
        if(trace == 1) cout << "EXECUTE SUBCYCLE";
        vespRISCV.add = vespRISCV.cmp = 0;
        execute();
        cout << "\nClock cycle = " << vespRISCV.clock << "\n\n";
        //AYO: Display the registers

       if(trace == 1)
       {
        displayregisters();
        cout << "add = " << vespRISCV.add << " ";
        cout << "complement = " << vespRISCV.cmp << "\n\n";
        if( (vespRISCV.IR >> 12 ) == 2  ||  (vespRISCV.IR   >> 12 ) == 3)
        {cout << "Memory[";
         cout.fill('0'); cout.width(4); cout.setf(ios::uppercase);
         cout << hex << (vespRISCV.IR & 0x0FFF) << "] = ";
         cout.fill('0'); cout.width(4); cout.setf(ios::uppercase);
         cout << hex << (0x0000FFFF & vespRISCV.MEMORY[vespRISCV.IR & 0x0FFF])<< "\n\n";
         }
        }
}

void fetch(int trace)
{
if(trace == 1)
        {cout << "PC = "; cout.fill('0'); cout.width(8); cout.setf(ios::uppercase);
        cout << hex << vespRISCV.PC << ", ";}
        vespRISCV.IR = vespRISCV.INST_BUFFER[vespRISCV.PC];
        vespRISCV.PC = vespRISCV.PC +1; //Increment PC.
        vespRISCV.clock = vespRISCV.clock +1;
if(trace == 1)
        {cout << "IR = "; cout.fill('0'); cout.width(8); cout.setf(ios::uppercase);
         cout << hex << vespRISCV.IR << ", ";}
}

void decode(void)
{cout << "instruction: ";
     switch( vespRISCV.IR & 0x0000007F)
         {//LUI
          case  0x37: cout << "LUI\n"; break;
          //AUIPC
          case  0x17: cout << "AUIPC\n"; break;
          //JAL
          case  0x6F: cout << "JAL\n"; break;
          case  0x67: if(((vespRISCV.IR >> 12) & 0x00000007) == 0) cout <<  "JALR\n"; break;
          case  0x63://Several more branch instructions
                    switch(vespRISCV.IR >> 12 & 0x00000007)
                    {
                     case 0: cout << "BEQ\n"; break;
                     case 1: cout << "BNE\n"; break;
                     case 4: cout << "BLT\n"; break;
                     case 5: cout << "BGE\n";      break;
                     case 6: cout << "BLTU\n";     break;
                     case 7: cout << "BGEU\n";     break;
                    }
          break;
          //Several load  instructions
          case 0x03:
                  switch(vespRISCV.IR >> 12 & 0x00000007)
                  {
                   case 0: cout << "LB\n";  break;
                   case 1: cout << "LH\n";  break;
                   case 2:  cout << "LW\n"; break;
                   case 4: cout << "LBU\n"; break;
                   case 5: cout << "LHU\n"; break;
                  }
          break;
          //Several store  instructions
          case 0x23:
                 switch(vespRISCV.IR >> 12 & 0x00000007)
                 {
                 case 0: cout << "SB\n";  break;
                 case 1: cout << "SH\n";  break;
                 case 2:  cout << "SW\n"; break;
                 }
         break;
        //Arithmetic, shift, logic instructions
         case 0x13:
              if((vespRISCV.IR >> 25) != 0 || (vespRISCV.IR >> 25) != 32 )
              {  switch((vespRISCV.IR >> 12) & 0x00000007)
                 {
                 case 0: cout << "ADDI(\n";  break;
                 case 2: cout << "SLTI\n";  break;
                 case 3: cout << "SLTIU\n"; break;
                 case 4: cout << "XORI\n";  break;
                 case 6: cout << "ORI\n";   break;
                 case 7: cout << "ANDI\n";  break;
                 }
              }
              else
              {  switch(vespRISCV.IR >> 12 & 0x00000007)
                  {
                      case 1: cout << "SLLI(\n";  break;
                      case 5:
                          switch(vespRISCV.IR >> 25)
                      {case 0: cout << "SRLI\n";  break;
                       case 32:cout << "SRAI\n";  break;
                      }

                  }
              }
        break;
        //More arithmetic, shift, and logic instructions
        case 0x33:
    {  if(vespRISCV.IR >> 25  == 0) {
            switch(vespRISCV.IR >> 12 & 0x00000007)
                  {case 0: cout << "ADD\n";  break;
                   case 1: cout << "SLL\n";  break;
                   case 2: cout << "SLT\n";  break;
                   case 3: cout << "SLTU\n"; break;
                   case 4: cout << "XOR\n";  break;
                   case 5: cout << "SRL\n";  break;
                   case 6: cout << "OR\n";   break;
                   case 7: cout << "AND\n"; break;
                  }
            }
       else{
           if(vespRISCV.IR >> 25  == 32)
           {
            switch(vespRISCV.IR >> 12 & 0x00000007)
               {case 0: cout << "SUB\n"; break;
                case 5: cout << "SRA\n"; break;
               }
           }
           }
          }
        break;
          case 0x0F: cout << "NIM\n"; break;
          case 0x73: cout << "NIM\n"; break;
          }
}

void execute(void)
{
  switch( vespRISCV.IR & 0x0000007F)
      {//LUI
      int rd = vespRISCV.IR >> 7 & 0x0000001F;
      int j_imm = (vespRISCV.IR & 0x80000000) >> (31 - 11) | (vespRISCV.IR & 0x80) << 3 | (vespRISCV.IR & 0x7e000000) >> (30 )

       case  0x37: cout << "LUI\n";
       vespRISCV.REGISTERS[vespRISCV.IR >> 7] = (vespRISCV.IR >> 12) << 12; break;
       //AUIPC
       case  0x17: cout << "AUIPC\n";
       vespRISCV.REGISTERS[vespRISCV.IR >> 7] = ((vespRISCV.IR >> 12) << 12) + vespRISCV.PC; break;
       //JAL
       case  0x6F: cout << "JAL\n"; break;
       case  0x67: if(((vespRISCV.IR >> 12) & 0x00000007) == 0) cout <<  "JALR\n"; break;
       case  0x63://Several more branch instructions
                 switch(vespRISCV.IR >> 12 & 0x00000007)
                 {
                  case 0: cout << "BEQ\n";
                    if(vesp.REGISTERS[vespRISCV.IR >> 15 & 0x000001F] == vesp.REGISTERS[vespRISCV.IR >> 20]) {
                    vespRISCV.PC = vespRISCV.PC + (vespRISCV.IR &
                    } break;
                  case 1: cout << "BNE\n"; break;
                  case 4: cout << "BLT\n"; break;
                  case 5: cout << "BGE\n";      break;
                  case 6: cout << "BLTU\n";     break;
                  case 7: cout << "BGEU\n";     break;
                 }
       break;
       //Several load  instructions
       case 0x03:
               switch(vespRISCV.IR >> 12 & 0x00000007)
               {
                case 0: cout << "LB\n";  break;

                case 1: cout << "LH\n";  break;
                case 2:  cout << "LW\n"; break;
                case 4: cout << "LBU\n"; break;
                case 5: cout << "LHU\n"; break;
               }
       break;
       //Several store  instructions
       case 0x23:
              switch(vespRISCV.IR >> 12 & 0x00000007)
              {
              case 0: cout << "SB\n";  break;
              case 1: cout << "SH\n";  break;
              case 2:  cout << "SW\n"; break;
              }
      break;
     //Arithmetic, shift, logic instructions
      case 0x13:
           if((vespRISCV.IR >> 25) != 0 || (vespRISCV.IR >> 25) != 32 )
           {  switch((vespRISCV.IR >> 12) & 0x00000007)
              {
              case 0: cout << "ADDI(\n";  break;
              case 2: cout << "SLTI\n";  break;
              case 3: cout << "SLTIU\n"; break;
              case 4: cout << "XORI\n";  break;
              case 6: cout << "ORI\n";   break;
              case 7: cout << "ANDI\n";  break;
              }
           }
           else
           {  switch(vespRISCV.IR >> 12 & 0x00000007)
               {
                   case 1: cout << "SLLI(\n";  break;
                   case 5:
                       switch(vespRISCV.IR >> 25)
                   {case 0: cout << "SRLI\n";  break;
                    case 32:cout << "SRAI\n";  break;
                   }

               }
           }
     break;
     //More arithmetic, shift, and logic instructions
     case 0x33:
 {  if(vespRISCV.IR >> 25  == 0) {
         switch(vespRISCV.IR >> 12 & 0x00000007)
               {case 0: cout << "ADD\n";  break;
                case 1: cout << "SLL\n";  break;
                case 2: cout << "SLT\n";  break;
                case 3: cout << "SLTU\n"; break;
                case 4: cout << "XOR\n";  break;
                case 5: cout << "SRL\n";  break;
                case 6: cout << "OR\n";   break;
                case 7: cout << "AND\n"; break;
               }
         }
    else{
        if(vespRISCV.IR >> 25  == 32)
        {
         switch(vespRISCV.IR >> 12 & 0x00000007)
            {case 0: cout << "SUB\n"; break;
             case 5: cout << "SRA\n"; break;
            }
        }
        }
       }
     break;
       case 0x0F: cout << "NIM\n"; break;
       case 0x73: cout << "NIM\n"; break;
       }
}
