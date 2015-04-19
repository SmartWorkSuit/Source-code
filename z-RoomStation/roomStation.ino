
#include <math.h>
#include <SoftwareSerial.h>
#define SSID "NASA"
#define PASS ""
SoftwareSerial dbgSerial(10,11);

int analogPin1 = 0;
int analogPin2 = 1;
int escala = 2;
double tempMin = 0.0;

//Datos para las ecuaciones

float Vin = 5.0;     // [V]       
float Rfija = 1000;  // [ohm]   
float R25 = 2800;    // [ohm]   
float Beta = 3900.0; // [K]      
float T0 = 273.15;   // [K]      

float Vout = 0.0;    // [V]    
float Rntc = 0.0;    // [ohm]     

float TempK = 0.0;   // [K]      
float TempC = 0.0;   // [ºC] 

void setup() {
  dbgSerial.begin(9600); // debuging
  Serial.begin(115200);
  dbgSerial.println("Init");
  pinMode(analogPin1,INPUT);
  pinMode(analogPin2,INPUT);
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

void loop(){
  tempMin = analogRead(analogPin2);
  tempMin = map (tempMin, 0, 1023, -100, 800);
  tempMin = tempMin/10;
  Vout=(Vin/1024)*(analogRead(analogPin1));
  Rntc=(Vout*Rfija)/(Vin-Vout);
  TempK = Beta/(log(Rntc/R25)+(Beta/T0));
  TempC = 273.15-TempK;
  dbgSerial.print(TempC);
  dbgSerial.println(" Grad Celcius");
  delay(4000);
}

boolean connectWiFi(){
  dbgSerial.println("Connecting to Wifi");
  Serial.println("AT+CIPMUX=0");
  Serial.println("AT+CWMODE=1");
  delay(2000);
  String cmd="AT+CWJAP=\"";
  cmd+=SSID;
  cmd+="\",\"";
  cmd+=PASS;
  cmd+="\"";
  dbgSerial.println(cmd);
  Serial.println(cmd);
  delay(5000);
  if(Serial.find("OK")){
      Serial.println("WIFI OK");
    return true;
  }else{
     Serial.println("WIFI NOT OK");
    return false;
  }
  delay(5000);
}

void sendToSuit(String data)
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
  String getStr;
  getStr += String(data);
  getStr += "\r\n";
  
  // send data length
  Serial.print("AT+CIPSEND=");
  Serial.println(getStr.length());
  if(Serial.find(">")){
    Serial.println(getStr);
    dbgSerial.print(">");
  }else{
    Serial.println("AT+CIPCLOSE");
    dbgSerial.println("connection timeout");
    //return;
  }
   if(Serial.find("OK")){
    dbgSerial.println("RECEIVED: OK");
  }else{
    dbgSerial.println("RECEIVED: Error");
  }
  delay(11000);
}
