//#include <Firebase_ESP_Client.h>
#include <FirebaseESP32.h>
// sensor api dan asap
int smoke =34 ;
int relay =4;// mendefinisikan pin yang digunakan adalah pin Digital
int Sensor_Api = 5;
//token
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
//wifi
#define WIFI_SSID "Andrian"
#define WIFI_PASSWORD "andrian123"
// to firebase
#define FIREBASE_HOST "monitoring-kebakaran-fce31-default-rtdb.firebaseio.com/"                          // the project name address from firebase id
#define FIREBASE_AUTH "caEuP9y3PSP4aktea2wVrtxAbxisIZNql8nrG7e5" 
#define API_KEY "AIzaSyCvGJ1eTR2aQXzUJATNF7B_gNQ4LuO-QQc"
#define DATABASE_URL "https://monitoring-kebakaran-fce31-default-rtdb.firebaseio.com/"

//data firebase
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;

//time send data
unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

void setup() {
Serial.begin(9600);
pinMode(smoke, INPUT); //mode pada pin D4 dijadikan sebagai input
pinMode(relay, OUTPUT);
pinMode (Sensor_Api,INPUT);

//wifi begin
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(300);
    }   
Serial.println();
Serial.print("Connected with IP: ");
Serial.println(WiFi.localIP());
Serial.println();

config.api_key = API_KEY;
config.database_url = DATABASE_URL;
    
    if (Firebase.signUp(&config, &auth, "", "")) {
      Serial.println("ok");
          signupOK = true;
        }
        else {
              Serial.printf("%s\n", config.signer.signupError.message.c_str());}
    
config.token_status_callback = tokenStatusCallback; 
Firebase.begin(&config, &auth);
Firebase.reconnectWiFi(true);

}

void loop() {
int Kadar_asap = analogRead(smoke); // membaca nilai analog
int Kondisi_api = digitalRead(Sensor_Api);

String Asap= (String)Kadar_asap;
String Api= (String)Kondisi_api;



if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1500 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    // menulis data suhu dari DHT11 ke firebase
    if (Firebase.RTDB.setString(&fbdo, "/KadarAsap",Asap )) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
      delay(2000);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
    count++;
    // menulis data kelembaban dari DHT11 ke firebase
    if (Firebase.RTDB.setString(&fbdo, "/KondisiApi", Api)) {
      Serial.println("PASSED");
      Serial.println("PATH: " + fbdo.dataPath());
      Serial.println("TYPE: " + fbdo.dataType());
      delay(2000);
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
}

if (Kondisi_api ==0){
  (Firebase.RTDB.setString(&fbdo, "/Kondisi"," Bahaya"));
     digitalWrite(relay,HIGH);
}
if (Kadar_asap > 400){
  (Firebase.RTDB.setString(&fbdo, "/Kondisi","Berasap"));
     digitalWrite(relay,HIGH);
}
else{
  (Firebase.RTDB.setString(&fbdo, "/Kondisi"," Normal"));
     digitalWrite(relay,LOW);
  }  
}
