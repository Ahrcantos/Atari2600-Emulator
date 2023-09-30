#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "./processor.h"


processor_t *processor_init() {
  processor_t *processor = malloc(sizeof(processor_t));
  processor->program_counter = 0xFF;
}

void processor_free(processor_t *processor) {
  free(processor);
}


void processor_tick(processor_t *processor) {
  if (processor->remaining_cycles > 0) {
    processor->remaining_cycles--;
    return;
  }

  run_opcode(processor);
}

uint8_t processor_memory_read(processor_t *processor, uint16_t addr) {
  switch(addr) {
    case 0x0000 ... 0x1FFF:
      return processor->memory_cpu[addr];
    default:
      return 0xFF;
  }
}


// Instructions


void run_opcode(processor_t *processor) {
  uint8_t code;

  code = processor_memory_read(processor, processor->program_counter);
}

void run_opcode_ld(uint8_t *reg, uint8_t addr_mode) {

}

// Util

void get_address_from_mode(processor_t *processor, uint8_t addr_mode, uint16_t *addr_out, bool *boundary_crossed_out) {
  *boundary_crossed_out = false;

  switch(addr_mode) {
    case ADDR_MODE_IMMEDIATE:
      *addr_out = processor->program_counter + 1;
      break;
    case ADDR_MODE_ZERO_PAGE:
      *addr_out = processor_memory_read(processor, processor->program_counter + 1);
      break;

    case ADDR_MODE_ZERO_PAGE_X:
      *addr_out = processor_memory_read(processor, processor->program_counter + 1) + processor->index_x;
      break;

    case ADDR_MODE_ZERO_PAGE_Y:
      *addr_out = processor_memory_read(processor, processor->program_counter + 1) + processor->index_y;
      break;

    case ADDR_MODE_ABSOLUTE:
      *addr_out = processor_memory_read(processor, processor->program_counter + 1);
      *addr_out <<= 8;
      *addr_out |= processor_memory_read(processor, processor->program_counter + 2);
      break;

    case ADDR_MODE_ABSOLUTE_X:
      *addr_out = processor_memory_read(processor, processor->program_counter + 1);
      *boundary_crossed_out = *addr_out + processor->index_x > 0xFF;
      *addr_out <<= 8;
      *addr_out |= processor_memory_read(processor, processor->program_counter + 2);
      *addr_out += processor->index_x;
      break;

    case ADDR_MODE_ABSOLUTE_Y:
      *addr_out = processor_memory_read(processor, processor->program_counter + 1);
      *boundary_crossed_out = *addr_out + processor->index_y > 0xFF;
      *addr_out <<= 8;
      *addr_out |= processor_memory_read(processor, processor->program_counter + 2);
      *addr_out += processor->index_y;
      break;

    case ADDR_MODE_INDIRECT:
      break;

    case ADDR_MODE_INDIRECT_X:
      break;

    case ADDR_MODE_INDIRECT_Y:
      break;

    case ADDR_MODE_RELATIVE:
      break;
  }
}
