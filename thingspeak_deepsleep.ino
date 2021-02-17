/*
 * 
 * 
 * Program tartalma: 
 *     - Thingspeakre valo adat közles (V 1.00)
 *     - Pontos szinkronizalt idovel println (2020.07.30.  V 1.01)
 *     - 3 percig deepsleep, olvas, deepsleep
 *     - Homerseklet, paratartalom es legnyomas meres
 * 
 * 
 */
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
/*
*                                     *
*                                     *
*       D0 - RST  osszekotes          *
*                                     *
*                                     *
 */

float h, t, p, pin, dp;
const long utcOffsetInSeconds = 7200;
char daysOfTheWeek[7][12] = {"Hetfo", "Kedd", "Szerda", "Csutortok", "Pentek", "Szombat", "Vasarnap"};
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);
char temperatureFString[6];
char dpString[6];
char humidityString[6];
char pressureString[7];
char pressureInchString[6];

Adafruit_BME280 bme; // I2C
// replace with your "ESP8266-NodeMCU-12E-BME280" (The first channel you created) channel’s thingspeak API write key,
String apiKey = "";
// replace with your routers SSID
const char* ssid = "";
// replace with your routers password
const char* password = "";

const char* server = "api.thingspeak.com";
WiFiClient client;


/**************************  
 *   S E T U P
 **************************/
// only runs once on boot
void setup() {
  // Initializing serial port for debugging purposes
  Serial.begin(115200);
    timeClient.begin();
  delay(100);
  bme.begin(0x76);   
  // Connecting to WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  
    // Printing the ESP IP address
  Serial.println(WiFi.localIP());
  Serial.println(F("BME280 test"));

 /* if (!bme.begin()) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    while (1);
  }*/
}

  /**************************  
 *  L O O P
 **************************/
void loop() {
    timeClient.update();
    Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print(timeClient.getHours());
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  Serial.print(":");
  Serial.println(timeClient.getSeconds());
  //Serial.println(timeClient.getFormattedTime());
    h = bme.readHumidity();
    t = bme.readTemperature();
        delay(10000);

    Serial.println("Szuz szenzor ertek:");
    Serial.println(h);
    Serial.println(t);

    dp = t-0.36*(100.0-h);
    
    p = (bme.readPressure() / 100.0F)+10;
    pin = 0.02953*p;
    dtostrf(t, 5, 1, temperatureFString);
    dtostrf(h, 5, 1, humidityString);
    dtostrf(p, 5, 1, pressureString);
    dtostrf(pin, 5, 2, pressureInchString);
    dtostrf(dp, 5, 1, dpString);

    Serial.print("Temperature = ");
    Serial.println(temperatureFString);
    Serial.print("Humidity = ");
    Serial.println(humidityString);
    Serial.print("Pressure = ");
    Serial.println(pressureString);
    Serial.print("Pressure Inch = ");
    Serial.println(pressureInchString);
    Serial.print("Dew Point = ");
    Serial.println(dpString);
    
    if (client.connect(server,80))  // "184.106.153.149" or api.thingspeak.com
    {
        String postStr = apiKey;
        postStr +="&field1=";
        postStr += String(temperatureFString);
        postStr +="&field2=";
        postStr += String(humidityString);
        postStr +="&field3=";
        postStr += String(pressureString);
        postStr += "\r\n\r\n";
        
        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(postStr.length());
        client.print("\n\n");
        client.print(postStr);    
    }
    client.stop(); 
    //every 5 Min   
    delay(100);
  Serial.println("I'm awake, but I'm going into deep sleep mode for 3 minutes");

     ESP.deepSleep(18e7); 
}
