
// Proyecto Final //
//____________________________________//________________________
#include <driver/i2s.h>
#include <WiFi.h>
#include <Wire.h>


WiFiServer server(80);  // servidor web en puerto 80

const i2s_port_t I2S_PORT = I2S_NUM_0; // puerto i2s
const int BLOCK_SIZE = 1024;


// Credenciales de WiFi
const char* ssid     = "HUAWEI Mate 20"; // "Familia Nieto"
const char* password = "omendoza95"; //fnd19625   omendoza95

int limite = 55 ;  int dBCorreccion = -4 ;

// variables
int contconexion = 0; // contador de intento de conexion a red
String header; // Variable para guardar el HTTP request
String estadoBotonUno = "off";
String estadoBotonDos = "off";

const int ledPin = 5; // alumbra en el limite

char vol = 0;
char c = 0, caso = 0;
char dif = 0;
byte x = 0;

//------------------------CODIGO HTML------------------------------
String paginaInicio = "<!DOCTYPE html>"
"<html>"
"<head>"
"<meta charset='utf-8' />"
"<META HTTP-EQUIV='Refresh' CONTENT='3'>"
"<title> Limitador SPL ESP32 </title>"
"</head>"
"<body>"
"<center>"
"<h3> Limitador SPL ESP32 </h3>";

String Nivel = "<!DOCTYPE html>"
"<p> dB SPL:"
"</body> ";

String Atenuacion = "<!DOCTYPE html>"
"<p> dB Atenuados:"
"</body> ";

String botonUno = "<!DOCTYPE html>"
"<ahref='/onGain'><button> Gain </button> </a> "
"</body> ";

String botonDos = "<!DOCTYPE html>"
"<ahref='/onDim'><button> Dim  </button></a>"
"</body> ";



String paginaFin = "</center>"
"</body>"
"</html>";


//____________________________________//________________________

//


void setup() {
  Serial.begin(115200);
  Wire.begin();
  tda(0, 0); tda(1, 0); delay(20);
  
  // ____________________Configuracion I2s
  Serial.println("Configuring I2S...");
  esp_err_t err;
  

  pinMode (ledPin, OUTPUT);

  // The I2S config as per the example
  const i2s_config_t i2s_config = {
      .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX), // recepcion
      .sample_rate = 16000,                         // 16KHz
      .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT, // could only get it to work with 32bits
      .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT, // although the SEL config should be left, it seems to transmit on right
      .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,     // Interrupt level 1
      .dma_buf_count = 8,                           // numero de buffers
      .dma_buf_len = BLOCK_SIZE                     // muestras por buffer
      };

  // The pin config as per the setup
  const i2s_pin_config_t pin_config = {
      .bck_io_num = 14,   // BCKL 14  reloj d14
      .ws_io_num = 15,    // LRCL d15
      .data_out_num = -1, // no se usa
      .data_in_num = 32   // DOUT 32 datos d32
      };

  // configurar puertos i2s
  // funcion llamada en cualquier i2s
  err = i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  if (err != ESP_OK) {
    Serial.printf("Fallo instalacion de drive: %d\n", err);
    while (true);
    }
  
  err = i2s_set_pin(I2S_PORT, &pin_config);
  if (err != ESP_OK) { 
    Serial.printf("fallo ajuste de pin: %d\n", err);
    while (true);
    }
  
  Serial.println("I2S driver installed.");
  


   // ____________________Conexión WIFI
   WiFi.begin(ssid, password);
   while (WiFi.status() != WL_CONNECTED and contconexion <50) { 
    ++contconexion;
    delay(500);
    Serial.print(".");
    }
    if (contconexion <50) {
      //para usar con ip fija
//      IPAddress ip(172,17,22,196);  // casa : 192,168,1,103
//      IPAddress gateway(172,17,0,1); // router casa : 192,168,1,1
//      IPAddress subnet(255,255,224,0); // casa: 255,255,255,0
//      WiFi.config(ip, gateway, subnet); 
        
      Serial.println("");
      Serial.println("WiFi conectado");
      Serial.println(WiFi.localIP());
      server.begin(); // iniciamos el servidor
      }else { 
        Serial.println("");
        Serial.println("Error de conexion");
        }


}



//____________________________________//________________________
void loop() {
  Serial.println(WiFi.localIP()); // mostrar la ip del servidor para conectarme

  caso = Serial.read(); Serial.write(caso); delay(50);
  int dBSpl = SPL() ;
  int dBcorregido = correccionSPL(dBSpl) ;
  //___________________ wi _ fi________________
  WiFiClient client = server.available();   // Escucha a los clientes entrantes

  if (client) {                             // Si se conecta un nuevo cliente
    Serial.println("New Client.");          // 
    String currentLine = "";                //
    while (client.connected()) {            // loop mientras el cliente está conectado
      if (client.available()) {             // si hay bytes para leer desde el cliente
        char c = client.read();             // lee un byte
        Serial.write(c);                    // imprime ese byte en el monitor serial
        header += c;
        if (c == '\n') {                    // si el byte es un caracter de salto de linea
          // si la nueva linea está en blanco significa que es el fin del 
          // HTTP request del cliente, entonces respondemos:
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            // Muestra la página web
            //client.println( paginaInicio + Nivel + String( abs( dBSpl )  ) + Atenuacion + String( dBcorregido - abs( dBSpl )  ) + botonUno + botonDos + paginaFin);

            int BSPL = random(40, 85); // prueba con spl aleatorios pq se jodio el microfono     
            int BCorreccion = 55 - abs( BSPL ) ; // 55 es el limite se puede cambiar
            if( BCorreccion > 0){
              BCorreccion = 0 ;
              }

            if(BSPL >= limite){
              digitalWrite (ledPin, HIGH);  // encender led cuando se pasa el nivel limite
              delay(500);  // wait for half a second or 500 milliseconds
              digitalWrite (ledPin, LOW);  // turn off the LED
              delay(500);  // wait for half a second or 500 milliseconds
              }

            // Muestra la página web 
            client.println( paginaInicio + Nivel + String( abs( BSPL )  ) + Atenuacion + String( BCorreccion ) + botonUno + botonDos + paginaFin);


            // leer botones de el servidor web
            // boton gain
            if(header.indexOf("GET/onGain") >= 0){
              estadoBotonUno = "on";
              Serial.println("Ganancia activada");
              vol= vol + 5; if (vol > 48)vol = 48;
              Serial.print(" Ganancia ");
              tda(1, vol);
              delay(50);
              
              }
            // boton dim
            if(header.indexOf("GET/onDim") >= 0){
              estadoBotonUno = "on";
              Serial.println("Atenuacion  activada");
              vol--; if (vol <= 0) vol = 0;
              tda(1, vol);
              delay(50);
      
              }
              delay(80);

              
              
            // la respuesta HTTP temina con una linea en blanco
            client.println();
            break;
          } else { // si tenemos una nueva linea limpiamos currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // si C es distinto al caracter de retorno de carro
          currentLine += c;      // lo agrega al final de currentLine
        }
      }
    }
    // Limpiamos la variable header
    header = "";
    // Cerramos la conexión
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}



int SPL(){
  //___________________ SPL________________
  // leer multiples muestras y calculo de presion sonora
  int32_t samples[BLOCK_SIZE];
  int num_bytes_read = i2s_read_bytes(I2S_PORT, 
                                      (char *)samples, 
                                      BLOCK_SIZE,     // the doc says bytes, but its elements.
                                      portMAX_DELAY); // no timeout
  
  int samples_read = num_bytes_read / 8;
  if (samples_read > 0) {

    float mean = 0;
    for (int i = 0; i < samples_read; ++i) {
      mean += samples[i];
      }
    float ref = pow(20, -6) ; float presion_de_fondo = pow(2, -3)  ;
    float dBSpl = 20 * log10( mean ) ;
    //Serial.println( mean  );
    //dB = ( 94 * mean) / -189 ; // -34439169 del valor de calibracion
    int dB = ( 94 * mean) / 10000000000 ; // Serial.println( dB  );  //Serial.println( dB  ); // aca se imprimen lo d emic
    return dB ;
    }
  }

int correccionSPL(int decibelesMedidos){
   // aviso superacion limites en dB  de acuerdo a res 0627 65 dia 55 noche
  
   if (decibelesMedidos ==  limite){
    //Serial.println( "___________________ "  );
    int dBCorreccion  = limite - decibelesMedidos ; 
    //Serial.println( " Emision superior a 55dB SPL  se deben reducir " );Serial.println(dBCorreccion);
    //Serial.println( "___________________ "  );
    }
    return dBCorreccion;
  }


 //______
 void tda (char op, char var) {
  switch (op) {
    case 0:
      Wire.beginTransmission(0x44);
      Wire.write(0x00);
      Wire.write(0x03);
      Serial.print(". ");
      delay(200);
      Wire.endTransmission();
      Wire.beginTransmission(0x44);
      Wire.write(0x01);
      Wire.write(0x01);
      Serial.print(". . ");
      delay(400);
      Wire.endTransmission();
      Wire.beginTransmission(0x44);
      Wire.write(0x06);
      Wire.write(0x00);
      Serial.print(". . .  ");
      delay(200);
      Wire.endTransmission();
      Wire.beginTransmission(0x44);
      Wire.write(0x07);
      Wire.write(0x00);
      Serial.print(". . . . ");
      delay(200);
      Wire.endTransmission();
      break;
    case 1:
      Wire.beginTransmission(0x44);
      Wire.write(0x02);
      Serial.print(". . . . . ");
      delay(200);
      var = 48 - var;
      if (var == 48)var = 0x3F;
      Wire.write(var);
      Wire.endTransmission();
      break;

      }
   }
