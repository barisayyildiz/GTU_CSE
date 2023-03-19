String incomingByte;
int num;
int ledNumber = 13;


//function prototypes...
void ledOn();
void ledOff();
void blinkThreeTimes();

void setup() {
  pinMode(ledNumber, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  if (Serial.available() > 0) {
    incomingByte = Serial.readString();
    
      if(incomingByte == "1")
      {
        ledOn();
      }else if(incomingByte == "2")
      {
        ledOff();
      }else if(incomingByte == "3")
      {
        blinkThreeTimes();
      }else
      {
        num = incomingByte.toInt();
        Serial.println(num * num);
      }
      
    }
}

void ledOn()
{
  digitalWrite(ledNumber, HIGH);
}

void ledOff()
{
  digitalWrite(ledNumber, LOW);
}


void blinkThreeTimes()
{
  for(int i=0; i<3; i++)
  {
      digitalWrite(ledNumber, HIGH);
      delay(500);
      digitalWrite(ledNumber, LOW);
      delay(500);
    
  }
}
