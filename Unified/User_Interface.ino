#include <LiquidCrystal.h> 
#define BUTTON_UP 2 // Typical top button
#define BUTTON_DOWN 1 // Beta bottom button

LiquidCrystal lcd(13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3); // LCD THE GOAT

int currentMode = 0;
String modes[4] = {"Both", "Only Lights", "Only Sound", "Off"}; // All the modes 

int lastUpState = HIGH; // Saves the state of the button, helpful to know if it was pressed
int lastDownState = HIGH; // Ditto

void updateLCD() {
  // Sets up the LCD to be used
  lcd.clear();
  lcd.print("Mode: ");
  lcd.print(modes[currentMode]); 
}

void setup() { 
  // Classic set up, really just pins
  lcd.begin(16,2); 
  
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  
  lcd.setCursor(0,0);
  updateLCD();
} 


void loop() {
  int upState   = digitalRead(BUTTON_UP);
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
	
  //Updates the mode going down
  if (downState == HIGH && lastDownState == LOW) { 
    currentMode--;
    if (currentMode < 0) {
      currentMode = 3;
    }
    updateLCD();
  }
  lastDownState = downState;
}
