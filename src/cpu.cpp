#include "../include/cpu.hpp"

CPU::CPU()
{

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

    //Get the value from the given memory address
    switch(adr_mode)
    {
        case Immediate: 
            adr = PC + 1;
            remainingCycles += 2;
            PC += 2;
            break;
        case ZeroPage:
            adr = read(PC + 1);
            remainingCycles += 3;
            PC += 2;
            break;
        case ZeroPageX:
            adr = read(PC + 1) + X;
            remainingCycles += 4;
            PC += 2;
            break;
        case ZeroPageY:
            adr = read(PC + 1) + Y;
            remainingCycles += 4;
            PC += 2;
        case Absolute:
            adr = read(PC + 1);
            adr <<= 8;
            adr |= read(PC + 2);
            remainingCycles += 4;
            PC += 3;
            break;
        case AbsoluteX:
            adr = read(PC + 1);
            boundryCrossed = adr + X > 0xFF; //Check if a page boundary would be crossed
            adr <<= 8;
            adr |= read(PC + 2);
            adr += X;
            remainingCycles += boundryCrossed ? 5 : 4;
            PC += 3;
            break;
        case AbsoluteY:
            adr = read(PC + 1);
            boundryCrossed = adr + Y > 0xFF; //Check if a page boundary would be crossed
            adr <<= 8;
            adr |= read(PC + 2);
            adr += Y;
            remainingCycles += boundryCrossed ? 5 : 4;
            PC += 3;
            break;
        case IndirectX:
            adr = read(PC + 1);
            boundryCrossed = adr + X > 0xFF; //Check if a page boundary would be crossed
            adr <<= 8;
            remainingCycles += 6;
            adr += X;
            PC += 2;
            break;
        case IndirectY:
            adr = read(PC + 1);
            boundryCrossed = adr + Y > 0xFF; //Check if a page boundary would be crossed
            adr <<= 8;
            adr += Y;
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
    }
}