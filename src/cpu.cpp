#include "../include/cpu.hpp"

CPU::CPU()
{
    //Stack Pointer is initialized to 0xFF
    S = 0xFF;
}

//Action performed each tick of the CPU
void CPU::tick()
{
    if(remainingCycles == 0)
    {
        opcode(read(PC));
    }
    remainingCycles--;
}

//Reads the address of the entire CPU RAM from the correct array
uint8_t CPU::read(uint16_t adr)
{
    switch(adr)
    {
        //Internal memory from 0 to 0x7FF the rest is mirrors 
        case 0 ... 0x1FFF:
            return CPU_RAM[adr % 0x800];
        //PPU Adress registers from 0x2000 to 0x2007 and their mirrors from 0x2008 to 0x3FFFF
        case 0x2000 ... 0x3FFF:
            return PPU_REG[(adr - 0x2000) % 8];
        //APU and I/O Registers from 0x4000 to 0x4017
        case 0x4000 ... 0x4017:
            return APU_IO_REG[adr - 0x4000];
        //All other RAM locations are mapped to 0xFF
        default:
            return 0xFF;
    }
}

//Writes the value to the correct array given by the address
void CPU::write(uint16_t adr, uint8_t val)
{
    switch(adr)
    {
        //Write to internal memory from 0 to 0x800 and the mirrors
        case 0 ... 0x1FFF:
            CPU_RAM[adr % 0x800] = val;
            break;
        //Write to the PPU Registers from 0x2000 to 0x2007 and their mirrors
        case 0x2000 ... 0x3FFF:
            PPU_REG[(adr - 0x2000) % 8] = val;
            break;
        //Write to APU and I/O Registers from 0x4000 to 0x4017
        case 0x4000 ... 0x4017:
            APU_IO_REG[adr - 0x4000] = val;
            break;
        //Ignore the instruction if the address is not mapped
        default:
            break;
    }
}

//================Flag-Operations=================================

//Sets the Negative Flag if the value is smaller than or equal to 0x7F
void CPU::set_NEG(uint8_t val)
{
    if(val <= 0x7F)
        P &= 0b01111111;
    else
        P |= 0b10000000;
}

//Sets the Zero Flag if the value is zero
void CPU::set_ZERO(uint8_t val)
{
    if(val == 0x00)
        P |= 0b00000010;
    else
        P &= 0b11111101;
}

//Sets the Carry Flag
void CPU::set_CARRY(bool carry)
{
    if(carry)
        P |= 0b00000001;
    else
        P &= 0b11111110;
}

//Sets Overflow Flag
void CPU::set_OVERFLOW(bool overflow)
{
    if(overflow)
        P |= 0b01000000;
    else
        P &= 0b10111111;
}


bool CPU::get_NEG()
{
    uint8_t tmp = P;
    tmp &= 0b10000000;
    tmp >>= 7;
    return (bool) tmp;
}

bool CPU::get_ZERO()
{
    int8_t tmp = P;
    tmp &= 0b00000010;
    tmp >>= 1;
    return (bool) tmp;
}

bool CPU::get_CARRY()
{
    int8_t tmp = P;
    tmp &= 0b00000001;
    return (bool) tmp;
}

bool CPU::get_OVERFLOW()
{
    int8_t tmp = P;
    tmp &= 0b01000000;
    tmp >>= 6;
    return (bool) tmp;
}


//================Instructions========================

void CPU::ld(uint8_t* reg, uint8_t adr_mode)
{
    uint8_t value = 0;
    uint16_t adr = 0;
    bool boundryCrossed = false;

    getAdrFromMode(adr_mode, adr, &boundryCrossed);

    //Get the value from the given memory address
    switch(adr_mode)
    {
        case Immediate: 
            remainingCycles += 2;
            PC += 2;
            break;
        case ZeroPage:
            remainingCycles += 3;
            PC += 2;
            break;
        case ZeroPageX:
            remainingCycles += 4;
            PC += 2;
            break;
        case Absolute:
            remainingCycles += 4;
            PC += 3;
            break;
        case AbsoluteX:
            remainingCycles += boundryCrossed ? 5 : 4;
            PC += 3;
            break;
        case AbsoluteY:
            remainingCycles += boundryCrossed ? 5 : 4;
            PC += 3;
            break;
        case IndirectX:
            remainingCycles += 6;
            PC += 2;
            break;
        case IndirectY:
            remainingCycles += boundryCrossed ? 6 : 5;
            PC += 2;
            break;
        default:
            break;
    }

    //Add Register depending on address mode
    value = read(adr);
    set_NEG(value);
    set_ZERO(value);
    reg[0] = value;
}

void CPU::st(uint8_t &reg, uint8_t adr_mode)
{
    uint16_t adr = 0;
    bool boundryCrossed = false;
    getAdrFromMode(adr_mode, adr, &boundryCrossed);
    write(adr, reg);

    switch(adr_mode)
    {
        case ZeroPage:
            remainingCycles += 3;
            PC += 2;
            break;
        case ZeroPageX:
            remainingCycles += 5;
            PC += 2;
            break;
        case ZeroPageY:
            remainingCycles += 4;
            PC += 2;
            break;
        case Absolute:
            remainingCycles += 4;
            PC += 3;
            break;
        case AbsoluteX:
            remainingCycles += boundryCrossed ? 5 : 4;
            PC += 3;
            break;
        case AbsoluteY:
            remainingCycles += boundryCrossed ? 5 : 4;
            PC += 3;
            break;
        case IndirectX:
            remainingCycles += 6;
            PC += 2;
            break;
        case IndirectY:
            remainingCycles += boundryCrossed ? 6 : 5;
            PC += 2;
            break;
    }
}

void CPU::t(uint8_t* from, uint8_t* to)
{
    uint8_t value = *from;
    set_NEG(value);
    set_ZERO(value);
    *to = value;
    PC += 1;
    remainingCycles += 2;
}

//NOT FINISHED
void CPU::adc(uint8_t adr_mode)
{
    /*uint16_t adr = 0;
    bool boundryCrossed = false;
    getAdrFromMode(adr_mode, &adr, &boundryCrossed);
    uint8_t value = read(adr);*/
    
}

void CPU::dec(uint8_t adr_mode)
{
    uint16_t adr = 0;
    bool boundryCrossed = false;
    getAdrFromMode(adr_mode, adr, &boundryCrossed);
    uint8_t value = read(adr);
    value--;
    set_NEG(value);
    set_ZERO(value);
    write(adr, value);

    switch(adr_mode)
    {
        case ZeroPage:
            remainingCycles += 5;
            PC += 2;
            break;
        case ZeroPageX:
            remainingCycles += 6;
            PC += 2;
            break;
        case Absolute:
            remainingCycles += 6;
            PC += 3;
            break;
        case AbsoluteX:
            remainingCycles += 7;
            PC += 3;
            break;
    }
}

void CPU::de(uint8_t &reg)
{
    reg = reg - 1;
    set_NEG(reg);
    set_ZERO(reg);
    remainingCycles += 2;
    PC += 1;
}

void CPU::inc(uint8_t adr_mode)
{
    uint16_t adr = 0;
    bool boundryCrossed = false;
    getAdrFromMode(adr_mode, adr, &boundryCrossed);
    uint8_t value = read(adr);
    value++;
    set_NEG(value);
    set_ZERO(value);
    write(adr, value);

    switch(adr_mode)
    {
        case ZeroPage:
            remainingCycles += 5;
            PC += 2;
            break;
        case ZeroPageX:
            remainingCycles += 6;
            PC += 2;
            break;
        case Absolute:
            remainingCycles += 6;
            PC += 3;
            break;
        case AbsoluteX:
            remainingCycles += 7;
            PC += 3;
            break;
    }
}

void CPU::in(u_int8_t &reg)
{
    reg = reg + 1;
    set_NEG(reg);
    set_ZERO(reg);
    remainingCycles += 2;
    PC += 1;
}

void CPU::jmp(uint8_t adr_mode)
{
    //Get Address
    uint16_t adr = 0;
    bool boundryCrossed = false;
    getAdrFromMode(adr_mode, adr, &boundryCrossed);

    switch(adr_mode)
    {
        case Absolute:
            remainingCycles += 3;
            break;
        case Indirect:
            remainingCycles += 3;
            break;
    }
    //Jump to address
    PC = adr;
}

//=================OP-Codes===========================

void CPU::opcode(uint8_t code)
{
    switch(code)
    {
        //LDA
        case 0xA9: ld(&A, Immediate); break;
        case 0xA5: ld(&A, ZeroPage); break;
        case 0xB5: ld(&A, ZeroPageX); break;
        case 0xAD: ld(&A, Absolute); break;
        case 0xBD: ld(&A, AbsoluteX); break;
        case 0xB9: ld(&A, AbsoluteY); break;
        case 0xA1: ld(&A, IndirectX); break;
        case 0xB1: ld(&A, IndirectY); break;

        //LDX
        case 0xA2: ld(&X, Immediate); break;
        case 0xA6: ld(&X, ZeroPage); break;
        case 0xB6: ld(&X, ZeroPageX); break;
        case 0xAE: ld(&X, Absolute); break;
        case 0xBE: ld(&X, AbsoluteX); break;

        //LDY
        case 0xA0: ld(&Y, Immediate); break;
        case 0xA4: ld(&Y, ZeroPage); break;
        case 0xB4: ld(&Y, ZeroPageX); break;
        case 0xAC: ld(&Y, Absolute); break;
        case 0xBC: ld(&Y, AbsoluteX); break;

        //STA
        case 0x85: st(A, ZeroPage); break;
        case 0x95: st(A, ZeroPageX); break;
        case 0x8D: st(A, Absolute); break;
        case 0x9D: st(A, AbsoluteX); break;
        case 0x99: st(A, AbsoluteY); break;
        case 0x81: st(A, IndirectX); break;
        case 0x91: st(A, IndirectY); break;

        //STX
        case 0x86: st(X, ZeroPage); break;
        case 0x96: st(X, ZeroPageY); break;
        case 0x8E: st(X, Absolute); break;
        //STY
        case 0x84: st(Y, ZeroPage); break;
        case 0x94: st(Y, ZeroPageX); break;
        case 0x8C: st(Y, Absolute); break;

        //Transfer
        case 0xAA: t(&A, &X); break; //TAX
        case 0xA8: t(&A, &Y); break; //TAY
        case 0xBA: t(&S, &X); break; //TSX
        case 0x8A: t(&X, &A); break; //TXA
        case 0x9A: t(&X, &S); break; //TXS
        case 0x98: t(&Y, &A); break; //TYA

        //Add with Carry

        //Decrement
        case 0xC6: dec(ZeroPage); break;
        case 0xD6: dec(ZeroPageX); break;
        case 0xCE: dec(Absolute); break;
        case 0xDE: dec(AbsoluteX); break;
        case 0xCA: de(X); break;
        case 0x88: de(Y); break;

        //Increment
        case 0xE6: inc(ZeroPage); break;
        case 0xF6: inc(ZeroPageX); break;
        case 0xEE: inc(Absolute); break;
        case 0xFE: inc(AbsoluteX); break;
        case 0xE8: in(X); break;
        case 0xC8: in(Y); break;

        //Jump
        case 0x4C: jmp(Absolute); break;
        case 0x6C: jmp(Indirect); break;


    }
}

//===================Helper-Functions========================
void CPU::getAdrFromMode(uint8_t adr_mode, uint16_t &adr, bool* boundryCrossed)
{
    switch(adr_mode)
    {
        case Immediate: 
            adr = PC + 1;
            break;
        case ZeroPage:
            adr = read(PC + 1);
            break;
        case ZeroPageX:
            adr = read(PC + 1) + X;
            break;
        case ZeroPageY:
            adr = read(PC + 1) + Y;
            break;
        case Absolute:
            adr = read(PC + 1);
            adr <<= 8;
            adr |= read(PC + 2);
            break;
        case AbsoluteX:
            adr = read(PC + 1);
            *boundryCrossed = adr + X > 0xFF; //Check if a page boundary would be crossed
            adr <<= 8;
            adr |= read(PC + 2);
            adr += X;
            break;
        case AbsoluteY:
            adr = read(PC + 1);
            *boundryCrossed = adr + Y > 0xFF; //Check if a page boundary would be crossed
            adr <<= 8;
            adr |= read(PC + 2);
            adr += Y;
            break;
        case Indirect:
            {
            uint16_t i_adr = read(PC + 1);
            i_adr <<= 8;
            i_adr |= read(PC + 2);
            adr = read(i_adr);
            adr <<= 8;
            adr |= read(i_adr + 1);
            }
            break;

        case IndirectX:
            adr = read(PC + 1);
            *boundryCrossed = adr + X > 0xFF; //Check if a page boundary would be crossed
            adr <<= 8;
            adr += X;
            break;
        case IndirectY:
            adr = read(PC + 1);
            *boundryCrossed = adr + Y > 0xFF; //Check if a page boundary would be crossed
            adr <<= 8;
            adr += Y;
            break;
        default:
            break;
    }
}