


// Tested on ESP8266 and NodeMCU



#include <ESP8266WiFi.h>

#define PORT 80

const char WiFiAPPSK[] = "iulian098";     //Arduino password

//Setup wifi data
const char* ssid = "Your WiFi ssid";
const char* password = "Your WiFi password";

IPAddress ip(192,168,0,155);              //Set IP
IPAddress gateway(192,168,0,1);           //Set Gateway
IPAddress subnet(255,255,255,0);          //Set Subnet

WiFiServer server(PORT);                  // create a server at port 80
String request;                           // stores the HTTP request

int Status = WL_IDLE_STATUS;
int tries = 0;
int pin0 = D0; // D0 is for NodeMCU, please change it to match the Arduino you use


void setup()
{
  Serial.begin(115200);
  Serial.println("Starting arduino...");
  delay(50);
  
  Serial.println("Status : " + String(Status));
  
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);
  
  Serial.print("Connecting");
  
  while(WiFi.status() != WL_CONNECTED){
    Serial.print(".");
    delay(500);
    tries = tries + 1;
    if(tries > 20)
      break;
  }

  if(WiFi.status() == WL_CONNECT_FAILED | WiFi.status() == WL_CONNECTION_LOST){
    Serial.println("Failed to connect.");
  }else{
    Serial.println("Connected");
    Serial.println("IP Address : ");
    Serial.println(WiFi.localIP());
  }
  
  delay(100);
  Serial.println("Starting WiFi server.");
  server.begin();
  Serial.println("Server setup finished");

  delay(200);

  Serial.println("All done");
}

void loop()
{
  HtmlPage();
  delay(10);
}

void HtmlPage(){
  WiFiClient client = server.available();  // try to get client
  if (client) {  // got client?
    boolean currentLineIsBlank = true;
    
    while (client.connected()) {
      if (client.available()) {   // client data available to read
        char c = client.read(); // read 1 byte (character) from client
        request += c;  // save the HTTP request 1 char at a time
        // last line of client request is blank and ends with \n
        // respond to client only after last line received
        if (c == '\n' && currentLineIsBlank) {
          // Send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();
          // Send web page
          client.println("<!DOCTYPE html>");
          client.println("<html>");
          client.println("<head>");
          client.println("</head>");
          client.println("<body>");
          client.println("<form method=\"get\">");
          HtmlButtons(client);
          Commands();
          client.println("</form>");
          client.println("</body>");
          client.println("</html>");
          request = "";    // finished with request, empty string
          break;
          }
        // every line of text received from the client ends with \r\n
        if (c == '\n') {
          // last character on line of received text
          // starting new line with next character read
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // a text character was received from client
          currentLineIsBlank = false;
        }
      }
    }
    delay(10);      // give the web browser time to receive the data
    client.stop(); // close the connection
  } 
}

void HtmlButtons(WiFiClient cl){
  //Show buttons to html page
  cl.println("<input type=\"button\" value=\"VALUE 0\" onclick=location.href=\"/?PIN0[0]\">");
  cl.println("<input type=\"button\" value=\"VALUE 100\" onclick=location.href=\"/?PIN0[100]\">");
  cl.println("<input type=\"button\" value=\"VALUE 150\" onclick=location.href=\"/?PIN0[150]\">");
  cl.println("<input type=\"button\" value=\"VALUE 200\" onclick=location.href=\"/?PIN0[200]\">");
  cl.println("<input type=\"button\" value=\"VALUE 255\" onclick=location.href=\"/?PIN0[255]\">");
}

void Commands(){
	
  //To change value you need to send '/?PIN0[yourValue]'
  
  if (request.indexOf("/?PIN0") > -1)
  { 
	int startIndex = request.indexOf("[");
	int endIndex = request.indexOf("]");
	String val;
	
	for(int i = startIndex+1; i < endIndex; i++){
      val += request[i];  
    }
	
    Serial.println("LED value: " + val);
    analogWrite(D0, val.toInt());
  }
  
}

void setupWiFi()
{
  uint8_t mac[WL_MAC_ADDR_LENGTH];
  WiFi.softAPmacAddress(mac);
  String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX);
                 String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
  macID.toUpperCase();
  String AP_NameString = "LED_Strip_WIFI";

  char AP_NameChar[AP_NameString.length() + 1];
  memset(AP_NameChar, 0, AP_NameString.length() + 1);

  for (int i=0; i<AP_NameString.length(); i++)
    AP_NameChar[i] = AP_NameString.charAt(i);

  WiFi.softAP(AP_NameChar, WiFiAPPSK);
}
