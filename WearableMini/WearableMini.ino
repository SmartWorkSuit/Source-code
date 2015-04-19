
//This class is used to control the sensors and microcontroller in the SmartWork Suit.
//Authors: Maribel Zamorano Castro, Egael Gomez

#include "Wire.h"
#include "DHT.h"
#include <SoftwareSerial.h>
#define DHTPIN 2   
extern uint8_t SmallFont[];
#define DHTTYPE DHT22   // DHT 22  (AM2302)
#define SSID "NASA"
#define PASS ""
#define IP "184.106.153.149" // thingspeak.com
SoftwareSerial dbgSerial(10,11);
// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);
// Arduino pin numbers

const int DO_pin = 2;
String temperature,humidity;
//const int A5_pin = 5;
String GET = "GET /update?key=LYRP8MPL5WFIJHC1&field1=";
int gasVal;
float t=0.0f;
 
 
void setup() {
  dbgSerial.begin(9600); // for debuging
  Serial.begin(115200);
  dbgSerial.println("Init");
  
  Wire.begin();
  dht.begin(); 
  
  Serial.println("AT");
  delay(5000);
  if(Serial.find("OK")) {
    dbgSerial.println("WiFi - Module is ready");
    connectWiFi();
  }else{
    dbgSerial.println("Module WiFi dosn't respond.");
    dbgSerial.println("Please reset.");
  }
}


void loop() {
  char c;
  if (Serial.available()) {
    c = Serial.read();
    dbgSerial.print(c);
  }
  if (dbgSerial.available()) {
    c = dbgSerial.read();
    Serial.print(c);
  }
  delay(200); 
  getTemperature();
  getGasValue();
  char buffer[10];
  String tempC =temperature;// dtostrf(t, 4, 1, buffer);
  update(tempC);
  sendDataLCD(temperature);
}


boolean connectWiFi(){
  dbgSerial.println("Connecting to Wifi");
  Serial.println("AT+CIPMUX=0");
  Serial.println("AT+CWMODE=1");
//Serial.println("AT+CIFSR"); // To make the WiFi module a server.
//Serial.println("AT+CIPSERVER=1,1336");
//delay(2000);
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  dbgSerial.println(cmd);
  Serial.println(cmd);
  delay(5000);
  if(Serial.find("OK")){
      dbgSerial.println("WIFI OK");
    return true;
  }else{
     dbgSerial.println("WIFI NOT OK");
    return false;
  }
  delay(5000);
}

void getGasValue(){
  gasVal=analogRead(1);//Read Gas value from analog 0
  if(gasVal>=250)
  {
        // sendDataLCD(gasValue);// send values to the web
        delay(1000);  
        int note = 988;
        int pin=8;
        //3 dots for the S
        for (int i=0; i<3; i++)
        {
        tone(pin, note, 100);
        delay(200);
        noTone(pin);
        }
        delay(200);
        //3 dashes for the O
        for (int i=0; i<3; i++)
        {
        tone(pin, note, 300);
        delay(400);
        noTone(pin);
        }
        delay(200);
        //3 dots for the S again
        for (int i=0; i<3; i++)
        {
        tone(pin, note, 100);
        delay(200);
        noTone(pin);
        }
        delay(200);
        //wait 3 seconds
        delay(3000);
  }
  Serial.print("Gas Value: "); 
  Serial.println(gasVal,DEC);
}

void getTemperature(){
 
  delay(2000);

  dbgSerial.print("\nRetrieving information from temperature sensor: ");
  dbgSerial.print("Read sensor: ");
  float h = dht.readHumidity();
  // Read temperature as Celsius
   t = dht.readTemperature();
  // Read temperature as Fahrenheit
  float f = dht.readTemperature(true);
  
  // Check if any reads failed and exit early.
  if (isnan(h) || isnan(t) || isnan(f)) {
    dbgSerial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index
  // Send temp in Fahrenheit
  float hi = dht.computeHeatIndex(f, h);

  dbgSerial.print("Humidity: "); 
  dbgSerial.print(h);
  dbgSerial.print(" %\t");
  dbgSerial.print("Temperature: "); 
  dbgSerial.print(t);
  dbgSerial.print(" *C ");
  dbgSerial.print(f);
  dbgSerial.print(" *F\t");
  dbgSerial.print("Heat index: ");
  dbgSerial.print(hi);
  dbgSerial.println(" *F");
  int temp=(int)t;
  temperature=String(temp);
  humidity=String(h);
}

void sendDataLCD(String data){
      Wire.beginTransmission(5);
      String sensorsValues="<"+temperature+","+humidity+","+gasVal+">";
      char* cString = (char*) malloc(sizeof(char)*(sensorsValues.length() + 1));
      sensorsValues.toCharArray(cString, sensorsValues.length() + 1);
      Wire.write(cString);   
      Wire.endTransmission();
      Serial.println("Sending Data to LCD");
      delay(1000);
}

void update(String tenmpC)
{
String cmd ="AT+CIPSTART=\"TCP\",\""; cmd += IP;
cmd +="\",80";
  Serial.println(cmd);
  delay(2000);
  if(Serial.find("Error")){
    dbgSerial.println("AT+CIPSTART error");
    return;
  }
  
    // prepare GET string
  String getStr = "GET /update?key=";
  getStr += "LYRP8MPL5WFIJHC1";
  getStr +="&field1=";
  getStr += String(tenmpC);
  getStr += "\r\n";
  
  cmd = GET;
  cmd += tenmpC;
  cmd += "\r\n";
  
 // send data length
  Serial.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  if(Serial.find(">")){
    Serial.println(cmd);
    dbgSerial.print(">");
  }else{
    Serial.println("AT+CIPCLOSE");
    dbgSerial.println("connection timeout");
  }
   if(Serial.find("OK")){
    dbgSerial.println("RECEIVED: OK");
  }else{
    dbgSerial.println("RECEIVED: Error");
  }
  delay(11000);
}
