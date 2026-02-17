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

    //Now that we know the format, we can make cases for each format to determine the fields + their values.
    //What we are given for the list of instructions that they'll test is as follows:
    //R-type: add, and, or, sll, slt, sra, srl, sub, xor, sltu, 
    //I-Type: lb, lw, sltiu, addi, andi, jalr, lh, ori, slli, slti, srai, srli, ori
    //S-Type: sb, sh, sw
    //SB-Type: beq, blt, bge, bne,
    //U-Type:
    //UJ-Type: jal

    if(formatType[0] == 'R') {
        // R-type: opcode (7 bits), rd (5 bits), funct3 (3 bits), rs1 (5 bits), rs2 (5 bits), funct7 (7 bits)
        char rd[6], funct3[4], rs1[6], rs2[6], funct7[8];
        strncpy(rd, instruction + 20, 5);
        rd[5] = '\0';
        strncpy(funct3, instruction + 17, 3);
        funct3[3] = '\0';
        strncpy(rs1, instruction + 12, 5);
        rs1[5] = '\0';
        strncpy(rs2, instruction + 7, 5);
        rs2[5] = '\0';
        strncpy(funct7, instruction, 7);
        funct7[7] = '\0';
        
        printf("R-type fields:\n");
        printf("rd: %s\n", rd);
        printf("funct3: %s\n", funct3);
        printf("rs1: %s\n", rs1);
        printf("rs2: %s\n", rs2);


        if(funct7 == "0000000") {
            printf("This is an ADD instruction.\n");
        } elif (funct7 == "0100000") {
            printf("This is a SUB instruction.\n");
        } 
    }




    main();
    return 0;
}