#include "Button.h"
#include "DFPlayerMini_Fast.h"
#include <SoftwareSerial.h>

#define slow_btn_pin 3
#define med_btn_pin 8
#define fast_btn_pin 5
#define softSerialRX 10
#define softSerialTX 11
#define audioMonitor 14
#define ledPin 4
#define slowBtnLED_pin 2
#define medBtnLED_pin 7
#define fastBtnLED_pin 9

Button Slow_Btn;
Button Med_Btn;
Button Fast_Btn;

DFPlayerMini_Fast DFplayer;  // create audio player "DFplayer"
SoftwareSerial mySerial(softSerialRX, softSerialTX); // open software serial

int heartSpeed = 0;
int audioRead =0;
bool is_playing = 0;
int pulseThreshold = 560;
int beatDuration = 0;
unsigned long prevPulseMillis = 0;
unsigned long currentMillis =0;
unsigned long lastPressMillis=0;
unsigned long timeOut = 15000;
int ledState =0;

void setup() {

  Serial.begin(9600);
  mySerial.begin(9600);

  pinMode(slowBtnLED_pin, OUTPUT);
  pinMode(medBtnLED_pin, OUTPUT);
  pinMode(fastBtnLED_pin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(audioMonitor, INPUT);
    
  Slow_Btn.setup(slow_btn_pin, [](int state){
    if (!state){
      Serial.println("beat: Slow");
      heartSpeed = 1;
      beatDuration = 350;
      turnOffButtonLEDs();
      digitalWrite(slowBtnLED_pin,HIGH);
    }
    
  }, 60);
  Med_Btn.setup(med_btn_pin, [](int state){
    if (!state){
      Serial.println("beat: medium");
      heartSpeed = 2;
      beatDuration = 250;
      turnOffButtonLEDs();
      digitalWrite(medBtnLED_pin,HIGH);
    }
  }, 60);
  Fast_Btn.setup(fast_btn_pin, [](int state){
    if (!state){
      Serial.println("beat: fast");
      heartSpeed = 3;
      beatDuration = 190;
      turnOffButtonLEDs();
      digitalWrite(fastBtnLED_pin,HIGH);
    }
  }, 60);

  DFplayer.begin(mySerial);
  //Serial.println("Setting volume to max");
  DFplayer.volume(25);
  delay(150);

  Serial.println("SMM Heart Pacer");
  
  DFplayer.play(1); // play slow beat 70 bpm
  
  digitalWrite(slowBtnLED_pin,HIGH);  
  delay(1000); // wait for clip to play
  DFplayer.play(2); // play med beat 105 bpm
  digitalWrite(medBtnLED_pin,HIGH);
  delay(1000); 
  DFplayer.play(3); // play fast beat 140 bpm
  digitalWrite(fastBtnLED_pin,HIGH);
  delay(1000); 
   heartSpeed = 0;
}

void loop() {
  currentMillis = millis();
  audioRead = analogRead(audioMonitor);

  if (heartSpeed>0){    
    digitalWrite(ledPin, LOW);
    ledState = 0;
    Serial.println(heartSpeed);
    DFplayer.pause();
    delay(500);
    DFplayer.loop(heartSpeed);      
    delay(200);  
    is_playing = 1;
    heartSpeed = 0;
    lastPressMillis = currentMillis;
    
  }

  if ((audioRead > pulseThreshold) && (ledState == 0)){ // if a beat is detected on the audio and light is off.
    Serial.print("0");
    digitalWrite(ledPin,HIGH);   // turn on pace LED    
    prevPulseMillis = currentMillis;
    ledState = 1;
  }

  if (((currentMillis - prevPulseMillis) > beatDuration) && (ledState ==1)){   // if led has been on for lub and dub
    Serial.print("1");
    digitalWrite(ledPin,LOW);      
    ledState =0;    
  }
  
  if (((currentMillis - lastPressMillis) > timeOut) && (is_playing == 1)){       
    Serial.print("timed out"); 
    DFplayer.pause();
    delay(200);
    is_playing = 0; 
      
    digitalWrite(slowBtnLED_pin,HIGH);  
    digitalWrite(medBtnLED_pin,HIGH);
    digitalWrite(fastBtnLED_pin,HIGH);
  }

  Slow_Btn.idle();
  Med_Btn.idle();
  Fast_Btn.idle();
}

void turnOffButtonLEDs(){
  digitalWrite(slowBtnLED_pin,LOW);  
  digitalWrite(medBtnLED_pin,LOW);
  digitalWrite(fastBtnLED_pin,LOW);
}
