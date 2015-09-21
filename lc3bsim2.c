/*
    REFER TO THE SUBMISSION INSTRUCTION FOR DETAILS

    Name 1: Brendan Wong 
    Name 2: Nathan Elling
    UTEID 1: bmw2383
    UTEID 2: nae343
*/

/***************************************************************/
/*                                                             */
/*   LC-3b Instruction Level Simulator                         */
/*                                                             */
/*   EE 460N                                                   */
/*   The University of Texas at Austin                         */
/*                                                             */
/***************************************************************/

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3b machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* MEMORY[A][0] stores the least significant byte of word at word address A
   MEMORY[A][1] stores the most significant byte of word at word address A 
*/

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM][2];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3b State info.                                           */
/***************************************************************/
#define LC_3b_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

  int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
  int REGS[LC_3b_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {                                                    
  printf("----------------LC-3b ISIM Help-----------------------\n");
  printf("go               -  run program to completion         \n");
  printf("run n            -  execute program for n instructions\n");
  printf("mdump low high   -  dump memory from low to high      \n");
  printf("rdump            -  dump the register & bus values    \n");
  printf("?                -  display this help menu            \n");
  printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  process_instruction();
  CURRENT_LATCHES = NEXT_LATCHES;
  INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3b for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
  int i;

  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating for %d cycles...\n\n", num_cycles);
  for (i = 0; i < num_cycles; i++) {
    if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
    }
    cycle();
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3b until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating...\n\n");
  while (CURRENT_LATCHES.PC != 0x0000)
    cycle();
  RUN_BIT = FALSE;
  printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
  int address; /* this is a byte address */

  printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  printf("-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    printf("  0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (address = (start >> 1); address <= (stop >> 1); address++)
    fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x%.2x\n", address << 1, address << 1, MEMORY[address][1], MEMORY[address][0]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
  int k; 

  printf("\nCurrent register/bus values :\n");
  printf("-------------------------------------\n");
  printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
  printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  printf("Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
  fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  fprintf(dumpsim_file, "Registers:\n");
  for (k = 0; k < LC_3b_REGS; k++)
    fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
  char buffer[20];
  int start, stop, cycles;

  printf("LC-3b-SIM> ");

  scanf("%s", buffer);
  printf("\n");

  switch(buffer[0]) {
  case 'G':
  case 'g':
    go();
    break;

  case 'M':
  case 'm':
    scanf("%i %i", &start, &stop);
    mdump(dumpsim_file, start, stop);
    break;

  case '?':
    help();
    break;
  case 'Q':
  case 'q':
    printf("Bye.\n");
    exit(0);

  case 'R':
  case 'r':
    if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
    else {
	    scanf("%d", &cycles);
	    run(cycles);
    }
    break;

  default:
    printf("Invalid Command\n");
    break;
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
  int i;

  for (i=0; i < WORDS_IN_MEM; i++) {
    MEMORY[i][0] = 0;
    MEMORY[i][1] = 0;
  }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {                   
  FILE * prog;
  int ii, word, program_base;

  /* Open program file. */
  prog = fopen(program_filename, "r");
  if (prog == NULL) {
    printf("Error: Can't open program file %s\n", program_filename);
    exit(-1);
  }

  /* Read in the program. */
  if (fscanf(prog, "%x\n", &word) != EOF)
    program_base = word >> 1;
  else {
    printf("Error: Program file is empty\n");
    exit(-1);
  }

  ii = 0;
  while (fscanf(prog, "%x\n", &word) != EOF) {
    /* Make sure it fits. */
    if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
             program_filename, ii);
	    exit(-1);
    }

    /* Write the word to memory array. */
    MEMORY[program_base + ii][0] = word & 0x00FF;
    MEMORY[program_base + ii][1] = (word >> 8) & 0x00FF;
    ii++;
  }

  if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = (program_base << 1);

  printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */ 
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) { 
  int i;

  init_memory();
  for ( i = 0; i < num_prog_files; i++ ) {
    load_program(program_filename);
    while(*program_filename++ != '\0');
  }
  CURRENT_LATCHES.Z = 1;  
  NEXT_LATCHES = CURRENT_LATCHES;
    
  RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
  FILE * dumpsim_file;

  /* Error Checking */
  if (argc < 2) {
    printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
           argv[0]);
    exit(1);
  }

  printf("LC-3b Simulator\n\n");

  initialize(argv[1], argc - 1);

  if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
    printf("Error: Can't open dumpsim file\n");
    exit(-1);
  }

  while (1)
    get_command(dumpsim_file);
    
}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/



void process_instruction(){
  	/***********************************
  	*	function: process_instruction
   	*  
   	*   Process one instruction at a time  
   	*   	-Fetch one instruction
   	*       -Decode 
   	*       -Execute
   	*       -Update NEXT_LATCHES
   	************************************/

   	int i;
   	int current_instr;
   	int opcode;
   	int dr;
   	int sr1;
   	int sr2;
   	int immediate;
   	int identifier;
   	int temp;

   	/* Initialize the next latches to the current latches */
  	NEXT_LATCHES = CURRENT_LATCHES;

	/*************************/
   	/* Fetch one instruction */
   	/*************************/
   	
   	/* Fetch the MSB */
   	current_instr = MEMORY[CURRENT_LATCHES.PC/2][1];
   	current_instr = current_instr << 8;

   	/* Fetch the LSB */
   	current_instr += MEMORY[CURRENT_LATCHES.PC/2][0];

	/*************************/
   	/* Decode the opcode     */
   	/*************************/
   	opcode = (current_instr & 0xF000) >> 12;

/******************************************************
 ********************     ADD     ********************* 		Nathan
 ******************************************************/

   	if (opcode == 0x1){

   	}

/******************************************************
 ********************     AND     ********************* 		Brendan
 ******************************************************/

   	else if (opcode == 0x0101){
   		
   		dr = (current_instr & 0x0E00) >> 9; /* decodes the destination register */
   		sr1 = (current_instr & 0x01C0) >> 6; /* decodes the first source register */

   		identifier = (current_instr & 0x0020) >> 5; /* checks the instruction identifier */
   		/* if it is an immediate instruction do the following */
		if (identifier == 0x1) {
			immediate = current_instr & 0x001F; /* decode the immediate value */
			
			/* sign extend the immediate */
   			if (immediate & 0x0010){
   				immediate += 0xFFE0;
   			}

			NEXT_LATCHES.REGS[dr] = Low16bits(CURRENT_LATCHES.REGS[sr1] & immediate); /* execute the instruction */
			
		} 
		/* if it is an and between two registers do the following */
		else {
			sr2 = current_instr & 0x0007; /* decode the second source register */
			NEXT_LATCHES.REGS[dr] = Low16bits(CURRENT_LATCHES.REGS[sr1] & CURRENT_LATCHES.REGS[sr2]); /* execute the instruction */
		}

		/* set the condition code latches */
		if ((NEXT_LATCHES.REGS[dr] & 0x8000) == 1){
			NEXT_LATCHES.N = 1;
			NEXT_LATCHES.Z = 0;
			NEXT_LATCHES.P = 0;
		} else if (NEXT_LATCHES.REGS[dr] == 0){
			NEXT_LATCHES.N = 0;
			NEXT_LATCHES.Z = 1;
			NEXT_LATCHES.P = 0;
		} else {
			NEXT_LATCHES.N = 0;
			NEXT_LATCHES.Z = 0;
			NEXT_LATCHES.P = 1;
		}
		
		/* set the new PC */
		NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);
   	}

/******************************************************
 ********************     BR      ********************* 		Nathan
 ******************************************************/

   	else if (opcode == 0x0){
   	}

/******************************************************
 ********************     JMP     ********************* 		Brendan
 ******************************************************/

   	else if (opcode == 0xC){

   		sr1 = (current_instr & 0x01C0) >> 6; /* decodes the first source register */
   		NEXT_LATCHES.PC = CURRENT_LATCHES.REGS[sr1]; /* execute the instruction */

   	}

/******************************************************
 ********************    JSR(R)  ********************** 		Nathan
 ******************************************************/

   	else if (opcode == 0x4){
   	}

/******************************************************
 ********************     LDB     ********************* 		Brendan
 ******************************************************/

   	else if (opcode == 0x2){
   		
   		dr = (current_instr & 0x0E00) >> 9; /* decodes the destination register */
   		sr1 = (current_instr & 0x01C0) >> 6; /* decodes the first source register */
   		immediate = (current_instr & 0x003F); /* decode the base offset */
   		
   		/* sign extend the immediate */
   		if (immediate & 0x0020){
   			immediate += 0xFFC0;
   		}

   		temp = CURRENT_LATCHES.REGS[sr1];
   		temp += immediate;
   		
   		NEXT_LATCHES.REGS[dr] = MEMORY[temp/2][temp%2]; /* execute the instruction */

		/* set the condition code latches */
		if ((NEXT_LATCHES.REGS[dr] & 0x8000) == 1){
			NEXT_LATCHES.N = 1;
			NEXT_LATCHES.Z = 0;
			NEXT_LATCHES.P = 0;
		} else if (NEXT_LATCHES.REGS[dr] == 0){
			NEXT_LATCHES.N = 0;
			NEXT_LATCHES.Z = 1;
			NEXT_LATCHES.P = 0;
		} else {
			NEXT_LATCHES.N = 0;
			NEXT_LATCHES.Z = 0;
			NEXT_LATCHES.P = 1;
		}

		/* set the next pc */
		NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);
   		
   	}

/******************************************************
 ********************     LDW     ********************* 		Nathan
 ******************************************************/

   	else if (opcode == 0x6){
   	}

/******************************************************
 ********************     LEA     ********************* 		Brendan
 ******************************************************/

   	else if (opcode == 0xE){

   		dr = (current_instr & 0x0E00) >> 9; /* decodes the destination register */
   		immediate = (current_instr & 0x01FF); /* decode the base offset */
   		
   		/* sign extend the immediate */
   		if (immediate & 0x0100){
   			immediate += 0xFE00;
   		}
   		
   		NEXT_LATCHES.REGS[dr] = Low16bits(CURRENT_LATCHES.PC + 2 + (immediate * 2)); /* execute the instruction */

		/* set the condition code latches */
		if ((NEXT_LATCHES.REGS[dr] & 0x8000) == 1){
			NEXT_LATCHES.N = 1;
			NEXT_LATCHES.Z = 0;
			NEXT_LATCHES.P = 0;
		} else if (NEXT_LATCHES.REGS[dr] == 0){
			NEXT_LATCHES.N = 0;
			NEXT_LATCHES.Z = 1;
			NEXT_LATCHES.P = 0;
		} else {
			NEXT_LATCHES.N = 0;
			NEXT_LATCHES.Z = 0;
			NEXT_LATCHES.P = 1;
		}

		/* set the next pc */
		NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);

   	}

/******************************************************
 ********************     SHF     ********************* 		Brendan
 ******************************************************/

   	else if (opcode == 0xD){
   		
   		dr = (current_instr & 0x0E00) >> 9; /* decodes the destination register */
   		sr1 = (current_instr & 0x01C0) >> 6; /* decodes the first source register */

		immediate = current_instr & 0x000F; /* decode the immediate value */
		
		/* sign extend the immediate */
   		if (immediate & 0x0008){
   			immediate += 0xFFF0;
   		}

   		identifier = (current_instr & 0x0030) >> 4; /* checks the instruction identifier */
   		/* if it is an LSHF instruction do the following */
		if (identifier == 0x0) {

			NEXT_LATCHES.REGS[dr] = Low16bits(CURRENT_LATCHES.REGS[sr1] << immediate); /* execute the instruction */

		} 
		/* if it is an RSHFL do the following */
		else if (identifier == 0x1){

			NEXT_LATCHES.REGS[dr] = Low16bits(CURRENT_LATCHES.REGS[sr1] >> immediate); /* execute the instruction */

		}
		/* if it is an RSHFA do the following */
		else if (identifier == 0x3){

			NEXT_LATCHES.REGS[dr] = Low16bits(CURRENT_LATCHES.REGS[sr1] >> immediate); /* execute the instruction */

			/* sign extend the result of the arithmetic shift */
			if (CURRENT_LATCHES.REGS[sr1] & 0x8000){
				temp = 0x8000;
				for (i = 0; i < immediate; i++){
					temp = temp >> 1;
					temp += 0x8000;
				}
				NEXT_LATCHES.REGS[dr] += temp;
			}
		}

		/* set the condition code latches */
		if ((NEXT_LATCHES.REGS[dr] & 0x8000) == 1){
			NEXT_LATCHES.N = 1;
			NEXT_LATCHES.Z = 0;
			NEXT_LATCHES.P = 0;
		} else if (NEXT_LATCHES.REGS[dr] == 0){
			NEXT_LATCHES.N = 0;
			NEXT_LATCHES.Z = 1;
			NEXT_LATCHES.P = 0;
		} else {
			NEXT_LATCHES.N = 0;
			NEXT_LATCHES.Z = 0;
			NEXT_LATCHES.P = 1;
		}
		
		/* set the new PC */
		NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);
   	}

/******************************************************
 ********************     STB     ********************* 		Nathan
 ******************************************************/

   	else if (opcode == 0x3){
   	}

/******************************************************
 ********************     STW     ********************* 		Brendan
 ******************************************************/

   	else if (opcode == 0x7){
   		
   		sr1 = (current_instr & 0x0E00) >> 9; /* decodes the destination register */
   		sr2 = (current_instr & 0x01C0) >> 6; /* decodes the first source register */
   		immediate = (current_instr & 0x003F); /* decode the base offset */
   		
   		/* sign extend the immediate */
   		if (immediate & 0x0020){
   			immediate += 0xFFC0;
   		}

   		temp = CURRENT_LATCHES.REGS[sr2];
   		temp += immediate;
   		
   		MEMORY[temp/2][0] = CURRENT_LATCHES.REGS[sr1] & 0x00FF; /* execute the instruction for the LSB */
   		MEMORY[temp/2][1] = (CURRENT_LATCHES.REGS[sr1] & 0xFF00) >> 8; /* execute the instruction for the MSB */

		/* set the next pc */
		NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);
   		
   	}

/******************************************************
 ********************     TRAP    ********************* 		Nathan
 ******************************************************/

   	else if (opcode == 0xF){
   	}

/******************************************************
 ********************     XOR    ********************** 		Brendan
 ******************************************************/

   	else if (opcode == 0x9){
   		
   		dr = (current_instr & 0x0E00) >> 9; /* decodes the destination register */
   		sr1 = (current_instr & 0x01C0) >> 6; /* decodes the first source register */

   		identifier = (current_instr & 0x0020) >> 5; /* checks the instruction identifier */
   		/* if it is an immediate instruction do the following */
		if (identifier == 0x1) {
			immediate = current_instr & 0x001F; /* decode the immediate value */
			
			/* sign extend the immediate */
   			if (immediate & 0x0010){
   				immediate += 0xFFE0;
   			}

			NEXT_LATCHES.REGS[dr] = Low16bits(CURRENT_LATCHES.REGS[sr1] ^ immediate); /* execute the instruction */
			
		} 
		/* if it is an and between two registers do the following */
		else {
			sr2 = current_instr & 0x0007; /* decode the second source register */
			NEXT_LATCHES.REGS[dr] = Low16bits(CURRENT_LATCHES.REGS[sr1] ^ CURRENT_LATCHES.REGS[sr2]); /* execute the instruction */
		}

		/* set the condition code latches */
		if ((NEXT_LATCHES.REGS[dr] & 0x8000) == 1){
			NEXT_LATCHES.N = 1;
			NEXT_LATCHES.Z = 0;
			NEXT_LATCHES.P = 0;
		} else if (NEXT_LATCHES.REGS[dr] == 0){
			NEXT_LATCHES.N = 0;
			NEXT_LATCHES.Z = 1;
			NEXT_LATCHES.P = 0;
		} else {
			NEXT_LATCHES.N = 0;
			NEXT_LATCHES.Z = 0;
			NEXT_LATCHES.P = 1;
		}
		
		/* set the new PC */
		NEXT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + 2);

   	}
}
