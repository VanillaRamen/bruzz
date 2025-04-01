// user interface
#include <LiquidCrystal.h> 
LiquidCrystal lcd(13, // rs
                  12, // en
                  7,  // d4
                  6,  // d5
                  5,  // d6
                  4); // d7
// ground rw pin
#define BUTTON_UP 2 // Typical top button
#define BUTTON_DOWN 1 // Beta bottom button
// int currentMode = 0;
// String modes[4] = {"Both", "Only Lights", "Only Sound", "Off"}; // All the modes
enum DEVICE_MODE {BOTH, ONLY_LIGHTS, ONLY_SOUND, OFF};
String modes[4] {"Both", "Only Lights", "Only Sound", "Off"};
int currentMode = BOTH;
int lastUpState = HIGH; // Saves the state of the button, helpful to know if it was pressed
int lastDownState = HIGH;

// 8 pins


// weight
#define fsrpin A0
int fsrreading { 0 }; // from 0 to 1023
enum WEIGHT_DETECTED {NOT_DETECTED, DETECTED};
int objectDetected = NOT_DETECTED;
// one pin

// motion detection
int ledPin = (0); //Placeholder value for ledPin
// for motion detection LED, see RGB LED assignments
#define pin_pir 8
int pirState = LOW; // PIR     detection state
int val = 0; // Value of PIR Input
bool alertUser = false;
// 2 pins

// alert
// use currentMode for mode checking
#define pin_buzzer  11
#define rgb_red     10
#define rgb_green   9
#define rgb_blue    3
int RED { 127 }; // RGB VALS
int GREEN { 127 }; // initiated halfway through spectrum
int BLUE { 127 }; // to avoid darkness
int colorphase { 0 };
// 5 pins


// forward declarations
void updateLCD();


void setup() {
  while (!Serial) { // for debugging
    Serial.begin(9600);
  }

  // user interface
  lcd.begin(16, 2);
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  lcd.setCursor(0,0);
  updateLCD();

  // motion detection
  pinMode(pin_pir,INPUT); // Sets PIR to Input data

  // alerting
  pinMode(pin_buzzer, OUTPUT);
  pinMode(rgb_red, OUTPUT);
  pinMode(rgb_green, OUTPUT);
  pinMode(rgb_blue, OUTPUT);
}

void loop() {
  int upState   = digitalRead(BUTTON_UP);
  int downState = digitalRead(BUTTON_DOWN);
  // user interface
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
  // end user interface
 

  // weight detection
  // Read the FSR pin and store the outputs fsrreading:
  fsrreading = analogRead(fsrpin);
  Serial.print("Analog reading = ");
  // Print the fsrreading:
  Serial.print(fsrreading);
  // We can set some thresholds to display how much pressure is roughly applied:
  // TODO: use this value in an if() or while() as a threshold gate
  if (fsrreading > 200) {
    objectDetected = DETECTED;
  }
   else {
    objectDetected = NOT_DETECTED;
  }
  delay(1000); // problematic
  // end weight detection


  // motion detection
  val = digitalRead(pin_pir);  // read input value
  if (val == HIGH && objectDetected == DETECTED) {            // check if the input is HIGH
    // TODO: refactor for RGB LED
    alertUser = true;
    if (pirState == HIGH) {
      
      
      Serial.println("Motion detected!"); //PIR sensor is detecting
     
      pirState = LOW; //Set to low so Statement is only printed once
    }
  }
   else {
    // TODO: refactor for RGB LED
    // digitalWrite(ledPin, LOW); // turn LED OFF
    if (pirState == LOW){
      
      Serial.println("Motion ended!"); //PIR sensor has stopped detecting
      
      pirState = HIGH; //Set to high so statement is only printed once
    }
  }
  // end motion detection


  // alerting
  // TODO: refactor this entire thing
  // while (currentMode == ONLY_SOUND && objectDetected == DETECTED) { // switch is set to buzzer
  //   // TODO: change escape value
  //   if (mode != BUZZER) { // resets RGBLED values
  //     mode = BUZZER; // RGBLEDs stay lit without this
  //     analogWrite(6, 0);
  //     analogWrite(5, 0);
  //     analogWrite(3, 0);
  //   }
  //   tone(10, 440, 1000); // buzzer sounds (later on, hardcode "Animals" by Maroon 5 into a function)
  //   // TODO: if button pressed break;
  // }

  // while (currentMode == ONLY_LIGHTS) { // switch is set to LED
  //   // TODO: change escape value
  //   // TODO: if button pressed break;
  //   if (mode != RGBLED) {
  //     mode = RGBLED;
  //     noTone(10);
  //   }

  //   if (colorphase == 0) {
  //     RED = 255;
  //     GREEN = 0;
  //     BLUE = 0;
  //     for (; digitalRead(13) == LOW && RED > 0;) {
  //       // TODO: if button pressed break;
  //       analogWrite(6, RED); analogWrite(5, GREEN);
  //       RED--;
  //       GREEN++;
  //       delay(5);
  //     }
  //     colorphase++;
  //   } else if (colorphase == 1) {
  //     RED = 0;
  //     GREEN = 255;
  //     BLUE = 0;
  //     for (; digitalRead(13) == LOW && GREEN > 0;) {
  //       // TODO: if button pressed break;
  //       analogWrite(5, GREEN); analogWrite(3, BLUE);
  //       GREEN--;
  //       BLUE++;
  //       delay(5);
  //     }
  //     colorphase++;
  //   } else if (colorphase == 2) {
  //     RED = 0;
  //     GREEN = 0;
  //     BLUE = 255;
  //     for (; digitalRead(13) == LOW && BLUE > 0;) {
  //       // TODO: if button pressed break;
  //       analogWrite(6, RED); analogWrite(3, BLUE);
  //       RED++;
  //       BLUE--;
  //       delay(5);
  //     }
  //     colorphase = 0;
  //   }
  // }
  // // end alerting


} // end loop()

void updateLCD() {
  // Sets up the LCD to be used
  lcd.clear();
  lcd.print("Mode: ");
  lcd.print(modes[currentMode]);
}
