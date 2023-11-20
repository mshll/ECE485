/**
 * @file    main.c
 * @author  ECE485/585 Team 5
 *          members:
 *            Abdulaziz Alateeqi,
 *            Meshal Almutairi,
 *            Gene Hu,
 *            Eduardo Sanchez Simancas
 *            
 * @brief   This program simulates a memory controller scheduler for a 12-core
 *          4.8 GHz processor employing a 16GB PC5-38400 DIMM
 * @version 0.1
 * @date    2023-11-07
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "dimm.h"
#include "memory_request.h"
#include "parser.h"
#include "queue.h"

/*** macro(s), enum(s), and struct(s) ***/
#define MAX_QUEUE_SIZE 16
#define DEFAULT_INPUT_FILE "trace.txt"
#define DEFAULT_OUTPUT_FILE "dram.txt"

enum CommandLine {
  NO_ARGS = 1,
  INPUT_ONLY = 2,
  INPUT_OUTPUT = 3,
};

/*** function prototype(s) ***/
void process_args(int argc, char *argv[], char **input_file, char **output_file);

/*** function(s) ***/
int main(int argc, char *argv[]) {
  char *input_file = NULL;
  char *output_file = NULL;
  process_args(argc, argv, &input_file, &output_file);

  Parser_t *parser = parser_init(input_file);

  unsigned long long clock_cycle = 0;  // tracking the clock cycle (CPU clock). DIMM clock cycle is 1/2.
  DIMM_t *PC5_38400 = NULL;
  Queue_t *global_queue = NULL;

  dimm_create(&PC5_38400);
  queue_create(&global_queue, MAX_QUEUE_SIZE);  // create queue of size 16

  MemoryRequest_t *request_buffer = NULL;
  MemoryRequest_t *dimm_request = NULL;

  while (true) {
    if (request_buffer == NULL && !queue_is_full(global_queue)) {
      request_buffer = parser_next_request(parser, clock_cycle);  // only returns the request if the current cycle >= request's time

      if (request_buffer) LOG("Request buffer: %s\n", memory_request_to_string(request_buffer));
    }

    // DIMM clock cycle
    if (clock_cycle % 2 == 0 && !queue_is_empty(global_queue)) {
      if (dimm_request == NULL) {  // if there is no request being processed, dequeue next request
        dimm_request = malloc(sizeof(MemoryRequest_t));
        *dimm_request = dequeue(&global_queue);
        LOG("Dequeued: %s\n", memory_request_to_string(dimm_request));

      } else {  // otherwise, process the current request
        LOG_DEBUG("Processing: %s\n", memory_request_to_string(dimm_request));
        process_request(&PC5_38400, dimm_request, clock_cycle);
      }

      // if the current request is complete, free it
      if (dimm_request != NULL && dimm_request->state == COMPLETE) {
        LOG("Completed: %s\n", memory_request_to_string(dimm_request));
        free(dimm_request);
        dimm_request = NULL;
      }
    }

    // CPU clock cycle
    if (request_buffer != NULL) {
      enqueue(&global_queue, *request_buffer);
      LOG("Enqueued: %s\n", memory_request_to_string(request_buffer));
      request_buffer = NULL;
    }

    if (parser->status == END_OF_FILE && queue_is_empty(global_queue)) {
      LOG("END OF SIMULATION\n");
      break;
    }

    clock_cycle++;  // increment clock cycle
  }

  parser_destroy(parser);
  queue_destroy(&global_queue);
  dimm_destroy(&PC5_38400);
  return 0;
}

void process_args(int argc, char *argv[], char **input_file, char **output_file) {
  switch (argc) {
    case NO_ARGS:
      *input_file = DEFAULT_INPUT_FILE;
      *output_file = DEFAULT_OUTPUT_FILE;
      break;
    case INPUT_ONLY:
      *input_file = argv[1];
      *output_file = DEFAULT_OUTPUT_FILE;
      break;
    case INPUT_OUTPUT:
      *input_file = argv[1];
      *output_file = argv[2];
      break;
    default:
      fprintf(stderr, "Usage: %s [input_file] [output_file]\n", argv[0]);
      exit(1);
  }
  LOG("Input file: %s\n", *input_file);
  LOG("Output file: %s\n", *output_file);
}