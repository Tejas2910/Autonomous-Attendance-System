#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "Tejas";
const char* password = "ijgm3932";

String course = "EE101";
String rollno = "180110050";
String apiURL = "http://api.pushingbox.com/pushingbox?devid=v7222E1819A1150F&course=";


void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  // Define PushingBox API URL and parameters
  // String apiURL = "https://api.pushingbox.com/pushingbox?devid=your_device_id&temperature=25&humidity=50";
  
  // Create HTTP client and send GET request
  
}

void loop() {
  apiURL = "http://api.pushingbox.com/pushingbox?devid=v7222E1819A1150F&course=";
  // do nothing
  while (Serial.available() == 0) {}     //wait for data available
  String teststr = Serial.readString();  //read until timeout
  teststr.trim();
  int i=0;
  for(i=0; i<10; i++){
    if(teststr[i] == ' '){
      break;      
    }
  }

  course = teststr.substring(0,i);
  rollno = teststr.substring(i+1, i+1+9);
  Serial.println(course);
  Serial.println(rollno);

  apiURL = apiURL + course + "&rollno=" + rollno;
  Serial.println(apiURL);

  HTTPClient http;
  WiFiClient client;
  http.begin(client, apiURL);
  int httpCode = http.GET();

  // Check for successful response
  if (httpCode > 0) {
    Serial.println("HTTP request successful");
    Serial.println(httpCode);
  } else {
    Serial.println("HTTP request failed");
  }

  // Disconnect HTTP client
  http.end();
}
