#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <windows.h>
#include <stdlib.h>

/*

    AM8 = Alintosh Machine 8-bit
    AA8 = Alintosh Architure 8-bit

    MIT License

    Alintosh Machine 8-bit is a simple 8-bit machine emulator.
    Honestly, This machine emulator is a hobby, for testing purposes.
    I designed it to be similar to "x86" Machines (PCs and Laptops).
    Similar to x86 means I used Dynamic Instruction Compeletion (DIC).
    That is because ARM32, ARM64 (AArch64), Thumb Architures imposes upon you Constant Number of Instructions.
    Max Instruction Size: 2 Bytes.
    Physical Memory Size: 256 Bytes.
    
    Opcode Byte:
    
    1 1_1_1_1_1 1_1
    |    |____   |______
    v         v         v
    #Operands Operation Between R / M ?

    if highest bit of Opcode is set and frist bit and second bit of Opcode are cleared, This mean Next byte is Constant (e.g. 3).
    if highest bit of Opcode is set and frist bit of Opcode is set and second bit of Opcode are cleared and Next byte is smaller than 4,
    This mean Next byte is Register.
    else if Next byte isn't smaller than 4 , This mean "InvalidOpcodeError" (Proccessor exits with exit code 6).
    if highest bit of Opcode is set and frist bit of Opcode is cleared and second bit of Opcode is set,
    This mean Next byte is Memory Address (e.g. [0xEE]).
    if highest bit of Opcode is set and frist bit and second bit of Opcode are set and Next byte is smaller than 4,
    This mean Next byte is Memory Address but with Register instead of Constant (e.g. [a]).
    if highest bit of Opcode is cleared, This mean Extended Opcode.

    FLAGS:
    FLAGS Reg. used to store the status of the last operation.

    __________________________________________
    |  Bit  | Description                   |
    |   0   | Equal Flag                    |
    |   1   | Greater Than Flag             |
    |   2   | Smaller Than Flag             |
    |   3   | Overflow Flag                 |
    | 4 - 8 | Reserved (yet)                |

    Supported Main Opcodes:

    mov : 0 (Successful)
    add : 1 (Successful, but don't add (a or [a]) to (a or [a]), because It will add b to a instead.)
    sub : 2 (Successful, but don't subtract (a or [a]) from (a or [a]), because It will subtract b from a instead.)
    mul : 3 (Successful, but don't multiply (a or [a]) by (a or [a]), because a will be 0 instead.)
    lod : 4 (Successful, but don't load a to a, because It will load a to itself instead.)
    jmp : 5 (Successful)
    cmp : 6 (Successful)
    je : 7

*/

uint8_t ram[256] = {0};
uint8_t regs[2] = {0, 2};
uint8_t flags = 0;
uint8_t ip = 0;
uint8_t mp = 0;
uint8_t cr = 0;
uint8_t ss = 0;
uint8_t ds = 0;
uint8_t cs = 0;
int8_t input[2] = {0};

void print_ram(uint8_t *ram) {
    system("cls");
    for (int i = 0; i < (32 * 5); i++) {
        printf("%02X ", ram[i]);
        if ((i + 1) % 32 == 0) {
            printf("\n");
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Too few args.\n");
        return 1;
    }
    FILE *codefile = fopen(argv[1], "r");
    fgets(ram, 256, codefile);
    fclose(codefile);
    uint8_t opcode_byte = 0;
    system("cls");
    while (1) {
        print_ram(ram);
        printf("Current Memory Address (CMA) is : %d\n", ((int) mp) & 0xFF);
        //printf("#Instruction Executed (#IE) is : %d\n", ((int) ip) & 0xFF);
        opcode_byte = ram[mp];
        printf("Current Opcode Byte (COB) is : %d\n", ((int) opcode_byte) & 0xFF);
        if (opcode_byte & 128) {
            if (((opcode_byte >> 2) & 31) == 0) {
                if ((opcode_byte & 3) == 0) {
                    mp++;
                    regs[0] = ram[mp];
                    mp++;
                    printf("Register A is : %d\n", ((int) regs[0]) & 0xFF);
                } else if ((opcode_byte & 3) == 1) {
                    if (ram[mp + 1] < 2) {
                        regs[0] = regs[ram[mp - 1]];
                        printf("Register A is : %d\n", ((int) regs[0]) & 0xFF);
                    } else {
                        printf("InvalidOpcodeError\n");
                        return 6;
                    }
                } else if ((opcode_byte & 3) == 2) {
                    mp++;
                    regs[0] = ram[ram[mp]];
                    printf("Register A is : %d\n", ((int) regs[0]) & 0xFF);
                } else if ((opcode_byte & 3) == 3) {
                    if (ram[mp + 1] < 2) {
                        regs[0] = ram[regs[ram[mp - 1]]];
                        printf("Register A is : %d\n", ((int) regs[0]) & 0xFF);
                    } else {
                        printf("InvalidOpcodeError\n");
                        return 6;
                    }
                }
            } else if (((opcode_byte >> 2) & 31) == 1) {
                if ((opcode_byte & 3) == 0) {
                    mp++;
                    regs[0] += ram[mp];
                    mp++;
                    printf("Register A is : %d\n", ((int) regs[0]) & 0xFF);
                } else if ((opcode_byte & 3) == 1) {
                    if (ram[mp + 1] < 2) {
                        regs[0] = regs[0] + regs[ram[mp - 1]];
                        printf("Register A is : %d\n", ((int) regs[0]) & 0xFF);
                    } else {
                        printf("InvalidOpcodeError\n");
                        return 6;
                    }
                } else if ((opcode_byte & 3) == 2) {
                    mp++;
                    regs[0] += ram[ram[mp]];
                    printf("Register A is : %d\n", ((int) regs[0]) & 0xFF);
                } else if ((opcode_byte & 3) == 3) {
                    if (ram[mp + 1] < 2) {
                        regs[0] += ram[regs[ram[mp - 1]]];
                        printf("Register A is : %d\n", ((int) regs[0]) & 0xFF);
                    } else {
                        printf("InvalidOpcodeError\n");
                        return 6;
                    }
                }
            } else if (((opcode_byte >> 2) & 31) == 2) {
                if ((opcode_byte & 3) == 0) {
                    mp++;
                    regs[0] -= ram[mp];
                    printf("Register A is : %d\n", ((int) regs[0]) & 0xFF);
                } else if ((opcode_byte & 3) == 1) {
                    if (ram[mp + 1] < 2) {
                        regs[0] = regs[0] - regs[ram[mp - 1]];
                        printf("Register A is : %d\n", ((int) regs[0]) & 0xFF);
                  } else {
                        printf("InvalidOpcodeError\n");
                        return 6;
                }
                } else if ((opcode_byte & 3) == 2) {
                    mp++;
                    regs[0] -= ram[ram[mp]];
                    printf("Register A is : %d\n", ((int) regs[0]) & 0xFF);
                } else if ((opcode_byte & 3) == 3) {
                    if (ram[mp + 1] < 2) {
                        regs[0] -= ram[regs[ram[mp - 1]]];
                        printf("Register A is : %d\n", ((int) regs[0]) & 0xFF);
                    } else {
                        printf("InvalidOpcodeError\n");
                        return 6;
                    }
                }
            } else if (((opcode_byte >> 2) & 31) == 3) {
                if ((opcode_byte & 3) == 0) {
                    mp++;
                    regs[0] = regs[0] * ram[mp];
                    printf("Register A is : %d\n", ((int) regs[0]) & 0xFF);
                } else if ((opcode_byte & 3) == 1) {
                    if (ram[mp + 1] < 2) {
                        regs[0] = regs[0] * regs[ram[mp - 1]];
                        printf("Register A is : %d\n", ((int) regs[0]) & 0xFF);
                    } else {
                        printf("InvalidOpcodeError\n");
                        return 6;
                    }
                } else if ((opcode_byte & 3) == 2) {
                    mp++;
                    regs[0] = regs[0] * ram[ram[mp]];
                    printf("Register A is : %d\n", ((int) regs[0]) & 0xFF);
                } else if ((opcode_byte & 3) == 3) {
                    if (ram[mp + 1] < 2) {
                        regs[0] = regs[0] * ram[regs[ram[mp - 1]]];
                        printf("Register A is : %d\n", ((int) regs[0]) & 0xFF);
                    } else {
                        printf("InvalidOpcodeError\n");
                        return 6;
                    }
                }
            } else if (((opcode_byte >> 2) & 31) == 4) {
                if ((opcode_byte & 3) == 0) {
                    mp++;
                    regs[0] = ram[mp];
                    printf("Register A is : %d\n", ((int) regs[0]) & 0xFF);
                } else if ((opcode_byte & 3) == 1) {
                    if (ram[mp + 1] < 2) {
                        regs[ram[mp - 1]] = regs[0];
                        printf("Register A is : %d\n", ((int) regs[0]) & 0xFF);
                    } else {
                        printf("InvalidOpcodeError\n");
                        return 6;
                    }
                } else if ((opcode_byte & 3) == 2) {
                    mp++;
                    ram[ram[mp]] = regs[0];
                    printf("Register A is : %d\n", ((int) regs[0]) & 0xFF);
                } else if ((opcode_byte & 3) == 3) {
                    if (ram[mp + 1] < 2) {
                        ram[regs[ram[mp - 1]]] = regs[0];
                        printf("Register A is : %d\n", ((int) regs[0]) & 0xFF);
                    } else {
                        printf("InvalidOpcodeError\n");
                        return 6;
                    }
                }
            } else if (((opcode_byte >> 2) & 31) == 5) {
                if ((opcode_byte & 3) == 0) {
                    mp++;
                    mp = ram[mp];
                    printf("Jumped to : %d\n", ((int) mp) & 0xFF);
                } else if ((opcode_byte & 3) == 1) {
                    if (ram[mp + 1] < 2) {
                        mp = regs[ram[mp - 1]];
                        printf("Jumped to : %d\n", ((int) mp) & 0xFF);
                    } else {
                        printf("InvalidOpcodeError\n");
                        return 6;
                    }
                } else if ((opcode_byte & 3) == 2) {
                    mp++;
                    mp = ram[ram[mp]];
                    printf("Jumped to : %d\n", ((int) mp) & 0xFF);
                } else if ((opcode_byte & 3) == 3) {
                    if (ram[mp + 1] < 2) {
                        mp = ram[regs[ram[mp - 1]]];
                        printf("Jumped to : %d\n", ((int) mp) & 0xFF);
                    } else {
                        printf("InvalidOpcodeError\n");
                        return 6;
                    }
                }
            } else if (((opcode_byte >> 2) & 31) == 6) {
                if ((opcode_byte & 3) == 0) {
                    if (ram[mp + 1] == regs[0]) {
                        flags |= 1;
                    } else if (ram[mp + 1] > regs[0]) {
                        flags |= 2;
                    } else if (ram[mp + 1] < regs[0]) {
                        flags |= 4;
                    } else if (ram[mp + 1] >= regs[0]) {
                        flags |= 3;
                    } else if (ram[mp + 1] <= regs[0]) {
                        flags |= 5;
                    }
                } else if ((opcode_byte & 3) == 1) {
                    if (ram[mp + 1] < 2) {
                        if (regs[ram[mp - 1]] == regs[0]) {
                            flags |= 1;
                        } else if (regs[ram[mp - 1]] > regs[0]) {
                            flags |= 2;
                        } else if (regs[ram[mp - 1]] < regs[0]) {
                            flags |= 4;
                        } else if (regs[ram[mp - 1]] >= regs[0]) {
                            flags |= 3;
                        } else if (regs[ram[mp - 1]] <= regs[0]) {
                            flags |= 5;
                        }
                    } else {
                        printf("InvalidOpcodeError\n");
                        return 6;
                    }
                } else if ((opcode_byte & 3) == 2) {
                    mp++;
                    if (ram[ram[mp]] == regs[0]) {
                        flags |= 1;
                    } else if (ram[ram[mp]] > regs[0]) {
                        flags |= 2;
                    } else if (ram[ram[mp]] < regs[0]) {
                        flags |= 4;
                    } else if (ram[ram[mp]] >= regs[0]) {
                        flags |= 3;
                    } else if (ram[ram[mp]] <= regs[0]) {
                        flags |= 5;
                    }
                } else if ((opcode_byte & 3) == 3) {
                    if (ram[mp + 1] < 2) {
                        if (ram[regs[ram[mp - 1]]] == regs[0]) {
                            flags |= 1;
                        } else if (ram[regs[ram[mp - 1]]] > regs[0]) {
                            flags |= 2;
                        } else if (ram[regs[ram[mp - 1]]] < regs[0]) {
                            flags |= 4;
                        } else if (ram[regs[ram[mp - 1]]] >= regs[0]) {
                            flags |= 3;
                        } else if (ram[regs[ram[mp - 1]]] <= regs[0]) {
                            flags |= 5;
                        }
                    } else {
                        printf("InvalidOpcodeError\n");
                        return 6;
                    }
                } else if (((opcode_byte >> 2) & 31) == 7/*je*/) {
                    if ((opcode_byte & 3) == 0) {
                        if (flags & 1) {
                            mp = ram[mp];
                            printf("Jumped to : %d\n", ((int) mp) & 0xFF);
                        }
                    } else {
                        printf("InvalidOpcodeError\n");
                        return 6;
                    }
                } else if (((opcode_byte >> 2) & 31) == 8 /*jne*/) {
                    if ((opcode_byte & 3) == 0) {
                        if (!(flags & 1)) {
                            mp = ram[mp];
                            printf("Jumped to : %d\n", ((int) mp) & 0xFF);
                        }
                    } else {
                        printf("InvalidOpcodeError\n");
                        return 6;
                    }
                } else if (((opcode_byte >> 2) & 31) == 9 /*jg*/) {
                    if ((opcode_byte & 3) == 0) {
                        if (flags & 2) {
                            mp = ram[mp];
                            printf("Jumped to : %d\n", ((int) mp) & 0xFF);
                        }
                    } else {
                        printf("InvalidOpcodeError\n");
                        return 6;
                    }
                } else if (((opcode_byte >> 2) & 31) == 10 /*jng*/) {
                    if ((opcode_byte & 3) == 0) {
                        if (!(flags & 2)) {
                            mp = ram[mp];
                            printf("Jumped to : %d\n", ((int) mp) & 0xFF);
                        }
                    } else {
                        printf("InvalidOpcodeError\n");
                        return 6;
                    }
                } else if (((opcode_byte >> 2) & 31) == 11 /*js*/) {
                    if ((opcode_byte & 3) == 0) {
                        if (flags & 4) {
                            mp = ram[mp];
                            printf("Jumped to : %d\n", ((int) mp) & 0xFF);
                        }
                    } else {
                        printf("InvalidOpcodeError\n");
                        return 6;
                    }
                } else if (((opcode_byte >> 2) & 31) == 12 /*jns*/) {
                    if ((opcode_byte & 3) == 0) {
                        if (!(flags & 4)) {
                            mp = ram[mp];
                            printf("Jumped to : %d\n", ((int) mp) & 0xFF);
                        }
                    } else {
                        printf("InvalidOpcodeError\n");
                        return 6;
                    }
                } else if (((opcode_byte >> 2) & 31) == 13 /*jge*/) {
                    if ((opcode_byte & 3) == 0) {
                        if ((flags & 1) || (flags & 2)) {
                            mp = ram[mp];
                            printf("Jumped to : %d\n", ((int) mp) & 0xFF);
                        }
                    } else {
                        printf("InvalidOpcodeError\n");
                        return 6;
                    }
                } else if (((opcode_byte >> 2) & 31) == 14 /*jse*/) {
                    if ((opcode_byte & 3) == 0) {
                        if ((flags & 1) || (flags & 4)) {
                            mp = ram[mp];
                            printf("Jumped to : %d\n", ((int) mp) & 0xFF);
                        }
                    } else {
                        printf("InvalidOpcodeError\n");
                        return 6;
                    }
                }
            mp++;
            ip++;
        } else {
            mp++;
        }
        printf("FLAGS : %d\n", ((int) flags) & 0xFF);
        printf("A : %d\n", ((int) regs[0]) & 0xFF);
        printf("B : %d\n", ((int) regs[1]) & 0xFF);
        /*
        printf("> ");
        scanf("%2s", input);
        if (input[0] >= 'a' && input[0] <= 'z') {
            input[0] -= 32;
        }
        if (input[1] >= 'a' && input[1] <= 'z') {
            input[1] -= 32;
        }
        if (input[0] < 'A' || input[0] > 'F' || input[1] < 'A' || input[1] > 'F') {
            printf("Invalid Input. Please enter two letters between A and F.\n");
            continue;
        }
        printf("%02X\n",((int)ram[((input[0] - 'A') + (input[1] - 'A') * 16)]) & 0xFF);
        */
        Sleep(1000);
        system("cls");
        }
    }
}