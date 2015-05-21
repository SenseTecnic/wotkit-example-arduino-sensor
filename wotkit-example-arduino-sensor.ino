
/*
 A demo example publishing analog sensor data to the STS IoT Platform
*/

#include <SPI.h>
#include <Ethernet.h>
 
// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetClient client; // Initialize the Ethernet client library

//Address of the WoTKit where we will be publishing data
IPAddress server(142,103,25,37); // WoTKit IP Address
String sensorName = "yourusername.sensorname"; //yoursensorname

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 10*1000;  // delay between updates, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop

int sensorPin = A0;
int sensorValue = 0;  // variable to store the value coming from the sensor

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:    
    for(;;) {
    }
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);  
}

void postValue(long value) {
  if (client.connect(server, 80)) {
    //You can add as many key=value pairs as you want joined by the & sign
    String PostData = "value=" + String(value)+ "&message=Uploaded%20With%20Arduino";
    client.println("POST /api/v1/sensors/" + sensorName + "/data HTTP/1.0");
    client.println("Content-Type: application/x-www-form-urlencoded;");
    // encode your "username:password" in a service like https://www.base64encode.org/
    client.println("Authorization: Basic USERNAME:PASSWORD-64ENCODED");
    client.print("Content-Length: ");
    client.println(PostData.length());
    client.println();
    client.println(PostData);
    lastConnectionTime = millis();
    Serial. println("Data POSTed: " + PostData);
    client.stop();
  } else {
    Serial.println("POST failed");
    Serial.println("disconnecting.");
    client.stop();  
  }
}

void loop()
{ 
 
  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    sensorValue = analogRead(sensorPin);
    postValue(sensorValue);
  }
  
  // store the state of the connection for next time through the loop:
  lastConnected = client.connected();

}

