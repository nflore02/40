/*
 * Homework 6 UM - Nick Flores & Daniel Rodrigues
 * um.c
 *
 * This is the implementation file for the main operations of the UM
 * It reads in the file, grabs the instructions, and executes the intsructions
 * within the execute cycle (main's while loop)
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <inttypes.h>
#include "set.h"
#include "seq.h"
#include "bitpack.h"
#include "assert.h"
#include "segments.h"

void read_instructions(const char *input_string, memory *mem);
void initialize(memory *mem);
void add(int reg_a, int reg_b, int reg_c, uint32_t *next_counter, 
                uint32_t registers[]);
void mulitply(int reg_a, int reg_b, int reg_c, uint32_t *next_counter,
                uint32_t registers[]);
void divide(int reg_a, int reg_b, int reg_c, uint32_t *next_counter,
                uint32_t registers[]);
void conditional_move(int reg_a, int reg_b, int reg_c, uint32_t *next_counter,
                uint32_t registers[]);
void nand(int reg_a, int reg_b, int reg_c, uint32_t *next_counter,
                uint32_t registers[]);
void halt(int reg_a, int reg_b, int reg_c, uint32_t *next_counter,
                uint32_t registers[], memory *mem);
void map_segment(int reg_a, int reg_b, int reg_c, uint32_t *next_counter,
                uint32_t registers[], memory *mem);
void unmap_segment(int reg_a, int reg_b, int reg_c, uint32_t *next_counter,
                uint32_t registers[], memory *mem);
void input(int reg_a, int reg_b, int reg_c, uint32_t *next_counter,
                uint32_t registers[]);
void output(int reg_a, int reg_b, int reg_c, uint32_t *next_counter,
                uint32_t registers[]);
void load_program(int reg_a, int reg_b, int reg_c, uint32_t *next_counter,
                uint32_t registers[], memory *mem);
void load_val(int reg_a, uint32_t value, uint32_t *next_counter,
                uint32_t registers[]);
void execute_instruction(uint32_t instruction, uint32_t *next_counter,
                memory *mem, uint32_t registers[]);

/*
 * main:
 *      initializes the memory struct
 *      reads instructions from input file
 *      loops while program counter does not read all 1's
 *      executing the instruction of the program counter
 */
int main(int argc, char *argv[])
{
        (void)argc;
        const char *input_string = argv[1];
        memory mem;
        uint32_t registers[8] = {0, 0, 0, 0, 0, 0, 0, 0};
        uint32_t prgm_counter = 0;
        uint32_t next_counter = 0;

        uint32_t val = 0;

        initialize(&mem);
        read_instructions(input_string, &mem);

        val = Seg_load(&mem, 0, next_counter);
        prgm_counter = val;
        uint32_t end_flag = ~0;
        while (prgm_counter != end_flag) {
                execute_instruction(prgm_counter, &next_counter,
                                &mem, registers);
                val = Seg_load(&mem, 0, next_counter);
                prgm_counter = val;
        }
        Seg_free(&mem);
        return 0;
}

/*
 * read_instructions:
 *      takes in file pointer from main
 *      opens file and finds its size using stat
 *      uses Bitpack to unpack insruction word
 *      closes file
 */
void read_instructions(const char *input_string, memory *mem)
{
        uint32_t word = 0;
        uint32_t cast_word = 0;

        FILE *input = fopen(input_string, "rb");
        assert(input != NULL);
        struct stat file_stat;
        if (stat(input_string, &file_stat) == -1) {
                fprintf(stderr, "Faulty file format\n");
                exit(EXIT_FAILURE);
        }
        int num_words = (file_stat.st_size / 4);
        uint32_t Seg_id = Seg_map(mem, num_words + 1);
        mem->total_ids = 1;
        int i;
        for (i = 0; i < num_words; i++) {
                unsigned char c = getc(input);
                word = Bitpack_newu(word, 8, 24, (uint64_t)c);

                c = getc(input);
                word = Bitpack_newu(word, 8, 16, (uint64_t)c);

                c = getc(input);
                word = Bitpack_newu(word, 8, 8, (uint64_t)c);

                c = getc(input);
                word = Bitpack_newu(word, 8, 0, (uint64_t)c); 

                cast_word = (uint32_t)word;
                assert(Seg_store(mem, cast_word, Seg_id, i) == true);
        }
        cast_word = ~0;
        assert(Seg_store(mem, cast_word, Seg_id, num_words) == true); 
        fclose(input);
}

/*
 * initialize:
 *      initializes the sequences and number of total ids
 */
void initialize(memory *mem)
{
        mem->segments = Seq_new(1);
        mem->unmapped_ids = Seq_new(0);
        mem->total_ids = 0;
}

/*
 * NOTE: all the following instruction functions take in registers a, b, & c
 *       they also all update next_counter
 */

/*
 * add:
 *      add register b and c and place into register a
 */
void add (int reg_a, int reg_b, int reg_c, uint32_t *next_counter,
                uint32_t registers[])
{
        uint32_t c = registers[reg_c];
        uint32_t b = registers[reg_b];
        uint32_t result = c+b;
        registers[reg_a] = result;
        *next_counter += 1;
}

/*
 * multiply:
 *      multiply register b and c and place into register a
 */
void multiply(int reg_a, int reg_b, int reg_c, uint32_t *next_counter,
                uint32_t registers[])
{
        uint32_t c = registers[reg_c];
        uint32_t b = registers[reg_b];
        uint32_t result = (c*b);
        registers[reg_a]= result;
        *next_counter += 1;
}

/*
 * divide:
 *      divide register c by b and place into register a
 */
void divide(int reg_a, int reg_b, int reg_c, uint32_t *next_counter,
                uint32_t registers[])
{
        uint32_t c = registers[reg_c];
        uint32_t b = registers[reg_b];
        uint32_t result = (b / c);
        registers[reg_a] = result;
        *next_counter += 1;
}

/*
 * conditional_move:
 *      jump to register b unless register c is 0
 */
void conditional_move(int reg_a, int reg_b, int reg_c, uint32_t *next_counter,
                uint32_t registers[])
{
        uint32_t c = registers[reg_c];
        uint32_t b = registers[reg_b];
        if (c != 0) {
                registers[reg_a] = b;
        } 
        *next_counter += 1;
}

/*
 * nand:
 *      bitwise NOT AND registers b & c, place in register a
 */
void nand(int reg_a, int reg_b, int reg_c, uint32_t *next_counter,
                uint32_t registers[])
{
        uint32_t c = registers[reg_c];
        uint32_t b = registers[reg_b];
        uint32_t result= ~(b & c);
        registers[reg_a]= result; 
        *next_counter += 1;
}

/*
 * load_program:
 *      if register b (segment ID) is 0, set next_counter to c
 *      otherwise, replace replace the segment given the ID
 *      and set next_counter to c
 */
void load_program(int reg_a, int reg_b, int reg_c, uint32_t *next_counter,
                uint32_t registers[], memory *mem)
{
        (void)reg_a;
        uint32_t c = registers[reg_c];
        uint32_t segID = registers[reg_b];
        if (segID == 0) {
                *next_counter = c;
                return;
        }
        *next_counter = c;
        Seg_replace(mem, segID);
}

/*
 * input:
 *      read from stdin and store value in register c
 *      if stdin is EOF, set c to all 1's
 */
void input(int reg_a, int reg_b, int reg_c, uint32_t *next_counter,
                uint32_t registers[])
{
        (void)reg_a;
        (void)reg_b;
        char c;
        uint32_t val;
        c = getc(stdin);
        if (c == EOF) {
                val = ~0; 
        } else {
                val = (uint32_t)c;
        }
        registers[reg_c] = val;
        *next_counter += 1;
}

/*
 * output:
 *      stdout the value in register c
 */
void output(int reg_a, int reg_b, int reg_c, uint32_t *next_counter,
                uint32_t registers[])
{
        (void)reg_a;
        (void)reg_b;
        uint32_t val = registers[reg_c];
        putc(val, stdout);
        *next_counter += 1;
}

/*
 * segmented_load:
 *      grab ID & offset from registes b and c
 *      load the value into register a given the acquired ID & offset
 */
void segmented_load(int reg_a, int reg_b, int reg_c, uint32_t *next_counter,
                uint32_t registers[], memory *mem)
{
        uint32_t segID = registers[reg_b];
        uint32_t offset = registers[reg_c];
        registers[reg_a] = Seg_load(mem, segID, offset); 
        *next_counter += 1;
}

/*
 * segmented_store:
 *      grab ID, offset, & value from registers a, b, & c
 *      store the value with the given id and offset
 */
void segmented_store(int reg_a, int reg_b, int reg_c, uint32_t *next_counter,
                uint32_t registers[], memory *mem)
{
        uint32_t segID = registers[reg_a];
        uint32_t offset = registers[reg_b];
        uint32_t value = registers[reg_c];
        assert(Seg_store(mem, value, segID, offset)); 
        *next_counter += 1;
}

/*
 * map_segment:
 *      grab size from register c
 *      grab ID from return of Seg_map given size
 *      store ID in register b
 */
void map_segment(int reg_a, int reg_b, int reg_c, uint32_t *next_counter,
                uint32_t registers[], memory *mem)
{
        uint32_t size = registers[reg_c];
        uint32_t segID = Seg_map(mem, size);
        registers[reg_b] = segID;
        (void)reg_a;
        *next_counter += 1;
}

/*
 * unmap_segment:
 *      grab ID from register c
 *      unmap the segment at given ID
 */
void unmap_segment(int reg_a, int reg_b, int reg_c, uint32_t *next_counter,
                uint32_t registers[], memory *mem)
{    
        uint32_t segID = registers[reg_c];
        Seg_unmap(mem, segID);
        (void)reg_b;
        (void)reg_a; 
        *next_counter += 1;
}

/*
 * halt:
 *      free sequence of segments
 *      exit with success
 *
 * NOTE:
 *      this function doesn't update next_counter
 */
void halt(int reg_a, int reg_b, int reg_c, uint32_t *next_counter,
                uint32_t registers[], memory *mem)
{
        (void)reg_c;
        (void)reg_b;
        (void)reg_a;
        (void)registers;
        (void)next_counter;
        Seg_free(mem);
        exit(0);
}

/*
 * load_value:
 *      store the passed in value into register a
 *
 * NOTE:
 *      this function only takes in a register and the value
 */
void load_val(int reg_a, uint32_t value, uint32_t *next_counter,
                uint32_t registers[])
{
        registers[reg_a] = value;
        *next_counter += 1;
}

/*
 * execute_instruction:
 *      takes in the uint32_t word instruction from main
 *      creates a value for the opcode by shifting right 28 bits
 *      to get the 4 bits that decode into the opcode value
 *      does similar shift operations on the instruction to grab
 *      the register (a, b, & c) values
 *      firstly check if opcode is 13 meaning loading a value
 *      then goes through switch case statement to decide which instruction
 *      to execute, then calls that function
 */
void execute_instruction(uint32_t instruction, uint32_t *next_counter,
                memory *mem, uint32_t registers[])
{
        uint32_t opcode = instruction >> 28;
        if (opcode == 13) {
                uint32_t a = instruction << 4;
                a = a >> 29;
                uint32_t value = instruction << 7;
                value = value >> 7;
                load_val(a, value, next_counter, registers);     
        } else {
                uint32_t a = instruction << 23;
                a = a >> 29;
                uint32_t b = instruction << 26;
                b= b >> 29;
                uint32_t c = instruction << 29;
                c = c >> 29;
                switch (opcode){
                        case 0 :
                                conditional_move(a, b, c, next_counter,
                                                        registers);
                                break;
                        case 1 :
                                segmented_load(a, b, c, next_counter, registers,
                                                mem);
                                break;
                        case 2 :
                                segmented_store(a, b, c, next_counter,
                                                registers, mem); 
                                break;
                        case 3 :
                                add(a, b, c, next_counter, registers); 
                                break;
                        case 4 :
                                multiply(a, b, c, next_counter, registers);
                                break;
                        case 5 :
                                divide(a, b, c, next_counter, registers);
                                break;
                        case 6 :
                                nand(a, b, c, next_counter, registers);
                                break;
                        case 7 :
                                halt(a, b, c, next_counter, registers, mem); 
                                break;
                        case 8 :
                                map_segment(a, b, c, next_counter, registers,
                                                mem);
                                break;
                        case 9 :
                                unmap_segment(a, b, c, next_counter, registers,
                                                mem);
                                break;
                        case 10 :
                                output(a, b, c, next_counter, registers);
                                break;
                        case 11 :
                                input(a, b, c, next_counter, registers);
                                break;
                        case 12 :
                                load_program(a, b, c, next_counter, registers,
                                                mem);
                                break;
                        default :
                                exit(EXIT_FAILURE);
                }
        }     
}
