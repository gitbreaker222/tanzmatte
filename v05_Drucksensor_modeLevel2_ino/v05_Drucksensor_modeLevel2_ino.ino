
// These constants won't change.  They're used to give names
// to the pins used:
const int analogInPin = A0;  // Analog input pin that the potentiometer is attached to
const int analogOutPin1 = 9;  // Analog output pin that the LED is attached to
const int analogOutPin2 = 5;

// instead of delay(10) at the end of the loop, the last analogRead-time
// is saved, to settle the analog-digital converter w/o interrupting the whole program
long lastAnalogReadTime = 0;


int sensorValue = 0;        // value read from the pot
int lastSensorValue = 0;    // saves the last messured value
int delta = 0;              // the difference between current and last sensor value
int lastDelta = 0;
int outputValue = 0;        // value output to the PWM (analog out)

//variables for the debounce
long lastDebounceTime = 0;  // the last time the delta was toggled
long debounceDelay = 40;    // the debounce time; increase if the output flickers



//VARIABLES FOR THE DIFFERENT MODES----
long timestamp = 0;

//modeIdle
int timeout = 2000;
int fadeLevel = 5;
int fadeAmount = 8;

//modeLevel1
int counter = 0;


//-------------------------------------

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
}

///////////////////////////////////////
// function for the idle mode
void modeIdle(){
  //swich direction of the fade amount, if borders are reached
  if(fadeLevel >= 255 || fadeLevel <= 0){
    fadeAmount = -fadeAmount;
    //if border also passed, it's nessasary to go back before writing the wrong signal
    fadeLevel = fadeLevel + fadeAmount;
  }

  //write the signalamount
  analogWrite(analogOutPin1, fadeLevel);
  //calculate next level
  fadeLevel = fadeLevel + fadeAmount;
  //turn unnessesary light off
  
  analogWrite(analogOutPin2, 0);
}
///////////////////////////////////////




///////////////////////////////////////
// function for the first effect level
void modeLevel1(int level){
  level = level * 10;
  if(level > 255){
    level = 255;
  }
  analogWrite(analogOutPin1, level);
}
/////////////////////////////////////// 



///////////////////////////////////////
// function for the second effect level
void modeLevel2(int level){
  level = (level-25) * 10;
  if(level > 255){
    level = 255;
  }
  analogWrite(analogOutPin1, 255);
  analogWrite(analogOutPin2, level);  
}
/////////////////////////////////////// 





void loop() {
  // wait 10 milliseconds before the next messure
  // for the analog-to-digital converter to settle
  // after the last reading:
  if((millis() - lastAnalogReadTime) > 10){
    
    // read the analog in value:
    sensorValue = analogRead(analogInPin);
    //calculate the difference to the last value
    delta = sensorValue - lastSensorValue;
    lastSensorValue = sensorValue;
    lastAnalogReadTime = millis();
    

  
    /*
    // map it to the range of the analog out:
    outputValue = map(sensorValue, 0, 80, 0, 255);
    */
    
    // change the analog out value if no bouncing:
    if(delta <= 1){
      // reset the debouncing timer
      lastDebounceTime = millis();
    }
    
  }  
  
  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the delta is at, it's been higher long enough
    // than the debounce delay, so switch to mode Level 1:
    
    //...and count the "number of jumps" for increasing effects
    //(counter will be reset after a while --> idle mode)
    counter = counter++;
    //...now switch to the funcion and send counter"
    if(counter * 10 < 255){
      modeLevel1(counter);
    }else{
      modeLevel2(counter);
    }
    
    //update the timestamp
    timestamp = millis();
    Serial.print("!!! ");
    Serial.print(counter);
  //if some time passes w/o any movement, decrease and switch to idle mode
  }else if((millis() - timestamp) > timeout){
    if(counter > 0){
      //therefor decrease the counter for the effect level
      counter = counter--;
      if(counter * 10 < 255){
      modeLevel1(counter);
      }else{
        modeLevel2(counter);
      }
    }else{
      //and call idle function
      modeIdle();
    } 
}
    
  

  // print the results to the serial monitor:
  Serial.print("\t sensor = " );                       
  Serial.println(sensorValue);
  Serial.print("\t delta = " );                       
  Serial.print(delta);  
  /*
  Serial.print("\t output = ");      
  Serial.println(outputValue);   
*/

  
}

