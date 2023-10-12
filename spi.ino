#include <SPI.h>
#define CS_PIN 10
#define W_PIN 5

byte rom[] = {...}; // put here the rom content

void setup() {
  pinMode(CS_PIN, OUTPUT);
  pinMode(W_PIN, INPUT);

  digitalWrite(CS_PIN, HIGH);

  Serial.begin(115200);
  SPI.begin();

  // wait any input in serial to start loop
  while (!Serial.available()) {}
}

void loop() {
  // requires input in "write pin" to start memory write
  if (digitalRead(W_PIN)) {
    Serial.println("\nWill Write Memory in 5s");
    delay(5000);
    Serial.println("\nWriting...");

    for (uint16_t i = 0; i < sizeof(rom); i++) {
      Serial.print("Writing address ");
      Serial.print(i);
      Serial.print("\n");

      enableWrite(CS_PIN);
      writeByteAt(CS_PIN, i, rom[i]);
    }
  }

  printStatusRegister(CS_PIN);
  printMemoryContent(CS_PIN);

  // wait 10s
  delay(10000);
}


uint8_t readByteAt(uint8_t cs, uint16_t addr) {
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  digitalWrite(cs, LOW);
  
  SPI.transfer(0x03);
  SPI.transfer(highByte(addr));
  SPI.transfer(lowByte(addr));

  uint8_t b = SPI.transfer(0); // read the actual byte

  digitalWrite(cs, HIGH);
  SPI.endTransaction();

  return b;
}

void writeByteAt(uint8_t cs, uint16_t addr, uint8_t b) {
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  digitalWrite(cs, LOW);
  
  SPI.transfer(0x02);
  SPI.transfer(highByte(addr));
  SPI.transfer(lowByte(addr));
  SPI.transfer(b);

  digitalWrite(cs, HIGH);
  SPI.endTransaction();

  delay(10); // delay of write
}

void enableWrite(uint8_t cs) {
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  digitalWrite(cs, LOW);
  
  SPI.transfer(0x06); // write enable

  digitalWrite(cs, HIGH);
  SPI.endTransaction();
}

void disableWrite(uint8_t cs) {
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  digitalWrite(cs, LOW);
  
  SPI.transfer(0x04); // write disable

  digitalWrite(cs, HIGH);
  SPI.endTransaction();
}

uint8_t readStatusRegister(uint8_t cs) {
  SPI.beginTransaction(SPISettings(4000000, MSBFIRST, SPI_MODE0));
  digitalWrite(cs, LOW);
  
  SPI.transfer(0x05); // read status register

  uint8_t b = SPI.transfer(0); // read the actual byte

  digitalWrite(cs, HIGH);
  SPI.endTransaction();

  return b;
}

void printStatusRegister(uint8_t cs) {
  uint8_t status = readStatusRegister(cs);
  Serial.print("\nStatus Register: ");
  Serial.print(status, HEX);
}

void printMemoryContent(uint8_t cs) {
  Serial.print("\nMemory Content:");

  char ch[30];
  for (uint16_t i = 0; i < 1024; i++) {
    if ((i&15) == 0) {
      sprintf(ch, "\n0x%03X: ", i);
      Serial.print(ch);
    }
    sprintf(ch, " 0x%02X", readByteAt(cs, i));
    Serial.print(ch);
  }
}
