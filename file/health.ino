
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <HTTPSRedirect.h>



// Enter network credentials:
const char* ssid = "OmniWheels";
const char* password = "omniwheels";

// Enter Google Script Deployment ID:
const char *GScriptId = "AKfycbw59kZs-ZRmpEtpmHTn3qv6r66XSwL0fzsntsVCpmSvYA4jZ_bikqlQ_fTP99W89w-Ufg";

// Enter command (insert_row or append_row) and your Google Sheets sheet name (default is Sheet1):
String payload_base =  "{\"command\": \"insert_row\", \"sheet_name\": \"Sheet1\", \"values\": ";
String payload = "";

// Google Sheets setup (do not edit)
const char* host = "script.google.com";
const int httpsPort = 443;
const char* fingerprint = "";
String url = String("/macros/s/") + GScriptId + "/exec";
HTTPSRedirect* client = nullptr;

int adc;
boolean counter;
int count;
int nilai_bpm;
unsigned long millisBefore;
unsigned long beatTime = 20000;
const int threshold = 500;



void setup() {

  Serial.begin(9600);        
  counter = true;
  millisBefore = millis();
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);             
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  // Use HTTPSRedirect class to create a new TLS connection
  client = new HTTPSRedirect(httpsPort);
  client->setInsecure();
  client->setPrintResponseBody(true);
  client->setContentTypeHeader("application/json");
  
  Serial.print("Connecting to ");
  Serial.println(host);

  // Try to connect for a maximum of 5 times
  bool flag = false;
  for (int i=0; i<5; i++){ 
    int retval = client->connect(host, httpsPort);
    if (retval == 1){
       flag = true;
       Serial.println("Connected");
       break;
    }
    else
      Serial.println("Connection failed. Retrying...");
  }
  if (!flag){
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    return;
  }
  delete client;    // delete HTTPSRedirect object
  client = nullptr; // delete HTTPSRedirect object
}


void loop() {

  nilai_bpm = (count*(60000/beatTime));
  adc = analogRead(A0);
//  Serial.println(adc);

  delay(10);
  if((millis() - millisBefore) < beatTime) {
    if(counter == true){
      if(adc >= threshold){
        count++;
        counter = false;
        Serial.print("Beat: ");
        Serial.println(count);  
      }  
    }
    if(adc < threshold) {
      counter = true;
    }  
  }
  else{
    Serial.print(nilai_bpm);
    Serial.println("BPM");
    count = 0;
    millisBefore = millis();
    delay(60000);  
  }

  static bool flag = false;
  if (!flag){
    client = new HTTPSRedirect(httpsPort);
    client->setInsecure();
    flag = true;
    client->setPrintResponseBody(true);
    client->setContentTypeHeader("application/json");
  }
  if (client != nullptr){
    if (!client->connected()){
      client->connect(host, httpsPort);
    }
  }
  else{
    Serial.println("Error creating client object!");
  }
  
  // Create json object string to send to Google Sheets
  payload = payload_base + "\"" + nilai_bpm + "\"}";
  
  // Publish data to Google Sheets
  Serial.println("Publishing data...");
  Serial.println(payload);
  if(client->POST(url, host, payload)){ 
    // do stuff here if publish was successful
  }
  else{
    // do stuff here if publish was not successful
    Serial.println("Error while connecting");
  }

  // a delay of several seconds is required before publishing again    
  delay(5000);
}
