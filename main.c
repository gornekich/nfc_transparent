#include "digital_signal/digital_signal.h"
#include "digital_signal/protocols/nfca.h"
#include "digital_signal/protocols/uart.h"
#include "digital_signal/protocols/nfca.h"
#include "digital_signal/protocols/nfca_fp.h"
#include "digital_signal/protocols/digital_signal_fp.h"

#include <stdio.h>

static void print_signal(DigitalSignal *signal) {
  printf("%s;", digital_signal_get_start_level(signal) ? "1" : "0");
  for (size_t i = 0; i < digital_signal_get_edges_cnt(signal); i++) {
    printf("%.12f ", digital_signal_get_edge(signal, i));
  }
  printf(";");
}

static void print_arr(DigitalSignal *signal, char* message) {
  printf("\r\n%s\r\n", message);
  uint32_t sum = 0;
  for (size_t i = 0; i < signal->edge_cnt; i++) {
    sum += signal->reload_reg_buff[i];
    printf("%d ", signal->reload_reg_buff[i]);
  }
  printf("\r\nArr sum: %d\r\n", sum);
}

static void print_arr_fp(DigitalSignalFP *signal, char* message) {
  printf("\r\n%s\r\n", message);
  uint32_t sum = 0;
  for (size_t i = 0; i < signal->edge_cnt; i++) {
    sum += signal->reload_reg_buff[i];
    printf("%d ", signal->reload_reg_buff[i]);
  }
  printf("\r\nArr sum: %d\r\n", sum);
}

int main() {
  // Data
  uint8_t data[] = {0xf1, 0x99, 0x41,  3,  4,  5,  6,  7,  8,
                    9, 10, 11, 12, 13, 14, 15, 16, 17};
  uint8_t parity[10] = {};
  // NFCA signal
  NfcaSignal* signal = nfca_signal_alloc();

  nfca_signal_encode(signal, data, sizeof(data) * 8, parity);

  // print_signal(signal->tx_signal);

  NfcaSignalFP* signal_fp = nfca_signal_fp_alloc();
  nfca_signal_fp_encode(signal_fp, data, sizeof(data) * 8, parity);

  digital_signal_prepare_arr(signal->tx_signal);
  print_arr(signal->tx_signal, "Float calc");
  digital_signal_fp_prepare_arr(signal_fp->tx_signal);
  print_arr_fp(signal_fp->tx_signal, "Fix Point calc");

  // float min = 1.0f;
  // float max = 0.0f;
  // for(size_t i = 0; i < signal->tx_signal->edge_cnt; i++) {
  //   if(signal->tx_signal->edge_timings[i] < min) {
  //     min = signal->tx_signal->edge_timings[i];
  //   }
  //   if(signal->tx_signal->edge_timings[i] > max) {
  //     max = signal->tx_signal->edge_timings[i];
  //   }
  // }

  // printf("\r\nmin:%.12f\r\nmax:%.12f\r\n", min, max);

  nfca_signal_free(signal);
  nfca_signal_fp_free(signal_fp);

  // uint8_t uart_data[] = {0x55, 0xaa, 0xcc, 0x12};
  // UartEncoder *uart = uart_encoder_alloc(10);
  // UartConfig config = {
  //     .baudrate = UrtBaudrate115200,
  //     .bit_order = UartBitOrderLSB,
  //     .parity_bit = UartParityBitNone,
  //     .polarity = UartPolarityNormal,
  //     .stop_bit = UartStopBit1,
  // };

  // uart_config(uart, &config);

  // uart_encode(uart, uart_data, sizeof(uart_data));

  // print_signal(uart->signal);

  // digital_signal_free(signal_a);
  // uart_free(uart);

  return 0;
}

#define Q 31

#define T_NFCA_31 158

#define T_TIM_31 34

#define T_MIN (uint64_t)1267

#define T_MAX (uint64_t)21538

#define FADD(a,b) ((a)+(b))
#define FSUB(a,b) ((a)-(b))
#define FMUL(a,b,q) (((a)*(b))>>(q))
#define FDIV(a,b,q) (((a)<<(q))/(b))

#define FCONV(a, q1, q2) (((q2)>(q1)) ? (a)<<((q2)-(q1)) : (a)>>((q1)-(q2)))

int _main() {

  uint64_t div_max = FDIV(T_MAX, T_TIM_31, Q);
  uint64_t div_min = FDIV(T_MIN, T_TIM_31, Q);

  printf("div_max: %ld, div: %ld \r\n", div_max, FCONV(div_max, Q, 0));
  printf("div_min: %ld, div: %ld \r\n", div_min, FCONV(div_min, Q, 0));
  

  return 0;
}