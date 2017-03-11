/*********
  Arnie Armstrong modified:
  Rui Santos
  Complete project details at http://randomnerdtutorials.com  
*********/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

MDNSResponder mdns;

// Replace with your network credentials
const char* ssid = "SSID";
const char* password = "PASSWORD";
const int buttonPin = D3;       // Button Board
const int ledPin = BUILTIN_LED; // the onboard LED
int buttonState = 0;        // Module Button State
int lilyState = 0;         // Lilys Happiness State
int brightness = 0;        // how bright the LED is (0 = full, 512 = dim, 1023 = off)
int fadeAmount = 5;        // how many points to fade the LED by
const int delayMillis = 1;      // how long to pause between each loop

extern "C"
{  
  #include "user_interface.h"
}

ESP8266WebServer server(80);

String webPage = "";
 
void setup(void){
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  // set initial state, LED off
  digitalWrite(ledPin, HIGH);  // LED on
  // Set Params  
  delay(1000);
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    // set the LED brightness
    analogWrite(ledPin, brightness);
  
    // increment/decrement the brightness for the next loop
    brightness = brightness + fadeAmount;
  
    // limit to 10-bit (0-1023)
    if (brightness < 0) brightness = 0;
    if (brightness > 1023) brightness = 1023;
  
    // reverse the direction of the fading at each end
    if (brightness == 0 || brightness == 1023) {
    fadeAmount = -fadeAmount;
    delay(500);
    Serial.print(".");
    } 
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Lily State: ");
  Serial.println(lilyState);
  analogWrite(ledPin, 1023); // set default led state
  
  if (mdns.begin("DECODEDMicro", WiFi.localIP())) {
    Serial.println("MDNS responder started");
  }

  // Set URL Params
  server.on("/", [](){
    // Setup Webpage Data
    if (lilyState == 0){
      webPage  = ("<!DOCTYPE HTML>");
      webPage += ("\r\n<html>\r\n<head>\r\n<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">");
      webPage += ("\r\n<link rel=\"stylesheet\" type=\"text/css\" href=\"//fonts.googleapis.com/css?family=Raleway\" />");
      webPage += ("\r\n\t<meta http-equiv=\"refresh\" content=\"2; url=/\">");
      webPage += ("\r\n\t<style>");
      webPage += ("\r\n\t\th1 { color: #111; font-family: 'Raleway', sans-serif; font-size: 125px; font-weight: bold; letter-spacing: -1px; line-height: 1; text-align: center; }\n\rh2 { color: #111; font-family: 'Raleway', sans-serif; font-size: 30px; font-weight: 300; line-height: 32px; margin: 0 0 72px; text-align: center; }\n\rp { color: #E7B91B; font-family: 'Raleway', sans-serif; font-size: 14px; line-height: 24px; margin: 0 0 24px; text-align: justify; text-justify: inter-word; }\n\r");
      webPage += ("\r\n\t\tbody {background-color: #E7B91B;}");
      webPage += ("\r\n\t\tfont {color: #111;}");
      webPage += ("\r\n\t</style>\r\n<title>Lily Status</title>\r\n</head>\r\n<body>");
      webPage += ("\r\n<h1>Lily Status</h1>");
      webPage += ("\r\n\t<center><img src='http://i.imgur.com/OoJKoB2.png' /></center>");
      webPage += ("\r\n<div style=\"font-size:x-small\">");
      //webPage += ("\r\n\t<BR>Lily State: "+ String((int)lilyState) +" ";
      webPage += ("\r\n\t<BR>free RAM: ");
      webPage += (uint32_t)system_get_free_heap_size();
      webPage += ("\r\n</div>\r\n</body>\r\n</html>");
    }
    else {
      webPage  = ("<!DOCTYPE HTML>");
      webPage += ("\r\n<html>\r\n<head>\r\n<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">");
      webPage += ("\r\n<link rel=\"stylesheet\" type=\"text/css\" href=\"//fonts.googleapis.com/css?family=Raleway\" />");
      webPage += ("\r\n\t<style>");
      webPage += ("\r\n\t\th1 { color: #111; font-family: 'Raleway', sans-serif; font-size: 125px; font-weight: bold; letter-spacing: -1px; line-height: 1; text-align: center; }\n\rh2 { color: #111; font-family: 'Raleway', sans-serif; font-size: 30px; font-weight: 300; line-height: 32px; margin: 0 0 72px; text-align: center; }\n\rp { color: #Fd4040; font-family: 'Raleway', sans-serif; font-size: 14px; line-height: 24px; margin: 0 0 24px; text-align: justify; text-justify: inter-word; }\n\r");
      webPage += ("\r\n\t\tbody {background-color: #Fd4040;}");
      webPage += ("\r\n\t\tfont {color: #000000;}");
      webPage += ("\r\n\t</style>\r\n<title>Lily Status</title>\r\n</head>\r\n<body>");
      webPage += ("\r\n<h1>Lily Status</h1>\r\n");
      webPage += ("\r\n<p><audio autoplay loop>\n<source src=\"http://soundjax.com/reddo/66084%5Eair-raid-siren-alert.mp3\" type=\"audio/mpeg\">\nYour browser does not support the audio element.</audio></p>\r\n");
      webPage += ("\r\n\t<center><a href=\"statereset\"><img src='http://i.imgur.com/e1H7BHr.png' /></a></center>");
      webPage += ("\r\n<div style=\"font-size:x-small\">");
      //webPage += ("\r\n\t<BR>Lily State: "+ String((int)lilyState) +" ";
      webPage += ("\r\n\t<BR>free RAM: ");
      webPage += (uint32_t)system_get_free_heap_size();
      webPage += ("\r\n</div>\r\n</body>\r\n</html>");
    }
    server.send(200, "text/html", webPage);
  });
  
  server.on("/statereset", [](){
    digitalWrite(ledPin, HIGH);  // LED on
    lilyState = 0;
    Serial.print("Lily State: ");
    Serial.println(lilyState);
    server.sendHeader("Location", String("/"), true);
    server.send ( 302, "text/plain", "");
    delay(1000);
  });

  // Start Server
  server.begin();
  Serial.println("HTTP server started");
  }
 
void loop(void){
  server.handleClient();
  // read button state, HIGH when pressed, LOW when not
  buttonState = digitalRead(buttonPin);

  // if the push button pressed, switch on the LED
  if (buttonState == LOW) {
    lilyState = 1;
    Serial.print("Lily State Set To: ");
    Serial.println(lilyState);
  }
if (lilyState == 1) {
    digitalWrite(ledPin, LOW);  // LED on
  }
}
