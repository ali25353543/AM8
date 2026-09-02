#include <stdio.h>
#include <string.h>
#include <stdint.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
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
    |   4   | Zero Flag                     |
    | 5 - 8 | Reserved (yet)                |

    Supported Main Opcodes:

    mov : 0 (Successful)
    add : 1 (Successful, but don't add (a or [a]) to (a or [a]), because It will add b to a instead.)
    sub : 2 (Successful, but don't subtract (a or [a]) from (a or [a]), because It will subtract b from a instead.)
    mul : 3 (Successful, but don't multiply (a or [a]) by (a or [a]), because a will be 0 instead.)
    lod : 4 (Successful, but don't load a to a, because It will load a to itself instead.)
    jmp : 5 (Successful)
    cmp : 6 (Successful)
    jcc : 7 - 16 ()

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

static void print_ram(uint8_t *ram) {
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
    for (int i = 0; i < (32 * 5); i++) {
        printf("%02X ", ram[i]);
        if ((i + 1) % 32 == 0) {
            printf("\n");
        }
    }
}

int AM8(char *filename) {
    FILE *codefile = fopen(filename, "r");
    fgets(ram, 256, codefile);
    fclose(codefile);
    if (ram[0] != '!' || ram[1] != 'A' || ram[2] != 'M' || ram[3] != '8') {
        printf("InvalidCodeFileError\n");
        return 5;
    } else {
        ram[0] = 0;
        ram[1] = 0;
        ram[2] = 0;
        ram[3] = 0;
        mp = 4;
    }
    uint8_t opcode_byte = 0;
    #ifdef _WIN32
    system("cls");
    #else
    system("clear");
    #endif
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
                    mp++;
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
                    mp++;
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
                    printf("InvalidOpcodeError\n");
                    return 6;
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
                mp++;
                if ((opcode_byte & 3) == 0) {
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
                    mp++;
                    if (ram[mp] == regs[0]) {
                        flags = 1;
                    } else if (ram[mp] > regs[0]) {
                        flags = 2;
                    } else if (ram[mp] < regs[0]) {
                        flags = 4;
                    } else if (ram[mp] <= regs[0]) {
                        flags = 5;
                    } else if (ram[mp] >= regs[0]) {
                        flags = 3;
                    } else {
                        flags = 0;
                    }
                } else if ((opcode_byte & 3) == 1) {
                    mp++;
                    if (ram[mp] < 2) {
                        if (regs[ram[mp - 1]] == regs[0]) {
                            flags = 1;
                        } else if (regs[ram[mp - 1]] > regs[0]) {
                            flags = 2;
                        } else if (regs[ram[mp - 1]] < regs[0]) {
                            flags = 4;
                        } else if (regs[ram[mp - 1]] <= regs[0]) {
                            flags = 5;
                        } else if (regs[ram[mp - 1]] >= regs[0]) {
                            flags = 3;
                        } else {
                            flags = 0;
                        }
                    } else {
                        printf("InvalidOpcodeError\n");
                        return 6;
                    }
                } else if ((opcode_byte & 3) == 2) {
                    mp++;
                    if (ram[ram[mp]] == regs[0]) {
                        flags = 1;
                    } else if (ram[ram[mp]] > regs[0]) {
                        flags = 2;
                    } else if (ram[ram[mp]] < regs[0]) {
                        flags = 4;
                    } else if (ram[ram[mp]] <= regs[0]) {
                        flags = 5;
                    } else if (ram[ram[mp]] >= regs[0]) {
                        flags = 3;
                    } else {
                        flags = 0;
                    }
                } else if ((opcode_byte & 3) == 3) {
                    mp++;
                    if (ram[mp] < 2) {
                        if (ram[regs[ram[mp - 1]]] == regs[0]) {
                            flags = 1;
                        } else if (ram[regs[ram[mp - 1]]] > regs[0]) {
                            flags = 2;
                        } else if (ram[regs[ram[mp - 1]]] < regs[0]) {
                            flags = 4;
                        } else if (ram[regs[ram[mp - 1]]] <= regs[0]) {
                            flags = 5;
                        } else if (ram[regs[ram[mp - 1]]] >= regs[0]) {
                            flags = 3;
                        } else {
                            flags = 0;
                        }
                    } else {
                        printf("InvalidOpcodeError\n");
                        return 6;
                    }
                }
            } else if (((opcode_byte >> 2) & 31) >= 7 && ((opcode_byte >> 2) & 31) <= 18) {
                uint8_t cond = (opcode_byte >> 2) & 31;
                if ((opcode_byte & 3) == 0) {
                    mp++;
                    int taken = 0;
                    switch (cond) {
                        case 7:  taken = (flags & 1); break;
                        case 8:  taken = !(flags & 1); break;
                        case 9:  taken = (flags & 2); break;
                        case 10: taken = !(flags & 2); break;
                        case 11: taken = (flags & 4); break;
                        case 12: taken = !(flags & 4); break;
                        case 13: taken = ((flags & 1) || (flags & 2)); break;
                        case 14: taken = ((flags & 1) || (flags & 4)); break;
                        case 15: taken = (flags & 16); break;
                        case 16: taken = !(flags & 16); break;
                        case 17: taken = (flags & 8); break;
                        case 18: taken = !(flags & 8); break;
                        default: taken = 0; break;
                    }
                    if (taken) {
                        mp = ram[mp];
                        printf("Jumped to : %d\n", ((int) mp) & 0xFF);
                    } else {
                        printf("Not jumped to : %d\n", ((int) mp) & 0xFF);
                    }
                } else {
                    printf("InvalidOpcodeError\n");
                    return 6;
                }
            }
            mp++;
            ip++;
        } else {
            if (opcode_byte == 0) {
                printf("Breakpoint reached.\n");
                break;
            }
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
        #ifdef _WIN32
        Sleep(1000);
        system("cls");
        #else
        sleep(1000);
        system("clear");
        #endif
        }
        return 0;
    }