#include <WiFi.h>
#include <Ultrasonic.h>

const int trigPin = 27;
const int echoPin = 26;
const int Alarm = 25;

String alarmgaaet = "";
String doorinfo = "";
bool alarmpower = true;
String alarmpowerstatus = "On";

//Netværk
const char* ssid     = "CiscoGang";
const char* password = "Ciscogang123";

//Port 80
WiFiServer server(80);

//HTTP request
String header;

// Variabel
long duration;
long cm;
int distance;

void setup() 
{
  pinMode(trigPin, OUTPUT); // Trigger Output
  pinMode(echoPin, INPUT); // Echo Input
  pinMode(Alarm,OUTPUT); //Alarmen går
  
  Serial.begin(115200); // Serial Output
  
  //Internet 
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

void loop() 
{
 
   digitalWrite(trigPin, LOW);
   delayMicroseconds(2);

   digitalWrite(trigPin, HIGH);
   delayMicroseconds(10);
   digitalWrite(trigPin, LOW);
   

   //echoPin In, til aflaesning
   pinMode(echoPin, INPUT);
   duration = pulseIn(echoPin, HIGH);

    if (alarmpower){
   //Udregning af afstand i cm
   distance = duration*0.0343/2;
    cm = (duration/2) / 29.1; 
    Serial.print( cm);
    Serial.print(" cm");
    Serial.println( );

    if ( distance <=48 ) {
    Serial.print ("Motion detected!!!");
    alarmgaaet = ("MOTION DETECTED!");
    digitalWrite(Alarm, HIGH);
   }
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
             if (header.indexOf("GET /25/on") >= 0) {
                Serial.println("alarm is on");
                alarmpower = true;
                alarmpowerstatus = "On";
                alarmgaaet = (" ");

                            
              } else if (header.indexOf("GET /25/off") >= 0) {
                Serial.println("alarm is off");
                alarmpower = false;
                alarmpowerstatus = "Off";
                digitalWrite(Alarm, LOW);
                
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
            client.println("<body><h1>Window alarm in room MU9-8</h1>");
                        
            client.println("<h2>SENSOR STATUS: " + alarmgaaet + " Distance " + distance + "</h2>");

            client.println("<h2>ALARM STATUS: " + alarmpowerstatus + "</h2>");
              
            
              if (alarmpower == true){
              
                client.println("<p><a href=\"/25/off\"><button class=\"button\">Turn off the Alarm</button></a></p>");
              }
            
              if (alarmpower == false){

                client.println("<p><a href=\"/25/on\"><button class=\"button button2\"> Turn on the Alarm</button></a></p>");

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
