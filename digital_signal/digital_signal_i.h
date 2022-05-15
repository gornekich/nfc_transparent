#pragma once

struct DigitalSignal {
  bool start_level;
  uint32_t edge_cnt;
  float *edge_timings;
};
