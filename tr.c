#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define F_SIG (13560000.0)
#define T_SIG (1.0 / F_SIG)

#define F_TIM (64000000.0)
#define T_TIM (1.0 / F_TIM)

#define MAX_SIG_SIZE (1000)

typedef struct {
  bool start_level;
  __uint32_t edge_cnt;
  float edge_timings[MAX_SIG_SIZE];
} DigitaSignal;

void signal_append(DigitaSignal *signal_a, DigitaSignal *signal_b) {
  bool end_level = signal_a->start_level ^ !(signal_a->edge_cnt % 2);
  __uint8_t start_copy = 0;
  if (end_level == signal_b->start_level) {
    signal_a->edge_timings[signal_a->edge_cnt - 1] += signal_b->edge_timings[0];
    start_copy += 1;
  }
  memcpy(&signal_a->edge_timings[signal_a->edge_cnt],
         &signal_b->edge_timings[start_copy],
         (signal_b->edge_cnt - start_copy) * sizeof(float));
  signal_a->edge_cnt += signal_b->edge_cnt - start_copy;
}

void nfca_add_bit(DigitaSignal *signal, bool bit) {
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

void nfca_add_byte(DigitaSignal* signal, __uint8_t byte, bool parity) {
    DigitaSignal one;
    DigitaSignal zero;
    nfca_add_bit(&one, true);
    nfca_add_bit(&zero, false);

    for(__uint8_t i = 0; i < 8; i++) {
        if(byte & (1 << i)) {
            signal_append(signal, &one);
        } else {
            signal_append(signal, &zero);
        }
    }
    if(parity) {
        signal_append(signal, &one);
    } else {
        signal_append(signal, &zero);
    }
}

void prepare_tim_auto_reload(DigitaSignal* signal, __uint32_t* arr) {
    float t_err = 0;
    float t_signal = 0;
    float t_current = 0;
    float r = 0;
    float r_int = 0;
    float r_dec = 0;

    for(size_t i = 0; i < signal->edge_cnt - 1; i++) {
        t_signal += signal->edge_timings[i];
        r = (t_signal - t_current) / T_TIM;
        r_dec = modff(r, &r_int);
        if(r_dec < 0.5f) {
            arr[i] = (__uint32_t) r_int - 1;
        } else {
            arr[i] = (__uint32_t) r_int;
        }
        t_current += (arr[i] + 1) * T_TIM;
    }
}

int main() {
  // Carrier data
//   printf("1;");
//   float carrier[1000] = {};
//   for(size_t i = 0; i < 300; i++) {
//       carrier[i] = T_SIG;
//       printf("%.12f ", carrier[i]);
//   }
//   printf(";");

  DigitaSignal signal_a;
  // Start of frame
  nfca_add_bit(&signal_a, true);
  // Data
  __uint8_t data[] = {0x55, 0x55, 0xff, 0x00};
  __uint8_t parity = (1 << 7) | (0 << 6);
  for(__uint8_t i = 0; i < sizeof(data); i++) {
      nfca_add_byte(&signal_a, data[i], parity & (1 << (7 - i)));
  }
  printf("%s;", signal_a.start_level ? "1" : "0");
  for (size_t i = 0; i < signal_a.edge_cnt; i++) {
    printf("%.12f ", signal_a.edge_timings[i]);
  }

  __uint32_t arr[1000] = {};
  prepare_tim_auto_reload(&signal_a, arr);
  float result[1000] = {};
  printf("; 1; ");
  fprintf(stderr, "\nArr: \n");
  for(size_t i = 0; i < signal_a.edge_cnt - 1; i++) {
      result[i] = (arr[i] + 1) * T_TIM;
      printf("%.12f ", result[i]);
      fprintf(stderr, "%d ", arr[i]);
  }
  fprintf(stderr, "\n");


  return 0;
}