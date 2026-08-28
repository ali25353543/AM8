# AM8 - Alintosh Machine 8-bit

MIT License

Alintosh Machine 8-bit is a simple 8-bit machine emulator.

Honestly, This machine emulator is a hobby, for testing purposes.

I designed it to be similar to "x86" Machines (PCs and Laptops).

Similar to x86 means I used Dynamic Instruction Compeletion (DIC).

That is because ARM32, ARM64 (AArch64), Thumb Architures imposes upon you Constant Number of Instructions.

Max Instruction Size: 2 Bytes.

Physical Memory Size: 256 Bytes.

n(Operands) Operation Between R / M ?

if highest bit of Opcode is set and frist bit and second bit of Opcode are cleared, This mean Next byte is Constant (e.g. 3).

if highest bit of Opcode is set and frist bit of Opcode is set and second bit of Opcode are cleared and Next byte is smaller than 4,
This mean Next byte is Register.

else if Next byte isn't smaller than 4 , This mean "InvalidOpcodeError" (Proccessor exits with exit code 6).

if highest bit of Opcode is set and frist bit of Opcode is cleared and second bit of Opcode is set,

This mean Next byte is Memory Address (e.g. [0xEE]).

if highest bit of Opcode is set and frist bit and second bit of Opcode are set and Next byte is smaller than 4,

This mean Next byte is Memory Address but with Register instead of Constant (e.g. [a]).

if highest bit of Opcode is cleared, This mean Extended Opcode.

Supported Main Opcodes:

mov : 0 (Successful)

add : 1 (Successful, but don't add (a or [a]) to (a or [a]), because It will add b to a instead.)

sub : 2 (Successful, but don't subtract (a or [a]) from (a or [a]), because It will subtract b from a instead.)

mul : 3 (Successful, but don't multiply (a or [a]) by (a or [a]), because a will be 0 instead.)

lod : 4 (Not Successful)
