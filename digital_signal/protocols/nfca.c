#include "nfca.h"

#include <assert.h>

#define F_SIG (13560000.0)
#define T_SIG (1.0 / F_SIG)

#define NFCA_SIGNAL_MAX_EDGES (1350)

#define FURI_BIT(x, n) (((x) >> (n)) & 1)

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

void nfca_add_byte(NfcaSignal* nfca_signal, uint8_t byte, bool parity) {
  for (uint8_t i = 0; i < 8; i++) {
    if (byte & (1 << i)) {
      digital_signal_append(nfca_signal->tx_signal, nfca_signal->one);
    } else {
      digital_signal_append(nfca_signal->tx_signal, nfca_signal->zero);
    }
  }
  if (parity) {
    digital_signal_append(nfca_signal->tx_signal, nfca_signal->one);
  } else {
    digital_signal_append(nfca_signal->tx_signal, nfca_signal->zero);
  }
}

NfcaSignal* nfca_signal_alloc() {
    NfcaSignal* nfca_signal = malloc(sizeof(NfcaSignal));
    nfca_signal->one = digital_signal_alloc(10);
    nfca_signal->zero = digital_signal_alloc(10);
    nfca_add_bit(nfca_signal->one, true);
    nfca_add_bit(nfca_signal->zero, false);
    nfca_signal->tx_signal = digital_signal_alloc(NFCA_SIGNAL_MAX_EDGES);

    return nfca_signal;
}

void nfca_signal_free(NfcaSignal* nfca_signal) {
    assert(nfca_signal);

    digital_signal_free(nfca_signal->one);
    digital_signal_free(nfca_signal->zero);
    digital_signal_free(nfca_signal->tx_signal);
    free(nfca_signal);
}

void nfca_signal_encode(NfcaSignal* nfca_signal, uint8_t* data, uint16_t bits, uint8_t* parity) {
    assert(nfca_signal);
    assert(data);
    assert(parity);

    nfca_signal->tx_signal->edge_cnt = 0;
    nfca_signal->tx_signal->start_level = true;
    // Start of frame
    digital_signal_append(nfca_signal->tx_signal, nfca_signal->one);

    if(bits < 8) {
        for(size_t i = 0; i < bits; i++) {
            if(FURI_BIT(data[0], i)) {
                digital_signal_append(nfca_signal->tx_signal, nfca_signal->one);
            } else {
                digital_signal_append(nfca_signal->tx_signal, nfca_signal->zero);
            }
        }
    } else {
        for(size_t i = 0; i < bits / 8; i++) {
            nfca_add_byte(nfca_signal, data[i], parity[i / 8] & (1 << (7 - (i & 0x07))));
        }
    }
}
