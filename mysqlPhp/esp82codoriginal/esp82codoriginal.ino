#include <ESP8266WiFi.h>
#include <WiFiClient.h> 

//-------------------VARIABLES GLOBALES--------------------------
int contconexion = 0;

const char *ssid = "----";
const char *password = "----";

unsigned long previousMillis = 0;

char host[48];  // 48 caracteres
String strhost = "192.168.1.115"; // el host es el dns o nombre d edominio o local host se sabe con ipconfig
String strurl = "/mysqlPhp/enviardatos.php"; // url del proyecto grabado en la carpeta xampp
String chipid = ""; // es un numero que identica al esp como el mac adress

//-------Función para Enviar Datos a la Base de Datos SQL--------

String enviardatos(String datos) {
  String linea = "error";
  WiFiClient client; // establece el cliente
  strhost.toCharArray(host, 49);
  if (!client.connect(host, 80)) { // conectar
    Serial.println("Fallo de conexion");
    return linea;
  }
  // si se conecta.. hace un http request con el metodo post
  client.print(String("POST ") + strurl + " HTTP/1.1" + "\r\n" + 
               "Host: " + strhost + "\r\n" +
               "Accept: */*" + "*\r\n" +
               "Content-Length: " + datos.length() + "\r\n" +
               "Content-Type: application/x-www-form-urlencoded" + "\r\n" +
               "\r\n" + datos);  //         
  delay(10);             
  
  Serial.print("Enviando datos a SQL...");
  
  unsigned long timeout = millis(); // conteo para saber si responde el servidor
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println("Cliente fuera de tiempo!");
      client.stop();
      return linea;
    }
  }
  // Lee todas las lineas que recibe del servidro y las imprime por la terminal serial
  while(client.available()){
    linea = client.readStringUntil('\r');
  }  
  Serial.println(linea);
  return linea;
}

//-------------------------------------------------------------------------

void setup() {

  // Inicia Serial
  Serial.begin(115200);
  Serial.println("");

  Serial.print("chipId: "); 
  chipid = String(ESP.getChipId());
  Serial.println(chipid); 

  // Conexión WIFI
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED and contconexion <50) { //Cuenta hasta 50 si no se puede conectar lo cancela
    ++contconexion;
    delay(500);
    Serial.print(".");
  }
  if (contconexion <50) {
      //para usar con ip fija
      IPAddress ip(192,168,1,156); 
      IPAddress gateway(192,168,1,1); 
      IPAddress subnet(255,255,255,0); 
      WiFi.config(ip, gateway, subnet); 
      
      Serial.println("");
      Serial.println("WiFi conectado");
      Serial.println(WiFi.localIP());
  }
  else { 
      Serial.println("");
      Serial.println("Error de conexion");
  }
}

//--------------------------LOOP--------------------------------
void loop() {

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= 10000) { //envia la temperatura cada 10 segundos
    previousMillis = currentMillis;
    int analog = analogRead(17);
    float temp = analog*0.322265625;
    Serial.println(temp);
    enviardatos("chipid=" + chipid + "&temperatura=" + String(temp, 2));
  }
}

// http://www.sinaptec.alomar.com.ar/2017/06/tutorial-6-esp8266-mysql-php-en.html
