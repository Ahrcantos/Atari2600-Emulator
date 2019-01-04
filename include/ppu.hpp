#include <stdint.h>


//The PPU(Picture PRocessing Unit) of the NES
class PPU
{
    //Ram of the PPU (16kB)
    uint8_t PPU_RAM[0x4000]{0};

    //Object Attribute Memory of the PPU which determines how sprites are rendered
    uint8_t PPU_OAM[256]{0};

    PPU();
    ~PPU();

    //Called every 3 CPU ticks
    void tick();

    //Draw the state of the PPU to the screen
    void draw();
};