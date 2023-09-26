#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <arduino-timer.h>

// ============== Timer ==============
Timer<2> timer;
#define GYRO_INTERVAL 500
#define ACC_INTERVAL 200

// ============== Accelerometer ==============
Adafruit_MPU6050 mpu;
#define INTERVAL_SEND_VALUES 1000
#define IMPACT_THRESHOLD 15.0
unsigned long time = 0;

// Event
sensors_event_t a, g, temp;

#define GYRO_COUNT 5
#define ACC_COUNT 5
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

// ============== Impact sensor ==============
#define PIN_IMPACT 3

void setup()
{
  Serial.begin(9600);
  init_giroscope();
  pinMode(PIN_IMPACT, INPUT);

  timer.every(GYRO_INTERVAL, save_gryo);
  timer.every(ACC_INTERVAL, save_acc);
  Serial.print("Size of timer: ");
  Serial.println(sizeof(timer));
}

void loop()
{
  loop_accelerometer();
  timer.tick();
  // int impactoRaw = digitalRead(PIN_IMPACT);
  // if (impactoRaw == HIGH) {
  //   Serial.print("Impacto detectado!");
  //   sendValues(true);
  // }

  // if (millis() >= time + INTERVAL_SEND_VALUES) {
  //   time += INTERVAL_SEND_VALUES;
  //   sendValues(false);
  // }

  // loop_accelerometer();
  // loop_gyro();
}

void loop_accelerometer()
{
  mpu.getEvent(&a, &g, &temp);

  if (a.acceleration.y > IMPACT_THRESHOLD)
  {
    Serial.print("Impacto detectado! (Y:");
    Serial.print(a.acceleration.y);
    Serial.println(")");

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
  Serial.print("Sending temperature value...");
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
  Serial.println("Sending acceleration X values...");
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
  Serial.println("Sending acceleration Y values...");
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
  Serial.println("Sending acceleration Z values...");
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
  Serial.println("Sending gyro X values...");
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
  Serial.println("Sending gyro Y values...");
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
  Serial.println("Sending gyro Z values...");
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

  Serial.println("Sending impact value...");
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
    Serial.print("Usage after acceleration values: ");
    Serial.println(doc.memoryUsage());
    Serial.print("ARRAY Usage after acceleration values: ");
    Serial.println(toClearDoc.memoryUsage());
  }

  if (doc.overflowed())
  {
    Serial.println("JSON Overflowed!");
  }

  serializeJson(doc, Serial);
  Serial.println();
  Serial.println();
  doc.garbageCollect();
  doc.clear();
}

void init_giroscope()
{
  Serial.println("Adafruit MPU6050 test!");

  // Try to initialize!
  if (!mpu.begin())
  {
    Serial.println("Failed to find MPU6050 chip");
    while (1)
    {
      delay(10);
    }
  }
  Serial.println("MPU6050 Found!");

  mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  Serial.print("Accelerometer range set to: ");
  switch (mpu.getAccelerometerRange())
  {
  case MPU6050_RANGE_2_G:
    Serial.println("+-2G");
    break;
  case MPU6050_RANGE_4_G:
    Serial.println("+-4G");
    break;
  case MPU6050_RANGE_8_G:
    Serial.println("+-8G");
    break;
  case MPU6050_RANGE_16_G:
    Serial.println("+-16G");
    break;
  }
  mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  Serial.print("Gyro range set to: ");
  switch (mpu.getGyroRange())
  {
  case MPU6050_RANGE_250_DEG:
    Serial.println("+- 250 deg/s");
    break;
  case MPU6050_RANGE_500_DEG:
    Serial.println("+- 500 deg/s");
    break;
  case MPU6050_RANGE_1000_DEG:
    Serial.println("+- 1000 deg/s");
    break;
  case MPU6050_RANGE_2000_DEG:
    Serial.println("+- 2000 deg/s");
    break;
  }

  mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
  Serial.print("Filter bandwidth set to: ");
  switch (mpu.getFilterBandwidth())
  {
  case MPU6050_BAND_260_HZ:
    Serial.println("260 Hz");
    break;
  case MPU6050_BAND_184_HZ:
    Serial.println("184 Hz");
    break;
  case MPU6050_BAND_94_HZ:
    Serial.println("94 Hz");
    break;
  case MPU6050_BAND_44_HZ:
    Serial.println("44 Hz");
    break;
  case MPU6050_BAND_21_HZ:
    Serial.println("21 Hz");
    break;
  case MPU6050_BAND_10_HZ:
    Serial.println("10 Hz");
    break;
  case MPU6050_BAND_5_HZ:
    Serial.println("5 Hz");
    break;
  }

  Serial.println("");
  delay(100);
}

float round2(float value)
{
  return (int)(value * 100 + 0.5) / 100.0;
}