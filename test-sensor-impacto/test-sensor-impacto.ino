
const int PIN_IMPACT = 3;

void setup() {
  Serial.begin(9600);
  pinMode(PIN_IMPACT, INPUT);
}

void loop() {
  int valor = digitalRead(PIN_IMPACT);

  if (valor == HIGH) {      
    Serial.print("Idle ");
    Serial.println(valor);
  } else {  
    Serial.print("Hit detected ");
    Serial.println(valor);
    delay(2000);
  }

}
