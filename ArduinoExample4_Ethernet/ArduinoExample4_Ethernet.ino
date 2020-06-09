
#include <SPI.h>
#include <Ethernet.h>

// MAC address from Ethernet shield sticker under board
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 0, 155); // IP address, may need to change depending on network
EthernetServer server(80);  // create a server at port 80
byte gateway[] = { 192, 168, 0, 1 };
byte subnet[] = { 255, 255, 255, 0 };
String request;

int pin3 = 3;

void setup()
{
  Ethernet.begin(mac, ip, gateway, subnet);  // initialize Ethernet device
  server.begin();           // start to listen for clients
  Serial.begin(9600);       // for diagnostics
  pinMode(pin3, OUTPUT);
  sendGET();
}

void loop()
{
  EthernetClient client = server.available();  // try to get client
  if (client) {
    boolean currentLineIsBlank = true;
    
    while (client.connected()) {
      if (client.available()) {   // client data available to read
        char c = client.read(); // read 1 byte (character) from client
        HTTP_req += c;  // save the HTTP request 1 char at a time
        // last line of client request is blank and ends with \n
        // respond to client only after last line received
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");
          client.println();
          // send web page
          client.println("<!DOCTYPE html>");
          client.println("<html>");
          client.println("<head>");
          client.println("<title>Arduino Control Panel</title>");
          client.println("</head>");
          client.println("<body>");
          client.println("<form method=\"get\">");
          Buttons(client);
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
    delay(1);      // give the web browser time to receive the data
    client.stop(); // close the connection
  }
}

void Buttons(EthernetClient cl)
{
	//Switch buttons
    cl.println("<br/><input type=\"button\" value=\"PIN 3 OFF\" onclick=location.href=\"/?PIN3OFF\">");
    cl.println("<br/><input type=\"button\" value=\"PIN 3 ON\" onclick=location.href=\"/?PIN3ON\">");
	
	//ReadData
	//id is used by application to read
	cl.println("<br/><p id=\"Pin3\">" + String(digitalRead(pin3)) + "</p>");
}

void Commands(){
	if(request.indexOf("/?PIN3ON") > -1){
		//Turn pin 3 HIGH
		digitalWrite(pin3, HIGH);
    }else if(request.indexOf("/?PIN3OFF") > -1){
		//Turn pin 3 LOW
		digitalWrite(pin3, LOW);
	}
}

void sendGET() //client function to send/receive GET request data.
{
  EthernetClient client;
  if (client.connect(serverName, 80)) {  //starts client connection, checks for connection
    Serial.println("connected");
    client.println("GET / HTTP/1.0"); //download text
    client.println("Host: checkip.dyndns.com");
    client.println(); //end of get request
  } 
  else {
    Serial.println("connection failed"); //error message if no client connect
    Serial.println();
  }

  while(client.connected() && !client.available()) delay(1); //waits for data
  while (client.connected() || client.available()) { //connected or data available
    char c = client.read(); //gets byte from ethernet buffer
    Serial.print(c); //prints byte to serial monitor 
  }

  Serial.println();
  Serial.println("disconnecting.");
  Serial.println("==================");
  Serial.println();
  client.stop(); //stop client

}
