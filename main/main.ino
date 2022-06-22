// Import required libraries
#ifdef ESP32
  #include <WiFi.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

const char* ssid = "Mavericks Gold";
const char* password = "its123nocaps";

const char* PARAM_INPUT_1 = "switch";

const int outputOne = 13;
const int outputTwo = 12;
const int outputThree = 14;
const int outputFour = 27;


// Variables will change:
int ledStateOne = HIGH;
int ledStateTwo = HIGH;
int ledStateThree = HIGH;
int ledStateFour = HIGH;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<body>
  <h2>Switches</h2>
  <button onclick=toggle(1)>One</button>
  <button onclick=toggle(2)>Two</button>
  <button onclick=toggle(3)>Three</button>
  <button onclick=toggle(4)>Four</button>
<script>function toggle(button) {
  var xhr = new XMLHttpRequest();
  xhr.open("GET", `/update?switch=${button}`, true);
  xhr.send();
}
</script>
</body>
</html>
)rawliteral";


void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

//  pinMode(outputOne, OUTPUT);
//  digitalWrite(outputOne, HIGH);

  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html);
  });

  // Send a GET request to <ESP_IP>/update?state=<inputMessage>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/update?state=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;

      if (inputMessage == "1"){
        ledStateOne = !ledStateOne;
      }
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.println(inputMessage);
    request->send(200, "text/plain", "OK");
  });

  // Start server
  server.begin();
}
  
void loop() {
  digitalWrite(outputOne, ledStateOne);
}
