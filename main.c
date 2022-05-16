#include "digital_signal/digital_signal.h"
#include "digital_signal/protocols/nfca.h"
#include "digital_signal/protocols/uart.h"

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
  // print_signal(signal_a);

  uint8_t uart_data[] = {0x55, 0xaa, 0xcc, 0x12};
  UartEncoder *uart = uart_encoder_alloc(10);
  UartConfig config = {
      .baudrate = UrtBaudrate115200,
      .bit_order = UartBitOrderLSB,
      .parity_bit = UartParityBitNone,
      .polarity = UartPolarityNormal,
      .stop_bit = UartStopBit1,
  };

  uart_config(uart, &config);

  uart_encode(uart, uart_data, sizeof(uart_data));

  print_signal(uart->signal);

  digital_signal_free(signal_a);
  uart_free(uart);

  return 0;
}