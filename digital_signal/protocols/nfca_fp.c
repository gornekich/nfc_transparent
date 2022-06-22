#include "nfca_fp.h"

#include <assert.h>

#define F_SIG (13560000.0)
#define T_SIG (1.0 / F_SIG)

#define T_SIG_Q31 (158)
#define T_SIG_8 (1267)
#define T_SIG_8_8 (10136)
#define T_SIG_8_9 (11403)


#define NFCA_SIGNAL_MAX_EDGES (1350)

#define FURI_BIT(x, n) (((x) >> (n)) & 1)

void nfca_fp_add_bit(DigitalSignalFP *signal, bool bit) {
  if (bit) {
    signal->start_level = true;
    for (size_t i = 0; i < 7; i++) {
      signal->edge_timings[i] = T_SIG_8;
    }
    signal->edge_timings[7] = T_SIG_8_9;
    signal->edge_cnt = 8;
  } else {
    signal->start_level = false;
    signal->edge_timings[0] = T_SIG_8_8;
    for (size_t i = 1; i < 9; i++) {
      signal->edge_timings[i] = T_SIG_8;
    }
    signal->edge_cnt = 9;
  }
}

void nfca_fp_add_byte(NfcaSignalFP* nfca_signal, uint8_t byte, bool parity) {
  for (uint8_t i = 0; i < 8; i++) {
    if (byte & (1 << i)) {
      digital_signal_fp_append(nfca_signal->tx_signal, nfca_signal->one);
    } else {
      digital_signal_fp_append(nfca_signal->tx_signal, nfca_signal->zero);
    }
  }
  if (parity) {
    digital_signal_fp_append(nfca_signal->tx_signal, nfca_signal->one);
  } else {
    digital_signal_fp_append(nfca_signal->tx_signal, nfca_signal->zero);
  }
}

NfcaSignalFP* nfca_signal_fp_alloc() {
    NfcaSignalFP* nfca_signal = malloc(sizeof(NfcaSignalFP));
    nfca_signal->one = digital_signal_fp_alloc(10);
    nfca_signal->zero = digital_signal_fp_alloc(10);
    nfca_fp_add_bit(nfca_signal->one, true);
    nfca_fp_add_bit(nfca_signal->zero, false);
    nfca_signal->tx_signal = digital_signal_fp_alloc(NFCA_SIGNAL_MAX_EDGES);

    return nfca_signal;
}

void nfca_signal_fp_free(NfcaSignalFP* nfca_signal) {
    assert(nfca_signal);

    digital_signal_fp_free(nfca_signal->one);
    digital_signal_fp_free(nfca_signal->zero);
    digital_signal_fp_free(nfca_signal->tx_signal);
    free(nfca_signal);
}

void nfca_signal_fp_encode(NfcaSignalFP* nfca_signal, uint8_t* data, uint16_t bits, uint8_t* parity) {
    assert(nfca_signal);
    assert(data);
    assert(parity);

    nfca_signal->tx_signal->edge_cnt = 0;
    nfca_signal->tx_signal->start_level = true;
    // Start of frame
    digital_signal_fp_append(nfca_signal->tx_signal, nfca_signal->one);

    if(bits < 8) {
        for(size_t i = 0; i < bits; i++) {
            if(FURI_BIT(data[0], i)) {
                digital_signal_fp_append(nfca_signal->tx_signal, nfca_signal->one);
            } else {
                digital_signal_fp_append(nfca_signal->tx_signal, nfca_signal->zero);
            }
        }
    } else {
        for(size_t i = 0; i < bits / 8; i++) {
            nfca_fp_add_byte(nfca_signal, data[i], parity[i / 8] & (1 << (7 - (i & 0x07))));
        }
    }
}
