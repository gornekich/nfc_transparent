#pragma once

#include "../digital_signal.h"

typedef enum {
  UrtBaudrate9600 = 9600,
  UrtBaudrate115200 = 115200,
  UrtBaudrate230400 = 230400,
} UartBaudrate;

typedef enum {
  UartStopBit1,
  UartStopBit1_5,
  UartStopBit2,
} UartStopBit;

typedef enum {
  UartParityBitNone,
  UartParityBitEven,
  UartParityBitOdd,
} UartParityBit;

typedef enum {
  UartBitOrderLSB,
  UartBitOrderMSB,
} UartBitOrder;

typedef enum {
  UartPolarityNormal,
  UartPolarityInverted,
} UartPolarity;

typedef struct {
  UartBaudrate baudrate;
  UartStopBit stop_bit;
  UartParityBit parity_bit;
  UartBitOrder bit_order;
  UartPolarity polarity;
} UartConfig;

typedef struct {
  UartConfig config;
  DigitalSignal *signal;
} UartEncoder;

UartEncoder *uart_encoder_alloc(uint32_t bytes);

void uart_free(UartEncoder *uart_encoder);

bool uart_config(UartEncoder *uart_encoder, UartConfig *config);

bool uart_encode(UartEncoder *uart_encoder, uint8_t* data, uint32_t len);
