/**
 * @file  memory_request.c
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "memory_request.h"

static void map_address(MemoryRequest_t *memory_request, uint64_t address) {
  memory_request->byte_select = address & ((1 << 2) - 1);
  memory_request->column_low = (address >> 2) & ((1 << 4) - 1);
  memory_request->channel = (address >> 6) & 1;
  memory_request->bank_group = (address >> 7) & ((1 << 3) - 1);
  memory_request->bank = (address >> 10) & ((1 << 2) - 1);
  memory_request->column_high = (address >> 12) & ((1 << 6) - 1);
  memory_request->row = (address >> 18) & ((1 << 16) - 1);
}

void memory_request_init(MemoryRequest_t *memory_request, uint64_t time, uint8_t core, uint8_t operation, uint64_t address) {
  memory_request->time = time;
  memory_request->core = core;
  memory_request->operation = operation;
  map_address(memory_request, address);
  memory_request->state = PENDING;
}

char *memory_request_to_string(MemoryRequest_t *memory_request) {
  char *response = malloc(sizeof(char) * 100);

  sprintf(response, "%llu %u %u [%X %X %X %X %X %X %X]", memory_request->time, memory_request->core, memory_request->operation,
          memory_request->byte_select, memory_request->column_low, memory_request->channel, memory_request->bank_group, memory_request->bank,
          memory_request->column_high, memory_request->row);

  return response;
}
