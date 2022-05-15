#include <digital_signal.h>

void nfca_add_bit(DigitalSignal *signal, bool bit);

void nfca_add_byte(DigitalSignal *signal, uint8_t byte, bool parity);