#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//Initialize all the variables section:

int pc = 0;
int next_pc = 0;
int branch_target = 0;
int alu_zero = 0;
int rf[32] = {0};          // Register file
int d_mem[32] = {0};       // Data memory (32 entries, each 4-byte)
int total_clock_cycles = 0;

// Initialize Control signals
int RegWrite = 0;
int MemRead = 0;
int MemWrite = 0;
int Branch = 0;
int ALUOp1 = 0;
int ALUOp0 = 0;
int ALUSrc = 0;
// Instruction fields (decoded in Decode())
int rd = 0;
int rs1 = 0;
int rs2 = 0;
int immediate = 0;
int alu_ctrl = 0;
int alu_result = 0;
int mem_data = 0;
int binaryToDecimal(char *binary) {
    int decimal = 0;
    for (int i = 0; binary[i] != '\0'; i++) {
        decimal = decimal * 2 + (binary[i] - '0');
    }
    return decimal;
}
int signExtend12bit(int value) {
    // Check if sign bit (bit 11) is 1
    if (value & 0x800) {
        value |= 0xFFFFF000;  // Sign extend to 32 bits
    }
    return value;
}
int signExtend20bit(int value) {
    // Check if sign bit (bit 19) is 1
    if (value & 0x80000) {
        value |= 0xFFF00000;  // Sign extend to 32 bits
    }
    return value;
}
char* getFormatType(int opcode) {
    switch (opcode) {
        case 51:    // 0110011
            return "R";
        case 19:    // 0010011
            return "I";
        case 3:     // 0000011
            return "I";
        case 35:    // 0100011
            return "S";
        case 99:    // 1100011
            return "SB";
        case 55:    // 0110111
            return "U";
        case 111:   // 1101111
            return "UJ";
        default:
            return "Unknown";
    }
}
void ControlUnit(int opcode) {
    RegWrite = 0;
    MemRead = 0;
    MemWrite = 0;
    Branch = 0;
    ALUOp1 = 0;
    ALUOp0 = 0;
    ALUSrc = 0;
    switch (opcode) {
        case 51:    // R-type (add, sub, and, or)
            RegWrite = 1;
            ALUOp1 = 1;
            ALUOp0 = 0;
            ALUSrc = 0;
            break;
        case 19:    // I-type (addi, andi, ori)
            RegWrite = 1;
            ALUOp1 = 0;
            ALUOp0 = 0;
            ALUSrc = 1;
            break;
        case 3:     // Load (lw)
            RegWrite = 1;
            MemRead = 1;
            ALUSrc = 1;
            break;
        case 35:    // Store (sw)
            MemWrite = 1;
            ALUSrc = 1;
            break;
        case 99:    // Branch (beq)
            Branch = 1;
            ALUOp1 = 0;
            ALUOp0 = 1;
            ALUSrc = 0;
            break;
        default:
            break;
    }
}
void ALUControl(int opcode, int funct3, int funct7) {
    alu_ctrl = 0;
    if (opcode == 51) {  // R-type
        if (funct3 == 0) {
            if (funct7 == 0) {
                alu_ctrl = 0010;  // add
            } else if (funct7 == 32) {
                alu_ctrl = 0110;  // sub
            }
        } else if (funct3 == 7) {
            alu_ctrl = 0000;  // and
        } else if (funct3 == 6) {
            alu_ctrl = 0001;  // or
        }
    } else if (opcode == 19) {  // I-type arithmetic
        if (funct3 == 0) {
            alu_ctrl = 0010;  // addi
        } else if (funct3 == 7) {
            alu_ctrl = 0000;  // andi
        } else if (funct3 == 6) {
            alu_ctrl = 0001;  // ori
        }
    } else if (opcode == 3) {  // lw
        alu_ctrl = 0010;  // add (for address calculation)
    } else if (opcode == 35) {  // sw
        alu_ctrl = 0010;  // add (for address calculation)
    } else if (opcode == 99) {  // beq
        alu_ctrl = 0110;  // sub (for comparison)
    }
}
int Execute(int operand1, int operand2, int alu_ctrl_val) {
    int result = 0;
    switch (alu_ctrl_val) {
        case 0000:  // AND
            result = operand1 & operand2;
            break;
        case 0001:  // OR
            result = operand1 | operand2;
            break;
        case 0010:  // ADD
            result = operand1 + operand2;
            break;
        case 0110:  // SUB
            result = operand1 - operand2;
            break;
        default:
            result = 0;
            break;
    }
    alu_zero = (result == 0) ? 1 : 0;
    return result;
}
int Fetch(FILE *fp, char *instruction) {
    int index = pc / 4;
    rewind(fp);
    for (int i = 0; i <= index; i++) {
        if (fscanf(fp, "%s", instruction) != 1) {
            return 0;  // End of file
        }
    }
    next_pc = pc + 4;
    return 1;
}
void Decode(char *instruction, int opcode, int *funct3, int *funct7) {
    int len = strlen(instruction);
    
    if (len != 32) {
        printf("Invalid instruction length\n");
        return;
    }
    // Extract rd (bits 7-11)
    char rd_bits[6];
    strncpy(rd_bits, instruction + 20, 5);
    rd_bits[5] = '\0';
    rd = binaryToDecimal(rd_bits);
    // Extract rs1 (bits 15-19)
    char rs1_bits[6];
    strncpy(rs1_bits, instruction + 12, 5);
    rs1_bits[5] = '\0';
    rs1 = binaryToDecimal(rs1_bits);
    // Extract rs2 (bits 20-24)
    char rs2_bits[6];
    strncpy(rs2_bits, instruction + 7, 5);
    rs2_bits[5] = '\0';
    rs2 = binaryToDecimal(rs2_bits);
    // Extract funct3 (bits 12-14)
    char funct3_bits[4];
    strncpy(funct3_bits, instruction + 17, 3);
    funct3_bits[3] = '\0';
    *funct3 = binaryToDecimal(funct3_bits);
    // Extract funct7 (bits 25-31)
    char funct7_bits[8];
    strncpy(funct7_bits, instruction, 7);
    funct7_bits[7] = '\0';
    *funct7 = binaryToDecimal(funct7_bits);
    // Generate control signals
    ControlUnit(opcode);
    // Generate ALU control signals
    ALUControl(opcode, *funct3, *funct7);
    // Decode immediate based on instruction type
    char *format = getFormatType(opcode);
    if (strcmp(format, "I") == 0) {  // I-type
        char imm_bits[13];
        strncpy(imm_bits, instruction, 12);
        imm_bits[12] = '\0';
        immediate = binaryToDecimal(imm_bits);
        immediate = signExtend12bit(immediate);
    } else if (strcmp(format, "S") == 0) {  // S-type (sw)
        char imm_high[8], imm_low[6];
        strncpy(imm_high, instruction, 7);
        imm_high[7] = '\0';
        strncpy(imm_low, instruction + 20, 5);
        imm_low[5] = '\0';
        char imm_full[13];
        strcpy(imm_full, imm_high);
        strcat(imm_full, imm_low);
        imm_full[12] = '\0';
        immediate = binaryToDecimal(imm_full);
        immediate = signExtend12bit(immediate);
    } else if (strcmp(format, "SB") == 0) {  // SB-type (beq)
        char imm12_bit[2], imm10_1_bits[11], imm11_bit[2], imm19_12_bits[9];
        strncpy(imm12_bit, instruction, 1);
        imm12_bit[1] = '\0';
        strncpy(imm10_1_bits, instruction + 1, 10);
        imm10_1_bits[10] = '\0';
        strncpy(imm11_bit, instruction + 11, 1);
        imm11_bit[1] = '\0';
        strncpy(imm19_12_bits, instruction + 12, 8);
        imm19_12_bits[8] = '\0';
        char imm_full[13];
        strcpy(imm_full, imm12_bit);
        strcat(imm_full, imm19_12_bits);
        strcat(imm_full, imm11_bit);
        strcat(imm_full, imm10_1_bits);
        imm_full[12] = '\0';
        immediate = binaryToDecimal(imm_full);
        immediate = signExtend12bit(immediate);
    }
}
void Mem(int addr, int write_data, int is_write) {
    if (addr < 0 || addr >= 32) {
        printf("Memory access out of bounds\n");
        return;
    }
    if (is_write) {
        d_mem[addr] = write_data;
    } else {
        mem_data = d_mem[addr];
    }
}
void Writeback(int write_value, int is_load, int dest_reg) {
    if (dest_reg == 0) {
        return;  // Cannot write to x0
    }
    int old_value = rf[dest_reg];
    if (is_load) {
        rf[dest_reg] = mem_data;
    } else {
        rf[dest_reg] = write_value;
    }
    if (rf[dest_reg] != old_value) {
        printf("x%d is modified to 0x%x\n", dest_reg, rf[dest_reg]);
    }
}
int main() {
    // Initialize registers as specified
    rf[1] = 0x20;
    rf[2] = 0x5;
    rf[10] = 0x70;
    rf[11] = 0x4;
    // Initialize data memory as specified
    d_mem[0x70 / 4] = 0x5;    // 0x70 = d_mem[28]
    d_mem[0x74 / 4] = 0x10;   // 0x74 = d_mem[29]
    FILE *fp;
    char filename[256];
    char instruction[33];
    printf("Enter the program file name to run:\n");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = 0;  // Remove newline
    fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Error opening file: %s\n", filename);
        return 1;
    }
    while (Fetch(fp, instruction)) {
        printf("total_clock_cycles %d :\n", total_clock_cycles + 1);
        // Extract opcode from instruction
        char opcode_bits[8];
        strncpy(opcode_bits, instruction + 25, 7);
        opcode_bits[7] = '\0';
        int opcode = binaryToDecimal(opcode_bits);
        // Temporary variables for decode
        int funct3 = 0, funct7 = 0;
        // Decode instruction
        Decode(instruction, opcode, &funct3, &funct7);
        // Execute
        int operand1 = rf[rs1];
        int operand2;
        if (ALUSrc == 1) {
            operand2 = immediate;
        } else {
            operand2 = rf[rs2];
        }
        alu_result = Execute(operand1, operand2, alu_ctrl);
        // Calculate branch target if needed
        if (Branch) {
            branch_target = (immediate << 1) + next_pc;
        }
        // Memory access
        if (MemRead) {
            int mem_addr = alu_result / 4;  // Convert address to array index
            if (mem_addr >= 0 && mem_addr < 32) {
                Mem(mem_addr, 0, 0);  // Read
            }
        } else if (MemWrite) {
            int mem_addr = alu_result / 4;  // Convert address to array index
            if (mem_addr >= 0 && mem_addr < 32) {
                Mem(mem_addr, rf[rs2], 1);  // Write
                printf("memory 0x%x is modified to 0x%x\n", alu_result, rf[rs2]);
            }
        }
        // Writeback
        if (RegWrite) {
            Writeback(alu_result, MemRead, rd);
        }
        // Update PC for next cycle
        if (Branch && alu_zero) {
            pc = branch_target;
        } else {
            pc = next_pc;
        }
        printf("pc is modified to 0x%x\n", pc);
        // Increment clock cycles
        total_clock_cycles++;
    }
    printf("program terminated:\n");
    printf("total execution time is %d cycles\n", total_clock_cycles);
    fclose(fp);
    return 0;
}