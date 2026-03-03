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
        case 3:    //0000011
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
    printf("\nEnter a 32-bit RISC-V instruction in binary: ");
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
    printf("Instruction type: %s\n", formatType);

    //Now that we know the format, we can make cases for each format to determine the fields + their values. I will delete the ones that I've done so we don't double up and it's easier to read.
    //What we are given for the list of instructions that they'll test is as follows:
    //R-type: , , , , , , , , , , 
    //I-Type: lb, lw, sltiu, addi, andi, jalr, lh, ori, slli, slti, srai, srli, ori
    //S-Type: sb, sh, sw
    //SB-Type: beq, blt, bge, bne,
    //U-Type:
    //UJ-Type: ,

    //These aren't on the list but they're on the reference sheet (TA said to do more than just the ones on the assignment):
    //addw, subw, sllw, srlw, sraw.
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

        /* convert registers to integers for printing */
        int rd_num   = binaryToDecimal(rd);
        int rs1_num  = binaryToDecimal(rs1);
        int rs2_num  = binaryToDecimal(rs2);
        int f3_num   = binaryToDecimal(funct3);
        int f7_num   = binaryToDecimal(funct7);

        const char *operation = "unknown";
        if (strcmp(funct3, "000") == 0) {
            if (strcmp(funct7, "0000000") == 0) operation = "add";
            else if (strcmp(funct7, "0100000") == 0) operation = "sub";
        } else if (strcmp(funct3, "111") == 0 && strcmp(funct7, "0000000") == 0) {
            operation = "and";
        } else if (strcmp(funct3, "110") == 0 && strcmp(funct7, "0000000") == 0) {
            operation = "or";
        } else if (strcmp(funct3, "001") == 0 && strcmp(funct7, "0000000") == 0) {
            operation = "sll";
        } else if (strcmp(funct3, "010") == 0 && strcmp(funct7, "0000000") == 0) {
            operation = "slt";
        } else if (strcmp(funct3, "101") == 0) {
            if (strcmp(funct7, "0000000") == 0)      operation = "srl";
            else if (strcmp(funct7, "0100000") == 0) operation = "sra";
        } else if (strcmp(funct3, "100") == 0 && strcmp(funct7, "0000000") == 0) {
            operation = "xor";
        } else if (strcmp(funct3, "011") == 0 && strcmp(funct7, "0000000") == 0) {
            operation = "sltu";
        }

        printf("Operation: %s\n", operation);
        printf("Rs1: x%d\n", rs1_num);
        printf("Rs2: x%d\n", rs2_num);
        printf("Rd: x%d\n", rd_num);
        printf("Funct3: %s (%d)\n", funct3, f3_num);
        printf("Funct7: %s (%d)", funct7, f7_num);
    }

    if(formatType[0]=='I'&& formatType[1]=='\0'){
        // I-type: opcode (7 bits), rd (5 bits), funct3 (3 bits), rs1 (5 bits), imm[11:0] (12 bits)
        char rd[6], funct3[4], rs1[6], imm11[13];
        strncpy(rd, instruction + 20, 5);
        rd[5]='\0';
        strncpy(funct3, instruction + 17, 3);
        funct3[3]='\0';
        strncpy(rs1,instruction + 12,5);
        rs1[5]='\0';
        strncpy(imm11, instruction, 12);
        imm11[12]='\0';

        printf("I-Type fields:\n");
        printf("rd: %s\n", rd);
        printf("funct3: %s\n", funct3);
        printf("rs1: %s\n", rs1);
        printf("imm11: %s\n", imm11);

        // too many if else; alternative? or
        //funct 3 has couple identicals. need to finish rest of cases and polish
        if(strcmp(funct3,"000")==0){

            if(strcmp(opcode,"0000011")==0){
            printf("This is an lb instruction");
            // FIX THIS WARNING BECAUSE YOU FORGOT HOW TO WRITE IN C
            return 0;
            }
            else if(strcmp(opcode,"0010011")==0){
                printf("This is an addi instruction");
            }
            else if (strcmp(opcode,"1100111")==0){
                printf("This is a jalr instruction");
            }
        }

        else if(strcmp(funct3, "010")){

            if (strcmp(opcode,"0000011")==0){
                printf("This is a lw instruction!");
            }
            else if (strcmp(opcode,"0010011")==0){
                printf("This is an slti instruction");
            }
        }

        else if (strcmp(funct3,"011")==0){
            printf("This is a sltiu instruction");
        }
        
        else if (strcmp(funct3,"111")==0){
            printf("This is a andi instruction");
        }
        
        
        else if (strcmp(funct3,"001")==0){
            if (strcmp(opcode,"0000011")==0){
            printf("This is a lh instruction");
            }
            else if(strcmp(opcode,"0010011")==0){
                printf("This is a slli instruction");
            }
        }
        else if (strcmp(funct3,"110")==0){
            printf("This is a ori instruction");
        }
        
        // NEED THE FULL IMM11 OR ELSE WILL NOT WORK
        else if (strcmp(funct3,"101")==0){
            if (strcmp(imm11,"0100000")==0){
                printf("This is a srai instruction");
            }
            else if (strcmp(imm11,"0000000")==0){
                printf("This is a srli instruction");
        }}
        
    }

    //S-Type: sb, sh, sw
    if(formatType[0]=='S'&& formatType[1]=='\0'){
        char imm4[6], funct3[3], rs1[6], rs2[6], imm11[8]; 

        strncpy(imm4, instruction + 20, 5);
        imm4[5] = '\0';
        strncpy(funct3, instruction + 17, 3);
        funct3[3] = '\0';
        strncpy(rs1, instruction + 12, 5);
        rs1[5] = '\0';
        strncpy(rs2, instruction + 7, 5);
        rs2[5] = '\0';
        strncpy(imm11, instruction, 7);
        imm11[7] = '\0';

        printf("s-type fields:\n");
        printf("imm[4:0]: %s\n", imm4);
        printf("funct3: %s\n", funct3);
        printf("rs1: %s\n", rs1);
        printf("rs2: %s\n", rs2);
        printf("imm[11:5] %s\n", imm11);

        if(strcmp(funct3,"000")==0){
            printf("This is a sb instruction");
        }
        else if (strcmp(funct3,"001")==0){
            printf("This is a sh instruction");
        }
        else if(strcmp(funct3,"010")==0){
            printf("This is a sw instruction");
        }
    }

    //SB-Type: beq, blt, bge, bne
    if(formatType[0]=='S' && formatType[1]=='B'){
        // SB-type fields to extract
        char imm41[6], funct3[3], rs1[6], rs2[6], imm12[8]; 

        strncpy(imm41, instruction + 20, 5);
        imm41[5] = '\0';
        strncpy(funct3, instruction + 17, 3);
        funct3[3] = '\0';
        strncpy(rs1, instruction + 12, 5);
        rs1[5] = '\0';
        strncpy(rs2, instruction + 7, 5);
        rs2[5] = '\0';
        strncpy(imm12, instruction, 7);
        imm12[7] = '\0';

        if(strcmp(funct3,"000")==0){
            printf("Operation: beq\n");
        }
        else if(strcmp(funct3,"100")==0){
            printf("Operation: blt\n");
        }
        else if (strcmp(funct3,"001")==0){
            printf("Operation: bne\n");
        }
        else if(strcmp(funct3,"101")==0){
            printf("Operation: bge\n");
        }
    

        // printf("sb-type fields:\n");
        printf("imm[4:1|11]: %s\n", imm41);
        printf("funct3: %s\n", funct3);
        printf("rs1: %s\n", rs1);
        printf("rs2: %s\n", rs2);
        printf("imm[12|10:5] %s\n", imm12);

    }

    // U-Type
    if(formatType[0]=='U' && formatType[1]=='\0'){
        // U-type: opcode (7 bits), rd (5 bits), imm[31:12] (20 bits)
        char rd[6], imm20[21];
        strncpy(rd, instruction + 20, 5);
        rd[5] = '\0';
        strncpy(imm20, instruction, 20);
        imm20[20] = '\0';
        int rd_num = binaryToDecimal(rd);
        int imm_val = binaryToDecimal(imm20);
        if(strcmp(opcode,"0110111")==0){
            printf("Operation: lui\n");
        }
        else if(strcmp(opcode,"0010111")==0){
            printf("Operation: auipc\n");
        }
        printf("Rd: x%d\n", rd_num);
        printf("Immediate: %d (0x%X)\n", imm_val, imm_val);
    }

    if(formatType[0]=='U' && formatType[1]=='J'){
        // UJ-type: opcode (7 bits), rd (5 bits), imm[20] (1 bit), imm[10:1] (10 bits), imm[11] (1 bit), imm[19:12] (8 bits)
        char rd[6];
        rd[5] = '\0';
        strncpy(rd, instruction + 20, 5);
        // Extract immediate parts
        //imm[20] is the sign bit, imm[10:1] is 30-21, imm[11] is 20, imm[19:12] is 19-12.
        char imm20[2], imm10_1[11], imm11[2], imm19_12[9];
        strncpy(imm20, instruction, 1);
        imm20[1] = '\0';
        strncpy(imm10_1, instruction + 1, 10);
        imm10_1[10] = '\0';
        strncpy(imm11, instruction + 11, 1);
        imm11[1] = '\0';
        strncpy(imm19_12, instruction + 12, 8);
        imm19_12[8] = '\0';
        // Reconstruct immediate in correct order
        char fullImm[22]; //It took me a bit to realize I needed 22 not 21 because I needed to shift left by one later for the offset.
        strcpy(fullImm, imm20);
        strcat(fullImm, imm19_12);
        strcat(fullImm, imm11);
        strcat(fullImm, imm10_1);
        fullImm[20] = '\0';
        int rd_num = binaryToDecimal(rd);
        int imm_val = binaryToDecimal(fullImm);
        imm_val = imm_val << 1; // Shift left by 1 to get byte offset
        printf("Operation: jal\n");
        printf("Rd: x%d\n", rd_num);
        printf("Immediate: %d (0x%X)\n", imm_val, imm_val);
    }

    main();
    return 0;
}