
//This class is used to control the lcd touch screen in the smart labsuit
//Authors: Maribel Zamorano Castro, Egael Gomez

#if defined(__AVR__)
	#define imagedatatype  unsigned int
#elif defined(__PIC32MX__)
	#define imagedatatype  unsigned short
#elif defined(__arm__)
	#define imagedatatype  unsigned short
#endif
#include <UTouch.h>
#include <UTFT.h>
#include <Wire.h>
#include <UTFT_Buttons.h>
#include <stdlib.h>

// Declare which fonts we will be using
extern uint8_t BigFont[];
extern uint8_t SmallFont[];
extern uint8_t SevenSegNumFont[];

// Declare which bitmaps we will be using
extern imagedatatype nasalogo[];
extern imagedatatype cameraicon[];
extern imagedatatype photo[];
extern imagedatatype mic2[];
extern imagedatatype docs[];
extern imagedatatype docManual[];

UTFT  myGLCD(ITDB32S,25,26,27,28);
UTouch        myTouch(6,5,32,3,2);
UTFT_Buttons  myButtons(&myGLCD, &myTouch);

char str[32];
String receivedMessage;
int position=0;
int but1, but2, but3, but4,but5, butDistance, temperature, humidity, gasValue;
int startStringPosition;
int endStringPosition;


void setup()
{
  randomSeed(analogRead(0));
  Serial.begin(9600);
// Setup the LCD
   myGLCD.InitLCD();
   myGLCD.clrScr();
   myGLCD.setFont(SmallFont);
   myGLCD.setColor(VGA_RED);
   myButtons.setTextFont(BigFont);
   Wire.begin(5);
   Wire.onReceive(receiveEvent);
  
  
   delay(2000);
   
   // Clear the screen and draw the frame
  myGLCD.clrScr();

  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect(0, 0, 319, 13);
  myGLCD.setColor(64, 64, 64);
  myGLCD.fillRect(0, 226, 319, 239);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(255, 0, 0);
  myGLCD.print("Prototype NASA SPACE APP CHALLENGE", CENTER, 1);
  myGLCD.setFont(BigFont);
  myGLCD.print("SmartWork Coat",CENTER, 90);
  myGLCD.setFont(SmallFont);
  myGLCD.setBackColor(64, 64, 64);
  myGLCD.setColor(255,255,0);
  myGLCD.print("Maribel Zamorano, Egael Gomez", CENTER, 187);

  myGLCD.setColor(0, 0, 255);
  myGLCD.drawRect(0, 14, 319, 225);
  delay(2000);
   
}

void loop()
{
  Serial.println("Init LCD");

  homeScreen();// go to menu
  delay (11000);
   
  //show image of a rocket manual
  myGLCD.clrScr(); 
  myGLCD.setBackColor(VGA_WHITE);
  but5 = myButtons.addButton( 7,  5, 310,  241, docManual);
  myButtons.drawButtons();
  delay(3000);
  myButtons.deleteButton(but5);
  
  //show image of camera functionality
  myGLCD.clrScr();
  myGLCD.setBackColor(VGA_BLACK);
  but5 = myButtons.addButton( 5,  5, 310,  239, photo);
  myButtons.drawButtons();
  delay(3000);
  myButtons.deleteButton(but5);
  
}

void receiveEvent(int howMany)
{
  Serial.println("Event received");
      str[0] = '\0';
       while(0 < Wire.available())
       {
         str[position] = Wire.read();
         if(str[position] == '>')
         {
           str[position+1]='\0';
           position=0;
           break;
         }
         else
         {
         position++;
         }    
       }

      receivedMessage = String(str);
      getTemperature();
      getHumidity();
      getGasValue();
      
      if(gasValue>=250){
        myGLCD.fillScr(0, 0, 255);
        myGLCD.setColor(255, 0, 0);
        myGLCD.fillRoundRect(80, 70, 239, 169);
        myGLCD.setColor(255, 255, 255);
        myGLCD.setBackColor(255, 0, 0);
        myGLCD.print("CAUTION GAS LEVEL:", CENTER, 30);
        myGLCD.print(String(gasValue), CENTER, 113);
        delay(3000);
      }
  
}

void homeScreen(){
  myGLCD.clrScr();
  myGLCD.setBackColor(VGA_BLACK);
  myGLCD.setColor(255, 0, 0);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(BigFont);
  but1 = myButtons.addButton( 0,  10, 100,  84, nasalogo);
  but2 = myButtons.addButton( 132,  164, 55,  55, cameraicon);
  but3 = myButtons.addButton( 10,  164, 55,  55, mic2);
  but4 = myButtons.addButton( 260,  164, 55,  55, docs);
  myButtons.drawButtons();
  
  myGLCD.print("Temperature:", RIGHT, 20);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.print(String(temperature), CENTER, 50);
  myGLCD.setFont(BigFont);
  myGLCD.print("C", RIGHT, 50);
  myGLCD.setFont(SmallFont);
  myGLCD.print("Humidity:", LEFT, 110);
  myGLCD.print(String(humidity), 90, 110);
  myGLCD.print("Gas Level:", LEFT, 133);
  myGLCD.print(String(gasValue), 90, 133);
  myGLCD.print("Sensor 1, Room 44: 20", RIGHT, 133);
  //myGLCD.print("GET DISTANCE", LEFT, 170);
 
  myGLCD.setFont(BigFont);
  myGLCD.print("Voice", LEFT, 220);
  myGLCD.print("Camera", CENTER, 220);
  myGLCD.print("Docs", RIGHT, 220);
  
  delay(5000);
  myButtons.deleteButton(but1);
  myButtons.deleteButton(but2);
  myButtons.deleteButton(but3);
  myButtons.deleteButton(but4);
}

void getTemperature(){
  startStringPosition= receivedMessage.indexOf('<')+1; 
  endStringPosition= receivedMessage.indexOf(',',startStringPosition+1);
  String temperatureString=receivedMessage.substring(startStringPosition,endStringPosition);
  char Ychar[temperatureString.length() +1];
  temperatureString.toCharArray(Ychar,sizeof(Ychar));
  temperature=atoi(Ychar);
}

void getHumidity()
{
 startStringPosition=endStringPosition+1;
 endStringPosition= receivedMessage.indexOf(',',startStringPosition+1);
 String humidityString=receivedMessage.substring(startStringPosition,endStringPosition);
 char Ychar[humidityString.length() +1];
 humidityString.toCharArray(Ychar,sizeof(Ychar));
 humidity=atoi(Ychar);
}

void getGasValue()
{
 startStringPosition=endStringPosition+1;
 endStringPosition= receivedMessage.indexOf('>',startStringPosition+1);
 String gasString =receivedMessage.substring(startStringPosition,endStringPosition);
 char Zchar[gasString.length() +1];
 gasString.toCharArray(Zchar,sizeof(Zchar));
 gasValue=atoi(Zchar);
}
