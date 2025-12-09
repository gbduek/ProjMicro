void setup() {
  Serial.begin(9600);      // Serial do PC
  Serial2.begin(9600);     // Serial2 = pinos 16 (TX2) e 17 (RX2)
  Serial.println("Arduino Mega pronto!");
}

void loop() {
  if (Serial2.available()) {
    String msg = Serial2.readStringUntil('\n');
    msg.trim();
    Serial.print("Recebido do ESP32: ");
    Serial.println(msg);

    // Responde de volta
    Serial2.println("OK_ARDUINO");
  }
}
