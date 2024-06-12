#include <iostream>
#include "CRC8.h"

int main() {
    // Values to calculate CRC for
    uint16_t values[] = {0x01f, 0x6667, 0x5eb9};
    // Buffer to hold values as bytes
    uint8_t buffer[6];

    // Convert values to bytes and fill the buffer
    for (int i = 0; i < 3; ++i) {
        buffer[2 * i] = (values[i] >> 8) & 0xFF;    // MSB
        buffer[2 * i + 1] = values[i] & 0xFF;       // LSB
    }

    // Calculate CRC for the buffer
    uint8_t crc = generateCRC(buffer, 6, CRC31_ff);

    std::cout << "CRC: 0x" << std::hex << static_cast<int>(crc) << std::endl;

    return 0;
}
