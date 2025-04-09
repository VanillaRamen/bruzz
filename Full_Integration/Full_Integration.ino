#include <LiquidCrystal.h>
#define BUTTON_UP 2    // Typical top button
#define BUTTON_DOWN 1  // Beta bottom button
#define fsrpin A0

#define pin_buzzer  11
#define rgb_red     10
#define rgb_green   9
#define rgb_blue    3
int RED { 0 }; // RGB VALS
int GREEN { 0 }; // initiated halfway through spectrum
int BLUE { 0 }; // to avoid darkness
int colorphase { 0 };
unsigned long recordedTime { 0 };
enum DEVICE_MODE {BOTH, ONLY_LIGHTS, ONLY_SOUND, OFF};

LiquidCrystal lcd(13,  // rs
                  12,  // en
                  7,   // d4
                  6,   // d5
                  5,   // d6
                  4);  // d7 // LCD THE GOAT

//Initalizes pins for all components
int ledPin = 10;
int pirPin = 8;
int pirState = LOW;  //PIR              detection state
int val = 0;         // Value of PIR Input

//Define variable to store sensor readings:
int fsrreading = { 0 };  // Variable to store FSR
bool objectDetected = false;
int currentMode = 0;
String modes[4] = { "Both", "Only Lights", "Only Sound", "Off" };  // All the modes

int upState = HIGH;
int downState = HIGH;
int lastUpState = HIGH;    // Saves the state of the button, helpful to know if it was pressed
int lastDownState = HIGH;  // Ditto

void updateLCD() {
  // Sets up the LCD to be used
  lcd.clear();
  lcd.print("Mode: ");
  lcd.print(modes[currentMode]);
}

void updateLCD();                 // user interface
bool cycleLED();                  // alerting
bool cycleLED_buzz();             // alerting
bool soundBuzzer();               // alerting
bool custom_delay(unsigned long); // alerting
bool button_pressed();            // alerting

void setup() {
  pinMode(ledPin, OUTPUT);  //Sets LED to Output
  pinMode(pirPin, INPUT);   //Sets PIR to Input data

  // Begin serial communication at a baudrate of 9600:
  Serial.begin(9600);

  // Classic set up, really just pins
  lcd.begin(16, 2);

  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);

  lcd.setCursor(0, 0);
  updateLCD();

  pinMode(pin_buzzer, OUTPUT);
  pinMode(rgb_red, OUTPUT);
  pinMode(rgb_green, OUTPUT);
  pinMode(rgb_blue, OUTPUT);
}


void loop() {
  int upState = digitalRead(BUTTON_UP);
  int downState = digitalRead(BUTTON_DOWN);

  // Updates the mode going up
  if (upState == HIGH && lastUpState == LOW) {
    currentMode++;
    if (currentMode >= 4) {
      currentMode = 0;
    }
    updateLCD();
  }
  lastUpState = upState;

  // Updates the mode going down
  if (downState == HIGH && lastDownState == LOW) {
    currentMode--;
    if (currentMode < 0) {
      currentMode = 3;
    }
    updateLCD();
  }
  lastDownState = downState;

  // Read the FSR pin and store the outputas fsrreading:
  
  fsrreading = analogRead(fsrpin);
  // Serial.print("Analog reading = ");

  // Print the fsrreading:
  // Serial.print(fsrreading);
  // Serial.println();
  // We can set some threshholds to display how much pressure is roughly applied:
  if (fsrreading > 200) {
    objectDetected = true;
    Serial.println("Weight Detected");
  } else {
    // Serial.println("Object Not Detected");
  }

  if (fsrreading > 200) {
    objectDetected = true;
  }

  val = digitalRead(pirPin);     // read input value
  if (val == HIGH) {             // check if the input is HIGH
    analogWrite(rgb_red, 31);  // turn LED ON
    if (pirState == HIGH) {

      Serial.println("Motion detected!");  //PIR sensor is detecting

      pirState = LOW;  //Set to low so Statement is only printed once
    }
  } else {
    analogWrite(rgb_red, 0);  // turn LED OFF
    if (pirState == LOW) {

      Serial.println("Motion ended!");  // PIR sensor has stopped detecting

      pirState = HIGH;  //Set to high so statement is only printed once
    }
  }

  // alert
  if (currentMode == OFF) {
    analogWrite(rgb_red, 0);
    analogWrite(rgb_green, 0);
    analogWrite(rgb_blue, 0);
    noTone(pin_buzzer);
  } else if (currentMode == BOTH && val == HIGH && fsrreading > 200) {
    while (cycleLED_buzz()) {}
  } else if (currentMode == ONLY_LIGHTS && val == HIGH && fsrreading > 200) {
    while (cycleLED()) {}
  } else if (currentMode == ONLY_SOUND && val == HIGH && fsrreading > 200) {
    while(!button_pressed()) {
      tone(pin_buzzer, 440);
    } noTone(pin_buzzer);
  }
} // loop end

bool cycleLED() {
  if (colorphase == 0) { // green rising, red falling
      RED = 127;
      GREEN = 0;
      BLUE = 0;

      for (; !button_pressed() && RED > 0;) {
        analogWrite(rgb_red, RED); analogWrite(rgb_green, GREEN);
        RED--;
        GREEN++;
        if (custom_delay(50)) { // delay 5 ms
          return false; // button pressed --> exit function
        }
      }

      colorphase++;

      return true;

  } else if (colorphase == 1) { // blue rising, green falling
      RED = 0;
      GREEN = 127;
      BLUE = 0;

      for (; !button_pressed() && GREEN > 0;) {
        analogWrite(rgb_green, GREEN); analogWrite(rgb_blue, BLUE);
        GREEN--;
        BLUE++;
        if (custom_delay(50)) { // delay 5 ms
          return false; // button pressed --> exit function
        }
      }

      colorphase++;

      return true;

  } else if (colorphase == 2) { // red rising, blue falling
      RED = 0;
      GREEN = 0;
      BLUE = 127;

      for (; !button_pressed() && BLUE > 0;) {
        analogWrite(rgb_red, RED); analogWrite(rgb_blue, BLUE);
        RED++;
        BLUE--;
        if (custom_delay(50)) { // delay 5 ms
          return false; // button pressed --> exit function
        }
      }

      colorphase = 0;

      return true;

  } else {
    
    return false;

  }
}

bool soundBuzzer() { // rough version for now // WARNING: unused
  while (!button_pressed()) {
    tone(pin_buzzer, 440);
    custom_delay(500);
  } return false;
}

bool custom_delay(unsigned long delay) { // delays for a time while detecting button press
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

bool cycleLED_buzz() { // does not exit??
  tone(pin_buzzer, 440);
  if (colorphase == 0) { // green rising, red falling
      RED = 63;
      GREEN = 0;
      BLUE = 0;

      for (; !button_pressed() && RED > 0;) {
        analogWrite(rgb_red, RED); analogWrite(rgb_green, GREEN);
        RED--;
        GREEN++;
        if (custom_delay(50)) { // delay 5 ms
          noTone(pin_buzzer);
          analogWrite(rgb_red, 0); analogWrite(rgb_green, 0); analogWrite(rgb_blue, 0);
          return false; // button pressed --> exit function
        }
      }

      colorphase++;

      return true;

  } else if (colorphase == 1) { // blue rising, green falling
      RED = 0;
      GREEN = 63;
      BLUE = 0;

      for (; !button_pressed() && GREEN > 0;) {
        analogWrite(rgb_green, GREEN); analogWrite(rgb_blue, BLUE);
        GREEN--;
        BLUE++;
        if (custom_delay(50)) { // delay 5 ms
          noTone(pin_buzzer);
          analogWrite(rgb_red, 0); analogWrite(rgb_green, 0); analogWrite(rgb_blue, 0);
          return false; // button pressed --> exit function
        }
      }

      colorphase++;

      return true;

  } else if (colorphase == 2) { // red rising, blue falling
      RED = 0;
      GREEN = 0;
      BLUE = 63;

      for (; !button_pressed() && BLUE > 0;) {
        analogWrite(rgb_red, RED); analogWrite(rgb_blue, BLUE);
        RED++;
        BLUE--;
        if (custom_delay(50)) { // delay 5 ms
          noTone(pin_buzzer);
          analogWrite(rgb_red, 0); analogWrite(rgb_green, 0); analogWrite(rgb_blue, 0);
          return false; // button pressed --> exit function
        }
      }

      colorphase = 0;

      return true;

  } else {
    return false; // error?
  }
}
