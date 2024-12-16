// Viral Science www.viralsciencecreativity.com www.youtube.com/c/viralscience
// Blynk IoT Smart Plant Monitoring System

/* Connections
Relay -> GPIO25
Button -> GPIO26
Soil -> GPIO34 (Analog)
PIR -> GPIO27
SDA -> GPIO21
SCL -> GPIO22
*/


#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME ""

#include <LiquidCrystal_I2C.h>
#include <DHT.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define BLYNK_PRINT Serial

LiquidCrystal_I2C lcd(0x3F, 16, 2);


char auth[] = "";  // Enter your Blynk Auth token
char ssid[] = "";  // Enter your Wi-Fi SSID
char pass[] = "";  // Enter your Wi-Fi Password

BlynkTimer timer;

DHT dht(2, DHT11);


#define SOIL_PIN A0      // GPIO34 for Soil Moisture Sensor (Analog) A0
#define IN1 14           // GPIO14 connected to IN1 on L298N D5
#define IN2 12           // GPIO12 connected to IN2 on L298N D6
#define ENA 15           // GPIO13 connected to ENA (PWM control) on L298N
#define PUSH_BUTTON_1 13 // D7 Button

const int trigPin = 16;
const int echoPin = 0;

// Reservoir height (in cm)
const float hauteurMaxReservoir = 100.0; // Adjust according to your reservoir

void checkPhysicalButton();
int motorState = LOW;
int pushButton1State = HIGH;
int soilMoistureThreshold = 30;

void setup() {

  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(PUSH_BUTTON_1, INPUT_PULLUP);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 0); // Ensure motor is off

  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  lcd.setCursor(0, 0);
  lcd.print("  Initializing  ");
  for (int a = 5; a <= 10; a++) {
    lcd.setCursor(a, 1);
    lcd.print(".");
    delay(500);
  }
  lcd.clear();
  lcd.setCursor(11, 1);
  lcd.print("W:OFF");

  // Call the functions periodically
  timer.setInterval(100L, soilMoistureSensor); // 1-second interval
  timer.setInterval(100L, DHT11sensor);
  timer.setInterval(500L, checkPhysicalButton);
  timer.setInterval(1000L, measureAndSendWaterLevel); // Ultrasonic every 2 seconds
}

void measureAndSendWaterLevel() {
  // Send ultrasonic pulse
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Read the echo response
  long duration = pulseIn(echoPin, HIGH);

  // Convert duration to distance (in cm)
  float distance = duration * 0.034 / 2;

  // Calculate water level in cm
  float niveauEau = hauteurMaxReservoir - distance;

  // Ensure the water level is within valid limits
  if (niveauEau < 0) {
    niveauEau = 0;
  } else if (niveauEau > hauteurMaxReservoir) {
    niveauEau = hauteurMaxReservoir;
  }

  // Calculate percentage of water level
  float pourcentageNiveau = (niveauEau / hauteurMaxReservoir) * 100;

  // Send data to Blynk and LCD
  Blynk.virtualWrite(V2, pourcentageNiveau);

  // Debugging
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.print(" cm, Water Level: ");
  Serial.print(niveauEau);
  Serial.print(" cm, Percentage: ");
  Serial.print(pourcentageNiveau);
  Serial.println(" %");
}

void DHT11sensor() {

  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);

  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(t);

  lcd.setCursor(8, 0);
  lcd.print("H:");
  lcd.print(h);

  Serial.print("T:");
  Serial.print(t);
  Serial.println("C");

  Serial.print("H:");
  Serial.print(h);
  Serial.println("h");

}

// Get the soil moisture values
void soilMoistureSensor() {
  int value = analogRead(SOIL_PIN);
  // value = map(value, 0, 4095, 0, 100); // Adjust for ESP32's ADC range
  value = map(value, 1023, 0, 0, 100);

  // Invert the value to match the soil moisture logic
  value = (value - 100) * -1;

  Blynk.virtualWrite(V3, value);
  lcd.setCursor(0, 1);
  lcd.print("S:");
  lcd.print(value);
  lcd.print("% ");

  Serial.print("Soil Moisture: ");
  Serial.print(value);
  Serial.println("%");

  // Automatically control the water pump based on soil moisture
  if (value < soilMoistureThreshold) {
    motorState = HIGH;
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 60); // Start motor
    lcd.setCursor(11, 1);
    lcd.print("W:ON ");
    Serial.println("Pump turned ON due to low soil moisture");
  } else if (value >= soilMoistureThreshold) {
    motorState = LOW;
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0); // Stop motor
    lcd.setCursor(11, 1);
    lcd.print("W:OFF");
    Serial.println("Pump turned OFF as soil moisture is sufficient");
  }
}

void checkPhysicalButton()
{
  if (digitalRead(PUSH_BUTTON_1) == LOW) {
    // Button is pressed, run the motor
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 60); // Start motor
    lcd.setCursor(11, 1);
    lcd.print("W:ON ");
    Serial.println("Pump running manually");
    Blynk.virtualWrite(V12, HIGH);
  } else {
    // Button released, stop the motor
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, 0); // Stop motor
    lcd.setCursor(11, 1);
    lcd.print("W:OFF");
    Serial.println("Pump stopped manually");
    Blynk.virtualWrite(V12, LOW);
  }
}

void loop() {

  Blynk.run(); // Run the Blynk library
  timer.run(); // Run the Blynk timer
}
