#include "digital_signal_fp.h"

#include <assert.h>
#include <math.h>
#include <string.h>
#include "fix_point.h"

#define F_TIM (64000000.0)
#define T_TIM (1.0 / F_TIM)

#define T_TIM_Q31 (34)
#define ONE_BY_TO_Q31 (1073741824)

DigitalSignalFP *digital_signal_fp_alloc(uint32_t max_edges_cnt) {
  DigitalSignalFP *signal = malloc(sizeof(DigitalSignalFP));
  signal->start_level = true;
  signal->edges_max_cnt = max_edges_cnt;
  signal->edge_timings = malloc(max_edges_cnt * sizeof(uint32_t));
  signal->reload_reg_buff = malloc(max_edges_cnt * sizeof(uint32_t));
  signal->edge_cnt = 0;

  return signal;
}

void digital_signal_fp_free(DigitalSignalFP *signal) {
  assert(signal);

  free(signal->edge_timings);
  free(signal->reload_reg_buff);
  free(signal);
}

bool digital_signal_fp_append(DigitalSignalFP *signal_a,
                              DigitalSignalFP *signal_b) {
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

void digital_signal_fp_prepare_arr(DigitalSignalFP *signal) {
  uint64_t t_signal = 0;
  uint64_t t_current = 0;
  uint64_t r = 0;
  uint64_t r_int = 0;
  uint64_t r_dec = 0;

  for (size_t i = 0; i < signal->edge_cnt - 1; i++) {
    t_signal += signal->edge_timings[i];

    r = FDIV(t_signal - t_current, T_TIM_Q31, 31);
    r_int = r >> 31;
    r_dec = (r - (r_int << 31));
    if (r_dec < ONE_BY_TO_Q31) {
      signal->reload_reg_buff[i] = (uint32_t)r_int - 1;
    } else {
      signal->reload_reg_buff[i] = (uint32_t)r_int;
    }
    t_current += FMUL((uint64_t)(signal->reload_reg_buff[i] + 1) << 31,  T_TIM_Q31, 31);
  }
}
