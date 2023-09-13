/*
Derek Law Pin Jie
510226244
dlaw7849
USYD CODE CITATION ACKNOWLEDGEMENT
This file contains acknowledgements of code
*/



#include <stdio.h>
#include <stdint.h>

#define MEMORY_MAX (1 << 16) //4 byte address
#define REG_COUNT 33
#define DATA_BIT_LEN 32
// #define SELF_DEBUG 1

uint32_t sign_extend(uint32_t x, int bit_count){

    if ((x >> (bit_count - 1) & 1)) {
        x |= (0xFFFFFFFF << bit_count);
    }
    return x;
}

int mem_write(uint8_t *memory_ptr, uint16_t address, uint32_t data, int byte_len) {

    for (int i = 0; i < byte_len; i++) {
        *(memory_ptr + address + i) = (data >> (i * 8)) & 0xff;
    }

    if (byte_len < 4) {
        for (int j = 0; j < byte_len; j++) {
            *(memory_ptr + address + (4 - byte_len) + j) = 0;
        }
    }

    return 0;
}

uint32_t mem_read(uint8_t *memory_ptr, uint16_t address, int byte_len) {
    
    uint32_t result = 0;

    for (int i = 0; i < byte_len; i++) {
        result |= (*(memory_ptr + address + (byte_len - i - 1)) << ((byte_len - i - 1) * 8));
    }
    return result;

}

void reg_write(uint32_t *reg, int index, uint32_t value) {

    if (index != 0) {
        *(reg + index) = value;
    }
    return;

}

// struct Heap {

//     int *free
//     struct Heap *next;

// };
 
int main(int argc, char **argv) {

    uint8_t memory[MEMORY_MAX]; /* 2^16 locations*/    
    uint32_t reg[REG_COUNT] = {0};

    // struct Heap block;

    enum {
        R_R0 = 0,
        R_R1,
        R_R2,
        R_R3,
        R_R4,
        R_R5,
        R_R6,
        R_R7,
        R_R8,
        R_R9,
        R_R10,
        R_R11,
        R_R12,
        R_R13,
        R_R14,
        R_R15,
        R_R16,
        R_R17,
        R_R18,
        R_R19,
        R_R20,
        R_R21,
        R_R22,
        R_R23,
        R_R24,
        R_R25,
        R_R26,
        R_R27,
        R_R28,
        R_R29,
        R_R30,
        R_R31,
        R_PC
    };

    enum opcode {

        OP_R     = 0b0110011, 
        OP_I     = 0b0010011,
        OP_I_MEM = 0b0000011,
        OP_I_JMP = 0b1100111,
        OP_U     = 0b0110111,
        OP_S     = 0b0100011,
        OP_SB    = 0b1100011,
        OP_UJ    = 0b1101111
    };

    enum R_Func7 {

        ADD_FUNC7 = 0b0000000,
        SUB_FUNC7 = 0b0100000,
        SRL_FUNC7 = 0b0000000,
        SRA_FUNC7 = 0b0100000
    };

    enum Func3 {
        ADD_FUNC3   = 0b000,
        SUB_FUNC3   = 0b000,
        XOR_FUNC3   = 0b100,
        OR_FUNC3    = 0b110,
        AND_FUNC3   = 0b111,
        SLL_FUNC3   = 0b001,
        SRL_FUNC3   = 0b101,
        SRA_FUNC3   = 0b101,
        SLT_FUNC3   = 0b010,
        SLTU_FUNC3  = 0b011,
        ADDI_FUNC3  = 0b000,
        XORI_FUNC3  = 0b100,
        ORI_FUNC3   = 0b110,
        ANDI_FUNC3  = 0b111,
        SLTI_FUNC3  = 0b010,
        SLTIU_FUNC3 = 0b011,
        LB_FUNC3    = 0b000,
        LH_FUNC3    = 0b001,
        LW_FUNC3    = 0b010,
        LBU_FUNC3   = 0b100,
        LHU_FUNC3   = 0b101,
        JALR_FUNC3  = 0b000,
        SB_FUNC3    = 0b000,
        SH_FUNC3    = 0b001,
        SW_FUNC3    = 0b010,
        BEQ_FUNC3   = 0b000,
        BNE_FUNC3   = 0b001,
        BLT_FUNC3   = 0b100,
        BLTU_FUNC3  = 0b110,
        BGE_FUNC3   = 0b101,
        BEGU_FUC3   = 0b111
    };

    enum VirtualRoutine {

        VIR_W_CHAR   = 0x0800,
        VIR_W_INT    = 0x0804,
        VIR_W_UINT   = 0x0808,
        VIR_HALT     = 0x080C,
        VIR_R_CHAR   = 0x0812,
        VIR_R_INT    = 0x0816,
        VIR_DUMP_PC  = 0x0820,
        VIR_DUMP_REG = 0x0824,
        VIR_DUMP_MEM = 0x0828,
        VIR_MALLOC   = 0x0830,
        VIR_FREE     = 0x0834

    };

    FILE *path;
    path = fopen(argv[1], "r");

    int count = 0;
    
    while(fread(&memory[count], sizeof(memory[count]), 1, path) != 0) {
        count++;
    }

    fclose(path);

    // printf("%d\n", count);

    uint32_t inst;
    uint8_t opcode;
    
    reg[R_PC] = 0;

    // int pro_c = 0;

    while (1) {

        inst = mem_read(memory, reg[R_PC], 4);

        #ifdef SELF_DEBUG
        printf("instruction code: %x\n", inst); 
        #endif
        
        #ifdef SELF_DEBUG
        printf("program counter: %x\n", reg[R_PC]);
        #endif 
        opcode = inst & 127;

        #ifdef SELF_DEBUG
        printf("opcode: %x\n", opcode);
        #endif
        /* opcode */ 
        switch(opcode) {

            // R type
            case(OP_R) :
                {
                    uint8_t rd = (inst >> 7) & 0b11111;
                    uint8_t func3 = (inst >> 12) & 0b111;
                    uint8_t rs1 = (inst >> 15) & 0b11111;
                    uint8_t rs2 = (inst >> 20) & 0b11111;
                    uint8_t func7 = (inst >> 25);


                    #ifdef SELF_DEBUG
                    if (rd == 8) {
                        printf("R - %d, %d, %d, %d\n", func3, func7, rs1, rs2);
                    }
                    printf("func: %x\n", func3);
                    #endif

                    switch (func3) {
                        case(ADD_FUNC3) :
                            
                            switch(func7) {

                                case(ADD_FUNC7) :
                                    reg_write(reg, rd, reg[rs1] + reg[rs2]);
                                    
                                    #ifdef SELF_DEBUG
                                    printf("add reg %d, reg %d to reg%d", rs1, rs2, rd);
                                    #endif

                                    break;

                                case(SUB_FUNC7) :
                                    reg_write(reg, rd, reg[rs1] - reg[rs2]);

                                    #ifdef SELF_DEBUG
                                    printf("sub reg %d, reg %d to reg%d", rs1, rs2, rd);
                                    #endif
                                    break;
                            }
                            break;
                        
                        case(XOR_FUNC3) :
                            reg_write(reg, rd, reg[rs1] ^ reg[rs2]);
                            break;

                        case(OR_FUNC3) :
                            reg_write(reg, rd, reg[rs1] | reg[rs2]);
                            break;

                        case(AND_FUNC3) :
                            reg_write(reg, rd, reg[rs1] & reg[rs2]);
                            break;
                        
                        case(SLL_FUNC3) :
                            reg_write(reg, rd, reg[rs1] << reg[rs2]);
                            break;

                            #ifdef SELF_DEBUG
                            printf("shift left reg %d, reg %d to reg%d", rs1, rs2, rd);
                            #endif
                        
                        case(SRL_FUNC3) :

                            switch(func7) {

                                case(SRL_FUNC7) :
                                    reg_write(reg, rd, reg[rs1] >> reg[rs2]);
                                    #ifdef SELF_DEBUG
                                    printf("shift right reg %d, reg %d to reg%d", rs1, rs2, rd);
                                    printf("data: %d\n", reg[rs1]);
                                    printf("bits: %d\n", reg[rs2]);

                                    #endif
                                    break;

                                case(SRA_FUNC7) :

                                    {
                                        unsigned int numbits = reg[rs2];

                                        #ifdef SELF_DEBUG
                                        printf("rotate reg %d by reg %d to reg%d", rs1, rs2, rd);
                                        #endif

                                        reg_write(reg, rd, (reg[rs1] >> numbits) | (DATA_BIT_LEN - numbits));
                                    
                                    }
                                    break;

                                break;
                            }
                        
                        case(SLT_FUNC3) :
                            {
                                int32_t s_reg1 = reg[rs1];
                                int32_t s_reg2 = reg[rs2];

                                reg_write(reg, rd, ((s_reg1 < s_reg2) ? 1 : 0));

                            }
                            break;

                        case(SLTU_FUNC3) :
                            reg_write(reg, rd, (reg[rs1] < reg[rs2]) ? 1 : 0);
                            break;
                    
                    }
                }
                reg[R_PC] += 0x4;
                break;
            
            case(OP_I):
                {
                    uint8_t rd = (inst >> 7) & 0b11111;
                    uint8_t func3 = (inst >> 12) & 0b111;
                    uint8_t rs1 = (inst >> 15) & 0b11111;
                    int32_t imm = sign_extend(inst >> 20, 12);

                    #ifdef SELF_DEBUG
                    if (rd == 8) {
                        printf("IMMEDIATE - %d, %d, %d\n", func3, rs1, imm);
                    }
                    #endif

                    switch(func3) {

                        case(ADDI_FUNC3) :
                            reg_write(reg, rd, reg[rs1] + imm);
                            break;

                        case(XORI_FUNC3) :
                            reg_write(reg, rd, reg[rs1] ^ imm);
                            break;

                        case(ORI_FUNC3) :
                            reg_write(reg, rd, reg[rs1] | imm);
                            break;

                        case(ANDI_FUNC3) :
                            reg_write(reg, rd, reg[rs1] & imm);
                            break;
                        
                        case(SLTI_FUNC3) :
                            reg_write(reg, rd, (reg[rs1] < imm) ? 1 : 0);
                            break;
                        
                        case(SLTIU_FUNC3) :
                            reg_write(reg, rd, (reg[rs1] < (uint16_t)imm) ? 1 : 0);
                            break;

                    }
                }
                reg[R_PC] += 0x4;
                break;

            /* load byte */
            case(OP_I_MEM) :
                {
                    uint8_t rd = (inst >> 7) & 0b11111;
                    uint8_t func3 = (inst >> 12) & 0b111;
                    uint8_t rs1 = (inst >> 15) & 0b11111;
                    int32_t imm = sign_extend(inst >> 20, 12);

                    #ifdef SELF_DEBUG
                    if (rd == 8) {
                        printf("LB - %d, %d, %d\n", func3, rs1, imm);
                    }
                    #endif

                    // printf("load byte - %d\n", reg[rs1] + sign_extend(imm, 12));
                    // printf("rs1 - %d\n", reg[rs1]);
                    // printf("imm - %d\n", sign_extend(imm, 12));

                    switch(reg[rs1] + imm) {

                        case(VIR_R_CHAR) :
                            #ifdef SELF_DEBUG
                            printf("scan char\n");
                            #endif

                            reg_write(reg, rd, (uint32_t)getchar());
                            break;

                        case(VIR_R_INT) :
                            #ifdef SELF_DEBUG
                            printf("scan int\n");
                            #endif

                            if (rd != 0) {
                                scanf("%d", &reg[rd]);
                            }
                            break;

                        default :
                            switch(func3) {

                                case(LB_FUNC3) :
                                    reg_write(reg, rd, sign_extend(mem_read(memory, reg[rs1] + imm, 1), 8));
                                    break;

                                case(LH_FUNC3):
                                    reg_write(reg, rd, sign_extend(mem_read(memory, reg[rs1] + imm, 2), 16));
                                    break;

                                case(LW_FUNC3):
                                    reg_write(reg, rd, mem_read(memory, reg[rs1] + imm, 4));
                                    break;

                                case(LBU_FUNC3):
                                    reg_write(reg, rd, (uint8_t) mem_read(memory, reg[rs1] + imm, 1));
                                    break;

                                case(LHU_FUNC3):
                                    reg_write(reg, rd, (uint16_t) mem_read(memory, reg[rs1] + imm, 2));
                                    break;
                            }
                        break;
                    }
                }
                reg[R_PC] += 0x4;
                break;

            /* jalr */
            case(OP_I_JMP):
                {
                    uint8_t rd = (inst >> 7) & 0b11111;
                    uint8_t rs1 = (inst >> 15) & 0b11111;
                    int32_t imm = sign_extend(inst >> 20, 12);

                    reg_write(reg, rd, reg[R_PC] + 4);
                    reg[R_PC] = reg[rs1] + imm;
                }
                break;

            /* load immediate */
            case(OP_U) :
                    uint8_t rd = (inst >> 7) & 0b11111;
                    int32_t imm = sign_extend((inst >> 12), 12);

                #ifdef SELF_DEBUG
                    if (rd == 8) {
                        printf("LOAD IMM - %d\n", imm);
                    }
                    #endif
                {   
                    #ifdef SELF_DEBUG
                    printf("load %d to reg %d", imm, rd);
                    #endif
                }
                reg_write(reg, rd, imm << 12);
                reg[R_PC] += 0x4;
                break;

            /* save byte */
            case(OP_S) :
                {
                    int8_t imm5 = (inst >> 7) & 31;
                    uint8_t func3 = (inst >> 12) & 7;
                    uint8_t rs1 = (inst >> 15) & 31;
                    uint8_t rs2 = (inst >> 20) & 31;
                    int8_t imm7 = inst >> 25;
                    int32_t imm = sign_extend((imm7 << 5) | imm5, 12);


                    #ifdef SELF_DEBUG
                    printf("save byte: reg %d\n", rs2);
                    if (rd == 8) {
                        printf("SAVE BYTE - %d, %d, %d\n", func3, imm, rs1, rs2);
                    }
                    #endif

                    switch(reg[rs1] + imm) {

                        case(VIR_W_CHAR) :

                            #ifdef SELF_DEBUG
                            printf("Console write char: reg %d", rs2);
                            #endif

                            printf("%c", (char)reg[rs2]);
                            fflush(stdout);
                            break;

                        case(VIR_W_INT) :

                            #ifdef SELF_DEBUG
                            printf("Console write int: reg %d", rs2);
                            #endif

                            printf("%d", (int32_t)reg[rs2]);
                            fflush(stdout);
                            break;

                        case(VIR_W_UINT) :

                            #ifdef SELF_DEBUG
                            printf("Console write uint in hex: reg %d", rs2);
                            #endif

                            printf("%x", reg[rs2]);

                            fflush(stdout);
                            break;

                        case(VIR_HALT) :

                            printf("CPU Halt Requested\n");
                            fflush(stdout);
                            return 0;
                            break;

                        case(VIR_DUMP_PC) :
                            
                            #ifdef SELF_DEBUG
                            printf("program counter: ");
                            #endif

                            printf("%x", reg[R_PC]);
                            fflush(stdout);
                            break;

                        case(VIR_DUMP_REG) :

                            printf("PC = %x\n", reg[R_PC]);

                            for (int i = R_R0; i <= R_R31; i++) {
                                printf("R[%d] = %x\n", i, reg[i]);
                            }
                            break;

                        case(VIR_DUMP_MEM) :

                            #ifdef SELF_DEBUG
                            printf("memory dump: ");
                            #endif

                            printf("%x\n", (uint32_t)mem_read(memory, reg[rs2],1));
                            fflush(stdout);
                            break;

                        // case(VIR_MALLOC) :

                        //     uint16_t start_addr;
                        //     uint32_t data_size;

                        //     int numBlock = (data_size / 64) +  (data_size % 64 != 0);

                        //     if (reg[29] == 0) {
                        //         start_addr = 0xb700;
                        //     }


                        //     int saved = 1;

                        //     for (int count = 1; count <= 128; count++) {
                                
                                 
                        //     }


                            



                        //     break;

                        default :

                            switch(func3) {
                                case (SB_FUNC3) :
                                    mem_write(memory, reg[rs1] + imm, reg[rs2], 1);
                                    break;

                                case (SH_FUNC3) :
                                    mem_write(memory, reg[rs1] + imm, reg[rs2], 2);
                                    break;

                                case (SW_FUNC3) :
                                    mem_write(memory, reg[rs1] + imm, reg[rs2], 4);
                                    break;
                            }
                        break;
                    }
                }
                reg[R_PC] += 0x4;
                break;

            case(OP_SB) :
                {
                    uint8_t imm5 = (inst >> 7) & 31;
                    uint8_t func3 = (inst >> 12) & 7;
                    uint8_t rs1 = (inst >> 15) & 31;
                    uint8_t rs2 = (inst >> 20) & 31;
                    uint8_t imm7 = inst >> 25;

                    /* shifting LSB to 11th position*/
                    
                    int16_t bit11 = (imm5 & 1) << 10;
                    int8_t bit4to1 = imm5 >> 1;
                    int8_t bit10to5 = (imm7 & 63) << 4;
                    int16_t bit12 = (imm7 >> 6) << 11;

                    int16_t imm = bit12 | bit11 | bit10to5 | bit4to1;

                    #ifdef SELF_DEBUG
                    printf("%x\n", imm);
                    #endif

                    switch (func3)
                    {
                        case (BEQ_FUNC3) :
                        // if(R[rs1] == R[rs2]) then PC = PC + (imm « 1)
                            if (reg[rs1] == reg[rs2]) {
                                reg[R_PC] = reg[R_PC] + sign_extend(imm << 1, 13);
                            } else {
                                reg[R_PC] += 0x4;
                            }
                            break;

                        case (BNE_FUNC3) :
                            // if(R[rs1] != R[rs2]) then PC = PC + (imm « 1)
                            if (reg[rs1] != reg[rs2]) {
                                reg[R_PC] = reg[R_PC] + sign_extend(imm << 1, 13);
                            } else {
                                    reg[R_PC] += 0x4;
                                }
                            break;

                        case (BLT_FUNC3) :
                        // if(R[rs1] < R[rs2]) then PC = PC + (imm « 1)
                            if (reg[rs1] < reg[rs2]) {
                                reg[R_PC] = reg[R_PC] + sign_extend(imm << 1, 13);
                            } else {
                                reg[R_PC] += 0x4;
                            }
                            break;

                        case (BLTU_FUNC3) :
                        // if(R[rs1] < R[rs2]) then PC = PC + (imm « 1)
                            if ((uint32_t)reg[rs1] < (uint32_t)reg[rs2]) {
                                reg[R_PC] = reg[R_PC] + sign_extend(imm << 1, 13);
                            } else {
                                reg[R_PC] += 0x4;
                            }
                            break;

                        case (BGE_FUNC3) :
                        // if(R[rs1] < R[rs2]) then PC = PC + (imm « 1)
                            if (reg[rs1] > reg[rs2]) {
                                reg[R_PC] = reg[R_PC] + sign_extend(imm << 1, 13);
                            } else {
                                reg[R_PC] += 0x4;
                            }
                            break;

                        case (BEGU_FUC3) :
                        // if(R[rs1] < R[rs2]) then PC = PC + (imm « 1)
                            if ((uint32_t)reg[rs1] > (uint32_t)reg[rs2]) {
                                reg[R_PC] = reg[R_PC] + sign_extend(imm << 1, 13);
                            } else {
                                reg[R_PC] += 0x4;
                            }
                            break;
                    }
                }
                break;
            
            /* jal */
            case(OP_UJ) :
                {
                uint8_t rd = (inst >> 7) & 0b11111;
                int32_t imm_scrambled = (inst >> 12);

                int32_t byte1 = (imm_scrambled >> 19 & 1) << 19; //bit 20
                int32_t byte2 = (imm_scrambled >> 9) & 1023; //bit 10 - 1
                int32_t byte3 = ((imm_scrambled >> 8) & 1) << 10; // bit 11
                int32_t byte4 = ((imm_scrambled) & 255) << 11; // bit 19 - 12

                // printf("imm scrambled - %x\n", imm_scrambled);
                
                int32_t imm = byte1 | byte2 | byte3 | byte4;

                #ifdef SELF_DEBUG
                printf("jump to addr: %x\n", reg[R_PC] + sign_extend(imm << 1, 19));
                #endif
                
                reg_write(reg, rd, reg[R_PC] + 0x4);
                reg[R_PC] = reg[R_PC] + sign_extend(imm << 1, 19);

                }

                break;

            default:
                printf("Instruction Not Implemented: %x\n", inst);
                break;

        }

        // #ifdef SELF_DEBUG
        //     for (int i = 0; i < 32; i++) {
        //     printf("reg %d: %d ", i, reg[i]);
        // }
        // printf("\n");
        // #endif
    }
}