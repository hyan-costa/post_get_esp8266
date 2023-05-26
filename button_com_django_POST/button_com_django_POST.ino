#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include "conexao_wifi.h"
#define SS_PIN D8
#define RST_PIN D3


//---------dados wifi----------
const char* ssid = SSID;
const char* password = SENHA;
//-----------------------------
const char* server_ip = SERVER_IP;

const byte BOTAO = D2;
const byte BUZZER = D0;

MFRC522 rfid(SS_PIN, RST_PIN); // Cria uma instância



void setup() {
  Serial.begin(115200);
  
  SPI.begin();
  rfid.PCD_Init();
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(BOTAO, INPUT_PULLUP);
  pinMode(BUZZER, 1);
    
  //-----------------------------------conexão_wifi--------------------------------------------------
  WiFi.begin(ssid,password);
  Serial.print("\n\n\nConectando");
  
  while(WiFi.status() != WL_CONNECTED){
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
  Serial.print("\nconectado! Endereco IP: ");
  Serial.print(WiFi.localIP());
  //-------------------------------------------------------------------------------------------------
  
}
void loop() {
  if (WiFi.status()== WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;
    String body;  //corpo da requisição http
    http.begin(client,"http://" SERVER_IP "/data/");
    http.addHeader("Content-Type", "application/json");

    //if(digitalRead(BOTAO) == 0){
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
      String uid = "";
      
      //get data card
      for (byte i = 0; i < rfid.uid.size; i++) {
        uid.concat(String(rfid.uid.uidByte[i], HEX));
      }

      
      digitalWrite(LED_BUILTIN, LOW);
      tone(BUZZER,1000);

      //serializacao
      StaticJsonDocument<200> json;
      json["uid"] = uid;
      String stringJson;
      serializeJson(json, stringJson);

      //post data
      int statusCode = http.POST(stringJson);

      //fim conection RFID
      rfid.PICC_HaltA();



      // STATUS 200?
      if (statusCode == HTTP_CODE_OK) {
        String response = http.getString();

        DynamicJsonDocument doc(1024);
        deserializeJson(doc, response);
  
        String results = doc["results"].as<String>();
        if(results){

          delay(500);
          tone(BUZZER,500);

          for(int i=0; i<5;i++){
            delay(200);
            digitalWrite(LED_BUILTIN,LOW);
            delay(200);
            digitalWrite(LED_BUILTIN,HIGH);
            
          }
          
          

          
        }
      }
 
    }
    else{
      
      digitalWrite(LED_BUILTIN,HIGH );
      noTone(BUZZER);
      
      }
    
    }//else
    
  //if
  else {
    Serial.println("WiFi is not connected");
  }//else

  
}//loop
