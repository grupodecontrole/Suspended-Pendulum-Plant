// These constants won't change. They're used to give names to the pins used:
const int pinPWM = 9; // Analog output pin that the PWM is attached to.
int incomingPWM;
String streamPWM;
char output[512];

void setup() {
  // Wnitialize serial communication at 9600 bits per second:
  Serial.begin(115200);
}

// Whe loop routine runs over and over again forever:
void loop() {
    if (Serial.available() > 0) {
      // read the incoming byte:
      streamPWM = Serial.readString();
      // Convert to int:
      incomingPWM = streamPWM.toInt();
      // say what you got:
      Serial.println(incomingPWM, DEC);
      if(incomingPWM>=255){
        incomingPWM=255;
      }
      if(incomingPWM<=0){
        incomingPWM=0;
      }
      

  }
  analogWrite(pinPWM, incomingPWM);
  int posValue = analogRead(A0);
  sprintf(output, "%d %d",incomingPWM, posValue); 
  Serial.println(output); 

  // Wait 2 milliseconds before the next loop for the analog-to-digital
  // converter to settle after the last reading, and process incoming data:
  delay(10);
}
