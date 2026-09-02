#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

int AM8(char *filename);
extern uint8_t ram[256];

int main(int argc, char *argv[]) {
    printf("AM8 v1.0\n");
    if (argc < 2) {
        int runnig = 1;
        while (runnig) {
            printf("> ");
            char input[256];
            fgets(input, 256, stdin);
            if ((((input[0] >= 'A' && input[0] <= 'Z') || (input[0] >= 'a' && input[0] <= 'z')) && (input[1] == ':' && input[2] == '\\')) || (input[0] == '.' && input[1] == '\\') || (input[0] == '.' && input[1] == '.' && input[2] == '\\')) {
                    input[strcspn(input, "\n")] = 0; // Remove newline character
                    FILE *codefile = fopen(input, "r");
                    if (codefile == NULL) {
                        printf("File not found.\n");
                        fclose(codefile);
                    continue;
                } else {
                    fclose(codefile);
                    printf("Loading AM8 Image File: %s ...\n", argv[1]);
                    #ifdef _WIN32
                    Sleep(750);
                    #else
                    sleep(750);
                    #endif
                    AM8(input);
                    for (int i = 0; i < 256; i++) {
                        ram[i] = 0;
                    }
                }
            } else {
                system(input);
            }
        }
    } else if (argc == 2) {
        FILE *codefile = fopen(argv[1], "r");
        if (codefile == NULL) {
            printf("File not found.\n");
            fclose(codefile);
            return 1;
        } else {
            fclose(codefile);
            printf("Loading AM8 Image File: %s ...\n", argv[1]);
            #ifdef _WIN32
                Sleep(750);
            #else
                sleep(750);
            #endif
            AM8(argv[1]);
        }
    } else {
        printf("Too many arguments.\n");
        return 1;
    }
    return 0;
}