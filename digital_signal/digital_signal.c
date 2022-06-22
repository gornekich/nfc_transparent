#include "digital_signal.h"

#include <assert.h>
#include <math.h>
#include <string.h>

#define F_TIM (64000000.0)
#define T_TIM (1.0 / F_TIM)

DigitalSignal *digital_signal_alloc(uint32_t max_edges_cnt) {
  DigitalSignal *signal = malloc(sizeof(DigitalSignal));
  signal->start_level = true;
  signal->edges_max_cnt = max_edges_cnt;
  signal->edge_timings = malloc(max_edges_cnt * sizeof(float));
  signal->reload_reg_buff = malloc(max_edges_cnt * sizeof(uint32_t));
  signal->edge_cnt = 0;

  return signal;
}

void digital_signal_free(DigitalSignal *signal) {
  assert(signal);

  free(signal->edge_timings);
  free(signal->reload_reg_buff);
  free(signal);
}

bool digital_signal_append(DigitalSignal *signal_a, DigitalSignal *signal_b) {
  assert(signal_a);
  assert(signal_b);

  if (signal_a->edges_max_cnt < signal_a->edge_cnt + signal_b->edge_cnt) {
    return false;
  }

  bool end_level = signal_a->start_level;
  if (signal_a->edge_cnt) {
    end_level = signal_a->start_level ^ !(signal_a->edge_cnt % 2);
  }
  uint8_t start_copy = 0;
  if (end_level == signal_b->start_level) {
    if (signal_a->edge_cnt) {
      signal_a->edge_timings[signal_a->edge_cnt - 1] +=
          signal_b->edge_timings[0];
      start_copy += 1;
    } else {
      signal_a->edge_timings[signal_a->edge_cnt] += signal_b->edge_timings[0];
    }
  }
  memcpy(&signal_a->edge_timings[signal_a->edge_cnt],
         &signal_b->edge_timings[start_copy],
         (signal_b->edge_cnt - start_copy) * sizeof(float));
  signal_a->edge_cnt += signal_b->edge_cnt - start_copy;

  return true;
}

void digital_signal_prepare_arr(DigitalSignal *signal, uint32_t *arr) {
  float t_signal = 0;
  float t_current = 0;
  float r = 0;
  float r_int = 0;
  float r_dec = 0;

  for (size_t i = 0; i < signal->edge_cnt - 1; i++) {
    t_signal += signal->edge_timings[i];
    r = (t_signal - t_current) / T_TIM;
    r_dec = modff(r, &r_int);
    if (r_dec < 0.5f) {
      signal->reload_reg_buff[i] = (uint32_t)r_int - 1;
    } else {
      signal->reload_reg_buff[i] = (uint32_t)r_int;
    }
    t_current += (signal->reload_reg_buff[i] + 1) * T_TIM;
  }
}

bool digital_signal_get_start_level(DigitalSignal *signal) {
  assert(signal);

  return signal->start_level;
}

uint32_t digital_signal_get_edges_cnt(DigitalSignal *signal) {
  assert(signal);

  return signal->edge_cnt;
}

float digital_signal_get_edge(DigitalSignal *signal, uint32_t edge_num) {
  assert(signal);
  assert(edge_num < signal->edge_cnt);

  return signal->edge_timings[edge_num];
}
