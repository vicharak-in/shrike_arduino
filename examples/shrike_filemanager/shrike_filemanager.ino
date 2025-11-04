#include "Shrike.h"

ShrikeFlash fpga;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("\n=== ShrikeFlash File Manager Example ===");

  if (!fpga.begin()) {
    Serial.println("Failed to initialize ShrikeFlash!");
    while (1);
  }

  // Print filesystem info
  fpga.printFSInfo();

  // List files
  fpga.listFiles();

  // Check if a file exists
  const char* filename = "/led_blink.bin";
  if (fpga.fileExists(filename)) {
    Serial.print("File ");
    Serial.print(filename);
    Serial.print(" found! Size: ");
    Serial.print(fpga.getFileSize(filename));
    Serial.println(" bytes");
  } else {
    Serial.print("File ");
    Serial.print(filename);
    Serial.println(" not found!");
  }

  // Delete an old test file
  const char* testfile = "/old_bitstream.bin";
  if (fpga.fileExists(testfile)) {
    Serial.println("Deleting old bitstream...");
    fpga.deleteFile(testfile);
  }

  Serial.println("File manager operations complete.");
}

void loop() {
  // Nothing here — all operations done in setup
}

