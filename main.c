#include "digital_signal/digital_signal.h"
#include "digital_signal/protocols/nfca.h"

#include <stdio.h>

static void print_signal(DigitalSignal *signal) {
  printf("%s;", digital_signal_get_start_level(signal) ? "1" : "0");
  for (size_t i = 0; i < digital_signal_get_edges_cnt(signal); i++) {
    printf("%.12f ", digital_signal_get_edge(signal, i));
  }
  printf(";");
}

int main() {
  DigitalSignal *signal_a = digital_signal_alloc(5000);
  // Start of frame
  nfca_add_bit(signal_a, true);
  // Data
  uint8_t data[] = {0, 1,  2,  3,  4,  5,  6,  7,  8,
                    9, 10, 11, 12, 13, 14, 15, 16, 17};
  for (uint8_t i = 0; i < 18; i++) {
    nfca_add_byte(signal_a, data[i], true);
  }
  print_signal(signal_a);

  uint8_t data_2[] = {0xaa, 0xbb};
  DigitalSignal* signal_b = digital_signal_alloc(5000);
  for(size_t i = 0; i < sizeof(data_2); i++) {
    nfca_add_byte(signal_b, data_2[i], false);
  }
  print_signal(signal_b);

  uint8_t data_3[] = {0xff, 0xce, 0x12, 0x22};
  DigitalSignal* signal_c = digital_signal_alloc(5000);
  for(size_t i = 0; i < 1; i++) {
    nfca_add_byte(signal_c, data_3[i], true);
  }
  print_signal(signal_c);

  digital_signal_free(signal_a);
  digital_signal_free(signal_b);
  digital_signal_free(signal_c);

  return 0;
}