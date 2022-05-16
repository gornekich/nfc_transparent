#include "uart.h"

#include <assert.h>

#define MAX_EDGES_PER_BYTE (10)

UartEncoder *uart_encoder_alloc(uint32_t bytes) {
  UartEncoder *uart_encoder = malloc(sizeof(UartEncoder));
  uart_encoder->signal = digital_signal_alloc(MAX_EDGES_PER_BYTE * bytes);

  return uart_encoder;
}

void uart_free(UartEncoder *uart_encoder) {
  assert(uart_encoder);
  digital_signal_free(uart_encoder->signal);
  free(uart_encoder);
}

bool uart_config(UartEncoder *uart_encoder, UartConfig *config) {
  assert(uart_encoder);
  assert(config);

  uart_encoder->config = *config;
  return true;
}

bool uart_encode(UartEncoder *uart_encoder, uint8_t *data, uint32_t len) {
  assert(uart_encoder);
  DigitalSignal *s = uart_encoder->signal;
  uint32_t edge_i = 0;
  float dt = 1.0 / (float)uart_encoder->config.baudrate;
  bool cur_lvl = false;
  s->start_level = cur_lvl;

  for (size_t i = 0; i < len; i++) {
    // Start bit
    s->edge_timings[edge_i++] = dt;
    // bytes
    for (uint8_t j = 0; j < 8; j++) {
        if(data[i] & (1 << (7 - j))) {
            
        }
    }
  }

  return true;
}
