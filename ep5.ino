//ขยายพอร์ท
#include <Adafruit_ADS1015.h>
#include <Wire.h>
Adafruit_ADS1115 ads;

// Microgear 
#include <AuthClient.h>
#include <debug.h>
#include <MicroGear.h>
#include <MQTTClient.h>
#include <PubSubClient.h>
#include <SHA1.h>

//Node WIFI
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>

// DHT11
#include <DHT.h>
#include <DHT_U.h>


const char* ssid     = ""; // ชื่อไวไฟ
const char* password = ""; // รหัสไวไฟ

#define APPID   " " //นำ APPID จาก NETPIE มาใส่
#define KEY     " " //APP KEY(Device Key) ของ NETPIE
#define SECRET  " " //APP SECRET(Device Key) ของ NETPIE""

#define ALIAS   "NodeMCU1"
#define TargetWeb "HTML_web"

DHT dht(9, DHT11);

int S = A0;


WiFiClient client;
MicroGear microgear(client);

void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) 
{
    Serial.print("Incoming message --> ");
    msg[msglen] = '\0';
    Serial.println((char *)msg);
}


void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) 
{
    Serial.println("Connected to NETPIE...");
    microgear.setAlias(ALIAS);
}

void setup() 
{
    Wire.begin(4,5);
    ads.begin();
     /* Event listener */
    microgear.on(MESSAGE,onMsghandler);
    microgear.on(CONNECTED,onConnected);

    dht.begin();
    Serial.begin(115200);
    Serial.println("Starting...");

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
    {
       delay(250);
       Serial.print(".");
    }

   
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    microgear.init(KEY,SECRET,ALIAS);
    microgear.connect(APPID);
}

void loop() 
{

  
  
    if (microgear.connected())
    {
       microgear.loop();
       Serial.println("connected");

        Serial.println("Clone Fleur");
       int Humidity = dht.readHumidity();
       int Temp = dht.readTemperature();  // Read temperature as Celsius (the default)
        Serial.print(" Temperature: ");
        Serial.print(Temp);
        Serial.print(" C ");
        Serial.print(" Humidity: ");
        Serial.print(Humidity);
        Serial.print(" RH ");

        int soil= analogRead(S);
        // for Arduino
        //Checksoil = map(Checksoil,0,710,100,0);
        // for NodeWIFI
        soil = map(soil,0,1024,100,0);
        Serial.print(" Moisture: ");
        Serial.print(soil);
        Serial.print(" %");

        int16_t adc0, adc1, adc2, adc3;
        adc0 = ads.readADC_SingleEnded(0);
       adc0 = map(adc0,0,10000,0,100);
        Serial.print(" Light: ");
        Serial.println(adc0);

         

        char msg[128];
    sprintf(msg,"%d,%d,%d,%d,CloneMontFleur", (int)Temp , (int)Humidity , (int)soil , (int)adc0 );
   
       /*String data = "/" + String(Humidity) + "/" + String(Temp);
       char msg[128];
       data.toCharArray(msg,data.length());*/
       Serial.println(msg);    

       microgear.chat(TargetWeb , msg);
    }
   else 
   {
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);
   }
    delay(1500);
}
