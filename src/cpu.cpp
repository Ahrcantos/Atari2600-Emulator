#include "../include/cpu.hpp"

CPU::CPU()
{

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
