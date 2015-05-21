# Publish Analog Sensor Data using Arduino

In this guide you will learn how to  publish data from an analog sensor with an Arduino Ethernet Shield to Sense Tecnic's STS IoT Platform community version (WoTKit).

## Dependencies 

* Arduino Software > 1.6.4 (http://www.arduino.cc/en/Main/Software)

## Complete Application Code

You can find the complete application and code walk-through at https://github.com/SenseTecnic/wotkit-example-arduino-sensor

## Getting Started

Download and install the latest Arduino software (http://www.arduino.cc/en/Main/Software). This guide assumes you have an Arduino board (http://www.arduino.cc/en/Main/ArduinoBoardUno) and an Ethernet shield (http://www.arduino.cc/en/Main/ArduinoEthernetShield).

### Create a WoTKit account

WoTKit is a community version of the STS IoT Platform, it allows you to manage, visualize and analyze data from sensors easily. Create an account at: http://wotkit.sensetecnic.com, its FREE! 

### Wire everything up

Our setup will be quite simple. We will be using a temperature sensor (LM35) connected to the A0 pin:

![alt tag](https://raw.githubusercontent.com/SenseTecnic/wotkit-example-arduino-sensor/master/schematic_bb.jpg)

### Encoding your credentials.

You will need to set up your credentials in the Arduino sketch. This example uses Basic Authentication to make a REST request (POST) to the WoTKit to publish data. You can use a service like https://www.base64encode.org/ to create this string using your WoTKit credentials or using a library.

To get started quickly, and since we're only going to do this once let's use https://www.base64encode.org/ to encode your credentials. You can encode the following string: "yourusername:yourpassword" which will yield the string "eW91cnVzZXJuYW1lOnlvdXJwYXNzd29yZA==". Note this as it will be useful in the following line later in this guide (remember to use your own username and password):

```
    client.println("Authorization: Basic eW91cnVzZXJuYW1lOnlvdXJwYXNzd29yZA==");
```

Alternatively, you can use a library:

* https://github.com/adamvr/arduino-base64
* https://github.com/amcewen/b64

## Code Walkthrough

First, let's include our dependencies:


```
#include <SPI.h>
#include <Ethernet.h>
```

And set up our Ethernet client library: 

```
// Enter a MAC address for your controller below.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
byte mac[] = {  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
EthernetClient client; // Initialize the Ethernet client library
```

Finally, let's declare the WoTKit ip address we will make requests to, and the name of our sensor:

```
//Address of the WoTKit where we will be publishing data
IPAddress server(142,103,25,37); // WoTKit IP Address
String sensorName = "yourusername.sensorname"; //yoursensorname
```

Our setup function will be very simple, we will initialize the serial port (to debug our application) and initialize our ethernet shield.

```
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; 
    // wait for serial port to connect. 
    //Needed for Leonardo only
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
```

We will need to create some variables. We will need the sensor pin where we will be connecting our analog sensor and a variable to hold its value:

```
int sensorPin = A0;
int sensorValue = 0;  // variable to store the value coming from the sensor
```

All that was just for setting our application up. Now we can have some fun. Let's write our loop function. We want to make a call to POST data to the API every 10 seconds, so we will write a nifty loop that will only execute a POST function when enough time has passed:


```
// last time you connected to the server, in milliseconds
unsigned long lastConnectionTime = 0;
// delay between updates, in milliseconds          
const unsigned long postingInterval = 10*1000; 
// state of the connection last time through the main loop 
boolean lastConnected = false;                 

void loop()
{ 
 
  if(!client.connected() && (millis() - lastConnectionTime > postingInterval)) {
    sensorValue = analogRead(sensorPin);
    postValue(sensorValue);
  }
  
  // store the state of the connection for next time through the loop:
  lastConnected = client.connected();

}
```

And now, let's write our function to POST data to the WoTKit API:

```
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
```

And we are done. Compile the code and upload it to your Arduino board. Enjoy!


