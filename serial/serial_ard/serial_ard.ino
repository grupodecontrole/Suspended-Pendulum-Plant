// Init Variables

char inByte;       // holds byte received from the serial port
int nBytesBuffer;  // holds the number of bytes at serial buffer

// Define Pins

int ledPin = 13; // Onboard LED (D13)


void setup() 
{
  // Set digital pins' modes
  pinMode(ledPin, OUTPUT);

  digitalWrite(ledPin,LOW);

  // Init Serial Communication
  Serial.begin(9600, SERIAL_8N1);

}

void loop() 
{
  // Send byte
  Serial.println("TESTE"); //Serial.write('T'); //Serial.print('T');
  
  // Get number of available bytes at buffer (max = 64)
  nBytesBuffer = Serial.available();

  if(nBytesBuffer > 0)
  {
    // read first buffer's byte
    inByte = Serial.read();

    switch(inByte)
    {
      case 'H':
        digitalWrite(ledPin,HIGH);
        break;
      case 'L':
        digitalWrite(ledPin,LOW);
        break;
      default:
        // need do send other byte
        break;  
    }
  }

  // Wait 1s for next reading
  delay(5000);
}
