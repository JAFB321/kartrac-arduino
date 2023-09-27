#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>

// ============== OLED ==============
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 4     // necesario por la libreria pero no usado

#define I2C_ADDRESS_OLED 0x3C
SSD1306AsciiAvrI2c oled;

// Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);	// crea objeto

// ============== Accelerometer ==============
Adafruit_MPU6050 mpu;
#define IMPACT_THRESHOLD 15.0

void setup()
{
  Serial.begin(115200);
  init_oled();
  init_giroscope();

  delay(500);
  // Wire.begin();

  // size of mpu
  Serial.print(F("Size of mpu: "));
  Serial.println(sizeof(mpu));

  // size of oled
  Serial.print(F("Size of oled: "));
  Serial.println(sizeof(oled));
}

void loop()
{
  loop_accelerometer();
  // loop_oled();
}

void loop_oled()
{
  // oled.clearDisplay();			// limpia pantalla
  // oled.setTextColor(WHITE);		// establece color al unico disponible (pantalla monocromo)
  oled.setCursor(0, 0); // ubica cursor en inicio de coordenadas 0,0
  // oled.setTextSize(1);			// establece tamano de texto en 1
  oled.print("Timer: ");       // escribe en pantalla el texto
  oled.print(millis() / 1000); // escribe valor de millis() dividido por 1000
  oled.print(" seg.");         // escribe en pantalla el texto
  oled.setCursor(10, 20);      // ubica cursor en coordenas 10,30
  // oled.setTextSize(2);			// establece tamano de texto en 2
  oled.print("KarTrac"); // escribe texto
  // oled.setTextSize(1.5);			// establece tamano de texto en 2
  oled.setCursor(10, 40);   // ubica cursor en coordenas 10,30
  oled.print("Status: OK"); // escribe texto
  // oled.display();			// muestra en pantalla todo lo establecido anteriormente
}

void loop_accelerometer()
{
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  if (a.acceleration.y > IMPACT_THRESHOLD)
  {
    Serial.print(F("Impacto detectado! (Y:"));
    Serial.print(a.acceleration.y);
    Serial.println(F(")"));

    // Print
    print_impact(a, g, temp);

    // Temperature
    sendFloat("temp", temp.temperature);
    // Acceleration
    sendFloat("ax", a.acceleration.x);
    sendFloat("ay", a.acceleration.y);
    sendFloat("az", a.acceleration.z);
    // Gyro
    sendFloat("gx", g.gyro.x);
    sendFloat("gy", g.gyro.y);
    sendFloat("gz", g.gyro.z);
    // Impact
    Impact();

    delay(1000);
  }

  delay(30);
}

void sendFloat(char *key, float val)
{
  StaticJsonDocument<70> doc;
  Serial.println(F("Sending values..."));
  doc["key"] = key;
  doc["val"] = round2(val);
  doSerializeJson(doc, true);
}

void Impact()
{
  StaticJsonDocument<70> doc;
  Serial.println(F("Sending impact value..."));
  doc["key"] = "impact";
  doc["val"] = true;
  doc["timestamp"] = millis();
  doSerializeJson(doc, false);
}

void doSerializeJson(DynamicJsonDocument doc, bool log)
{
  if (log == true)
  {
    Serial.print(F("Usage after acceleration values: "));
    Serial.println(doc.memoryUsage());
  }

  if (doc.overflowed())
  {
    Serial.println(F("JSON Overflowed!"));
  }

  serializeJson(doc, Serial);
  Serial.println();
  Serial.println();
  doc.garbageCollect();
  doc.clear();
}

void init_giroscope()
{
  Serial.println(F("Adafruit MPU6050 test!"));

  // Try to initialize!
  if (!mpu.begin())
  {
    Serial.println(F("Failed to find MPU6050 chip"));
    while (1)
    {
      delay(10);
    }
  }
  Serial.println(F("MPU6050 Found!"));

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print(F("Accelerometer range set to: "));
  switch (mpu.getAccelerometerRange())
  {
  case MPU6050_RANGE_2_G:
    Serial.println(F("+-2G"));
    break;
  case MPU6050_RANGE_4_G:
    Serial.println(F("+-4G"));
    break;
  case MPU6050_RANGE_8_G:
    Serial.println(F("+-8G"));
    break;
  case MPU6050_RANGE_16_G:
    Serial.println(F("+-16G"));
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print(F("Gyro range set to: "));
  switch (mpu.getGyroRange())
  {
  case MPU6050_RANGE_250_DEG:
    Serial.println(F("+- 250 deg/s"));
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println(F("+- 500 deg/s"));
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println(F("+- 1000 deg/s"));
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println(F("+- 2000 deg/s"));
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print(F("Filter bandwidth set to: "));
  switch (mpu.getFilterBandwidth())
  {
  case MPU6050_BAND_260_HZ:
    Serial.println(F("260 Hz"));
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println(F("184 Hz"));
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println(F("94 Hz"));
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println(F("44 Hz"));
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println(F("21 Hz"));
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println(F("10 Hz"));
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println(F("5 Hz"));
    break;
  }

  Serial.println(F(""));
  delay(100);
}

void init_oled(){
  oled.begin(&Adafruit128x64, I2C_ADDRESS_OLED);
  oled.setFont(Adafruit5x7);
  print_welcome();
}

void print_welcome(){
  oled.clear();
  oled.set2X();
  oled.println("Kartrac");
  oled.set1X();
  oled.println("");
  oled.println("Estatus: OK");
  oled.println();
}

void print_impact(sensors_event_t a, sensors_event_t g, sensors_event_t temp){
  oled.clear();
  oled.set2X();
  oled.println("Kartrac");
  oled.set1X();
  oled.println();
  oled.println("Estatus: ACCIDENTE!");
  oled.println();
  oled.print("Temperatura: ");
  oled.print(temp.temperature);
  oled.println(" C");
  oled.println();
  oled.print("Impacto a: ");
  oled.print(round2(a.acceleration.y));
  oled.println(" m/s2");
  // if()
}


float round2(float value)
{
  return (int)(value * 100 + 0.5) / 100.0;
}