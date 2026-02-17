#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//Obviously the first thing to convert RISC-V is to grab the last 7 bits to determine the opcode.

// Convert binary string to decimal
int binaryToDecimal(char *binary) {
    int decimal = 0;
    for (int i = 0; binary[i] != '\0'; i++) {
        decimal = decimal * 2 + (binary[i] - '0');
    }
    return decimal;
}

// Determine format type based on opcode
char* getFormatType(int opcode) {
    switch (opcode) {
        case 51:   // 0110011
            return "R";
        case 19:   // 0010011
            return "I";
        case 35:   // 0100011
            return "S";
        case 99:   // 1100011
            return "SB";
        case 55:   // 0110111
            return "U";
        case 111:  // 1101111
            return "UJ";
        default:
            return "Unknown";
    }
}

int main() {
    char instruction[999];
    printf("Enter a 32-bit RISC-V instruction in binary: ");
    scanf("%s", instruction);
    int len = strlen(instruction);
    if (len != 32) {
        printf("Invalid code\n");
        return 1;
    }
    for (int i = 0; i < len; i++) {
        if (instruction[i] != '0' && instruction[i] != '1') {
            printf("Invalid code\n");
            return 1;
        }
    }
    
    // Get the last 7 characters (digits) from the string
    char opcode[8];  // 7 bits + null terminator
    for (int i = len - 7; i < len; i++) {
        opcode[i - (len - 7)] = instruction[i];
    }
    opcode[7] = '\0';  // Null terminate the string
    
    int opcodeDecimal = binaryToDecimal(opcode);
    char *formatType = getFormatType(opcodeDecimal);
    
    printf("The opcode is: %s (decimal: %d)\n", opcode, opcodeDecimal);
    printf("Format type: %s\n", formatType);
    main();
    return 0;
}