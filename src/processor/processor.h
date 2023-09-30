#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef NESEMU_PROCESSOR_H
#define NESEMU_PROCESSOR_H


#define ADDR_MODE_IMMEDIATE 0
#define ADDR_MODE_ZERO_PAGE 1
#define ADDR_MODE_ZERO_PAGE_X 2
#define ADDR_MODE_ZERO_PAGE_Y 3
#define ADDR_MODE_ABSOLUTE 4
#define ADDR_MODE_ABSOLUTE_X 5
#define ADDR_MODE_ABSOLUTE_Y 6
#define ADDR_MODE_INDIRECT 7
#define ADDR_MODE_INDIRECT_X 8
#define ADDR_MODE_INDIRECT_Y 9
#define ADDR_MODE_RELATIVE 10

typedef struct processor_t {
  uint8_t stack_pointer;
  uint16_t program_counter;
  uint8_t accumulator;
  uint8_t index_x;
  uint8_t index_y;
  uint8_t status;
  uint8_t remaining_cycles;
  uint8_t memory_cpu[0x2000];
} processor_t;

processor_t *processor_init();
void processor_free(processor_t *processor);

void processor_tick(processor_t *processor);
uint8_t processor_memory_read(processor_t *processor, uint16_t addr);
void processor_memory_write(processor_t *processor, uint16_t addr, uint8_t value);

// Flags
void processor_set_flag_neg(processor_t *processor, uint8_t value);
bool processor_get_flag_neg(processor_t *processor);

void processor_set_flag_zero(processor_t *processor, uint8_t value);
bool processor_get_flag_carry(processor_t *processor);

void processor_set_flag_carry(processor_t *processor, bool value);
bool processor_get_flag_carry(processor_t *processor);

void processor_set_flag_overflow(processor_t *processor, bool value);
bool processor_get_flag_overflow(processor_t *processor);

void processor_address_from_mode(processor_t *processor, uint8_t addr_mode, uint16_t *addr_out, bool *boundary_crossed_out);

#endif
