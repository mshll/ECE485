/**
 * @file  dram.h
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __DIMM_H__
#define __DIMM_H__

#include "common.h"
#include "memory_request.h"
#include "queue.h"

/*** macro(s), enum(s), struct(s) ***/
#define TRC       115 // time interval between successive ACT commands to the same bank
#define TRAS       76 // time interval between a bank ACT command and issuing the PRE command
#define TRRD_L     12 // time interval between successive ACT commands to the same bank group
#define TRRD_S      8 // time interval between successive ACT commands to different bank group
#define TRP        39 // time interval between a PRE command and a ACT command
#define TRFC      708 // 295ns; 
#define TCWD       38 // aka tCWL; 
#define TCL        40 // aka tCAS; time interval between a RD command and the output of the first bit of data
#define TRCD       39 // time interval between a ACT command and a RD/WR command
#define TWR        30 // time interval between writing data and issuing a PRE command
#define TRTP       18 // delay between internal RD command to PRE command within the same bank
#define TCCD_L     12 // time interval between consecutive RD or WR commands between different banks in the same bank group
#define TCCD_S      8 // time interval between consecutive RD or WR commands between different banks in different bank group
#define TCCD_L_WR  48 // 
#define TCCD_S_WR   8 //
#define TBURST      8 // delay between the start and end of RD/WR data 
#define TCCD_L_RTW 16 // 
#define TCCD_S_RTW 16 //
#define TCCD_L_WTR 70 //
#define TCCD_S_WTR 52 //
#define TFAW       32 // time window where there can be at most four ACT commands

#define NUM_TFAW_COUNTERS 4
#define NUM_TIMING_CONSTRAINTS 20

#define NUM_BANKS 32
#define NUM_BANK_GROUPS 8
#define NUM_BANKS_PER_GROUP (NUM_BANKS / NUM_BANK_GROUPS)
#define NUM_CHANNELS 2
#define NUM_CHIPS_PER_CHANNEL 4

#define CACHE_LINE_BOUNDARY 64

typedef enum TimingConstraints {
  tRC,
  tRAS,
  tRRD_L,
  tRRD_S,
  tRP,
  tRFC,
  tCWD,
  tCL,
  tRCD,
  tWR,
  tRTP,
  tCCD_L,
  tCCD_S,
  tCCD_L_WR,
  tCCD_S_WR,
  tBURST,
  tCCD_L_RTW,
  tCCD_S_RTW,
  tCCD_L_WTR,
  tCCD_S_WTR
} TimingConstraints_t;

extern uint16_t timing_attribute[NUM_TIMING_CONSTRAINTS];

typedef struct Bank {
  bool is_precharged;
  bool is_active;
  uint32_t active_row;
  uint8_t timing_constraints[NUM_TIMING_CONSTRAINTS];
} Bank_t;

typedef struct BankGroup {
  Bank_t banks[NUM_BANKS_PER_GROUP];
} BankGroup_t;

typedef struct DRAM {
  BankGroup_t bank_groups[NUM_BANK_GROUPS];
  uint8_t tFAW_counters[NUM_TFAW_COUNTERS];
  uint16_t timing_constraints[NUM_BANK_GROUPS][NUM_BANKS_PER_GROUP][NUM_TIMING_CONSTRAINTS];
} DRAM_t;

typedef struct Channel {
  DRAM_t DDR5_chip[NUM_CHIPS_PER_CHANNEL];
} Channel_t;

typedef struct __attribute__((aligned(CACHE_LINE_BOUNDARY))) DIMM {
  Channel_t channels[NUM_CHANNELS];
  FILE *output_file;
} DIMM_t;

/*** function declaration(s) ***/
void dimm_create(DIMM_t **dimm, char *output_file_name);
void dimm_destroy(DIMM_t **dimm);
void process_request(DIMM_t **dimm, Queue_t **q, uint64_t dimm_cycle, uint8_t scheduling_algorithm);

// TODO add additional functions as necessary

#endif