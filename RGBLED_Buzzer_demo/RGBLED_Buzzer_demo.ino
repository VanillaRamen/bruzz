// macros
#define BUTTON_UP 2
#define BUTTON_DOWN 1

// vars
int lastUpState = HIGH; // Saves the state of the button, helpful to know if it was pressed
int lastDownState = HIGH;
int upState = 0;
int downState = 0;

// macros
#define rgb_red 9
#define rgb_green 10
#define rgb_blue 11
#define pin_buzz 5

// vars
int RED { 0 };
int GREEN { 0 };
int BLUE { 0 };
int colorphase { 0 };
unsigned long recordedTime { 0 };

// forward decls
bool cycleLED_buzz();
bool custom_delay(unsigned long);
bool button_pressed();


void setup() {
  // put your setup code here, to run once:
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);

  pinMode(rgb_red, OUTPUT);
  pinMode(rgb_green, OUTPUT);
  pinMode(rgb_blue, OUTPUT);
  pinMode(pin_buzz, OUTPUT);
}

void loop() {
  upState = digitalRead(BUTTON_UP);
  downState = digitalRead(BUTTON_DOWN);
  // put your main code here, to run repeatedly:
  if (button_pressed()) {
    // while(cycleLED()) {
    //   // loop
    // }
    // delay(50);

    while (cycleLED_buzz()) {}

    analogWrite(rgb_red, 0);
    analogWrite(rgb_green, 0);
    analogWrite(rgb_blue, 0);
  }
  lastUpState = upState;
  lastDownState = downState;
}

// defs
bool cycleLED_buzz() {
  tone(pin_buzz, 220);
  if (colorphase == 0) { // green rising, red falling
      RED = 47;
      GREEN = 0;
      BLUE = 0;

      for (; !button_pressed() && RED > 0;) {
        analogWrite(rgb_red, RED); analogWrite(rgb_green, GREEN);
        RED--;
        GREEN++;
        if (custom_delay(70)) { // delay 5 ms
          noTone(pin_buzz);
          return false; // button pressed --> exit function
        }
      }

      colorphase++;

      return true;

  } else if (colorphase == 1) { // blue rising, green falling
      RED = 0;
      GREEN = 47;
      BLUE = 0;

      for (; !button_pressed() && GREEN > 0;) {
        analogWrite(rgb_green, GREEN); analogWrite(rgb_blue, BLUE);
        GREEN--;
        BLUE++;
        if (custom_delay(70)) { // delay 5 ms
          noTone(pin_buzz);
          return false; // button pressed --> exit function
        }
      }

      colorphase++;

      return true;

  } else if (colorphase == 2) { // red rising, blue falling
      RED = 0;
      GREEN = 0;
      BLUE = 47;

      for (; !button_pressed() && BLUE > 0;) {
        analogWrite(rgb_red, RED); analogWrite(rgb_blue, BLUE);
        RED++;
        BLUE--;
        if (custom_delay(70)) { // delay 5 ms
          noTone(pin_buzz);
          return false; // button pressed --> exit function
        }
      }

      colorphase = 0;

      return true;

  } else {
    return false; // error?
  }
}

bool custom_delay(unsigned long delay) {
  while (millis() - recordedTime < delay) {
    if (button_pressed()) {
      recordedTime = millis();
      return true;
    }
  }
  recordedTime = millis();
  return false;
}

bool button_pressed() {
  upState = digitalRead(BUTTON_UP);
  downState = digitalRead(BUTTON_DOWN);

  if (upState == HIGH && lastUpState == LOW) {
    lastUpState = upState;
    return true;
  } else if (downState == HIGH && lastDownState == LOW) {
    lastDownState = downState;
    return true;
  }
  lastUpState = upState;
  lastDownState = downState;
  return false;
}