#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
  bool start_level;
  uint32_t edge_cnt;
  uint32_t edges_max_cnt;
  uint32_t *edge_timings;
  uint32_t *reload_reg_buff;
} DigitalSignalFP;

DigitalSignalFP *digital_signal_fp_alloc(uint32_t max_edges_cnt);

void digital_signal_fp_free(DigitalSignalFP *signal);

bool digital_signal_fp_append(DigitalSignalFP *signal_a, DigitalSignalFP *signal_b);

void digital_signal_fp_prepare_arr(DigitalSignalFP *signal);
