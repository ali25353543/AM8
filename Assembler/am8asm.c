#include <stdio.h>
#include <string.h>
#include <stdint.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include "am8asm.h"

int main() {
    uint8_t pointer = 0;
    char cmd[256];
    uint8_t executable[256];
    executable[0] = '!';
    executable[1] = 'A';
    executable[2] = 'M';
    executable[3] = '8';
    pointer = 4; // Set the pointer to the next available position in the executable array
    while (1) {
        printf("am8asm> ");
        fgets(cmd, sizeof(cmd), stdin);
        cmd[strcspn(cmd, "\n")] = 0; // Remove the newline character
        if (strcmp(cmd, "exit") == 0) {
            printf("Are you sure you want to exit without writing the executable? (y/n): ");
            char confirm;
            confirm = getc(stdin); // Consume the newline character left in the input buffer
            if (confirm == 'y' || confirm == 'Y') {
                printf("\nExitig...\n");
                break;
            } else {
                printf("\nExit canceled.\n");
                continue;
            }
        } else if (strcmp(cmd, "write") == 0) {
            printf("Enter the name of the executable file to write: ");
            char filename[256];
            scanf("%s", filename);
            FILE *file = fopen(filename, "xb");
            if (file == NULL) {
                printf("Error opening file for writing.\n");
                fclose(file);
                continue;
            }
            fwrite(executable,sizeof(uint8_t),  pointer, file);
            fclose(file);
            printf("Executable written to %s\n", filename);
        } else if (strncmp(cmd, "mov ", 4) == 0) {
            if (strtok(cmd, " ") == NULL /*|| strtok(NULL, " ") == NULL*/) {
                printf("Invalid mov command. Usage: mov <value>\n");
                continue;
            } else {
                char *value_str = strtok(NULL, " ");
                if (value_str[0] == '0' && value_str[1] == 'x') {
                    // Convert hex string to uint8_t
                    uint8_t value = (uint8_t)strtol(value_str, NULL, 16);
                    executable[pointer++] = MOV_CONST; // Assuming 0x80 is the opcode for mov
                    executable[pointer++] = value;
                } else if (strcmp(value_str, "b") == 0 || strcmp(value_str, "B") == 0 || strcmp(value_str, "a") == 0 || strcmp(value_str, "A") == 0) {
                    // Convert hex string to uint8_t
                    executable[pointer++] = MOV_REG; // Assuming 0x80 is the opcode for mov
                    executable[pointer++] = (value_str[0] == 'b' || value_str[0] == 'B') ? 0x01 : 0x00; // Assuming 0x00 is the code for register B and 0x01 for register A
                } else if (value_str[0] == '[' && value_str[strlen(value_str) - 1] == ']') {
                    // Convert decimal string to uint8_t
                    value_str[strlen(value_str) - 1] = '\0'; // Remove the closing bracket
                    uint8_t value = (uint8_t)strtol(value_str++, NULL, 16);
                    executable[pointer++] = MOV_ADDR; // Assuming 0x80 is the opcode for mov
                    executable[pointer++] = value;
                }
                printf("Value to move: %s\n", value_str);
            }
        } else {
            printf("Unknown command: %s\n", cmd);
        }
    }
    return 0;
}