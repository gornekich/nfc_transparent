#pragma once

#include <digital_signal.h>

typedef struct {
    DigitalSignal* one;
    DigitalSignal* zero;
    DigitalSignal* tx_signal;
} NfcaSignal;

void nfca_add_bit(DigitalSignal *signal, bool bit);

void nfca_add_byte(NfcaSignal* nfca_signal, uint8_t byte, bool parity);

NfcaSignal* nfca_signal_alloc();

void nfca_signal_free(NfcaSignal* nfca_signal);

void nfca_signal_encode(NfcaSignal* nfca_signal, uint8_t* data, uint16_t bits, uint8_t* parity);
