#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "seq.h"
#include "assert.h"

// This is the code from the lab handout
typedef uint32_t Um_instruction;

enum opcode { LOAD_PRGM = 12, MULTIPLY = 4, DIVIDE = 5, NAND = 6, HALT = 7, INPUT = 11, OUTPUT = 10, ADD = 3, CONDITIONAL = 0, MAP = 8, STORE = 2, LOAD = 1, UNMAP = 9};
typedef enum opcode Um_opcode;
Um_instruction three_register(Um_opcode op, int ra, int rb, int rc);
Um_instruction loadval(unsigned ra, unsigned val);
Um_instruction add(unsigned ra, unsigned rb, unsigned rc);
Um_instruction divide(unsigned ra, unsigned rb, unsigned rc);
Um_instruction multiply(unsigned ra, unsigned rb, unsigned rc);
Um_instruction nand(unsigned ra, unsigned rb, unsigned rc);
Um_instruction load_prgm(unsigned ra, unsigned rb, unsigned rc);
Um_instruction output(unsigned rc);
Um_instruction input(unsigned rc);
void print_six(Seq_T stream);
void conditional_move(Seq_T stream);
void load_seg(Seq_T stream);
void map_test(Seq_T stream);
Um_instruction conditional(unsigned ra, unsigned rb, unsigned rc);
Um_instruction map(unsigned ra, unsigned rb, unsigned rc);
Um_instruction store(unsigned ra, unsigned rb, unsigned rc);
Um_instruction load(unsigned ra, unsigned rb, unsigned rc);

static inline Um_instruction halt(void) 
{
        return three_register(HALT, 0, 0, 0);
}
enum regs { r0 = 0, r1 , r2 , r3 , r4 , r5, r6, r7, r8 };
extern void Um_write_sequence(FILE *output, Seq_T stream);
static inline void emit(Seq_T stream, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_addhi(stream, (void *)(uintptr_t)inst);
}
static inline Um_instruction get_inst(Seq_T stream, int i)
{
        assert(sizeof(Um_instruction) <= sizeof(uintptr_t));
        return (Um_instruction)(uintptr_t)(Seq_get(stream, i));
}

static inline void put_inst(Seq_T stream, int i, Um_instruction inst)
{
        assert(sizeof(inst) <= sizeof(uintptr_t));
        Seq_put(stream, i, (void *)(uintptr_t) inst);
}

void emit_halt_test(Seq_T stream)
{
        emit(stream, halt());
}

void emit_verbose_halt_test(Seq_T stream)
{
        emit(stream, halt());
        emit(stream, loadval(r1, 'B'));
        emit(stream, output(r1));
        emit(stream, loadval(r1, 'a'));
        emit(stream, output(r1));
        emit(stream, loadval(r1, 'd'));
        emit(stream, output(r1));
        emit(stream, loadval(r1, '!'));
        emit(stream, output(r1));
        emit(stream, loadval(r1, '\n'));
        emit(stream, output(r1));
}


Um_instruction three_register(Um_opcode op, int ra, int rb, int rc)
{
        if(op > 12 || op < 0)
                exit(EXIT_FAILURE);
        if(ra > 7 || rb > 7|| rc > 7 )
                exit(EXIT_FAILURE);
        uint32_t opcode = op << 28;
        Um_instruction instruction = 0 | opcode;
        uint32_t reg1 = ra << 6;
        instruction |= reg1;
        uint32_t reg2 = rb << 3;
        instruction |= reg2;
        uint32_t reg3 = rc;
        instruction |= reg3;
        return instruction;
}

Um_instruction loadval(unsigned ra, unsigned val)
{
        if(ra > 7)
                exit(EXIT_FAILURE);
        if(val > 33554431)
                exit(EXIT_FAILURE);
        uint32_t opcode = 13 << 28;
        Um_instruction instruction = 0 | opcode;
        uint32_t reg = ra << 25;
        instruction |= reg;
        uint32_t value = val;
        instruction |= value;

        return instruction; 

}


Um_instruction output(unsigned rc) 
{
        return three_register(OUTPUT, 0, 0, rc);
}

Um_instruction input(unsigned rc)
{
        return three_register(INPUT, 0, 0, rc);
}

void arith_test(Seq_T stream)
{ 
        emit(stream, loadval(r2, 20));
        emit(stream, loadval(r3, 10)); 
        emit(stream, add(r1, r2 , r3));
        emit(stream, multiply(r1, r2, r3));
        emit(stream, divide(r1, r2, r3));
        emit(stream, nand(r1, r2, r3));
        emit(stream, halt());    
}


Um_instruction load_prgm(unsigned ra, unsigned rb, unsigned rc)
{

        return three_register(LOAD_PRGM, ra, rb, rc);

}

Um_instruction add(unsigned ra, unsigned rb, unsigned rc)
{

        return three_register(ADD, ra, rb, rc);

}

Um_instruction divide(unsigned ra, unsigned rb, unsigned rc)
{

        return three_register(DIVIDE, ra, rb, rc);

}

Um_instruction multiply(unsigned ra, unsigned rb, unsigned rc)
{

        return three_register(MULTIPLY, ra, rb, rc);

}

Um_instruction nand(unsigned ra, unsigned rb, unsigned rc)
{

        return three_register(NAND, ra, rb, rc);

}

union Instruction {
    uint32_t instruction;
    char bytes[4];
};

void Um_write_sequence(FILE *output, Seq_T stream)
{
        while(Seq_length(stream) != 0){
                union Instruction instruction;
                instruction.instruction = (Um_instruction)(uintptr_t)Seq_remlo(stream);
                for(int i = 3; i >= 0; i--){
                      putc(instruction.bytes[i], output);
                }
        }
        
        
        
}

void print_six(Seq_T stream)
{
        emit(stream, input(r3));
        emit(stream, output(r3));
        emit(stream, loadval(r1, 36));
        emit(stream, loadval(r2, 36));
        emit(stream, add(r3, r1, r2));
        emit(stream, output(r3));
        emit(stream, halt());
}

void load_prgm_test(Seq_T stream)
{
        emit(stream, loadval(r1, 72));
        emit(stream, loadval(r4, 5));
        emit(stream, load_prgm(0, 0, r4));
        emit(stream, halt());
        emit(stream, halt());
        emit(stream, output(r1));
        emit(stream, halt());
}


Um_instruction conditional(unsigned ra, unsigned rb, unsigned rc)
{
      return three_register(CONDITIONAL, ra, rb, rc);


}

void conditional_move(Seq_T stream)
{
        emit(stream, loadval(r2, 72));
        emit(stream, loadval(r3, 2));
        emit(stream, conditional(r1, r2, r3));
        emit(stream, output(r1));
        emit(stream, halt());
}


Um_instruction map(unsigned ra, unsigned rb, unsigned rc)
{
      return three_register(MAP, ra, rb, rc);

}

Um_instruction store(unsigned ra, unsigned rb, unsigned rc)
{
      return three_register(STORE, ra, rb, rc);

}


Um_instruction load(unsigned ra, unsigned rb, unsigned rc)
{
      return three_register(LOAD, ra, rb, rc);

}

Um_instruction unmap(unsigned rc)
{
      return three_register(UNMAP, 0, 0, rc);

}

void load_seg(Seq_T stream)
{
        emit(stream, loadval(r1, 72));
        emit(stream, loadval(r5, 5));
        emit(stream, map(0, r2, r5));
        emit(stream, store(r2, r0, r1));
        emit(stream, load(r3, r2, 0));
        emit(stream, output(r3));
        emit(stream, unmap(r2));
        emit(stream, map(0, r2, r5));
        emit(stream, halt());
}

void map_test(Seq_T stream)
{
        emit(stream, loadval(r1, 72));
        emit(stream, loadval(r2, 5));
        emit(stream, loadval(r0, 0));
        emit(stream, map(0, r3, r2));
        emit(stream, map(0, r4, r2));
        emit(stream, map(0, r5, r2));
        emit(stream, map(0, r6, r2));
        emit(stream, map(0, r7, r2));
        emit(stream, store(r3, r0, r1)); //store 72 in r3(1) segment @ offset r0(0)
        emit(stream, load(r2, r3, r0)); //load value from segment r3(i) into r2
        emit(stream, output(r2));
        emit(stream, unmap(r3));
        emit(stream, unmap(r4));
        emit(stream, unmap(r5));
        emit(stream, unmap(r6));
        emit(stream, unmap(r7));
        emit(stream, loadval(r1, 72));
        emit(stream, loadval(r2, 5));
        emit(stream, loadval(r0, 0));
        emit(stream, map(0, r3, r2));
        emit(stream, map(0, r4, r2));
        emit(stream, map(0, r5, r2));
        emit(stream, map(0, r6, r2));
        emit(stream, map(0, r7, r2));
        emit(stream, store(r3, r0, r1)); //store 72 in r3(1) segment @ offset r0(0)
        emit(stream, load(r2, r3, r0)); //load value from segment r3(i) into r2
        emit(stream, output(r2));
        emit(stream, unmap(r3));
        emit(stream, unmap(r4));
        emit(stream, unmap(r5));
        emit(stream, unmap(r6));
        emit(stream, unmap(r7));
        emit(stream, halt());
}
