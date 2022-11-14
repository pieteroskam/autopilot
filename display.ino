#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>

/* Declare LCD object for SPI

CLK Serial Clock Input  SPI_CLK
DIN Serial Data Input SPI_D
DC Data Command GPIO
CE Chip Enable  GPIO
RST reset  GPIO

 Adafruit_PCD8544(CLK,DIN,D/C,CE,RST); */
Adafruit_PCD8544 display = Adafruit_PCD8544(18, 5, 17, 16, 4); 
int contrastValue = 60; // 60 Default Contrast Value
const int adcPin = 34;
int adcValue = 0;

String currentScreen = "";

void display_setup()
{
   /* Initialize the Display*/
  display.begin();
   /* Change the contrast using the following API*/
  display.setContrast(60);
  display.display();
  //delay(1000);

}

void display_loop(String display_msg = "")
{
  display.clearDisplay();
  
  main_screen();
  display.display(); 
     
  
 
}

void main_screen(){
  /* Now let us display some text */

  int heading_text = heading_float;

  
  display.setFont(&FreeSansBold12pt7b);
  display.setTextColor(BLACK);
  
  display.setCursor(0,18);
  if(heading_text <100) display.print(0);
  if(heading_text <10) display.print(0);
  
  display.println(heading_text);
    display.setCursor(0,36);
  if(ap_on){
    display.println(heading_command);
  }else{
    display.println("off");
  }
  

  //display.setFont(&FreeSans9pt7b);
  display.setFont();
    display.setTextSize(1);

  // check if a mode is slected
  //P
  display.setCursor(45,0);
  display.print("P:");
  if(mode_selected==1){
        display.setTextColor(WHITE, BLACK); // 'inverted' text
        display.println(gain_P);
        display.setTextColor(BLACK);
   }else{
      display.setTextColor(BLACK);
      display.println(P,0);
   }

   display.setCursor(45,10);
   display.print("I:");
   if(mode_selected==2){
        display.setTextColor(WHITE, BLACK); // 'inverted' text
        display.println(gain_I);
        display.setTextColor(BLACK);
   }else{
      display.setTextColor(BLACK);
      display.println(I,0);
   }

    display.setCursor(45,20);
    display.print("D:");
   if(mode_selected==3){
        display.setTextColor(WHITE, BLACK); // 'inverted' text
        display.println(gain_D);
        display.setTextColor(BLACK);
   }else{
      display.setTextColor(BLACK);
      display.println(D,0);
   }

  display.setCursor(45,30);
  display.print("DD:");
   if(mode_selected==4){
        display.setTextColor(WHITE, BLACK); // 'inverted' text
        display.println(gain_DD);
        display.setTextColor(BLACK);
   }else{
      display.setTextColor(BLACK);
      display.println(DD,0);
   }

   //rudder movement indicator
   //x,y,width,h
   //nokia is 84x48

   int width = map(PID_command,-255,255,42,-42); 
   int x = 42;
   if(width<0){
    width *= -1;
    x -= width;   
   }
   display.fillRect(x, 44, width, 4, BLACK);
}
