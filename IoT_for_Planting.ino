//#include <b64.h>
#include <HttpClient.h>
#include <LTask.h>
#include <LWiFi.h>
#include <LWiFiClient.h>
#include <LDateTime.h>
#define WIFI_AP "yunHTC"
#define WIFI_PASSWORD "0919341789"
#define WIFI_AUTH LWIFI_WPA  // choose from LWIFI_OPEN, LWIFI_WPA, or LWIFI_WEP.
#define per 50
#define per1 3
#define DEVICEID "DZ7uMS7i" // Input your deviceId
#define DEVICEKEY "uHpJhcFKnsmpMzpg" // Input your deviceKey
#define SITE_URL "api.mediatek.com"
#include "DHT.h"

#define DHTPIN 3     // what pin we're connected to
#define DHTTYPE DHT22   // DHT 22  (AM2302)
DHT dht(DHTPIN, DHTTYPE);


LWiFiClient c;
unsigned int rtc;
unsigned int lrtc;
unsigned int rtc1;
unsigned int lrtc1;
char port[4]={0};
char connection_info[21]={0};
char ip[21]={0};             
int portnum;
int val = 0;
String tcpdata = String(DEVICEID) + "," + String(DEVICEKEY) + ",0";
String upload_data;

String tcpcmd_LED_on = "Control_Lamp,1";
String tcpcmd_LED_off = "Control_Lamp,0";
String tcpcmd_fan_on="Control_Fan,1";
String tcpcmd_fan_off="Control_Fan,0";
String tcpcmd_pump_on="Control_Pump,1";
String tcpcmd_pump_off="Control_Pump,0";
String tcpcmd_auto_on="Auto_Mode,1";
String tcpcmd_auto_off="Auto_Mode,0";
String tcpcmd_input_moisture="Input_Moisture";
String tcpcmd_input_temp="Input_Temp";
String str="0";
int moisture=0;
float moisture_input=0;
float temp_input=0;
int automode=0;
String str_temp="";
LWiFiClient c2;
HttpClient http(c2);

//int index_v;
//int index_y=1;
float temp=0.0;
float humid_air=0.0;
int light=0;
int pinLight = A0;
int pinMoisture=A1;

void setup()
{

  dht.begin();// temp and moisture sensor
  
  LTask.begin();
  LWiFi.begin();
  Serial.begin(115200);
  while(!Serial) delay(1000); /* comment out this line when Serial is not present, ie. run this demo without connect to PC */

  Serial.println("Connecting to AP");
  while (0 == LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD)))
  {
    delay(1000);
  }
  Serial.println("");
  Serial.println("calling connection");

  while (!c2.connect(SITE_URL, 80))
  {
    Serial.println("Re-Connecting to WebSite");
    delay(1000);
  }
  delay(100);
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(5, OUTPUT);
  getconnectInfo();
  connectTCP();
}

void getconnectInfo(){
  //calling RESTful API to get TCP socket connection
  c2.print("GET /mcs/v2/devices/");
  c2.print(DEVICEID);
  c2.println("/connections.csv HTTP/1.1");
  c2.print("Host: ");
  c2.println(SITE_URL);
  c2.print("deviceKey: ");
  c2.println(DEVICEKEY);
  c2.println("Connection: close");
  c2.println();
  
  delay(500);

  int errorcount = 0;
  while (!c2.available())
  {
    Serial.println("waiting HTTP response: ");
    Serial.println(errorcount);
    errorcount += 1;
    if (errorcount > 10) {
      c2.stop();
      return;
    }
    delay(100);
  }
  int err = http.skipResponseHeaders();

  int bodyLen = http.contentLength();
  Serial.print("Content length is: ");
  Serial.println(bodyLen);
  Serial.println();
  char c;
  int ipcount = 0;
  int count = 0;
  int separater = 0;
  while (c2)
  {
    int v = c2.read();
    if (v != -1)
    {
      c = v;
      Serial.print(c);
      connection_info[ipcount]=c;
      if(c==',')
      separater=ipcount;
      ipcount++;    
    }
    else
    {
      Serial.println("no more content, disconnect");
      c2.stop();

    }
    
  }
  Serial.print("The connection info: ");
  Serial.println(connection_info);
  int i;
  for(i=0;i<separater;i++)
  {  ip[i]=connection_info[i];
  }
  int j=0;
  separater++;
  for(i=separater;i<21 && j<5;i++)
  {  port[j]=connection_info[i];
     j++;
  }
  Serial.println("The TCP Socket connection instructions:");
  Serial.print("IP: ");
  Serial.println(ip);
  Serial.print("Port: ");
  Serial.println(port);
  portnum = atoi (port);
  Serial.println(portnum);

} //getconnectInfo


void upload_temp(float t){
  //calling RESTful API to upload datapoint to MCS to report LED status
  Serial.println("calling connection");
  LWiFiClient c2;  

  while (!c2.connect(SITE_URL, 80))
  {
    Serial.println("Re-Connecting to WebSite");
    delay(1000);
  }
  delay(100);
  
  upload_data = "Display_Temp,,"+String(t);
  int thislength = upload_data.length();
  HttpClient http(c2);
  c2.print("POST /mcs/v2/devices/");
  c2.print(DEVICEID);
  c2.println("/datapoints.csv HTTP/1.1");
  c2.print("Host: ");
  c2.println(SITE_URL);
  c2.print("deviceKey: ");
  c2.println(DEVICEKEY);
  c2.print("Content-Length: ");
  c2.println(thislength);
  //c2.println(dataLength);
  c2.println("Content-Type: text/csv");
  c2.println("Connection: close");
  c2.println();
  c2.println(upload_data);
 // c2.println(data);
  delay(500);


}

void upload_moisture(float t){
  //calling RESTful API to upload datapoint to MCS to report LED status
  Serial.println("calling connection");
  LWiFiClient c2;  

  while (!c2.connect(SITE_URL, 80))
  {
    Serial.println("Re-Connecting to WebSite");
    delay(1000);
  }
  delay(100);
  
  upload_data = "Display_Moisture,,"+String(t);
  int thislength = upload_data.length();
  HttpClient http(c2);
  c2.print("POST /mcs/v2/devices/");
  c2.print(DEVICEID);
  c2.println("/datapoints.csv HTTP/1.1");
  c2.print("Host: ");
  c2.println(SITE_URL);
  c2.print("deviceKey: ");
  c2.println(DEVICEKEY);
  c2.print("Content-Length: ");
  c2.println(thislength);
  //c2.println(dataLength);
  c2.println("Content-Type: text/csv");
  c2.println("Connection: close");
  c2.println();
  c2.println(upload_data);
 // c2.println(data);
  delay(500);


}

void upload_light(String t){
  //calling RESTful API to upload datapoint to MCS to report LED status
  Serial.println("calling connection");
  LWiFiClient c2;  

  while (!c2.connect(SITE_URL, 80))
  {
    Serial.println("Re-Connecting to WebSite");
    delay(1000);
  }
  delay(100);
  upload_data = "Display_Light,,"+t;
  int thislength = upload_data.length();
  HttpClient http(c2);
  c2.print("POST /mcs/v2/devices/");
  c2.print(DEVICEID);
  c2.println("/datapoints.csv HTTP/1.1");
  c2.print("Host: ");
  c2.println(SITE_URL);
  c2.print("deviceKey: ");
  c2.println(DEVICEKEY);
  c2.print("Content-Length: ");
  c2.println(thislength);
  c2.println("Content-Type: text/csv");
  c2.println("Connection: close");
  c2.println();
  c2.println(upload_data);
  delay(500);


}

void uploadstatus(){
  //calling RESTful API to upload datapoint to MCS to report LED status
  Serial.println("calling connection");
  LWiFiClient c2;  

  while (!c2.connect(SITE_URL, 80))
  {
    Serial.println("Re-Connecting to WebSite");
    delay(1000);
  }
  delay(100);
  //if(digitalRead(6)==1)
  upload_data = "Display_Lamp,,1";
  //else
  //upload_data = "Display_Lamp,,0";
  int thislength = upload_data.length();

  //String data = "Display_Temp,,"+String(temp);
 // int dataLength = data.length();
  
  HttpClient http(c2);
  c2.print("POST /mcs/v2/devices/");
  c2.print(DEVICEID);
  c2.println("/datapoints.csv HTTP/1.1");
  c2.print("Host: ");
  c2.println(SITE_URL);
  c2.print("deviceKey: ");
  c2.println(DEVICEKEY);
  c2.print("Content-Length: ");
  c2.println(thislength);
  //c2.println(dataLength);
  c2.println("Content-Type: text/csv");
  c2.println("Connection: close");
  c2.println();
  c2.println(upload_data);
 // c2.println(data);

  
  delay(500);

  int errorcount = 0;
  while (!c2.available())
  {
    Serial.print("waiting HTTP response: ");
    Serial.println(errorcount);
    errorcount += 1;
    if (errorcount > 10) {
      c2.stop();
      return;
    }
    delay(100);
  }
  int err = http.skipResponseHeaders();

  int bodyLen = http.contentLength();
  Serial.print("Content length is: ");
  Serial.println(bodyLen);
  Serial.println();
  while (c2)
  {
    int v = c2.read();
    if (v != -1)
    {
      Serial.print(char(v));
    }
    else
    {
      Serial.println("no more content, disconnect");
      c2.stop();

    }
    
  }
}



void connectTCP(){
  //establish TCP connection with TCP Server with designate IP and Port
  c.stop();
  Serial.println("Connecting to TCP");
  Serial.println(ip);
  Serial.println(portnum);
  while (0 == c.connect(ip, portnum))
  {
    Serial.println("Re-Connecting to TCP");    
    delay(1000);
  }  
  Serial.println("send TCP connect");
  c.println(tcpdata);
  c.println();
  Serial.println("waiting TCP response:");
} //connectTCP

void heartBeat(){
  Serial.println("send TCP heartBeat");
  c.println(tcpdata);
  c.println();
    
} //heartBeat

void manual_mode(){
  
}
void auto_mode(){
  
}


void loop()
{

 // digitalWrite(7, LOW);
  // upload moisture
  moisture=analogRead(pinMoisture);
  moisture=moisture/10;
  Serial.println("------------------------------");
  Serial.print("Moisture = ");
  Serial.println(moisture);
  upload_moisture(moisture);
        
         if(automode==1){
         if(moisture < (moisture_input-2)) digitalWrite(7, HIGH);
         if(moisture > (moisture_input+2))  digitalWrite(7, LOW);
       }

 
// upload light
  light=analogRead(pinLight);
  Serial.println("------------------------------");
  Serial.print("light = ");
  Serial.println(light);
  if(light<500){
    upload_light("Night! You can turn on the lamp");
    if(automode==1)
     digitalWrite(6, HIGH);
  }
  else {
    upload_light("Day! You can turn off the lamp");
     if(automode==1)
     digitalWrite(6, LOW);
  }

// upload temp sensor
if(dht.readHT(&temp, &humid_air))
    {
        Serial.println("------------------------------");
        Serial.print("temperature = ");
        Serial.println(temp);  
        Serial.print("humidity = ");
        Serial.println(humid_air);
        upload_temp(temp);
       if(automode==1){
         if(temp<temp_input-2) digitalWrite(5, LOW);
         if(temp>temp_input+2)  digitalWrite(5, HIGH);
       }
       
    }
  
  //Check for TCP socket command from MCS Server 
  String tcpcmd="";
  while (c.available())
   {
      int v = c.read();
      if (v != -1)
      {
      Serial.print((char)v);
       
        tcpcmd += (char)v;
        str_temp=tcpcmd;
     

       if (tcpcmd.substring(40).equals(tcpcmd_auto_on)){
          automode=1;
          Serial.println("  -----auto mode enabled");
          tcpcmd="";
        } else if(tcpcmd.substring(40).equals(tcpcmd_auto_off)){  
          automode=0;
          Serial.println("   ------auto mode disabled");
          tcpcmd="";
        } 

        if(automode==0){
                if (tcpcmd.substring(40).equals(tcpcmd_LED_on)){
                digitalWrite(6, HIGH);
                tcpcmd="";
              } else if(tcpcmd.substring(40).equals(tcpcmd_LED_off)){  
                digitalWrite(6, LOW);   
                tcpcmd="";
              } 
              else if (tcpcmd.substring(40).equals(tcpcmd_fan_on)){
                digitalWrite(5, HIGH);
                tcpcmd="";
              }else if(tcpcmd.substring(40).equals(tcpcmd_fan_off)){  
                digitalWrite(5, LOW);
                tcpcmd="";
              }
              else if (tcpcmd.substring(40).equals(tcpcmd_pump_on)){
                digitalWrite(7, HIGH);
                tcpcmd="";
              }else if(tcpcmd.substring(40).equals(tcpcmd_pump_off)){  
                digitalWrite(7, LOW);
                tcpcmd="";
              }
        }

      }
   }
              if(str_temp.substring(40,50).equals(tcpcmd_input_temp)){       
                  temp_input=str_temp.substring(51,54).toFloat();  
              }
              if(str_temp.substring(40,54).equals(tcpcmd_input_moisture)){
                  moisture_input=str_temp.substring(55,58).toFloat();  
              }
                  Serial.println("----input moisture value: "+ String(moisture_input));  
                  Serial.println("----input temp value: "+ String(temp_input));   
          

 
  LDateTime.getRtc(&rtc);
  if ((rtc - lrtc) >= per) {
    heartBeat();
    lrtc = rtc;
  }
  //Check for report datapoint status interval
  LDateTime.getRtc(&rtc1);
  if ((rtc1 - lrtc1) >= per1) {
    uploadstatus();
    lrtc1 = rtc1;
  }
  
}
