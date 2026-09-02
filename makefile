all:
	gcc AM8.c Loader.c -o AM8
	gcc Assembler/am8asm.c -o AM8ASM
clean:
	del /s /q *.exe
	del /s /q *.bin