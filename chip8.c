#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct OperationCode {
    uint8_t instruction;
    uint8_t x;
    uint8_t y;
    uint8_t kk;
    uint16_t nnn;
    uint8_t n;
} OperationCode;

uint16_t fetch(uint16_t PC, uint8_t* memory) {
    uint16_t operand = (memory[PC] << 8 | memory[PC + 1]);
    return operand; 
}

OperationCode decode(uint16_t opcode) {
    OperationCode decoded;
    decoded.instruction = opcode >> 12; 
    decoded.x = (opcode >> 8) & 0x0F;
    decoded.y = (opcode >> 4) & 0x00F;
    decoded.kk = (opcode & 0XFF);
    decoded.nnn = opcode & 0X0FFF;
    decoded.n = opcode & 0x000F;
    return decoded;
}

void execute(uint8_t* v, uint16_t* PC, OperationCode decoded, uint16_t* stack, int8_t* SP, uint16_t* I, uint8_t* memory, bool display[32][64]) {
    if(decoded.instruction == 0x0 && decoded.x == 0 && decoded.kk == 0xEE) {
        *PC = stack[*SP];
        *SP -= 1;
    }
    else if(decoded.instruction == 0x1) {
        *PC = decoded.nnn;
    }
    else if(decoded.instruction == 0x2) {
        *SP += 1;
        stack[*SP] = *PC + 2;
        *PC = decoded.nnn;
    }
    else if(decoded.instruction == 0x3) {
        if(v[decoded.x] == decoded.kk) {
            *PC += 2;
        }
        *PC += 2;
    }
    else if(decoded.instruction == 0x4) {
        if(v[decoded.x] != decoded.kk) {
            *PC += 2;
        }
        *PC += 2;
    }
    else if(decoded.instruction == 0x5) {
        if(v[decoded.x] == v[decoded.y]) {
            *PC += 2;
        }
        *PC += 2;
    }
    else if(decoded.instruction == 0x6) {
        v[decoded.x] = decoded.kk;
        *PC += 2;
    }
    else if(decoded.instruction == 0x7) {
        v[decoded.x] += decoded.kk;
        *PC += 2;
    }
    else if(decoded.instruction == 0x8 && decoded.n == 0x0) {
        v[decoded.x] = v[decoded.y];
        *PC += 2;
    }
    else if(decoded.instruction == 0x8 && decoded.n == 0x1) {
        v[decoded.x] = v[decoded.x] | v[decoded.y]; 
        *PC += 2;
    }
    else if(decoded.instruction == 0x8 && decoded.n == 0x2) {
        v[decoded.x] = v[decoded.x] & v[decoded.y]; 
        *PC += 2;
    }
    else if(decoded.instruction == 0x8 && decoded.n == 0x3) {
        v[decoded.x] = v[decoded.x] ^ v[decoded.y]; 
        *PC += 2;
    }
    else if(decoded.instruction == 0x8 && decoded.n == 0x4) {
        uint16_t sum = v[decoded.x] + v[decoded.y];
        if(sum > 0xFF) 
            v[0x0F] = 1;
        else 
            v[0x0F] = 0;    
        v[decoded.x] = sum & 0xFF;
        *PC += 2;
    }
    else if(decoded.instruction == 0x8 && decoded.n == 0x5) {
        uint16_t diff = v[decoded.x] - v[decoded.y];
        if(v[decoded.x] > v[decoded.y]) 
            v[0x0F] = 1;
        else 
            v[0x0F] = 0;
        v[decoded.x] = diff & 0xFF;
        *PC += 2;
    }
    else if(decoded.instruction == 0x8 && decoded.n == 0x6) {
        if((v[decoded.x] & 0x01) == 0x01) 
            v[0x0F] = 1;
        else 
            v[0x0F] = 0;
        v[decoded.x] >>= 1;
        *PC += 2;
    } 
    else if(decoded.instruction == 0x8 && decoded.n == 0x7) {
        uint16_t diff = v[decoded.y] - v[decoded.x];
        if(v[decoded.y] > v[decoded.x]) 
            v[0x0F] = 1;
        else 
            v[0x0F] = 0;
        v[decoded.x] = diff & 0xFF;
        *PC += 2;
    }
    else if(decoded.instruction == 0x8 && decoded.n == 0xE) {
        if((v[decoded.x] & 0x80) == 0x80) 
            v[0x0F] = 1;
        else 
            v[0x0F] = 0;
        v[decoded.x] <<= 1;
        *PC += 2; 
    }
    else if(decoded.instruction == 0x9 && decoded.n == 0) {
        if(v[decoded.x] != v[decoded.y]) {
            *PC += 2;
        }
        *PC += 2;
    }
    else if(decoded.instruction == 0xA) {
        *I = decoded.nnn;
        *PC += 2;
    }
    else if(decoded.instruction == 0xB) {
        *PC = decoded.nnn + v[0];
    }
    else if(decoded.instruction == 0xD) {
        v[0X0F] = 0;
        // Sprite Length in Y-axis
        for(int i = 0; i < decoded.n; i++) {
            uint8_t sprite = memory[*I + i];
            // Sprite Width in X axis
            for(int j = 0; j < 8; j++) {
                uint8_t spriteBit = (sprite >> (7 - j)) & 0x1;
                // check if collision of display value and sprite bit, then VF = 1
                if(display[decoded.y + i][decoded.x + j] == 0x1 && spriteBit == 0x1)
                    v[0x0F] = 1;
                display[decoded.y + i][decoded.x + j] ^= spriteBit;
            }
        }
        *PC += 2;
    }
}

int main() {
    uint8_t memory[4096] = {0};
    uint8_t registers[16] = {0};
    bool display[32][64] = {0};
    display[10][20] = 1;
    uint16_t I = 0;
    uint16_t PC; 
    uint16_t stack[16];
    int8_t sp = -1;
    PC = 0x200;
    registers[1] = 0x90;
    memory[0x240] = 0x00;
    // memory[0x241] = 0x22;
    for(int i = 0; i < 1; i++) {
        uint16_t opcode = fetch(PC, memory);
        OperationCode decoded = decode(opcode);
        execute(registers, &PC, decoded, stack, &sp, &I, memory, display);
    }
    return 0;
}
