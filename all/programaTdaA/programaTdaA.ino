#include <Wire.h>
//#include <TDA7449.h> 
#include "TDA7449.h" 
char vol=0;

void setup() {
 Wire.begin();
 tda(0,0);
 tda(1,0);
 Serial.begin(115200);
 Serial.println("<esp is ready>");
}

void loop() {
  Wire.beginTransmission(0x44); // comienza transmision
  Wire.write(0x00); // seleccion de entrada 
  Wire.write(0x03); // entrada 1
  Wire.endTransmission();
  Wire.beginTransmission(0x44); 
  Wire.write(0x01); // Ganancia de Entrada            
  Wire.write(0x01); // Inicia ganancia en 2dB            
  Wire.endTransmission();  
  int tecla;

 tecla = Serial.read(); // leo valor por monitor serial
 
 Serial.println(tecla) ;
 
 switch(tecla){
  
  if (tecla == 1){ 
    vol-- ;
    if(vol<=0) vol = 0 ;
    tda(1,vol);
    Serial.println("Bajo nivel");
    }else if ( tecla == 2 ){ 
      vol++;
      if(vol >= 48) vol = 48 ;
      tda(1,vol);
      Serial.println("Subio Nivel");
      }     
}

delay(100);

}


void tda(char op, char val){
  switch(op){
    case 0:
    Wire.beginTransmission(0x44); // comienza transmision
    Wire.write(0x00); // seleccion de entrada 
    Wire.write(0x03); // entrada 1
    Wire.endTransmission();
    Wire.beginTransmission(0x44); 
    Wire.write(0x01); // Ganancia de Entrada            
    Wire.write(0x01); // Inicia ganancia en 2dB            
    Wire.endTransmission();      
    Wire.beginTransmission(0x44); 
    Wire.write(0x06); // Atenuacion en R            
    Wire.write(0x00); // Inicia en 0dB            
    Wire.endTransmission();     
    Wire.beginTransmission(0x44); 
    Wire.write(0x07); // atenuacion en L            
    Wire.write(0x00); // Inicia en 0dB            
    Wire.endTransmission();
    break;  
    case 1:
    Wire.beginTransmission(0x44);
    Wire.write(0x02); // Control de Volumen
    val=48-val;
    if(val==48)val=0x3F; // Valor hexadecimal de 48
    Wire.write(val); // Se escribe el valor resultante
    Wire.endTransmission();
    break;       
    }
}
