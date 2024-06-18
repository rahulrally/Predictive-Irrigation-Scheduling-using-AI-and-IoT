#include "FirebaseESP8266.h"	
#include <ESP8266WiFi.h>
#include <DHT.h>		


#define FIREBASE_HOST "soil-cbe22-default-rtdb.firebaseio.com/" 
#define FIREBASE_AUTH "zQRtX61sDCIDrlnCI6rNhApHU0wmJ81DvoYkLUVW"

#define WIFI_SSID "Rahul"
#define WIFI_PASSWORD "hotspotrahul"

#define DHTPIN D1		
#define LED D4
#define Motor D5			
#define Moisture D6
float Moisture_String;
float Pre_T,Pre_H;
float Curr_Soil=0.0, Pre_Soil=2.0;
String Curr_Motor="0", Pre_Motor="2";
boolean DHT_Status = 1;
#define DHTTYPE    DHT11
DHT dht(DHTPIN, DHTTYPE);


FirebaseData firebaseData;
FirebaseData motorData;
FirebaseData modeData;
FirebaseJson json;
int motormode =0;

void setup()
{

  Serial.begin(9600);

  dht.begin();
  pinMode(Motor,OUTPUT);
  pinMode(LED,OUTPUT);
  pinMode(Moisture,INPUT_PULLUP);
 
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(LED,HIGH);
    Serial.print(".");
    delay(50);
    digitalWrite(LED,LOW);
    delay(50);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  if (Firebase.setString(modeData, "/SoilProjectAuto/mode", "0"))
  {
    Serial.println("Mode PASSED");
  }
  if (Firebase.setString(motorData, "/SoilProjectAuto/motor", "0"))
  {
    Serial.println("Motor PASSED");
  }
}

void sensorUpdate(){
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if(DHT_Status==0)
  {
    digitalWrite(LED,HIGH);
    Serial.println("Check DHT Sensor Wire Connection");
    delay(500);
    digitalWrite(LED,LOW);
    delay(500);
    Pre_Motor=2;
  }
  if (isnan(h) || isnan(t)) 
 {
   
    DHT_Status=0;
    return;
  }
   DHT_Status=1;

  Serial.print(F("Humidity: "));  
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C"));
  
  if(digitalRead(Moisture)==LOW)
  Moisture_String=1;
  else 
  Moisture_String=0;


  
 // if(!(Curr_Soil==Pre_Soil))
  {
    if (Firebase.setFloat(firebaseData, "/SoilProjectAuto/soil", Moisture_String))
  {
    Serial.println("Soil PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("Soil FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
  Pre_Soil=Curr_Soil;
  }
  
  
 Curr_Soil=digitalRead(Moisture);
Serial.println(Curr_Soil);

  

 
  

  if(!(t==Pre_T))
  {

  if (Firebase.setFloat(firebaseData, "/SoilProjectAuto/temperature", t))
  {
    Serial.println("Temperature PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("Temperature FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
   Pre_T=t;
  }

   if(!(h==Pre_H))
  {
  if (Firebase.setFloat(firebaseData, "/SoilProjectAuto/humidity", h))
  {
    Serial.println("Humidity PASSED");
    Serial.println("PATH: " + firebaseData.dataPath());
    Serial.println("TYPE: " + firebaseData.dataType());
    Serial.println("ETag: " + firebaseData.ETag());
    Serial.println("------------------------------------");
    Serial.println();
  }
  else
  {
    Serial.println("Humidity FAILED");
    Serial.println("REASON: " + firebaseData.errorReason());
    Serial.println("------------------------------------");
    Serial.println();
  }
   Pre_H=h;
  }
  
 if (Firebase.getString(modeData, "/SoilProjectAuto/mode")){
    Serial.print("Mode Status:");
    Serial.println(modeData.stringData());
    if (modeData.stringData() == "1") {
    motormode=1;
    }
  else if (modeData.stringData() == "0"){
    motormode=0;
    }
  }
  

if(motormode==1)
{
  Firebase.getString(motorData, "/SoilProjectAuto/motor");
 Curr_Motor=motorData.stringData();

 
  if (Firebase.getString(motorData, "/SoilProjectAuto/motor")){
    Serial.print("Motor Status:");
    Serial.println(motorData.stringData());
    if (motorData.stringData() == "1") {
    digitalWrite(Motor, HIGH);
    }
  else if (motorData.stringData() == "0"){
    digitalWrite(Motor, LOW);
    }
  }
}
else
{
  if(Curr_Soil==1)
  {
     if (Firebase.setString(motorData, "/SoilProjectAuto/motor", "1"))
  {
    Serial.println("Motor ON");
    digitalWrite(Motor, HIGH);
  }
  }
  else
  {
    if (Firebase.setString(motorData, "/SoilProjectAuto/motor", "0"))
  {
    Serial.println("Motor OFf");
    digitalWrite(Motor, LOW);
  }
  }
}


    Firebase.getString(modeData, "/SoilProjectAuto/mode");


 
 
 
}



void loop() {
  sensorUpdate();

}
