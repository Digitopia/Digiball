#include <SPI.h>
#include <WiFiNINA.h>
#include <WiFiUdp.h>
#include <MKRIMU.h>
#include <stdlib.h>

int status = WL_IDLE_STATUS;

char ssid[] = "WRITE YOUR NEWTWORK SSID HERE";   // your network SSID (name)
char pass[] = "WRITE YOUR NETWORK PASSWORD HERE";  // your network password (use for WPA, or use as key for WEP)

char remoteip[] = "WRITE YOUR HOST COMPUTER IP HERE";
int port = 6061;
unsigned int localPort = 2390;  // local port to listen on


WiFiUDP Udp;

String heading_, roll_, pitch_;
String gyrox_, gyroy_, gyroz_;
String x_, y_, z_;
String magx_, magy_, magz_;


void setup() {

  //Initialize serial and wait for port to open:

  Serial.begin(9600);


  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");

    while (1)
      ;
  }


  // check for the WiFi module:

  if (WiFi.status() == WL_NO_MODULE) {

    Serial.println("Communication with WiFi module failed!");

    // don't continue

    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();

  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {

    Serial.println("Please upgrade the firmware");
  }

  // attempt to connect to Wifi network:

  while (status != WL_CONNECTED) {

    Serial.print("Attempting to connect to SSID: ");

    Serial.println(ssid);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:

    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:

    delay(10000);
  }

  Serial.println("Connected to wifi");

  printWifiStatus();

  Udp.begin(localPort);
}


void loop() {

  while (WiFi.status() != WL_CONNECTED) {

    Udp.stop();

    Serial.print("Connection Lost ");

    Serial.print("Attempting to connect to SSID: ");

    Serial.println(ssid);

    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:

    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:

    delay(5000);

    printWifiStatus();

    Udp.begin(localPort);
  }



  float heading, roll, pitch;
  float gyrox, gyroy, gyroz;
  float x, y, z;
  float magx, magy, magz;

  if (IMU.eulerAnglesAvailable()) {
    IMU.readEulerAngles(heading, roll, pitch);

    heading_ = "/" + String(heading);
    roll_ = "/" + String(roll);
    pitch_ = "/euler/" + String(pitch);
  }


  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(gyrox, gyroy, gyroz);

    gyrox_ = "/gyro/" + String(gyrox);
    gyroy_ = "/" + String(gyroy);
    gyroz_ = "/" + String(gyroz);
  }

  //ACCEL check
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);

    x_ = "/accel/" + String(x);
    y_ = "/" + String(y);
    z_ = "/" + String(z);
  }

  //Magneto check
  if (IMU.magneticFieldAvailable()) {
    IMU.readMagneticField(magx, magy, magz);

    magx_ = "/mag/" + String(magx);
    magy_ = "/" + String(magy);
    magz_ = "/" + String(magz);
  }

  Udp.beginPacket(remoteip, port);

  Udp.print(pitch_);
  Udp.print(roll_);
  Udp.print(heading_);
  Udp.print(gyrox_);
  Udp.print(gyroy_);
  Udp.print(gyroz_);
  Udp.print(x_);
  Udp.print(y_);
  Udp.print(z_);
  Udp.print(magx_);
  Udp.print(magy_);
  Udp.print(magz_);


  Udp.endPacket();
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
}