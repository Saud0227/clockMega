#include <Servo.h>
#include "U8glib.h"
#include <Wire.h>
#include <RtcDS3231.h>
#include <Adafruit_NeoPixel.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin, -1 for arduino share
#define SERVO_PIN 9
#define RAINBOW true


const int neoPin = 7;
const byte neoPixels = 24;
byte neoBright = 100;

Adafruit_NeoPixel ring = Adafruit_NeoPixel(neoPixels, neoPin, NEO_GRB);


U8GLIB_SSD1306_128X64 oled(U8G_I2C_OPT_NONE);

RtcDS3231<TwoWire> rtcModule(Wire);

//Servo object
Servo myServo;


byte colorArray [20] [3] = {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};

byte tH;
byte tM;
byte tS;

int ringTimeC = 0;

void rainbowUpdate(){
    // IN: Void
    // OUT: Void
    // DO: Randomizes rgb values in colorArray for rainbowMode. If rainbow mode is turned of this function is never called
    for (byte i = 0; i < 20; i++){
        colorArray[i][0]=byte(random(0,255));
        colorArray[i][1]=byte(random(0,255));
        colorArray[i][2]=byte(random(0,255));
    }
}


void setup(){
    // Serial start
    Serial.begin(9600);

    //Servo start
    myServo.attach(SERVO_PIN);

    //Oled font selection
    oled.setFont(u8g_font_helvB10);

    //RTC setup
    rtcModule.Begin();
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    rtcModule.SetDateTime(compiled);

    //Ring setup
	ring.begin();
	ring.setBrightness(neoBright);
	ring.show();

    //Ring color setup (ring or standard)
    if(RAINBOW == false){
        for (int i = 0; i < 20; i++){
            colorArray[i][0]=0;
            colorArray[i][1]=118;
            colorArray[i][2]=189;
        }
    }else{
        rainbowUpdate();
    }
}


void setPos(int degVal){
    // IN: Temperature OlEAD records.
    // OUT: Void function
    // DO: Maps the temperature between 0 and 60, sets the degree to the servo.
    myServo.write(map(degVal,0,60,0,179));
}



float rtcUpdate(){
    // IN: Nothing
    // OUT: temperature value (float)
    // DO: Gets stats from RTC (hours, minutes, seconds, temperature). Sets time to variables and returns temperature
    RtcDateTime now = rtcModule.GetDateTime();
    RtcTemperature rtcTemp = rtcModule.GetTemperature();

    tH = now.Hour();
    tM = now.Minute();
    tS = now.Second();
    return(rtcTemp.AsFloatDegC());

}

void screenWrite(float cTemp){
    // IN: temperature (float)
    // OUT: Void
    // DO: Format hours and minutes to grantee two digits for the display, then write the time and temperature to the screen
    oled.firstPage();
    do{
        String dispH;
        String dispM;
        if(tH<10){dispH = "0" + String(tH);}else{dispH = String(tH);}
        if(tM<10){dispM = "0" + String(tM);}else{dispM = String(tM);}
        oled.drawStr(10, 20, String(dispH + ":" + dispM).c_str());
        oled.drawStr(10,40,String(cTemp).c_str());
    }while(oled.nextPage());
}

void ringLight(){
    // IN: Nothing
    // OUT: Void
    // DO: Sets the color to the curent second. Flashes green lights when a new seconds is detected.
    if(tS != ringTimeC){
        ringTimeC = tS;
        ring.clear();
        ring.setPixelColor(23,ring.Color(0,255,0));
        ring.setPixelColor(22,ring.Color(0,255,0));
        ring.setPixelColor(0,ring.Color(0,255,0));
        ring.setPixelColor(1,ring.Color(0,255,0));


        for (int i = 0; i < int(tS/3); i++){
            ring.setPixelColor(i+2,ring.Color(colorArray[i][0],colorArray[i][1],colorArray[i][2]));
        }
        if(int(tS/3)+1 != 23){
            byte cLed = int(tS/3) +2;
            ring.setPixelColor(cLed, ring.Color(colorArray[cLed-2][0]/3*(tS%3),colorArray[cLed-2][1]/3*(tS%3),colorArray[cLed-2][2]/3*(tS%3)));
        }
    }else{
        ring.setPixelColor(23,ring.Color(0,120,0));
        ring.setPixelColor(22,ring.Color(0,120,0));
        ring.setPixelColor(0,ring.Color(0,120,0));
        ring.setPixelColor(1,ring.Color(0,120,0));
    }
    ring.show();
}

void loop(){
    // Call rtc
    float rtcTemp = rtcUpdate();
    //Write time and temperature to OLED
    screenWrite(rtcTemp);

    // Extra logic for rainbow mode (trigger by changing RAINBOW value in define)
    if((tS == 0)  && (RAINBOW == true)){
        rainbowUpdate();
    }
    // Write position to servo
    setPos(rtcTemp);
    // Update ring-light
    ringLight();
    // Delay 20ms
    delay(20);
}



/*
Lärdommar från första projektet:
Under detta projektet hittade jag github wiki sidan på github vilket var väldigt trevligt för det underlättade att hitta info om
arduino libraries (om skaparna hade tagit tid att göra dem), utöver det har jag nu lärt mig hur jag ska kommentera kod under denna kursen.
*/