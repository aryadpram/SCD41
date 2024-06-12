#ifndef _CRC_8_H_
#define _CRC_8_H_

#include <stdint.h>
#include <stdlib.h>

enum CrcPolynomial : uint8_t {
    CRC31_00 = 0x0,
    CRC31_ff = 0x1,
};

uint8_t generateCRCGeneric(const uint8_t* data, size_t count, uint8_t init,
                           uint8_t polynomial);

uint8_t generateCRC31_ff(const uint8_t* data, size_t count);

uint8_t generateCRC31_00(const uint8_t* data, size_t count);

/**
 * @brief Generate a crc for data given a polynomial type
 *
 * @param data data to calculate CRC for
 * @param count the array size of data
 * @param poly CRC polynomal to use
 */
uint8_t generateCRC(const uint8_t* data, size_t count, CrcPolynomial type);
 
#endif /* _CRC_8_H_ */