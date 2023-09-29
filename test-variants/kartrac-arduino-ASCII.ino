#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiAvrI2c.h"
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <arduino-timer.h>

// ============== OLED ==============
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 4			// necesario por la libreria pero no usado

#define I2C_ADDRESS_OLED 0x3C
SSD1306AsciiAvrI2c oled;

// Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);	// crea objeto

// ============== Timer ==============
Timer<2> timer;
#define GYRO_INTERVAL 500
#define ACC_INTERVAL 200

// ============== Accelerometer ==============
Adafruit_MPU6050 mpu;
#define INTERVAL_SEND_VALUES 1000
#define IMPACT_THRESHOLD 2.0
unsigned long time = 0;

// Event
sensors_event_t a, g, temp;

#define GYRO_COUNT 2
#define ACC_COUNT 2
// gyro data
float gyro_data_x[GYRO_COUNT];
float gyro_data_y[GYRO_COUNT];
float gyro_data_z[GYRO_COUNT];
int gyro_index = 0;
// acc data
float acc_data_x[ACC_COUNT];
float acc_data_y[ACC_COUNT];
float acc_data_z[ACC_COUNT];
int acc_index = 0;


void setup()
{
  Serial.begin(9600);
  init_giroscope();

  oled.begin(&Adafruit128x64, I2C_ADDRESS_OLED);
  // if(){ 
  //   Serial.println(F("SSD1306 allocation failed :("));
  //   for (;;); // Don't proceed, loop forever
  // }
  // oled.display();
  delay(500);
  // Wire.begin();

  timer.every(GYRO_INTERVAL, save_gryo);
  timer.every(ACC_INTERVAL, save_acc);
  
  // size of mpu
  Serial.print(F("Size of mpu: "));
  Serial.println(sizeof(mpu));  

  // size of timer
  Serial.print(F("Size of timer: "));
  Serial.println(sizeof(timer));

  // size of oled
  Serial.print(F("Size of oled: "));
  Serial.println(sizeof(oled));
}

void loop()
{
  loop_accelerometer();
  timer.tick();
  loop_oled();


  
  // if (millis() >= time + INTERVAL_SEND_VALUES) {
  //   time += INTERVAL_SEND_VALUES;
  //   sendValues(false);
  // }

}

void loop_oled(){
  // oled.clearDisplay();			// limpia pantalla
  // oled.setTextColor(WHITE);		// establece color al unico disponible (pantalla monocromo)
  oled.setCursor(0, 0);			// ubica cursor en inicio de coordenadas 0,0
  // oled.setTextSize(1);			// establece tamano de texto en 1
  oled.print("Timer: "); 	// escribe en pantalla el texto
  oled.print(millis() / 1000);		// escribe valor de millis() dividido por 1000
  oled.print(" seg."); 	// escribe en pantalla el texto
  oled.setCursor (10, 20);		// ubica cursor en coordenas 10,30
  // oled.setTextSize(2);			// establece tamano de texto en 2
  oled.print("KarTrac");			// escribe texto
  // oled.setTextSize(1.5);			// establece tamano de texto en 2
  oled.setCursor (10, 40);		// ubica cursor en coordenas 10,30
  oled.print("Status: OK");			// escribe texto
  // oled.display();			// muestra en pantalla todo lo establecido anteriormente
}

void loop_accelerometer()
{
  mpu.getEvent(&a, &g, &temp);

  if (a.acceleration.y > IMPACT_THRESHOLD)
  {
    Serial.print(F("Impacto detectado! (Y:"));
    Serial.print(a.acceleration.y);
    Serial.println(F(")"));

    void * s;
    save_acc(s);
    save_gryo(s);
    sendValues(true);
    delay(1000);
  }

  delay(30);
}

bool save_acc(void *argument)
{
  acc_data_x[acc_index] = a.acceleration.x;
  acc_data_y[acc_index] = a.acceleration.y;
  acc_data_z[acc_index] = a.acceleration.z;
  acc_index++;
  if (acc_index == ACC_COUNT)
    acc_index = 0;
  return true;
}

bool save_gryo(void *argument)
{
  gyro_data_x[gyro_index] = g.gyro.x;
  gyro_data_y[gyro_index] = g.gyro.y;
  gyro_data_z[gyro_index] = g.gyro.z;
  gyro_index++;
  if (gyro_index == GYRO_COUNT)
    gyro_index = 0;
  return true;
}

void sendValues(bool impact)
{
  temperature();
  AccelerationX();
  AccelerationY();
  AccelerationZ();
  GyroX();
  GyroY();
  GyroZ();
  if (impact) Impact();

  delay(1000);
}

// ============ Temperature ============
void temperature()
{
  StaticJsonDocument<70> doc;
  JsonArray array;
  Serial.print(F("Sending temperature value..."));
  Serial.println(temp.temperature);
  doc["k"] = "tmp";
  doc["v"] = round2(temp.temperature);
  doSerializeJson(doc, doc.createNestedArray(), false);
}

// ============ Acceleration X ============
void AccelerationX()
{
  StaticJsonDocument<70> doc;
  JsonArray array;
  Serial.println(F("Sending acceleration X values..."));
  doc["k"] = "ax";
  array = doc.createNestedArray("v");
  for (size_t i = 0; i < ACC_COUNT; i++)
  {
    array.add(round2(acc_data_x[i]));
  }
  doSerializeJson(doc, array, true);
}

// ============ Acceleration Y ============
void AccelerationY()
{
  StaticJsonDocument<70> doc;
  JsonArray array;
  Serial.println(F("Sending acceleration Y values..."));
  doc["k"] = "ay";
  array = doc.createNestedArray("v");
  for (size_t i = 0; i < ACC_COUNT; i++)
  {
    array.add(round2(acc_data_y[i]));
  }
  doSerializeJson(doc, array, true);
}

// ============ Acceleration Z ============
void AccelerationZ()
{
  StaticJsonDocument<70> doc;
  JsonArray array;
  Serial.println(F("Sending acceleration Z values..."));
  doc["k"] = "az";
  array = doc.createNestedArray("v");
  for (size_t i = 0; i < ACC_COUNT; i++)
  {
    array.add(round2(acc_data_z[i]));
  }
  doSerializeJson(doc, array, true);
}

// ============ Gyro X ============
void GyroX()
{
  StaticJsonDocument<70> doc;
  JsonArray array;
  Serial.println(F("Sending gyro X values..."));
  doc["k"] = "gx";
  array = doc.createNestedArray("v");
  for (size_t i = 0; i < GYRO_COUNT; i++)
  {
    array.add(round2(gyro_data_x[i]));
  }
  doSerializeJson(doc, array, true);
}

// ============ Gyro Y ============
void GyroY()
{
  StaticJsonDocument<70> doc;
  JsonArray array;
  Serial.println(F("Sending gyro Y values..."));
  doc["k"] = "gy";
  array = doc.createNestedArray("v");
  for (size_t i = 0; i < GYRO_COUNT; i++)
  {
    array.add(round2(gyro_data_y[i]));
  }
  doSerializeJson(doc, array, true);
}

// ============ Gyro Z ============
void GyroZ()
{
  StaticJsonDocument<70> doc;
  JsonArray array;
  Serial.println(F("Sending gyro Z values..."));
  doc["k"] = "gz";
  array = doc.createNestedArray("v");
  for (size_t i = 0; i < GYRO_COUNT; i++)
  {
    array.add(round2(gyro_data_z[i]));
  }
  doSerializeJson(doc, array, true);
}

// ============ Impact ============
void Impact()
{
  StaticJsonDocument<70> doc;
  JsonArray array;

  Serial.println(F("Sending impact value..."));
  doc["k"] = "impact";
  doc["v"] = true;
  doc["timestamp"] = millis();
  doc["gyro_last_index"] = gyro_index - 1;
  doc["acc_last_index"] = acc_index - 1;
  doSerializeJson(doc, doc.createNestedArray(), false);
}

void doSerializeJson(DynamicJsonDocument doc, JsonArray toClearDoc, bool log)
{
  if (log == true)
  {
    Serial.print(F("Usage after acceleration values: "));
    Serial.println(doc.memoryUsage());
    Serial.print(F("ARRAY Usage after acceleration values: "));
    Serial.println(toClearDoc.memoryUsage());
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

float round2(float value)
{
  return (int)(value * 100 + 0.5) / 100.0;
}