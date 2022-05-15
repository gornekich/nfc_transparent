#include "nfca.h"

#include <digital_signal_i.h>

#define F_SIG (13560000.0)
#define T_SIG (1.0 / F_SIG)

void nfca_add_bit(DigitalSignal *signal, bool bit) {
  if (bit) {
    signal->start_level = true;
    for (size_t i = 0; i < 7; i++) {
      signal->edge_timings[i] = 8 * T_SIG;
    }
    signal->edge_timings[7] = 9 * 8 * T_SIG;
    signal->edge_cnt = 8;
  } else {
    signal->start_level = false;
    signal->edge_timings[0] = 8 * 8 * T_SIG;
    for (size_t i = 1; i < 9; i++) {
      signal->edge_timings[i] = 8 * T_SIG;
    }
    signal->edge_cnt = 9;
  }
}

void nfca_add_byte(DigitalSignal *signal, uint8_t byte, bool parity) {
  DigitalSignal *one = digital_signal_alloc(100);
  DigitalSignal *zero = digital_signal_alloc(100);
  nfca_add_bit(one, true);
  nfca_add_bit(zero, false);

  for (uint8_t i = 0; i < 8; i++) {
    if (byte & (1 << i)) {
      digital_signal_append(signal, one);
    } else {
      digital_signal_append(signal, zero);
    }
  }
  if (parity) {
    digital_signal_append(signal, one);
  } else {
    digital_signal_append(signal, zero);
  }

  digital_signal_free(one);
  digital_signal_free(zero);
}
