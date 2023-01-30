void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // prints title with ending line break
  Serial.println("ASCII Table ~ Character Map");
}


int thisByte = 33;
void loop() {
  Serial.write(thisByte);

  Serial.print(", dec: ");
  Serial.print(thisByte);


  Serial.print(", hex: ");
  Serial.print(thisByte, HEX);

  Serial.print(", oct: ");
  Serial.print(thisByte, OCT);

  Serial.print(", bin: ");
  Serial.println(thisByte, BIN);
  
  if (thisByte == 126) {
    // This loop loops forever and does nothing
    while (true) {
      continue;
    }
  }
  
  thisByte++;
}
