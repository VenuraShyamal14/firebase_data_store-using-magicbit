
#include <WiFi.h>
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert your network credentials
#define WIFI_SSID "VenuraWifi"
#define WIFI_PASSWORD "notyabaya.com"

// Insert Firebase project API Key
#define API_KEY "AIzaSyDhjQiFCvXLotnVMuNR3u5Jiqsw2aKDyKA"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://notyabaya-iot-default-rtdb.asia-southeast1.firebasedatabase.app/" 

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

float pot_val = 0;
bool signupOK = false;

void setup(){
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")){
    Serial.println("ok");
    signupOK = true;
  }
  else{
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h
  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  pinMode(16,OUTPUT);
}

void loop(){


  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)){
    sendDataPrevMillis = millis();
      pot_val=analogRead(39);
      
    // Write an Int number on the database path test/int
    if (Firebase.RTDB.setInt(&fbdo, "test/pot_value", pot_val)){
      Serial.println("PASSED");
    }
    else {
      Serial.println("FAILED");
      Serial.println("REASON: " + fbdo.errorReason());
    }
  }
  if (Firebase.RTDB.getInt(&fbdo, "/test/led")) {
  if (fbdo.dataType() == "int") {
    int intValue = fbdo.intData();
    if (intValue==1){
      digitalWrite(16,HIGH);
      }
    else{
        digitalWrite(16,LOW);
      }
    Serial.println(intValue);
  }
}
else {
  Serial.println(fbdo.errorReason());
}
 
}
