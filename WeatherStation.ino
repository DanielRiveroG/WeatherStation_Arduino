#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

float temperature;
float humidity;
float pressure;
float altitude;
float rainAmount;
int windSpeed;

volatile unsigned int windPulses = 0;
volatile unsigned int rainPulses = 0;

const byte windInterruptPin = 2;
const byte rainInterruptPin = 3;


Adafruit_BME280 bme; // I2C

void setup(void) {
    Serial.begin(9600);

    // Set bme280 I2C connection
    bool i2cConnectionStatus;
    
    // The I2C address of the sensor I use is 0x76
    i2cConnectionStatus = bme.begin(0x76);
    if (!i2cConnectionStatus) {
        Serial.println("Error. Check connections.");
        while (1);
    }

    // Set wind sensor interruptions
    pinMode(windInterruptPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(windInterruptPin), WindHandler, RISING);
    
    // Set rain sensor interruptions
    pinMode(rainInterruptPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(rainInterruptPin), RainHandler, RISING);
}

void loop() {
    delay(5000);

    cli();
    
    GetBmeData();
    CalculateWindSpeed();
    CalculateRainAmount();

    Serial.print("Temperatura: ");
    Serial.print(temperature);
    Serial.print(" Humedad: ");
    Serial.print(humidity);
    Serial.print(" Presión: ");
    Serial.println(pressure);
    Serial.print("Altitud: ");
    Serial.println(altitude);
    Serial.print("Viento: ");
    Serial.println(windSpeed);
    Serial.print("Lluvia: ");
    Serial.print(rainAmount);
    Serial.println();

    sei();
}

void GetBmeData(){
    temperature = bme.readTemperature();
    humidity = bme.readHumidity();
    pressure = bme.readPressure();
    pressure = pressure/100.0F;
}

void CalculateWindSpeed(){
    int anemometerTurns = windPulses/2;

    // Multiply turns by 0.4 if we check the speed every 5 seconds.
    windSpeed = anemometerTurns * 0.4;
}

void CalculateRainAmount(){
    // Each rain pulse means 272 mililiters per square meter.
    rainAmount = rainPulses * 0.272;
}

void WindHandler() {
  static unsigned long lastWindInterruptTime = 0;
  unsigned long interruptTime = millis();

  // Lock out perioid to avoid debounce.
  if (interruptTime - lastWindInterruptTime > 5)
  {
    ++windPulses;
  }

  lastWindInterruptTime = interruptTime;
}

void RainHandler() {
  static unsigned long lastRainInterruptTime = 0;
  unsigned long interruptTime = millis();

  // Lock out perioid to avoid debounce.
  if (interruptTime - lastRainInterruptTime > 5)
  {
    ++rainPulses;
  }
  lastRainInterruptTime = interruptTime;
}