#include <WiFi.h>
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <Preferences.h>

const char* PARAM_INPUT_1 = "switch";
int ledStateOne = HIGH;
unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long previousCheckMillis = 0;        // will store last time LED was updated
const long interval = 2000;
const int outputOne = 4;
unsigned long intervl = 0;

Preferences preferences;


const byte DNS_PORT = 53;
IPAddress apIP(8,8,4,4); // The default android DNS
DNSServer dnsServer;
String responseHTML = "<!DOCTYPE html><html><head><title>BRADS A NAAI</title></head><body><p>select time in milliseconds (1sec = 1000ms)</p><input type=number id=val></input><button onclick=toggle(1)>Update</button><script>function toggle(button) {var xhr = new XMLHttpRequest();xhr.open('GET', `http://8.8.4.4:8080/update?switch=${document.querySelector('#val').value}`, true);xhr.send();window.setTimeout(() => {window.location.reload()}, 500);}</script><p> the current setting is: ";

// Create AsyncWebServer object on port 80
WiFiServer server(80);
AsyncWebServer Aserver(8080);


void setup() { 
  Serial.begin(115200);
  Serial.println(F("Initialize System"));
  
  WiFi.mode(WIFI_AP);
  WiFi.softAP("SWITCH_BITCH");
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  preferences.begin("myfile", false);
  
  unsigned long inter = preferences.getULong("interval", 1000);
  Serial.println(inter);
  preferences.putULong("interval", inter);
  preferences.end();




  // if DNSServer is started with "*" for domain name, it will reply with
  // provided IP to all DNS request
  dnsServer.start(DNS_PORT, "*", apIP);

  pinMode(outputOne, OUTPUT);
  digitalWrite(outputOne, HIGH);



//  // Route for root / web page
//  Aserver.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
//    request->send_P(200, "text/html", index_html);
//  });

  // Send a GET request to <ESP_IP>/update?state=<inputMessage>
  Aserver.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/update?state=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
        preferences.begin("myfile", false);
        preferences.putULong("interval", inputMessage.toInt());
        preferences.end();
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  // Start server
  DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
  Aserver.begin();
  server.begin();

}

void loop() {
  dnsServer.processNextRequest();
  WiFiClient client = server.available();   // listen for incoming clients

  

  if (client) {
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.print(responseHTML);
            client.print(intervl);
            client.print(" milliseconds");
            client.print("</p></body></html>");
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    client.stop();
  }


  unsigned long currentMillis = millis();
  if (currentMillis - previousCheckMillis >= interval) {
    // save the last time you blinked the LED
    previousCheckMillis = currentMillis;
    preferences.begin("myfile", false);
    intervl = preferences.getULong("interval", 1000);
    preferences.end();
  }
    

  if (currentMillis > 1000 && currentMillis - previousMillis >= intervl) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    ledStateOne = !ledStateOne;
    digitalWrite(outputOne, ledStateOne);
  }

}
