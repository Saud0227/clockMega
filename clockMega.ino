#include <Servo.h>
#include "U8glib.h"
#include <Wire.h>
#include <RtcDS3231.h>
#include <Adafruit_NeoPixel.h>


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin, -1 for arduino share
#define SERVO_PIN 9
#define ANALOG_READ_PIN 14
#define OVERRIDE_SNAP 8

const byte neoPin = 7;
const byte neoPixels = 24;
byte neoBright = 100;

Adafruit_NeoPixel ring = Adafruit_NeoPixel(neoPixels, neoPin, NEO_GRB);


U8GLIB_SSD1306_128X64 oled(U8G_I2C_OPT_NONE);

RtcDS3231<TwoWire> rtcModule(Wire);

//Servo object
Servo myServo;
//Curent angle
int cPos = 0;
//Screen Switch


byte tH;
byte tM;
byte tS;

int loopC = 0;


void setup(){
    Serial.begin(9600);
    myServo.attach(SERVO_PIN);
    pinMode(ANALOG_READ_PIN, 0);
    pinMode(OVERRIDE_SNAP, 0);
    oled.setFont(u8g_font_helvB10);

    rtcModule.Begin();
    RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
    rtcModule.SetDateTime(compiled);

	ring.begin();
	ring.setBrightness(neoBright); 
	ring.show();
}


void setPos(int inVal, int tolerance=0){
    //IN: Degree to set servo, tolerance to be the min degree to be set, standard is 0.
    //OUT: Void function
    //DO: Check if change exceeds tolerance, if true change motor position and store new position to memory.
    if (abs(inVal-cPos)>tolerance){
        myServo.write(inVal);
        cPos = inVal;
        Serial.print(inVal);
        Serial.print("\n");
    }
}



float rtcUpdate(){
    RtcDateTime now = rtcModule.GetDateTime();
    RtcTemperature rtcTemp = rtcModule.GetTemperature();

    tH = now.Hour();
    tM = now.Minute();
    tS = now.Second();
    return(rtcTemp.AsFloatDegC());

}

void screenWrite(float cTemp){
    oled.firstPage();
    do{
        //oled.drawStr(0,30, String(cPos).c_str());
        oled.drawStr(10, 20, String(String(tH) + ":" + String(tM)).c_str());
        oled.drawStr(10,40,String(cTemp).c_str());
    }while(oled.nextPage());
}


void loop(){
    //int inVal = analogRead(ANALOG_READ_PIN);
    //int degWrite = map(inVal, 0, 1023, 0, 180);

    //setPos(degWrite,15*(abs(digitalRead(OVERRIDE_SNAP)-1)));
    float rtcTemp = rtcUpdate();

    screenWrite(rtcTemp);
	if(int(loopC/24) == 0){
		ring.setPixelColor(loopC, ring.Color(0,118,189));
    	ring.show();
	}else if (int(loopC/24) == 1){
		ring.setPixelColor(loopC-24, ring.Color(0,0,0));
    	ring.show();
	}else{
		loopC = -1;
	}
	loopC +=1;
    delay(5);

}


