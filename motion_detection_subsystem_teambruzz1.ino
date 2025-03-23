//Initalizes pins for all components
int ledPin = 3;
int pirPin = 2;
int pirState = LOW; //PIR              detection state
int val = 0; // Value of PIR Input

void setup()
{
  pinMode(ledPin, OUTPUT); //Sets LED to Output
  pinMode(pirPin,INPUT); //Sets PIR to Input data
  
  Serial.begin(9600);
}

void loop()
{
  val = digitalRead(pirPin);  // read input value
  if (val == HIGH) {            // check if the input is HIGH
    digitalWrite(ledPin, HIGH);  // turn LED ON
    if (pirState == HIGH) {
      
      Serial.println("Motion detected!"); //PIR sensor is detecting
     
      pirState = LOW; //Set to low so Statement is only printed once
    }
  } else {
    digitalWrite(ledPin, LOW); // turn LED OFF
    if (pirState == LOW){
      
      Serial.println("Motion ended!"); //PIR sensor has stopped detecting
      
      pirState = HIGH; //Set to high so statement is only printed once
    }
  }
}