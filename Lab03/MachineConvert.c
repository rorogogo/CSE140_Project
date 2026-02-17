#include <stdio.h>
#include <stdint.h>
//Obviously the first thing to convert RISC-V is to grab the last 7 bits to determine the opcode.


int main() {
    int instruction;
    printf("Enter a 32-bit RISC-V instruction in machine code:");
    scanf("%d", &instruction);
    int dig1 = (instruction / 1 ) % 10;
    int dig2 = (instruction / 10 ) % 10;
    int dig3 = (instruction / 100 ) % 10;
    int dig4 = (instruction / 1000) % 10;
    int dig5 = (instruction / 10000) % 10;
    int dig6 = (instruction / 100000) % 10;
    int dig7 = (instruction / 1000000) % 10;

    int opcode = dig1 + 10*dig2 + 100*dig3 + 1000*dig4 + 10000*dig5 + 100000*dig6 + 1000000*dig7;
    printf("The opcode is: %d\n", opcode);
}