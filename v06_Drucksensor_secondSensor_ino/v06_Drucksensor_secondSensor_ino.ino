
// These constants won't change.  They're used to give names
// to the pins used:
const int analogInPin1 = A0;  // Analog input pin that the potentiometer is attached to
const int analogInPin2 = A1;
const int analogOutPin1 = 5;  // Analog output pin that the LED is attached to
const int analogOutPin2 = 9;

// w/ multible sensors we need a) the number of connected sensors and b) an array
// where to save the messured values:
const int numberOfSensors = 2;
int sensorValues[numberOfSensors];
//...and the last messured values...
int lastSensorValues[numberOfSensors];
//...to calculate the delta for each sensor
int delta[numberOfSensors];

/* variables used for one-sensor-level:
int sensorValue = 0;        // value read from the pot
int lastSensorValue = 0;    // saves the last messured value
int delta = 0;              // the difference between current and last sensor value
*/

// instead of delay(10) at the end of the loop (Examples > Analog > AnalogInOutSerial),
// the last analogRead-time is saved, to not interrupting the whole program
long lastAnalogReadTime = 0;


//variables for the debounce
long lastDebounceTimes[numberOfSensors];  // the last time the delta was toggled
long debounceDelay = 120;    // the debounce time; increase if the output flickers



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
  //messure every sensor one after another - i equals the analogInPin
  for(int i = 0; i < numberOfSensors; i++){
    // read the analog in value for the current pin:
    sensorValues[i] = analogRead(i);
    
    //calculate the difference to the last value...
    delta[i] = sensorValues[i] - lastSensorValues[i];
    //and update the lastSensorValue to the current array slot
    lastSensorValues[i] = sensorValues[i];
    
    
    // change the analog out value if no bouncing:
    if(delta[i] <= 1){
      // reset the debouncing timer
      lastDebounceTimes[i] = millis();
    }
    //wait 9ms for the analog-digital-converter to settle
    delay(9);
    
    
    // print the results to the serial monitor:
    Serial.print("sensor " );
    Serial.print(i);
    Serial.print(" = " );
    Serial.print(sensorValues[i]);
    Serial.print("\t delta = " );                       
    Serial.println(delta[i]);  
  }
  Serial.println(millis());
  Serial.println("messure loop over");
  lastAnalogReadTime = millis();
 
  
  // now the arrays are freshly filled with data - time to make use of it:
  // first of all we set up a new loop to check the deltas for markable rises
  for(int i = 0; i < numberOfSensors; i++){
    
    //then we check the debounce of the current delta
    if ((millis() - lastDebounceTimes[i]) > debounceDelay) {
      // whatever the current delta is at, it's been high long enough
      // than the debounce delay, so count one "jump" that should have
      // happend right now:
      counter = counter++;
      //(counter will be reset after a while --> idle mode)
      Serial.println("counter increase");
      
      //update the timestamp to save the last "jump-time"
      timestamp = millis();
    } 
  }
  // (From here on the uniqe sonsors are unrelevant, so no need for loop-array-checking)
  
  //...now switch to the effect-level-functions and send counter
  if(counter * 10 < 255){   //few jumps trigger level 1
    modeLevel1(counter);
  }else{                    //many jumps trigger higher levels
    modeLevel2(counter);
  }
    
    
  //if some time passes w/o any movement, decrease counter and switch to idle mode
  if((millis() - timestamp) > timeout){
    if(counter > 0){
      //therefor decrease the counter for the effect level
      counter = counter--;
      
      /*
      if(counter * 10 < 255){
      modeLevel1(counter);
      }else{
        modeLevel2(counter);
      }
      */
      //let the fade-out be more slow
      
    }else{
      //and call idle function
      modeIdle();
    } 
  }
  Serial.println(millis());
  Serial.println("whole loop over");
}
