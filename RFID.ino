//Importo las Librerias que necesito para El uso Del modulo WIFI y DEL RFID
#include <SPI.h> // LIBRERIA ESP8266
#include <MFRC522.h> // Libreria RFID
#include <ESP8266WiFi.h>  //Libreria Para el uso del WIFI
#include <WiFiClient.h> //Libreria para Usar como Cliente
#define SS_PIN D4 // define la coneccion al pin D4 del ESP
#define RST_PIN D2 // Defina la coneccion al PIN D2 del ESP
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Instancia MFRC522


//--------------Variables-------------------------------
String Id = "";
int statuss = 0; // Variable de estado
int out = 0; // Variable de Salida
int contconexion = 0; // Variable de Coneccion
int salida = 0;  // Variable para El led
byte nuidPICC[7];
char host[48]; // PUERTO HOST
String strhost = ""; // IP DEL SERVIDOR
String strurl = "EnviodeDatos.php"; // PHP DE COMUNICACION
String chipid = ""; // Nombre del CHIP
String url = "http://0.0.0.0/PAGINAARDUINO/EnviodeDatos.php";// URL DE COMUNICACION
//---------------- COnstantes para el WIFI 3 REDES MODIFICABLES---------------
//const char *ssid = "NEIDER"; // NOMBRE DE LA RED
//const char *password = ""; // CONTRASEÑA DE LA RED


const char *ssid = "";
const char *password = "";

//const char *ssid = ""; // NOMBRE DE LA RED
//const char *password = ""; // CONTRASEÑA DE LA RED

unsigned long previousMillis = 0;



//-------Función para Enviar Datos a la Base de Datos SQL--------

String enviardatos(String datos) {
  String linea = "error";
  WiFiClient client;
  strhost.toCharArray(host, 49);
  if (!client.connect(host, 80)) {
    Serial.println("Fallo de conexion");
    return linea;
  }

  client.print(String("POST ") + url + " HTTP/1.1" + "\r\n" +
               "Host: " + strhost + "\r\n" +
               "Accept: */*" + "*\r\n" +
               "Content-Length: " + datos.length() + "\r\n" +
               "Content-Type: application/x-www-form-urlencoded" + "\r\n" +
               "\r\n" + datos);
  delay(10);



  Serial.print("Enviando datos a SQL...");

  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println("Cliente fuera de tiempo!");
      client.stop();
      return linea;
    }

  }

  // Lee todas las lineas que recibe del servidor y las imprime por la terminal serial
  while (client.available()) {

    linea = client.readStringUntil('\r');
  }
  Serial.println("Recibiendo Respuesta del Servidor");
  Serial.println(linea);
  return linea;
}

//-------------------------------------------------------------------------


// Funcion para ver la tarjeta

void printArray(byte *buffer, byte bufferSize) {

  for (byte i = 0; i < bufferSize; i++) {
    Serial.println(buffer[i] < 0x10 ? " 0" : " ");
    Serial.println(buffer[i], HEX);
  }
}

//--------------TOMAR COMO STRING LA TARJETA
void array_to_string(byte array[], unsigned int len, char buffer[])
{
  for (unsigned int i = 0; i < len; i++)
  {
    byte nib1 = (array[i] >> 4) & 0x0F;
    byte nib2 = (array[i] >> 0) & 0x0F;
    buffer[i * 2 + 0] = nib1  < 0xA ? '0' + nib1  : 'A' + nib1  - 0xA;
    buffer[i * 2 + 1] = nib2  < 0xA ? '0' + nib2  : 'A' + nib2  - 0xA;
  }
  buffer[len * 2] = '\0';
}



//--------------------------------------FUNCION DE CONFIGURACION--------------------------------
void setup()
{
  Serial.begin(9600);   //Serial de Comunicacion, con el el puerto ****
  SPI.begin();      // SPI BUS Inicializacion del mismo
  mfrc522.PCD_Init();   // Iniciar MFRC522
  //-----------Coneccion WIFI---------------
  // Inicia Serial
  Serial.begin(115200);
  Serial.println("");

  Serial.print("chipId: ");
  chipid = String(ESP.getChipId());
  Serial.println(chipid);

  // Conexión WIFI
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED and contconexion < 50) { //Cuenta hasta 50 si no se puede conectar lo cancela
    ++contconexion;
    delay(500);
    Serial.print(".");
  }
  if (contconexion < 50) {
    //para usar con ip fija
    IPAddress ip(192, 168, 56, 1);
    IPAddress gateway(192, 168, 1, 1);
    IPAddress subnet(255, 255, 255, 0);
    WiFi.config(ip, gateway, subnet);

    Serial.println("");
    Serial.println("WiFi conectado");
    Serial.println(WiFi.localIP());
  }
  else {
    Serial.println("");
    Serial.println("Error de conexion");
  }
  //------------------FInal Wifi------------------
}


//-------------------------------------FUNCION LOOP------------------------------------------
void loop()
{

  // Detectar tarjeta
  if (mfrc522.PICC_IsNewCardPresent())
  {
    if (mfrc522.PICC_ReadCardSerial())
    {
      Serial.print(F("Card UID:"));
      // Guardamos EL ID de la TARJET EN UN BYTE
      for (byte i = 0; i < 8; i++) {
        nuidPICC[i] = mfrc522.uid.uidByte[i];
      }
      char str[32] = "";
      array_to_string(nuidPICC, 4, str);
      Id="";
      Id = String(Id + str);
      Serial.println(Id);
      if (Id == "5BB18EAF") {
        Serial.println(" Sesion Iniciada ");
        Serial.println(" Bienvenido Neider ");
        Id="";
        while (Id != "5BB18EAF") {
          // Detectar tarjeta
          if (mfrc522.PICC_IsNewCardPresent())
          {
            if (mfrc522.PICC_ReadCardSerial())
            {
              Serial.print(F("Card UID:"));
              // Guardamos EL ID de la Tarjeta EN UN BYTE
              for (byte i = 0; i < 8; i++) {
                nuidPICC[i] = mfrc522.uid.uidByte[i];
              }
              char str[32] = "";
              array_to_string(nuidPICC, 4, str);
              Id="";
              Id = String(Id + str);
              Serial.println(Id);
              int Asistencia=1;
              enviardatos("Identificacion="+chipid+"&RFID="+String(Id)+"&Asistencia="+Asistencia);
            } delay(1000);
          } else delay(500); 
          
            // Finalizar lectura actual
            mfrc522.PICC_HaltA();
        }
        Serial.println("Sesion Finalizada! ");
      }
        }
      }
      delay(250);

    }
    
