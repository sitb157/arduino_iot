#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUDP.h>

byte mac[] = {0x40, 0xD2, 0xDA, 0x10, 0x53, 0x07}; // MAC Address 
IPAddress ip(192, 168, 2, 123); // IP Address
EthernetServer server(1234); // Port 

//The broadcast IP of our network
byte broadCastIp[] = { 192 , 168, 2, 255 };
//MAC address of the machine we want to wake up
byte remote_MAC_ADD[] = { 0xXX, 0xXX, 0xXX, 0xXX, 0xXX, 0xXX };
//UDP WOL port
int wolPort = 7;
//Magic Packet
byte magicPacket[102];

EthernetUDP Udp;

//  Style Font Size
int h1_font_size = 50;
int h1_font_size_mobile = 35;
int button_font_size = 40;
int button_font_size_mobile = 25;
// Button Action Name
String action_name = "/server_power"; 
String action_name_mobile = "/mobile/server_power";

void setup()
{
  // You can use Ethernet.init(pin) to configure the CS pin
  //Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(15);  // ESP8266 with Adafruit Featherwing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet
  Ethernet.begin(mac, ip);  // initialize Ethernet device
  server.begin(); // Start to listen for clients
  Serial.begin(115200);
  Udp.begin(wolPort);
}
 
void loop()
{
    EthernetClient client = server.available();  // try to get client
    if (client) {
      Serial.println("New client connected");
      handleClientRequest(client);               
      client.stop(); // close the connection
      Serial.println("Client disconnected");
    }
}

void server_on() {
  byte preamble[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  char i;
  Udp.beginPacket(broadCastIp, wolPort);
  Udp.write(preamble, sizeof preamble);
  for (i=0; i<16; i++)
    Udp.write(remote_MAC_ADD, sizeof remote_MAC_ADD);
  Udp.endPacket();
}

void handleClientRequest(EthernetClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println();
  client.println("<html><body>");
  
  String request = client.readStringUntil('\r');

  if (request.indexOf("/mobile") != -1) {
    // Mobile version
    setHTML(client, h1_font_size_mobile, button_font_size_mobile, action_name_mobile);
  } else {
    // PC Version
    setHTML(client, h1_font_size, button_font_size, action_name);
  }
  if (request.indexOf("Btn=on") != -1) {
    server_on();
    client.println("<p>server power is on.</p>");
  } else if (request.indexOf("Btn=off") != -1) {
    client.println("<p>server power is off.</p>");
  }

  client.println("</body></html>");
}

void makeButton(EthernetClient client, String action_name) { 
  client.print("<form action=\'"+ action_name +"\' method='GET'>");
  client.print("<input type='hidden' name='Btn' value='on'>");
  client.print("<input class='onBtn' type='submit' value='on'/>");
  client.print("</form>");
  client.print("<form action='"+ action_name +"\' method='GET'>");
  client.print("<input type='hidden' name='Btn' value='off'>");
  client.print("<input class='offBtn' type='submit' value='off'/>"); 
  client.print("</form>"); 
}

void setHTML(EthernetClient client, int h1_font_size, int button_font_size, String action_name) {
  // send web page
  client.println("<!DOCTYPE html>");
  client.println("<html lang=\"en\">");
  client.println("<head>");
  client.println("<meta charset=\"UTF-8\">");
  client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
  client.println("<title>Arduino IoT</title>");
  client.println("</head>");
  client.println("<style>");
  client.println("h1{color:#17AAAA; font-size:" + String(h1_font_size) + "px;}");
  client.println(".box{border:solid 0px #a111a1; width:50%; height: 180px;}");
  client.println("input{width:40%; height: 100px; font-size:"+ String(button_font_size) + "px; color:#fff; border:none; background: #17A1A5;}");
  client.println(".onBtn{margin:40px 0 20px 0; float:left;}");
  client.println(".offBtn{margin:40px 0 20px 0; float:right;}");
  client.println("</style>");
  client.println("<body>");
  client.println("<center>");
  client.println("<br><br><br><br><br>");
  client.println("<h1>Control to Server Power</h1>");
  client.println("<br><br>");
  client.println("<div class=\"box\">");
  makeButton(client, action_name);
  client.println("</div>");
  client.println("</center>");
  client.println("<br />");
}
