#include "CRC8.h"

uint8_t generateCRCGeneric(const uint8_t* data, size_t count, uint8_t init,
                           uint8_t polynomial) {
    uint8_t crc = init;

    /* calculates 8-Bit checksum with given polynomial */
    for (size_t current_byte = 0; current_byte < count; ++current_byte) {
        crc ^= (data[current_byte]);
        for (uint8_t crc_bit = 8; crc_bit > 0; --crc_bit) {
            if (crc & 0x80)
                crc = (crc << 1) ^ polynomial;
            else
                crc = (crc << 1);
        }
    }
    return crc;
}

uint8_t generateCRC31_ff(const uint8_t* data, size_t count) {
    return generateCRCGeneric(data, count, 0xff, 0x31);
}

uint8_t generateCRC31_00(const uint8_t* data, size_t count) {
    return generateCRCGeneric(data, count, 0x00, 0x31);
}

uint8_t generateCRC(const uint8_t* data, size_t count, CrcPolynomial type) {
    if (CRC31_00 == type) {
        return generateCRC31_00(data, count);
    }
    return generateCRC31_ff(data, count);
}