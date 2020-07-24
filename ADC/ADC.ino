void setup() {
  Serial.begin(115200);

}

void loop() {
  Serial.println( analogRead(36)  ) ; // vp
  delay(1000) ;

}

// https://randomnerdtutorials.com/esp32-pinout-reference-gpios/
