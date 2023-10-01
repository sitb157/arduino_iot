#include <SPI.h>
#include <Ethernet.h>
#include <Servo.h>

byte mac[] = {0x90, 0xA2, 0xDA, 0x10, 0x53, 0x07}; // MAC Address 
IPAddress ip(192, 168, 0, 50); // IP Address
EthernetServer server(8080); // Port 8080

int servoPosition = 90; // Intialize position 

//  Style Font Size
int h1_font_size = 50;
int h1_font_size_mobile = 35;
int button_font_size = 40;
int button_font_size_mobile = 25;
// Button Action Name
String action_name = "/server_power"; 
String action_name_mobile = "/mobile/server_power";
// Initalize servo position value
Servo servo;
int default_degree = 30;
int power_on_degree = 160;
int power_off_degree = 30;

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
    servo.attach(9); // PIN to attach servo
    servo.write(default_degree);
    Serial.begin(115200);
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
  servo.write(power_on_degree);
  delay(500);
  servo.write(default_degree);
}

void handleClientRequest(EthernetClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println();
  client.println("<html><body>");
  
  String request = client.readStringUntil('\r'); // HTTP 요청을 읽음

  if (request.indexOf("/mobile") != -1) {
    // Mobile version
    setHTML(client, h1_font_size_mobile, button_font_size_mobile, action_name_mobile);
  } else {
    // PC Version
    setHTML(client, h1_font_size, button_font_size, action_name);
  }
  // check the button status to rotate servo
  if (request.indexOf("Btn=on") != -1) {
    // "on" button to rotate for 30 degree
    server_on();
    client.println("<p>server power is on.</p>");
  } else if (request.indexOf("Btn=off") != -1) {
    // "off" button to rotate for 90 degree.
    //servo.write(power_off_degree);
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
