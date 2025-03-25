void setup() {
  while (!Serial) { // for debugging & monitoring
    Serial.begin(9600);
  }

  pinMode(13, INPUT); // for current mode (buzz/led)
  // mode could also be handled by bool for in-device mode handling
  pinMode(10, OUTPUT); // BUZZER PIN
  pinMode(6, OUTPUT); // RGBLED RED
  pinMode(5, OUTPUT); // RGBLED GREEN
  pinMode(3, OUTPUT); // RGBLED BLUE

}

int RED { 127 }; // RGB VALS
int GREEN { 127 }; // initiated halfway through spectrum
int BLUE { 127 }; // to avoid darkness
enum MODE { BUZZER = 0, RGBLED }; // human-friendly ints
int mode { 0 }; // ^^^ mode
int colorphase { 0 };

void loop() {
  while (digitalRead(13) == HIGH) { // switch is set to buzzer

    if (mode != BUZZER) { // resets RGBLED values
      mode = BUZZER; // RGBLEDs stay lit without this
      analogWrite(6, 0);
      analogWrite(5, 0);
      analogWrite(3, 0);
    }
    tone(10, 440, 1000); // buzzer sounds (later on, hardcode "Animals" by Maroon 5 into a function)
  } // exits loop if digitalRead(13) == LOW
  

  while (digitalRead(13) == LOW) { // switch is set to LED
    if (mode != RGBLED) {
      mode = RGBLED;
      noTone(10);
    }

    if (colorphase == 0) {
      RED = 255;
      GREEN = 0;
      BLUE = 0;
      for (; digitalRead(13) == LOW && RED > 0;) {
        analogWrite(6, RED); analogWrite(5, GREEN);
        RED--;
        GREEN++;
        delay(5);
      }
      colorphase++;
    } else if (colorphase == 1) {
      RED = 0;
      GREEN = 255;
      BLUE = 0;
      for (; digitalRead(13) == LOW && GREEN > 0;) {
        analogWrite(5, GREEN); analogWrite(3, BLUE);
        GREEN--;
        BLUE++;
        delay(5);
      }
      colorphase++;
    } else if (colorphase == 2) {
      RED = 0;
      GREEN = 0;
      BLUE = 255;
      for (; digitalRead(13) == LOW && BLUE > 0;) {
        analogWrite(6, RED); analogWrite(3, BLUE);
        RED++;
        BLUE--;
        delay(5);
      }
      colorphase = 0;
    }
}
