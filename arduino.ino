void setup() {
  Serial.begin(9600);
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "LED_ON") {
      digitalWrite(13, HIGH);
      Serial.println("OK_LED_ON");
    } else if (cmd == "LED_OFF") {
      digitalWrite(13, LOW);
      Serial.println("OK_LED_OFF");
    }
  }
}
