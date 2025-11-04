# ShrikeFlash

Arduino library for flashing FPGA bitstreams over SPI on RP2040 microcontrollers.

## Features

- Flash FPGA bit-streams from LittleFS filesystem
- Configurable SPI settings and pin mapping
- Transfer timing statistics and performance monitoring
- File management utilities (list, check, delete files)
- Easy-to-use API with detailed serial output

## Hardware Requirements

- Shrike Dev Board

## Default Pin Configuration

| Function | RP2040 Pin | Description |
|----------|------------|-------------|
| EN       | GPIO 13    | FPGA Enable |
| PWR      | GPIO 12    | FPGA Power  |
| SS       | GPIO 1     | SPI Chip Select |
| SCK      | GPIO 2     | SPI Clock   |
| MOSI     | GPIO 3     | SPI Data Out |
| MISO     | GPIO 0     | SPI Data In |

## Installation

### Via Arduino Library Manager (Once Published)
1. Open Arduino IDE
2. Go to **Sketch → Include Library → Manage Libraries**
3. Search for "ShrikeFlash"
4. Click **Install**

### Manual Installation
1. Download the latest release from [GitHub](https://github.com/yourusername/ShrikeFlash)
2. Extract the ZIP file
3. Move the `ShrikeFlash` folder to your Arduino libraries directory:
   - **Windows**: `Documents\Arduino\libraries\`
   - **Mac**: `~/Documents/Arduino/libraries/`
   - **Linux**: `~/Arduino/libraries/`
4. Restart Arduino IDE

## Usage

### Basic Example

```cpp
#include <Shrike.h>

ShrikeFlash fpga;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
  
  // Initialize the library
  if (!fpga.begin()) {
    Serial.println("Initialization failed!");
    while(1);
  }
  
  // Flash the FPGA
  fpga.flash("/bitstream.bin");
}

void loop() {
  // Your code here
}
```

### Upload Bitstream Files

1. Create a `data` folder in your sketch directory
2. Place your `.bin` bitstream files in the `data` folder
3. Use **Tools → Pico LittleFS Data Upload** to upload files to the RP2040
4. Upload your sketch

### Custom Pin Configuration

```cpp
// ShrikeFlash(EN, PWR, SS, SCK, MOSI, MISO)
ShrikeFlash fpga(13, 12, 1, 2, 3, 0);

void setup() {
  fpga.begin();
  fpga.flash("/custom_bitstream.bin");
}
```

## API Reference

### Initialization

#### `begin(spi_speed)`
Initialize the library with optional SPI speed.
- **Parameters**: `spi_speed` (default: 1600000 Hz)
- **Returns**: `bool` - true if successful

```cpp
fpga.begin();              // Default 1.6 MHz
fpga.begin(2000000);       // Custom 2 MHz
```

### Flashing

#### `flash(filename, word_size)`
Flash FPGA with bitstream file.
- **Parameters**: 
  - `filename` - Path to bitstream file (e.g., "/bitstream.bin")
  - `word_size` - Transfer chunk size (default: 46408 bytes)
- **Returns**: `bool` - true if successful

```cpp
fpga.flash("/led_blink.bin");
fpga.flash("/custom.bin", 8192);  // Custom chunk size
```

#### `reset()`
Reset the FPGA (power cycle).

```cpp
fpga.reset();
```

### Timing & Statistics

#### `getLastFlashTime()`
Get the time taken for last flash operation.
- **Returns**: `unsigned long` - Time in milliseconds

```cpp
unsigned long time = fpga.getLastFlashTime();
Serial.print("Flash took: ");
Serial.print(time);
Serial.println(" ms");
```

#### `getTransferRate()`
Get the transfer rate of last flash operation.
- **Returns**: `float` - Transfer rate in KB/s

```cpp
float rate = fpga.getTransferRate();
Serial.print("Transfer rate: ");
Serial.print(rate);
Serial.println(" KB/s");
```

#### `printStats()`
Print detailed statistics of last flash operation.

```cpp
fpga.printStats();
// Output:
// === Flash Statistics ===
// Time taken: 2345 ms
// Transfer rate: 85.32 KB/s
// ===========================
```

### File Management

#### `fileExists(filename)`
Check if a file exists in LittleFS.
- **Returns**: `bool`

```cpp
if (fpga.fileExists("/bitstream.bin")) {
  Serial.println("File found!");
}
```

#### `getFileSize(filename)`
Get file size in bytes.
- **Returns**: `size_t` - File size in bytes (0 if not found)

```cpp
size_t size = fpga.getFileSize("/bitstream.bin");
Serial.print("File size: ");
Serial.print(size);
Serial.println(" bytes");
```

#### `listFiles()`
List all files in LittleFS filesystem.

```cpp
fpga.listFiles();
// Output:
// === Files in LittleFS ===
//   /bitstream.bin - 135040 bytes
//   /led_blink.bin - 92416 bytes
// ===========================
```

#### `deleteFile(filename)`
Delete a file from LittleFS.
- **Returns**: `bool` - true if successful

```cpp
if (fpga.deleteFile("/old_file.bin")) {
  Serial.println("File deleted successfully");
}
```

#### `printFSInfo()`
Print filesystem information (total, used, free space).

```cpp
fpga.printFSInfo();
// Output:
// === Filesystem Info ===
// Total space: 2048 KB
// Used space: 256 KB
// Free space: 1792 KB
// ===========================
```

## Complete Example

```cpp
#include <Shrike.h>

ShrikeFlash fpga;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
  
  Serial.println("ShrikeFlash Example");
  
  // Initialize
  if (!fpga.begin()) {
    Serial.println("Failed to initialize!");
    while(1);
  }
  
  // Show filesystem info
  fpga.printFSInfo();
  
  // List available files
  fpga.listFiles();
  
  // Check if file exists
  if (!fpga.fileExists("/led_blink.bin")) {
    Serial.println("Bitstream file not found!");
    Serial.println("Please upload files using LittleFS Data Upload tool");
    while(1);
  }
  
  // Flash the FPGA
  Serial.println("Starting flash...");
  if (fpga.flash("/led_blink.bin")) {
    Serial.println("Flash successful!");
    
    // Print statistics
    fpga.printStats();
    
    Serial.print("Time: ");
    Serial.print(fpga.getLastFlashTime());
    Serial.println(" ms");
    
    Serial.print("Rate: ");
    Serial.print(fpga.getTransferRate(), 2);
    Serial.println(" KB/s");
  } else {
    Serial.println("Flash failed!");
  }
}

void loop() {
  // Empty
}
```

## Troubleshooting

### LittleFS Mount Failed
- Make sure you've selected a board configuration with LittleFS support
- Go to **Tools → Flash Size** and select an option with FS (e.g., "2MB Sketch + 2MB FS")
- Upload your files using **Tools → Pico LittleFS Data Upload**

### File Not Found
- Ensure files are in the `data` folder of your sketch
- Use **Tools → Pico LittleFS Data Upload** before uploading sketch
- Check filenames match exactly (case-sensitive)

### FPGA Not Programming
- Verify wiring connections
- Check power supply to FPGA
- Try reducing SPI speed: `fpga.begin(800000);`
- Verify bitstream file is valid

## Board Support

This library requires the **Arduino-Pico** or **Arduino Mbed OS RP2040** board package.

Install from Board Manager:
- **Arduino-Pico**: https://github.com/earlephilhower/arduino-pico
- Add to Board Manager URLs: `https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json`

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

MIT License - see LICENSE file for details
