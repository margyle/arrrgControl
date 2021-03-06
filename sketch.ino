

#include "Adafruit_NeoTrellis.h"

#define Y_DIM 4 //number of rows of key
#define X_DIM 16 //number of columns of keys

//rotary stuff
 #define outputA 3
 #define outputB 2
 int counter = 0; 
 int aState;
 int aLastState;  
 //end rotary

 int previous;
 int movement; //1=forward, 0=back
//create a matrix of trellis panels
Adafruit_NeoTrellis t_array[Y_DIM/4][X_DIM/4] = {
  
  { Adafruit_NeoTrellis(0x2E), Adafruit_NeoTrellis(0x36),Adafruit_NeoTrellis(0x30), Adafruit_NeoTrellis(0x3E) }
  
};

/*
If you were using a 2x2 array of NeoTrellis boards, the above lines would be:

#define Y_DIM 8 //number of rows of key
#define X_DIM 8 //number of columns of keys

//create a matrix of trellis panels
Adafruit_NeoTrellis t_array[Y_DIM/4][X_DIM/4] = {
  
  { Adafruit_NeoTrellis(0x2E), Adafruit_NeoTrellis(0x2F) },

  { Adafruit_NeoTrellis(LOWER_LEFT_I2C_ADDR), Adafruit_NeoTrellis(LOWER_RIGHT_I2C_ADDR) }
  
};
*/

//pass this matrix to the multitrellis object
Adafruit_MultiTrellis trellis((Adafruit_NeoTrellis *)t_array, Y_DIM/4, X_DIM/4);

// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return seesaw_NeoPixel::Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return seesaw_NeoPixel::Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return seesaw_NeoPixel::Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  return 0;
}

//define a callback for key presses
TrellisCallback blink(keyEvent evt){
  
  if(evt.bit.EDGE == SEESAW_KEYPAD_EDGE_RISING)
    trellis.setPixelColor(evt.bit.NUM, Wheel(map(evt.bit.NUM, 0, X_DIM*Y_DIM, 0, 255))); //on rising
  else if(evt.bit.EDGE == SEESAW_KEYPAD_EDGE_FALLING)
    trellis.setPixelColor(evt.bit.NUM, 0); //off falling
    
  trellis.show();
  return 0;
}

void setup() {
  //rotary
  pinMode (outputA,INPUT);
  pinMode (outputB,INPUT);
  //end rotary
  
  Serial.begin(9600);
  //while(!Serial);

  if(!trellis.begin()){
    Serial.println("failed to begin trellis");
    while(1);
  }
  //rotary
  aLastState = digitalRead(outputA);

  /* the array can be addressed as x,y or with the key number */
  for(int i=0; i<Y_DIM*X_DIM; i++){
      trellis.setPixelColor(i, Wheel(map(i, 0, X_DIM*Y_DIM, 0, 255))); //addressed with keynum
      trellis.show();
      delay(10);
  }
  
  for(int y=0; y<Y_DIM; y++){
    for(int x=0; x<X_DIM; x++){
      //activate rising and falling edges on all keys
      trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_RISING, true);
      trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_FALLING, true);
      trellis.registerCallback(x, y, blink);
      trellis.setPixelColor(x, y, 0x000000); //addressed with x,y
      trellis.show(); //show all LEDs
      delay(10);
    }
  }

}

void getRotary(){
    aState = digitalRead(outputA); // Reads the "current" state of the outputA
   // If the previous and the current state of the outputA are different, that means a Pulse has occured
   if (aState != aLastState){     
     // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
     if (digitalRead(outputB) != aState) { 
       counter ++;
        movement = 1;
     } else {
       movement = 0;
       counter --;
     }
     Serial.print("Position: ");
     Serial.println(counter);
   } 
   aLastState = aState; // Updates the previous state of the outputA with the current state

}

void locateButton(){
   //button select
  trellis.setPixelColor(counter, Wheel(map(counter, 0, X_DIM*Y_DIM, 0, 255))); //addressed with keynum
  trellis.show();
}

void buttonForward(){
   //button select
   previous = (counter - 1);
   trellis.setPixelColor(previous, 0x000000); //addressed with x,y
   trellis.show(); //show all LEDs
}

void buttonBack(){
   //button select
   previous = (counter + 1 );
   trellis.setPixelColor(previous, 0x000000); //addressed with x,y
   trellis.show(); //show all LEDs
}


void loop() {
  getRotary();
  //button select
  trellis.read();
  delay(20);
  
  locateButton();

  //turn off surrounding buttons
  if(movement == 1){
   buttonForward();
   }
  else if(movement == 0){
   buttonBack();
  }
  else{
  }
 char buttonNumber = counter;
}
