#pragma once

#include <digital_signal_fp.h>

typedef struct {
    DigitalSignalFP* one;
    DigitalSignalFP* zero;
    DigitalSignalFP* tx_signal;
} NfcaSignalFP;

void nfca_fp_add_bit(DigitalSignalFP *signal, bool bit);

void nfca_fp_add_byte(NfcaSignalFP* nfca_signal, uint8_t byte, bool parity);

NfcaSignalFP* nfca_signal_fp_alloc();

void nfca_signal_fp_free(NfcaSignalFP* nfca_signal);

void nfca_signal_fp_encode(NfcaSignalFP* nfca_signal, uint8_t* data, uint16_t bits, uint8_t* parity);
