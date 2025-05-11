// control flow for this program relies
// heavily on if...else statements and
// while() loops.

#include <LiquidCrystal.h>
#define BUTTON_UP 2    // Typical top button
#define BUTTON_DOWN 1  // Beta bottom button
#define fsrpin A0

#define pin_buzzer  11
#define rgb_red     10
#define rgb_green   9
#define rgb_blue    3
int RED { 0 }; // RGB values
int GREEN { 0 };
int BLUE { 0 };
int colorphase { 0 }; // tracks which part of the spectrum is cycling
unsigned long recordedTime { 0 }; // time tracking (for use with millis())
enum DEVICE_MODE {BOTH, ONLY_LIGHTS, ONLY_SOUND, OFF};

LiquidCrystal lcd(13,  // rs
                  12,  // en
                  7,   // d4
                  6,   // d5
                  5,   // d6
                  4);  // d7 // LCD THE GOAT

// Initalizes pins for all components
int ledPin = 10;
int pirPin = 8;
int pirState = LOW;  //PIR              detection state
int pirVal = 0;         // Value of PIR Input

// Define variable to store sensor readings:
int fsrreading = { 0 };  // Variable to store FSR reading
bool objectDetected = false;
int currentMode = 0; // cycle through modes and use with below array for display
String modes[4] = { "Both", "Only Lights", "Only Sound", "Off" };  // All the modes (for display)

int upState = HIGH;
int downState = HIGH;
int lastUpState = HIGH; // Saves the state of the button, helpful to know if it was pressed
int lastDownState = HIGH;

void updateLCD() {
  // Sets up the LCD to be used
  // and updates it later
  lcd.clear();
  lcd.print("Mode: ");
  lcd.print(modes[currentMode]);
}

// forward function declarations
void updateLCD();                 // user interface
bool cycleLED();                  // alerting
bool cycleLED_buzz();             // alerting
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
    if (currentMode > 3) {
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

  // // lock it in off
  // currentMode = 3;

  // Read the FSR pin and store the outputas fsrreading:
  
  fsrreading = analogRead(fsrpin);
  // Serial.print("Analog reading = ");

  // debugging
  Serial.print("FSR Reading: ");
  Serial.println(fsrreading);

  // threshold
  if (fsrreading > 400) {
    objectDetected = true;
  } else {
    objectDetected = false;
  }

  pirVal = digitalRead(pirPin);     // read input value
  if (pirVal == HIGH) {             // check if the input is HIGH
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
  } else if (currentMode == BOTH && pirVal == HIGH && objectDetected) {
    while (cycleLED_buzz()) {}
  } else if (currentMode == ONLY_LIGHTS && pirVal == HIGH && objectDetected) {
    while (cycleLED()) {}
  } else if (currentMode == ONLY_SOUND && pirVal == HIGH && objectDetected) {
    // ONLY_SOUND mode handles buzzer in line without
    while(!button_pressed()) {
      tone(pin_buzzer, 440);
    } noTone(pin_buzzer);
  }
} // loop end


// function definitions
// cycles LED only but no buzzer
bool cycleLED() {
  if (colorphase == 0) { // green rising, red falling
      RED = 127;
      GREEN = 0;
      BLUE = 0;

      for (; RED > 0;) {
        analogWrite(rgb_red, RED); analogWrite(rgb_green, GREEN);
        RED--;
        GREEN++;
        if (custom_delay(25)) { // delay 5 ms
          analogWrite(rgb_red, 0); analogWrite(rgb_green, 0); analogWrite(rgb_blue, 0);
          return false; // button pressed --> exit function
        }
      }

      colorphase++;

      return true;

  } else if (colorphase == 1) { // blue rising, green falling
      RED = 0;
      GREEN = 127;
      BLUE = 0;

      for (; GREEN > 0;) {
        analogWrite(rgb_green, GREEN); analogWrite(rgb_blue, BLUE);
        GREEN--;
        BLUE++;
        if (custom_delay(25)) { // delay 5 ms
          analogWrite(rgb_red, 0); analogWrite(rgb_green, 0); analogWrite(rgb_blue, 0);
          return false; // button pressed --> exit function
        }
      }

      colorphase++;

      return true;

  } else if (colorphase == 2) { // red rising, blue falling
      RED = 0;
      GREEN = 0;
      BLUE = 127;

      for (; BLUE > 0;) {
        analogWrite(rgb_red, RED); analogWrite(rgb_blue, BLUE);
        RED++;
        BLUE--;
        if (custom_delay(25)) { // delay 5 ms
          analogWrite(rgb_red, 0); analogWrite(rgb_green, 0); analogWrite(rgb_blue, 0);
          return false; // button pressed --> exit function
        }
      }

      colorphase = 0;

      return true;

  } else {
    
    return false;

  }
}


// button-interruptible delay
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

// light function to check if either button has been pressed
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

// cycles LED and sounds buzzer
bool cycleLED_buzz() {
  tone(pin_buzzer, 440);
  if (colorphase == 0) { // green rising, red falling
      RED = 127;
      GREEN = 0;
      BLUE = 0;

      for (; RED > 0;) {
        analogWrite(rgb_red, RED); analogWrite(rgb_green, GREEN);
        RED--;
        GREEN++;
        if (custom_delay(25)) { // delay 5 ms
          noTone(pin_buzzer);
          analogWrite(rgb_red, 0); analogWrite(rgb_green, 0); analogWrite(rgb_blue, 0);
          return false; // button pressed --> exit function
        }
      }

      colorphase++;

      return true;

  } else if (colorphase == 1) { // blue rising, green falling
      RED = 0;
      GREEN = 127;
      BLUE = 0;

      for (; GREEN > 0;) {
        analogWrite(rgb_green, GREEN); analogWrite(rgb_blue, BLUE);
        GREEN--;
        BLUE++;
        if (custom_delay(25)) { // delay 5 ms
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
      BLUE = 127;

      for (; BLUE > 0;) {
        analogWrite(rgb_red, RED); analogWrite(rgb_blue, BLUE);
        RED++;
        BLUE--;
        if (custom_delay(25)) { // delay 5 ms
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
