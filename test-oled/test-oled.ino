#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET 4			// necesario por la libreria pero no usado
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);	// crea objeto


void setup() {
  Wire.begin();					// inicializa bus I2C
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);	// inicializa pantalla con direccion 0x3C
  Serial.begin(9600);
}

void loop() {
  oled.clearDisplay();			// limpia pantalla
  oled.setTextColor(WHITE);		// establece color al unico disponible (pantalla monocromo)
  oled.setCursor(0, 0);			// ubica cursor en inicio de coordenadas 0,0
  oled.setTextSize(1);			// establece tamano de texto en 1
  oled.print("Timer: "); 	// escribe en pantalla el texto
  oled.print(millis() / 1000);		// escribe valor de millis() dividido por 1000
  oled.print(" seg."); 	// escribe en pantalla el texto
  oled.setCursor (10, 20);		// ubica cursor en coordenas 10,30
  oled.setTextSize(2);			// establece tamano de texto en 2
  oled.print("KarTrac");			// escribe texto
  oled.setTextSize(1.5);			// establece tamano de texto en 2
  oled.setCursor (10, 40);		// ubica cursor en coordenas 10,30
  oled.print("Status: OK");			// escribe texto
  oled.display();			// muestra en pantalla todo lo establecido anteriormente
}
 