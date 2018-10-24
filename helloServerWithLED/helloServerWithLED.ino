#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char* ssid = "<your wifi ssid here>";
const char* password = "<your wifi password here>";

ESP8266WebServer server(80);

//Change your pinouts here. This was based on the Wemos D1 Mini pin layout
#define led_power D5
#define led_red D0
#define led_green D6
#define led_blue D7

void handleRoot() {
  server.send(200, "text/plain", "Hello from esp8266");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup(void) {
  pinMode(led_power, OUTPUT);
  pinMode(led_red, OUTPUT);
  pinMode(led_blue, OUTPUT);
  pinMode(led_green, OUTPUT);

  digitalWrite(led_power, HIGH);
  digitalWrite(led_red, LOW);
  //can set this to 9600 for the serial monitor
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  //if you need to create a static IP address on your network
  //  IPAddress ip(10,254,227,188);
  //  IPAddress gateway(10,254,227,1);
  //  IPAddress subnet(255,255,255,0);
  //  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  digitalWrite(led_power, LOW);
  digitalWrite(led_red, HIGH);
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/on", []() {
    led_state = 0;
    led_power_state = 1;
    digitalWrite(led_power, HIGH);
    digitalWrite(led_red, LOW);
    server.send(200, "text/plain", "the LED is now on");
  });

  server.on("/off", []() {
    led_state = 1;
    led_power_state = 0;
    digitalWrite(led_power, LOW);
    digitalWrite(led_red, HIGH);
    server.send(200, "text/plain", "the LED is now off");
  });

  //see if you can't change the colour with a http request instead of simply putting it on or off
  
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
