#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int pc = 0;
int next_pc = 0;
int branch_target = 0;
int rf[32] = {0}; //This is the register file

int binaryToDecimal(char *binary) {
    int decimal = 0;
    for (int i = 0; binary[i] != '\0'; i++) {
        decimal = decimal * 2 + (binary[i] - '0');
    }
    return decimal;
}

char* getFormatType(int opcode) {
    switch (opcode) {
        case 51:
            return "R";
        case 19:
            return "I";
        case 3:
            return "I";
        case 35:
            return "S";
        case 99:
            return "SB";
        case 55:
            return "U";
        case 111:
            return "UJ";
        default:
            return "Unknown";
    }
}

int Fetch(FILE *fp, char *instruction) {
    int index = pc / 4;
    rewind(fp);
    for (int i = 0; i <= index; i++) {
        if (fscanf(fp, "%s", instruction) != 1) {
            return 0;
        }
    }
    next_pc = pc + 4;
    pc = next_pc;
    return 1;
}

void Decode(char *instruction) {
    int len = strlen(instruction);
    if (len != 32) {
        printf("Invalid code\n");
        return;
    }
    for (int i = 0; i < len; i++) {
        if (instruction[i] != '0' && instruction[i] != '1') {
            printf("Invalid code\n");
            return;
        }
    }

    char opcode[8];
    for (int i = len - 7; i < len; i++) {
        opcode[i - (len - 7)] = instruction[i];
    }
    opcode[7] = '\0';

    int opcodeDecimal = binaryToDecimal(opcode);
    char *formatType = getFormatType(opcodeDecimal);

    printf("Instruction type: %s\n", formatType);

    if(formatType[0] == 'R') {
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

        int rd_num = binaryToDecimal(rd);
        int rs1_num = binaryToDecimal(rs1);
        int rs2_num = binaryToDecimal(rs2);
        int f3_num = binaryToDecimal(funct3);
        int f7_num = binaryToDecimal(funct7);

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
        printf("Rs1: x%d (value: %d)\n", rs1_num, rf[rs1_num]);
        printf("Rs2: x%d (value: %d)\n", rs2_num, rf[rs2_num]);
        printf("Rd: x%d\n", rd_num);
        printf("Funct3: %d\n", f3_num);
        printf("Funct7: %d\n", f7_num);
    }

    if(formatType[0]=='I'&& formatType[1]=='\0'){
        char rd[6], funct3[4], rs1[6], imm11[13];
        strncpy(rd, instruction + 20, 5);
        rd[5]='\0';
        strncpy(funct3, instruction + 17, 3);
        funct3[3]='\0';
        strncpy(rs1,instruction + 12,5);
        rs1[5]='\0';
        strncpy(imm11, instruction, 12);
        imm11[12]='\0';

        int rdNum = binaryToDecimal(rd);
        int rs1Num = binaryToDecimal(rs1);
        int immVal= binaryToDecimal(imm11);

        char imm115[8];
        strncpy(imm115,imm11,7);
        imm115[7]='\0';

        if (imm11[0] =='1'){
            immVal-= (1<< 12);
        }

        if(strcmp(funct3,"000")==0){
            if(strcmp(opcode,"0000011")==0){
                printf("Operation: lb\n");
            }
            else if(strcmp(opcode,"0010011")==0){
                printf("Operation: addi\n");
            }
            else if (strcmp(opcode,"1100111")==0){
                printf("Operation: jalr\n");
            }
        }
        else if(strcmp(funct3, "010")==0){
            if (strcmp(opcode,"0000011")==0){
                printf("Operation: lw\n");
            }
            else if (strcmp(opcode,"0010011")==0){
                printf("Operation: slti\n");
            }
        }
        else if (strcmp(funct3,"011")==0){
            printf("Operation: sltiu\n");
        }
        else if (strcmp(funct3,"111")==0){
            printf("Operation: andi\n");
        }
        else if (strcmp(funct3,"001")==0){
            if (strcmp(opcode,"0000011")==0){
                printf("operation: lh\n");
            }
            else if(strcmp(opcode,"0010011")==0){
                printf("Operation: slli\n");
            }
        }
        else if (strcmp(funct3,"110")==0){
            printf("Operation: ori");
        }
        else if (strcmp(funct3,"101")==0){
            if (strcmp(imm115,"0100000")==0){
                printf("Operation: srai");
            }
            else if (strcmp(imm115,"0000000")==0){
                printf("Operation: srli");
        }}
        printf("Rs1: x%d (value: %d)\n",rs1Num, rf[rs1Num]);
        printf("Rd: x%d\n",rdNum);
        printf("Immediate: %d (or 0x%X)\n", immVal, immVal & 0xFFF);
    }

    if(formatType[0]=='S'&& formatType[1]=='\0'){
        char imm4[6], funct3[4], rs1[6], rs2[6], imm11[8]; 
        char imm12[13];

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

        strcpy(imm12, imm11);
        strcat(imm12, imm4);
        imm12[12] = '\0';

        int rs1Num = binaryToDecimal(rs1);
        int rs2Num = binaryToDecimal(rs2);
        int immVal = binaryToDecimal(imm12);

        if (imm12[0] == '1') {
            immVal -= (1 << 12);
        }

        if(strcmp(funct3,"000")==0){
            printf("Operation: sb\n");
        }
        else if (strcmp(funct3,"001")==0){
            printf("Operation: sh\n");
        }
        else if(strcmp(funct3,"010")==0){
            printf("Operation: sw\n");
        }
        printf("Rs1: x%d (value: %d)\n",rs1Num, rf[rs1Num]);
        printf("Rs2: x%d (value: %d)\n",rs2Num, rf[rs2Num]);
        printf("Immediate: %d (or 0x%X)\n",immVal,immVal & 0xFFF);
    }

    if(formatType[0]=='S' && formatType[1]=='B'){
        char imm41[6], funct3[4], rs1[6], rs2[6], imm12[8]; 

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
        char imm[13];
        imm[0] = imm12[0];
        strncpy(imm + 1, imm12 + 1, 6);
        strncpy(imm + 7, imm41, 4);
        imm[11] = imm41[4];
        imm[12] = '\0';
        int immVal = binaryToDecimal(imm);
        if(imm[0] == '1') {
            immVal -= (1 << 12);
        }
        printf("rs1: x%d (value: %d)\n", binaryToDecimal(rs1), rf[binaryToDecimal(rs1)]);
        printf("rs2: x%d (value: %d)\n", binaryToDecimal(rs2), rf[binaryToDecimal(rs2)]);
        printf("Immediate: %d\n", immVal);
    }

    if(formatType[0]=='U' && formatType[1]=='\0'){
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
        char rd[6];
        strncpy(rd, instruction + 20, 5);
        rd[5] = '\0';
        char imm20[2], imm10_1[11], imm11[2], imm19_12[9];
        strncpy(imm20, instruction, 1);
        imm20[1] = '\0';
        strncpy(imm10_1, instruction + 1, 10);
        imm10_1[10] = '\0';
        strncpy(imm11, instruction + 11, 1);
        imm11[1] = '\0';
        strncpy(imm19_12, instruction + 12, 8);
        imm19_12[8] = '\0';
        char fullImm[22];
        strcpy(fullImm, imm20);
        strcat(fullImm, imm19_12);
        strcat(fullImm, imm11);
        strcat(fullImm, imm10_1);
        fullImm[20] = '\0';
        int rd_num = binaryToDecimal(rd);
        int imm_val = binaryToDecimal(fullImm);
        if (imm20[0] == '1') {
            imm_val -= (1 << 20);
        }
        imm_val = imm_val << 1;
        printf("Operation: jal\n");
        printf("Rd: x%d\n", rd_num);
        printf("Immediate: %d (0x%X)\n", imm_val, imm_val);
    }
}

int main() {
    FILE *fp = fopen("program.txt", "r");
    if (fp == NULL) {
        printf("Error opening file\n");
        return 1;
    }

    char instruction[33];

    while (Fetch(fp, instruction)) {
        printf("\n=== Cycle (PC = %d) ===\n", pc - 4);
        printf("Instruction: %s\n", instruction);
        Decode(instruction);
    }

    fclose(fp);
    return 0;
}