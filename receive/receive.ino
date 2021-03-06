#ifdef ESP8266
#include <ESP8266WiFi.h>
#else
#include <WiFi.h>
#endif
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <OSCBundle.h>
#include <OSCData.h>
char ssid[] = "Milab";     // your network SSID (name)
char pass[] = "Milab2016"; // your network password

// A UDP instance to let us send and receive packets over UDP
WiFiUDP Udp;
const IPAddress outIp(10, 40, 10, 105); // remote IP (not needed for receive)
const unsigned int outPort = 9999;      // remote port (not needed for receive)
const unsigned int localPort = 8888;    // local port to listen for UDP packets (here's where we send the packets)
OSCErrorCode error;


//--- Position Pins----//
int relay4 = 4;
int relay3 = 12;
int relay2 = 14;
int relay1 = 16;


void setup()
{
    
  Serial.begin(115200);

  //---setting pins as uptputs---/
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  
  digitalWrite(relay1, HIGH);
  digitalWrite(relay2, HIGH);
  digitalWrite(relay3, HIGH);
  digitalWrite(relay4, HIGH);


    // Connect to WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
#ifdef ESP32
  Serial.println(localPort);
#else
  Serial.println(Udp.localPort());
#endif
}

void imuquat(OSCMessage &msg)
{

    
    
   ///---- sET ALL PINS TO LOW IN WHEN WE STRAT----///
   digitalWrite(relay1, HIGH);
   digitalWrite(relay2, HIGH);
   digitalWrite(relay3, HIGH);
   digitalWrite(relay4, HIGH);

   
  float roatationAnd = msg.getFloat(0); // when battry is up - left +, right -
  float sideAng = msg.getFloat(1); // when battry is up - left +, right -
  float pitchAng = msg.getFloat(2); // when battry is up - back -, fornt +
  
  if (sideAng >= 41 && sideAng <= 90) {
    Serial.println("most left side, relay 1");
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, HIGH);
    digitalWrite(relay3, HIGH);
     digitalWrite(relay4, HIGH);
  }

  else if (sideAng >= 10 && sideAng <= 40) {
    Serial.println("helf left side, relay 2");
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, LOW);
    digitalWrite(relay3, HIGH);
    digitalWrite(relay4, HIGH);
  }

  else if (sideAng <= -10 && sideAng >= -40) {
    Serial.println("helf right side, relay 3");
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, HIGH);
    digitalWrite(relay3, LOW);
    digitalWrite(relay4, HIGH);
  }

  else if (sideAng <= -41 && sideAng >= -90) {
    Serial.println("most right side, relay 4");
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, HIGH);
    digitalWrite(relay3, HIGH);
    digitalWrite(relay4, LOW);
  }
  else {
    Serial.println("no side, doing nothing");
    digitalWrite(relay1, HIGH);
    digitalWrite(relay2, HIGH);
    digitalWrite(relay3, HIGH);
    digitalWrite(relay4, HIGH);
  }
}

void loop()
{
  handleMsg();
}

void handleMsg()
{
  OSCMessage msg;
  int size = Udp.parsePacket();
  if (size > 0)
  {
    //-- try puting in to an arry
    //change the wihle to be until the size of arry
    while (size--)
    {
      msg.fill(Udp.read());
    }
    if (!msg.hasError())
    {
      msg.dispatch("/imuquat", imuquat);
    }
    else
    {
      error = msg.getError();
      Serial.print("error: ");
      Serial.println(error);
    }
  }
}
