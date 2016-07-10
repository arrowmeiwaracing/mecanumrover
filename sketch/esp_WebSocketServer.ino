/*
  Copyright (c) 2016 arrowmeiwaracing
  Released under the MIT license
  https://github.com/arrowmeiwaracing/mecanumrover/blob/master/LICENSE
*/

#include <Arduino.h>

#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Hash.h>
#include <FS.h>

#include <ArduinoJson.h>

//#define DEBUG

#define BUFFER_SIZE 8192

const int M1I1[] = {16, 1, 4, 12}; //0:LF 1:LR 2:RR 3:RF
const int M1I2[] = {5, 3, 14, 13};

const char* path_root   = "/index.html";
const char* path_confW  = "/wificonfig.txt";
uint8_t htmlP[BUFFER_SIZE];
uint8_t confW[BUFFER_SIZE];
const char* ssid;
const char* pass;

ESP8266WiFiMulti WiFiMulti;

ESP8266WebServer server = ESP8266WebServer(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void getWifiSsttings() {
  SPIFFS.begin();
  File confFile = SPIFFS.open(path_confW, "r");
  if (!confFile) {
#ifdef DEBUG
    Serial.println("error! wificonfig.txt not found");
#endif
  }
  size_t size = confFile.size();
  confFile.read(confW, size);
  confFile.close();
  
  StaticJsonBuffer<200> jsonBuffer;
  JsonObject &root = jsonBuffer.parseObject((char *)confW);
  if (!root.success()) {
#ifdef DEBUG
    Serial.println("wificonf parseObject() failed");
#endif
  }
  ssid = root["ssid"];
  pass = root["password"];
#ifdef DEBUG
  Serial.printf("OK ssid=%s pass=%s\n", ssid, pass);
#endif
}

void webPageReadBuf() {
  SPIFFS.begin();
  File htmlFile = SPIFFS.open(path_root, "r");
  if (!htmlFile) {
#ifdef DEBUG
    Serial.println("error! index.html not found");
#endif
  }
  size_t size = htmlFile.size();
  htmlFile.read(htmlP, size);
  htmlFile.close();
#ifdef DEBUG
  Serial.printf("OK index.html size=%s\n", size);
#endif
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t lenght) {
  switch (type) {
    case WStype_DISCONNECTED:
#ifdef DEBUG
      Serial.printf("[%u] Disconnected!\n", num);
#endif
      break;
    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
#ifdef DEBUG
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
#endif
        // send message to client
        webSocket.sendTXT(num, "Connected");
      }
      break;
    case WStype_TEXT:
#ifdef DEBUG
      Serial.printf("[%u] get Text: %s\n", num, payload);
#endif
      if (payload[0] == '#') {
        String st1 = (const char *) &payload[0];
        int ix1 = st1.indexOf(':');
        int si1 = st1.substring(2, 3).toInt() - 1; // #c1:15
        int st2 = st1.substring(ix1 + 1).toInt();
        if (st2 >= 15) {
          analogWrite(M1I1[si1], ((st2 - 15) * 68)); // 68 is 1020(pwm 0-1023)
          analogWrite(M1I2[si1], 0);
        } else {
          analogWrite(M1I1[si1], 0);
          analogWrite(M1I2[si1], ((15 - st2) * 68));
        }
#ifdef DEBUG
        Serial.printf("debug=%d: %d\n", si1, st2);
#endif
      }
      break;
  }
}

void setup() {
#ifdef DEBUG
  Serial.begin(115200);
  //Serial.setDebugOutput(true);
  Serial.println();
  Serial.println();
  Serial.println();
#endif

  for (int i = 0; i < 4; i++) {
    analogWrite(M1I1[i], 0); //STOP 0-0
    analogWrite(M1I2[i], 0);
  }
  webPageReadBuf(); //html file to memBuf
  
  getWifiSsttings(); //WIFI Setting
  WiFiMulti.addAP(ssid, pass);

  while (WiFiMulti.run() != WL_CONNECTED) {
    delay(100);
  }

  // ota-----
  ArduinoOTA.setPort(8266);
  ArduinoOTA.setHostname("esp8266");
  ArduinoOTA.onStart([]() {
#ifdef DEBUG
    Serial.println("Start");
#endif
  });
  ArduinoOTA.onEnd([]() {
#ifdef DEBUG
    Serial.println("\nEnd");
#endif
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
#ifdef DEBUG
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
#endif
  });
  ArduinoOTA.onError([](ota_error_t error) {
#ifdef DEBUG
    Serial.printf("Error[%u]: ", error);
#endif
    if (error == OTA_AUTH_ERROR) {
#ifdef DEBUG
      Serial.println("Auth Failed");
#endif
    } else if (error == OTA_BEGIN_ERROR) {
#ifdef DEBUG
      Serial.println("Begin Failed");
#endif
    } else if (error == OTA_CONNECT_ERROR) {
#ifdef DEBUG
      Serial.println("Connect Failed");
#endif
    } else if (error == OTA_RECEIVE_ERROR) {
#ifdef DEBUG
      Serial.println("Receive Failed");
#endif
    } else if (error == OTA_END_ERROR) {
#ifdef DEBUG
      Serial.println("End Failed");
#endif
    }
  });
  ArduinoOTA.begin();
  // ota-----
  webSocket.begin();  // start webSocket server
  webSocket.onEvent(webSocketEvent);
  if (MDNS.begin("esp8266")) {
#ifdef DEBUG
    Serial.println("MDNS responder started. access for http://esp8266.local");
#endif
  }
  server.on("/", []() {
    server.send(200, "text/html", (char *)htmlP); //send index.html
  });
  server.begin();
  // Add service to MDNS
  MDNS.addService("http", "tcp", 80);
  MDNS.addService("ws", "tcp", 81);
}

void loop() {
  ArduinoOTA.handle();
  webSocket.loop();
  server.handleClient();
}
<<<<<<< HEAD

=======

>>>>>>> 70d51fa834b2b40fe29244ad6c6572da5e99fa63
