/**
 *  Arduino / ESP8266-12E / NodeMCU Sample v1.0.20160618
 *  Source code can be found here: https://github.com/JZ-SmartThings/SmartThings/blob/master/Devices/Generic%20HTTP%20Device
 *  Copyright 2016 JZ
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 *  in compliance with the License. You may obtain a copy of the License at:
 *      http://www.apache.org/licenses/LICENSE-2.0
 *  Unless required by applicable law or agreed to in writing, software distributed under the License is distributed
 *  on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License
 *  for the specific language governing permissions and limitations under the License.
 */

#include <ESP8266WiFi.h>

const char* ssid = "WIFI_SSID";
const char* password = "WIFI_PASSWORD";

int relayPin1 = D1; // GPIO5 = D1
int relayPin2 = D2; // GPIO4 = D2
WiFiServer server(80);

void(* resetFunction) (void) = 0;

void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(relayPin1, OUTPUT);
  pinMode(relayPin2, OUTPUT);
  digitalWrite(relayPin1, HIGH);
  digitalWrite(relayPin2, HIGH);

  // Connect to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }
  Serial.println(client.readString());
  //client.readString();

  // Read the first line of the request
  String request = client.readStringUntil('\r');
  String fullrequest = client.readString();
  Serial.println(request);
  client.flush();

/*
  // BASIC AUTHENTICATION
  // The below Base64 string is gate:gate1 for the username:password
  if (fullrequest.indexOf("Authorization: Basic Z2F0ZTpnYXRlMQ==") == -1)  {
    client.println("HTTP/1.1 401 Access Denied");
    client.println("WWW-Authenticate: Basic realm=\"ESP8266\"");
      //client.println("Content-Type: text/html");
      //client.println(""); //  do not forget this one
      //client.println("Failed : Authentication Required!");
      //Serial.println(fullrequest);
    return;
  }
*/

  // Match the request
  if (request.indexOf("/RebootNow") != -1)  {
    resetFunction();
  }

  if (request.indexOf("RELAY1=ON") != -1 || request.indexOf("MainTriggerOn=") != -1)  {
    digitalWrite(relayPin1, LOW);
  }
  if (request.indexOf("RELAY1=OFF") != -1 || request.indexOf("MainTriggerOff=") != -1)  {
    digitalWrite(relayPin1, HIGH);
  }
  if (request.indexOf("RELAY1=MOMENTARY") != -1 || request.indexOf("MainTrigger=") != -1)  {
    digitalWrite(relayPin1, LOW);
    delay(100);
    digitalWrite(relayPin1, HIGH);
  }

  if (request.indexOf("RELAY2=ON") != -1 || request.indexOf("CustomTriggerOn=") != -1)  {
    digitalWrite(relayPin2, LOW);
  }
  if (request.indexOf("RELAY2=OFF") != -1 || request.indexOf("CustomTriggerOff=") != -1)  {
    digitalWrite(relayPin2, HIGH);
  }
  if (request.indexOf("RELAY2=MOMENTARY") != -1 || request.indexOf("CustomTrigger=") != -1)  {
    digitalWrite(relayPin2, LOW);
    delay(100);
    digitalWrite(relayPin2, HIGH);
  }

  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html><head><title>ESP8266 Dual 5V Relay</title></head><meta name=viewport content='width=500'><style type='text/css'>button {line-height: 2.2em; margin: 10px;} body {text-align:center;}");
  client.println("div {border:solid 1px; margin: 3px; width:150px;} .center { margin: auto; width: 300px; border: 3px solid #73AD21; padding: 10px;");
  client.println("</style></head>");

  String requestIn;
  requestIn = request;
  requestIn.replace("GET ", ""); requestIn.replace(" HTTP/1.1", "");
  client.println("<i>Current Request:</i><br><b>");
  client.println(requestIn);
  client.println("</b><hr>");

  client.print("<div class='center'>RELAY1 pin is now: ");
  if(digitalRead(relayPin1) == LOW) { client.print("On"); } else { client.print("Off"); }
  client.println("<br><a href=\"/RELAY1=ON\"><button>Turn On</button></a>");
  client.println("<a href=\"/RELAY1=OFF\"><button>Turn Off</button></a><br/>");  
  client.println("<a href=\"/RELAY1=MOMENTARY\"><button>MOMENTARY</button></a><br/></div>");  

  client.println("<hr>");
  client.print("<div class='center'>RELAY2 pin is now: ");
  if(digitalRead(relayPin2) == LOW) { client.print("On"); } else { client.print("Off"); }
  client.println("<br><a href=\"/RELAY2=ON\"><button>Turn On</button></a>");
  client.println("<a href=\"/RELAY2=OFF\"><button>Turn Off</button></a><br/>");  
  client.println("<a href=\"/RELAY2=MOMENTARY\"><button>MOMENTARY</button></a><br/></div>");  

  client.println("<hr><div class='center'><a target='_blank' href='https://community.smartthings.com/t/raspberry-pi-to-php-to-gpio-to-relay-to-gate-garage-trigger/43335'>Project on SmartThings Community</a></br>");
  client.println("<a target='_blank' href='https://github.com/JZ-SmartThings/SmartThings/tree/master/Devices/Generic%20HTTP%20Device'>Project on GitHub</a></br></div></html>");

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");
}