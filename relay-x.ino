#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>

#define TFT_CS     5
#define TFT_RST    4
#define TFT_DC     2

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

#define TRIG_A 14
#define ECHO_A 27

#define TRIG_B 13
#define ECHO_B 12

#define RELAY_PIN 25

int peopleCount = 0;
unsigned long sensorATime = 0;
unsigned long sensorBTime = 0;
bool sensorATriggered = false;
bool sensorBTriggered = false;
bool countingEnabled = true;  

void setup() {
  Serial.begin(115200);

  pinMode(TRIG_A, OUTPUT);
  pinMode(ECHO_A, INPUT);
  pinMode(TRIG_B, OUTPUT);
  pinMode(ECHO_B, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(3); 
  tft.fillScreen(ST77XX_BLACK);
  updateDisplay();
}

long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); 
  long distance = duration * 0.034 / 2; 
  return distance;
}

void updateDisplay() {

  tft.fillScreen(ST77XX_BLACK);

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_YELLOW);
  tft.setCursor(10, 30);
  tft.print("People Counter");

  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(10, 70);
  tft.print("Count: ");
  tft.print(peopleCount);
}

void loop() {
  long distA = getDistance(TRIG_A, ECHO_A);
  long distB = getDistance(TRIG_B, ECHO_B);

  unsigned long currentTime = millis();

  if (distA > 2 && distA <= 100 && !sensorATriggered && countingEnabled) {
    sensorATriggered = true;
    sensorATime = currentTime;
    Serial.println("Sensor A triggered");
  }

  if (distB > 2 && distB <= 100 && !sensorBTriggered && countingEnabled) {
    sensorBTriggered = true;
    sensorBTime = currentTime;
    Serial.println("Sensor B triggered");
  }


  if (sensorATriggered && sensorBTriggered) {
    countingEnabled = false; 
    
    if (sensorATime < sensorBTime && (sensorBTime - sensorATime) < 2000) {
      peopleCount++;
      Serial.println("Person entered - Count increased");
      updateDisplay();
    }
 
     else if (sensorBTime < sensorATime && (sensorATime - sensorBTime) < 2000 && peopleCount > 0) {
      peopleCount--;
      Serial.println("Person exited - Count decreased");
      updateDisplay();
    }
     
    sensorATriggered = false;
    sensorBTriggered = false;
    
    delay(500);
    countingEnabled = true;
  }

  if (sensorATriggered && (currentTime - sensorATime > 3000)) {
    sensorATriggered = false;
    countingEnabled = true;
    Serial.println("Sensor A timeout");
  }
  
  if (sensorBTriggered && (currentTime - sensorBTime > 3000)) {
    sensorBTriggered = false;
    countingEnabled = true;
    Serial.println("Sensor B timeout");
  }

  digitalWrite(RELAY_PIN, peopleCount > 0 ? HIGH : LOW);

  delay(50); 
}