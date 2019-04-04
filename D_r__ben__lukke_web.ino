
#include <WiFi.h>

const int ledOpen=27;
const int ledClose=26;
const int switchReed=25;
String alarmgaaet = "";
String doorinfo = "";
bool alarmpower = true;
String alarmpowerstatus = "On";


// Replace with your network credentials
const char* ssid     = "CiscoGang";
const char* password = "Ciscogang123";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

void setup(){
  Serial.begin(115200);
  
  pinMode(ledOpen, OUTPUT);
  pinMode(ledClose, OUTPUT);
  pinMode(switchReed, INPUT);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();

}

void loop(){
  
  if (alarmpower){
   // if (digitalRead(switchReed)==HIGH){
   //   digitalWrite(ledOpen, LOW);
   //   digitalWrite(ledClose, HIGH);
   //   alarmgaaet = "The door is closed";
   //   doorinfo = "off";
   //   }
       if (digitalRead(switchReed)==LOW){
        digitalWrite(ledOpen, HIGH);
        digitalWrite(ledClose, LOW);
        alarmgaaet = "The door is open!";
        doorinfo = "on";
  
       }
      //else {
      //  digitalWrite(ledOpen, HIGH);
      //  digitalWrite(ledClose, LOW);
      //  alarmgaaet = "The door is open!";
      //  doorinfo = "on";
      //}
    delay(1);
  }


WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
       char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
       if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // turns the GPIOs on and off
             if (header.indexOf("GET /28/on") >= 0) {
                Serial.println("alarm is on");
                alarmpower = true;
                alarmpowerstatus = "On";
                alarmgaaet = "The door is closed";

                                
               
              } else if (header.indexOf("GET /28/off") >= 0) {
                Serial.println("alarm is off");
                alarmpower = false;
                alarmpowerstatus = "Off";
                digitalWrite(ledOpen, LOW);
                digitalWrite(ledClose, HIGH);
              }
              
             // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.print("<head>");
            client.print("<meta content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #ff0000; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #32CD32;}</style></head>");

             // Web Page Heading
            client.println("<body><h1>Door alarm in room MU9-8</h1>");
            //client.println("<h3>Alarm status: " + doorinfo + "</h3>");
            
             // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<h2>SENSOR STATUS: " + alarmgaaet + "</h2>");

            client.println("<h2>ALARM STATUS: " + alarmpowerstatus + "</h2>");
              
            
              if (alarmpower == true){
              
                client.println("<p><a href=\"/28/off\"><button class=\"button\">Turn off the Alarm</button></a></p>");
              }
            
              if (alarmpower == false){

                client.println("<p><a href=\"/28/on\"><button class=\"button button2\"> Turn on the Alarm</button></a></p>");

              } 
         
            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
   client.stop();
   Serial.println("Client disconnected.");
   Serial.println("");
}
}
