#include <stdint.h>

#define Immediate 0
#define ZeroPage 1
#define ZeroPageX 2
#define ZeroPageY 3
#define Absolute 4
#define AbsoluteX 5
#define AbsoluteY 6
#define IndirectX 7
#define IndirectY 8
#define Relative 9



//The CPU of the NES with 6 registers and 0x800 Byte of internal RAM
class CPU
{

public:
    //CPU Registers
    uint8_t A, X, Y;
    //Programm Counter
    uint16_t PC;
    //Stack Pointer
    uint8_t S;
    //Status Register
    uint8_t P;

    //Internal Ram of the NES
    uint8_t CPU_RAM[0x0800]{0};

    //=========PPU Registers(8Byte from 0x2000 to 0x2007) used for adressing the PPU==================
    //PPUCTRL       0x2000
    //PPUMASK	    0x2001	BGRs bMmG	color emphasis (BGR), sprite enable (s), background enable (b), sprite left column enable (M), background left column enable (m), greyscale (G)
    //PPUSTATUS	    0x2002	VSO- ----	vblank (V), sprite 0 hit (S), sprite overflow (O); read resets write pair for $2005/$2006
    //OAMADDR	    0x2003	aaaa aaaa	OAM read/write address
    //OAMDATA	    0x2004	dddd dddd	OAM data read/write
    //PPUSCROLL	    0x2005	xxxx xxxx	fine scroll position (two writes: X scroll, Y scroll)
    //PPUADDR	    0x2006	aaaa aaaa	PPU read/write address (two writes: most significant byte, least significant byte)
    //PPUDATA	    0x2007
    uint8_t PPU_REG[8];

    //NES APU and I/O Registers
    uint8_t APU_IO_REG[0x18];

    CPU();
    ~CPU();

    /*  Is called each simulated CPU tick to perform the actions the
        CPU is supposed to be doing */
    void tick();

    //Executes the correct instruction for the specific opcode
    void opcode(uint8_t code);

    //==============Read/Write====================

    /*  Reads a byte from the given address in the entire Ram
        of the CPU */
    uint8_t read(uint16_t adr);

    /*  Writes a byte to the given address in the entire Ram
        of the CPU */
    void write(uint16_t adr, uint8_t val);

    //==========Flag-Operations======================
    void set_NEG(uint8_t val);
    void set_ZERO(uint8_t val);
    void set_CARRY(bool carry);
    void set_INTERRUPT(bool interrupt);
    void set_DECIMAL(bool decimal);
    void set_OVERFLOW(bool overflow);

    bool get_NEG();
    bool get_ZERO();
    bool get_CARRY();
    bool get_INTERRUPT();
    bool get_DECIMAL();
    bool get_OVERFLOW();
private:

    /*Used to check if the CPU can perform an action or if another action
      is performed at the moment*/
    uint8_t remainingCycles = 0;

    //Load
    void ld(uint8_t* reg, uint8_t adr_mode);

    //Store
    void sta();
    void stx();
    void sty();

    //Transfer
    void tax();
    void tay();
    void tsx();
    void txa();
    void txs();
    void tya();

    //Add with Carry
    void adc();

    //Decrement
    void dec();
    void dex();
    void dey();

    //Increment
    void inc();
    void inx();
    void iny();

    //Subtraction with borrow
    void sbc();

};


