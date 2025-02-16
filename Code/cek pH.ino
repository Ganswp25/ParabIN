
#define PHPin A0

//Setup PH
int buf[10];
float ph(float voltage) {
  return 7 + ((2.5 - voltage) / 0.18);
}
void setup() {
  Serial.begin(9600);

  pinMode(PHPin, INPUT);
}

void loop() {
  for (int i = 0; i < 10; i++) {
    buf[i] = analogRead(PHPin);
    delay(100);
  }

  float avgValue = 0;
  for (int i = 0; i < 10; i++)
    avgValue += buf[i];

  float pHVol = (float)avgValue * 5.0 / 4096 / 10;
  float pHValue =  -3.109* pHVol + 19.54; // hasil regresi linear pH
  
  Serial.print("ph:");
  Serial.println(pHValue);
  Serial.print("ph voltage:");
  Serial.println(pHVol);

  delay(1000);
}