Opcode is an encoded instruction which instructs the hardware or virtual machine to perform a task.

In the opcode of CHIP8, 6XKK,
    6 is the instruction type
    X is the instruction/register number
    KK is the 8-bit value

An opcode for a CHIP8 is of size 2 bytes (8 + 8 bits = 16 bits),
where it reads two memory locations, each containing 1 byte.

The earlier memory location is shifted left by 8 bits and then
combined with the next memory location using Bitwise OR.

More like,

opcode = (memory[PC] << 8) | memory[PC + 1]

For example,

memory[PC]     = 0x6A
memory[PC + 1] = 0x05

memory[PC] << 8
= 0x6A00

Then,

opcode = 0x6A00 | 0x05
       = 0x6A05

Therefore,

opcode = 0x6A05


--------------------------------------------------------------------

CHIP8 like all computers has three main components:

1. INPUT:
    A 16-key keyboard with hexadecimal symbols from 0 to F.
    We need not worry about this now as we will use SDL2 for it.

2. OUTPUT:
    It uses a screen to display the output and also has a sound buzzer.

3. CPU and MEMORY:

    Memory:
    The CHIP8 has a 4KB memory (4096 bytes).

    Since 1 byte = 8 bits,

    4096 bytes = 4096 * 8 bits
               = 32768 bits

    It has 16 V registers (each of size 1 byte),
    1 I register (of size 2 bytes),
    1 PC counter (of size 2 bytes).

    The V registers are:

        V0
        V1
        V2
        ...
        VF

    Therefore, there are 16 V registers in total.

    VF is also used as a flag register by several CHIP8 instructions.

    The I register stores a memory address and is 16 bits (2 bytes).

    The PC (Program Counter) stores the address of the
    current instruction and is also 16 bits (2 bytes).

    Usually, CHIP8 programs start at memory address 0x200.


--------------------------------------------------------------------

BITWISE Operations in C/C++:

1. | (Bitwise OR):

    This performs "OR" operation bit by bit.

    Example:

        10 OR 01 = 11

    Here the numbers are binary numbers.

    In C/C++, we can write:

        uint8_t a = 0b10;
        uint8_t b = 0b01;

        printf("%02X", a | b);

    Output:

        03

    Explanation:

        0b10
        0b01
        ----
        0b11

    NOTE:

        uint8_t a = 10;

    means decimal 10, NOT binary 10.

    Decimal 10 in binary is:

        00001010

    If we want binary 10, we write:

        uint8_t a = 0b10;

    uint8_t is used because it stores an unsigned 8-bit integer.

    Its range is:

        0 to 255


2. & (Bitwise AND):

    This performs Bitwise "AND" Operation.

    Example:

        10 AND 11 = 10

    Here the numbers are binary numbers.

    In C/C++, we can write:

        uint8_t a = 0b10;
        uint8_t b = 0b11;

        printf("%02X", a & b);

    Output:

        02

    Explanation:

        0b10
        0b11
        ----
        0b10

    Therefore:

        10 AND 11 = 10

& (Bitwise XOR):

    This performs Bitwise "XOR" Operation.

    Example:

        10 XOR 11 = 01

    Here the numbers are binary numbers.

    In C/C++, we can write:

        uint8_t a = 0b10;
        uint8_t b = 0b11;

        printf("%02X", a ^ b);

    Output:

        02

    Explanation:

        0b10
        0b11
        ----
        0b01

    Therefore:

        10 AND 11 = 01


--------------------------------------------------------------------

FETCH-DECODE-EXECUTE CYCLE:

The CHIP8 CPU repeatedly performs three main steps:

    1. FETCH
    2. DECODE
    3. EXECUTE


1. FETCH:

    The CPU reads two consecutive bytes from memory.

    uint16_t opcode =
        (memory[PC] << 8) | memory[PC + 1];


2. DECODE:

    The CPU extracts different parts of the opcode.

    For example:

        opcode = 0x6A05

    The first 4 bits represent the instruction type:

        uint8_t instruction = (opcode & 0xF000) >> 12;

    Therefore:

        instruction = 0x6


    The next 4 bits represent the register:

        uint8_t x = (opcode & 0x0F00) >> 8;

    Therefore:

        x = 0xA


    The last 8 bits represent the value:

        uint8_t kk = opcode & 0x00FF;

    Therefore:

        kk = 0x05


    So:

        opcode = 0x6A05

        instruction = 6
        x           = A
        kk          = 05


3. EXECUTE:

    The instruction 6XKK means:

        Vx = KK

    Therefore:

        V[A] = 0x05


    In C/C++:

        V[x] = kk;


So the complete process is:

    memory
       |
       v
    FETCH
       |
       v
    opcode
       |
       v
    DECODE
       |
       v
    instruction + operands
       |
       v
    EXECUTE
       |
       v
    update CPU state
       |
       v
    FETCH next opcode


This is the basic foundation of a CHIP8 emulator.


------------------------------------------------------------------------------

Instruction:

1. if instruction == 6:  // Load or Set Instruction
    v[x] = kk;

2. if instruction == 7: // Add Instruction
    v[x] += kk;

3.