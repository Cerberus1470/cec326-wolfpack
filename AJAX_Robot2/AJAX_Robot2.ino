/*
  AJAX Demonstration for the CEC 326 robot v0.5

 A simple web server that lets you blink an LED and read and ADC input
 without refreshing the page through AJAX. This also demonstrates serving 
 multiple webpages using a WiFiNINA based system.

 This sketch will print the IP address of your WiFi module (once connected)
 to the Serial Monitor. From there, you can open that address in a web browser
 to turn the LED on and off and read the ADC value.

 This example is written for a network using WPA encryption. For
 WEP or WPA, change the WiFi.begin() call accordingly.

 Seth McNeill
 2022 November 22
 */
#include <SPI.h>
#include <WiFiNINA.h>

#include "index.h" // index.html essentially
#include "arduino_secrets.h" 

#define RIGHT_BUTTON_PIN   A0
#define BUZZ_PIN           2
#define SERVO_PIN          9
#define NEO_PIN           17  // WARNING! THIS IS GPIO NOT D NUMBER for NeoPixels
#define NEO_COUNT         18  // number of NeoPixels
#define AIN1              0  // Motor driver A
#define AIN2              1  // Motor driver A
#define BIN1              A2  // Motor driver A
#define BIN2              8  // Motor driver A
#define TFT_CS            4  // display chip select
#define TFT_RST           A3 // display reset
#define TFT_DC            3  // display D/C
#define SD_CS             7
// To try different scaling factors, change the following define.
// Valid scaling factors are 1, 2, or 3.
#define SCALING 3

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key index number (needed only for WEP)
int LED_PIN = LED_BUILTIN;
int ADC_PIN = A0;

int status = WL_IDLE_STATUS;
WiFiServer server(80);

void setup() {
  Serial.begin(115200);      // initialize serial communication
  delay(3000); // wait for Serial to begin if connected
  Serial.println("Starting...");
  
  pinMode(LED_PIN, OUTPUT);      // set the LED pin mode

  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to Network named: ");
    Serial.println(ssid);                   // print the network name (SSID);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    if(!strcmp(pass, "")) {
      status = WiFi.begin(ssid);
    } else {
      Serial.println("Using password");
      status = WiFi.begin(ssid,pass);
    }
    // wait 10 seconds for connection:
    delay(1000);
  }
  server.begin();                           // start the web server on port 80
  printWifiStatus();                        // you're connected now, so print out the status

  IPAddress ip = WiFi.localIP();
  String ipString = String(ip[0]) + "." + ip[1] + "." + ip[2] + "." + ip[3];
}


void loop() {
  static int servoPos = 90;
  WiFiClient client = server.available();   // listen for incoming clients

  int adcVal = analogRead(ADC_PIN);  // Running this inside the if (client.available()) crashed the RP2040

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    String getLine = "";                    // line indicating the GET request
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:

            // parse out the get request
            int getStartIdx = getLine.indexOf('/');
            int getEndIdx = getLine.indexOf(' ', getStartIdx+1);
            String getRequest = getLine.substring(getStartIdx+1,getEndIdx);
            // do something based on what was requested
            if(getRequest == "H") {
              digitalWrite(LED_PIN, HIGH);               // GET /H turns the LED on
              client.println(createOKResponse("text/plain", "H"));
            } else if(getRequest == "L") {
              digitalWrite(LED_PIN, LOW);                // GET /L turns the LED off
              client.println(createOKResponse("text/plain", "L"));
            } else if(getRequest.startsWith("P=")) {
              String package = getRequest.substring(getRequest.indexOf('=')+1);
              int power = package.substring(0,getRequest.indexOf(',')).toInt();
              String steering = package.substring(getRequest.indexOf(',')+1);
              Serial.println("Power: " + String(power) + "\t steering: " + String(steering));
              steering = map(steering, -90, 90, -1, 1);
              if (power > 0) { // Forward
                power = map(power, 0, 90, 0, 255);
                Serial.println("AIN: " + String(power) + "\t bruh: " + String(steering));
                analogWrite(AIN1, power);
                analogWrite(AIN2, 0);
                analogWrite(BIN1, (1-steering)*power);
                analogWrite(BIN2, 0);
              }
              else if (power < 0) { // Backward
                power = map(power, 0, -90, 0, 255);
                analogWrite(AIN1, 0);
                analogWrite(AIN2, power);
                analogWrite(BIN1, 0);
                analogWrite(BIN2, power);
              }
              else {
                analogWrite(AIN1, 255);
                analogWrite(AIN2, 255);
                analogWrite(BIN1, 255);
                analogWrite(BIN2, 255);
              }
              Serial.println(getRequest);
            } else if(getRequest == "readADC") {
              client.println(createOKResponse("text/plain", String(adcVal)));
            } else if(getRequest == "") {
              client.println(createOKResponse("text/html", MAIN_page));
            } else {
              Serial.println("client requested something else<br>");
              client.println("HTTP/1.1 404 Not Found");
            }

            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            if (currentLine.startsWith("GET ")) {
              getLine = currentLine;
            }
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
}


String createOKResponse(String contentType, String content) {
  String outStr = "HTTP/1.1 200 OK\n";
  outStr += "Content-type:";
  outStr += contentType;
  outStr += "\n\n";
  outStr += content;
  return(outStr);
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}
