#include <stdio.h>
#include <stdint.h>

uint32_t mem_read(uint8_t *memory_ptr, uint16_t address, int byte_len) {
    
    uint32_t result = 0;

    for (int i = 0; i < byte_len; i++) {
        result |= (*(memory_ptr + address + (byte_len - i - 1)) << ((byte_len - i - 1) * 8));
    }
    return result;

}

int main(int argc, char* argv[]) {

//     int reg[3] = {0} ;
//     write_reg(reg, 0, 1);
//     printf("%d\n", read_reg(reg, 0));
//     printf("%d\n", read_reg(reg, 1));
//     printf("%d\n", read_reg(reg, 2));



// }

// void write_reg(int *reg, int index, int value) {

//     *(reg + index) = value;

// }

// int read_reg(int *reg, int index) {
//     return *(reg + index);

    // FILE *path;
    // path = fopen(argv[1], "r");

    // uint8_t memory[(1 << 16)]; /* 2^16 locations*/  

    // int count = 0;
    
    // while(fread(&memory[count], sizeof(memory[count]), 1, path) != 0) {
    //     count++;
    // }

    // fclose(path);
    // int index = 0;

    // printf("%x\n", mem_read(memory, index += 4, 4));
    // printf("%x\n", mem_read(memory, index, 4));
    uint8_t memory[(1<<16)] = {NULL};
    printf("%x\n", memory[0xbfff]);


}
