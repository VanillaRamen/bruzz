#define fsrpin A0
//Define variable to store sensor readings:
int fsrreading;  // Variable to store FSR


void setup() {
  // Begin serial communication at a baudrate of 9600:
  Serial.begin(9600);
}
void loop() {
  // Read the FSR pin and store the outputas fsrreading:
  fsrreading = analogRead(fsrpin);
  Serial.print("Analog reading = ");
  // Print the fsrreading:
  Serial.print(fsrreading);
  // We can set some threshholds to display how much pressure is roughly applied:
  if (fsrreading < 10) {
    Serial.println(" - No pressure");
  } else if (fsrreading < 200) {
    Serial.println(" - Light touch");
  } else if (fsrreading < 500) {
    Serial.println(" - Light squeeze");
  } else if (fsrreading < 800) {
    Serial.println(" - Medium squeeze");
  } else {
    Serial.println(" - Big squeeze");
  }
  delay(1000);
}
