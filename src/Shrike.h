/*
 * ShrikeFlash.h
 * 
 * Usage:
 *   #include "ShrikeFlash.h"
 *   ShrikeFlash fpga;
 *   fpga.begin();
 *   fpga.flash("/led_blink.bin");
 */

#ifndef SHRIKE_FLASH_H
#define SHRIKE_FLASH_H

#include <Arduino.h>
#include <SPI.h>
#include <LittleFS.h>

class ShrikeFlash {
private:
  uint8_t _en_pin;
  uint8_t _pwr_pin;
  uint8_t _ss_pin;
  uint8_t _sck_pin;
  uint8_t _mosi_pin;
  uint8_t _miso_pin;
  uint32_t _spi_speed;
  SPISettings* _spi_settings;
  unsigned long _last_flash_time;
  float _transfer_rate;

public:
  // Constructor with default pins
  ShrikeFlash(uint8_t en_pin = 13, uint8_t pwr_pin = 12, 
              uint8_t ss_pin = 1, uint8_t sck_pin = 2, 
              uint8_t mosi_pin = 3, uint8_t miso_pin = 0);

  // Initialize the library (call in setup())
  bool begin(uint32_t spi_speed = 1600000);

  // Flash FPGA with bitstream file
  bool flash(const char* filename, uint32_t word_size = 46408);

  // Reset FPGA
  void reset();

  // Timing & Stats
  unsigned long getLastFlashTime();
  float getTransferRate();
  void printStats();

  // File Management
  bool fileExists(const char* filename);
  size_t getFileSize(const char* filename);
  void listFiles();
  bool deleteFile(const char* filename);
  void printFSInfo();
};

#endif // SHRIKE_FLASH_H
