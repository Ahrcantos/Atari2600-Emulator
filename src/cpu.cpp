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