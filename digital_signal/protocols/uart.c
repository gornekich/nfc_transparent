#include "uart.h"

#include <assert.h>

#define MAX_EDGES_PER_BYTE (10)

#define GET_BIT(x, n) (((x) & (1 << (n))) == (1 << (n)))

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
    uint8_t parity_cnt = 0;
    // Start bit
    if (cur_lvl) {
      s->edge_timings[edge_i++] = dt;
      cur_lvl = !cur_lvl;
    } else {
      if (edge_i) {
        s->edge_timings[edge_i - 1] += dt;
      } else {
        s->edge_timings[edge_i++] = dt;
      }
    }
    // Data bits
    // TODO now LSB first. Rework with uart configuration
    for (uint8_t j = 0; j < 8; j++) {
      if (GET_BIT(data[i], j) == true) {
        parity_cnt++;
      }
      if (GET_BIT(data[i], j) == cur_lvl) {
        s->edge_timings[edge_i - 1] += dt;
      } else {
        s->edge_timings[edge_i++] = dt;
        cur_lvl = !cur_lvl;
      }
    }

    // Parity bit
    if (uart_encoder->config.parity_bit == UartParityBitEven) {
      if (parity_cnt % 2) {
        if (cur_lvl) {
          s->edge_timings[edge_i - 1] += dt;
        } else {
          s->edge_timings[edge_i++] = dt;
          cur_lvl = !cur_lvl;
        }
      } else {
        if (cur_lvl) {
          s->edge_timings[edge_i++] = dt;
          cur_lvl = !cur_lvl;
        } else {
          s->edge_timings[edge_i - 1] += dt;
        }
      }
    } else if (uart_encoder->config.parity_bit == UartParityBitOdd) {
      if (parity_cnt % 2) {
        if (cur_lvl) {
          s->edge_timings[edge_i++] = dt;
          cur_lvl = !cur_lvl;
        } else {
          s->edge_timings[edge_i - 1] += dt;
        }
      } else {
        if (cur_lvl) {
          s->edge_timings[edge_i - 1] += dt;
        } else {
          s->edge_timings[edge_i++] = dt;
          cur_lvl = !cur_lvl;
        }
      }
    }

    // Stop bit
    float stop_bit_dt = 0.0f;
    if (uart_encoder->config.stop_bit == UartStopBit1) {
      stop_bit_dt = dt;
    } else if (uart_encoder->config.stop_bit == UartStopBit1_5) {
      stop_bit_dt = dt * 1.5f;
    } else if (uart_encoder->config.stop_bit == UartStopBit2) {
      stop_bit_dt = dt * 2.0f;
    }
    if (cur_lvl) {
      s->edge_timings[edge_i - 1] += stop_bit_dt;
    } else {
      s->edge_timings[edge_i++] = stop_bit_dt;
      cur_lvl = !cur_lvl;
    }
  }
  s->edge_cnt = edge_i;

  return true;
}
