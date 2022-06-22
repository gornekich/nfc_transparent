#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct {
  bool start_level;
  uint32_t edge_cnt;
  uint32_t edges_max_cnt;
  float *edge_timings;
  uint32_t *reload_reg_buff;
} DigitalSignal;

DigitalSignal *digital_signal_alloc(uint32_t max_edges_cnt);

void digital_signal_free(DigitalSignal *signal);

bool digital_signal_append(DigitalSignal *signal_a, DigitalSignal *signal_b);

bool digital_signal_get_start_level(DigitalSignal *signal);

uint32_t digital_signal_get_edges_cnt(DigitalSignal *signal);

float digital_signal_get_edge(DigitalSignal *signal, uint32_t edge_num);

void digital_signal_prepare_arr(DigitalSignal *signal);