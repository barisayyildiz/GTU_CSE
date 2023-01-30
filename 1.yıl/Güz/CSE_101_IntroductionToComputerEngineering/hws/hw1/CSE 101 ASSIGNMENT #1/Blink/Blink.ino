void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(500);                       // waits for 0.5 seconds
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);                       // waits for 0.5 seconds
}
