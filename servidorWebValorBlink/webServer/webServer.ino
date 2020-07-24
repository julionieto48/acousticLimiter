/*
 WiFi Web Server LED Blink

 A simple web server that lets you blink an LED via the web.
 This sketch will print the IP address of your WiFi Shield (once connected)
 to the Serial monitor. From there, you can open that address in a web browser
 to turn on and off the LED on pin 5.

 If the IP address of your shield is yourAddress:
 http://yourAddress/H turns the LED on
 http://yourAddress/L turns it off

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the Wifi.begin() call accordingly.

 Circuit:
 * WiFi shield attached
 * LED attached to pin 5

 created for arduino 25 Nov 2012
 by Tom Igoe

ported for sparkfun esp32 
31.01.2017 by Jan Hendrik Berlin
 
 */

 // encende led por puerto 2 

#include <WiFi.h>

const char* ssid     = "Familia Nieto "; // casa: Familia Nieto 
const char* password = "fnd19625"; // casa: fnd19625

char host[48];  // 48 caracteres
String strhost = "192,168,1,101"; // el host es el dns o nombre d edominio o local host se sabe con ipconfig
String strurl = "/mysqlPhp/enviardatos.php"; // url del proyecto grabado en la carpeta xampp
String chipid = ""; // es un numero que identica al esp como el mac adress

String header; // Variable para guardar el HTTP request

int contconexion = 0; // para el intento de conexiones

const int ledPin = 2 ;
int valorVariable = 0 ;
float timeToSave = 50 * 1000 ; // 5 seg 


WiFiServer server(80);

void setup()
{
    Serial.begin(115200);
    pinMode(ledPin, OUTPUT);      // set the LED pin mode yo lo puse en el 2

    delay(10);                    // delay antes de entrar a wifi

    // We start by connecting to a WiFi network

    Serial.println(); Serial.println();
    Serial.print("Conectando a .... "); Serial.println(ssid); // red wi fi disponible
    

    WiFi.begin(ssid, password);  // inicia el wi fi

    while (WiFi.status() != WL_CONNECTED and contConexion < 50) {  // stablecer conexxion wifi
        ++contConexion ;
        delay(500);
        Serial.print("conecectando a wifi ..... :)");
    }

    if ()(contconexion <50) {
      Serial.println("");
      Serial.println("conectado a red WI-Fi.");
      Serial.println("IP address: ");
      Serial.println(WiFi.localIP());

      //para usar con ip fija depende de la red
      IPAddress ip(192,168,1,101);  // el num de ip a poner en el navegador web
      IPAddress gateway(192,168,1,1); //
      IPAddress subnet(255,255,255,0); 
      WiFi.config(ip, gateway, subnet); 
    
      server.begin();  // arranca el servidor  
    } else { 
      Serial.println();
      Serial.println("Error de conexion");
      }
    //HTTPClient http;
    //http.begin("http//:192,168,1,101/servidorPotenciometro/datos.php");
    //http.addHeader("Content-Type",);

    

}



void loop(){

 valorVariable = 15 ;
 
 
 WiFiClient client = server.available();   // busca y escucha a los clientes

  if (client) {                             // si hay cliente...
    Serial.println("hay nuevo cliente .");   // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    
    while (client.connected()) {            // mientras el cliente este conectado
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then  leerr dato dle cliente
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");    // repsuesta del servidor
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header: del tutorial de led blink
            //client.print("Click <a href=\"/H\">here</a> to turn the LED on pin 2 on.<br>");
            //client.print("Click <a href=\"/L\">here</a> to turn the LED on pin 2 off.<br>");

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /H" or "GET /L":
        if (currentLine.endsWith("GET /H")) {
          digitalWrite(5, HIGH);               // GET /H turns the LED on
        }
        if (currentLine.endsWith("GET /L")) {
          digitalWrite(5, LOW);                // GET /L turns the LED off
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
