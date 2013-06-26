/*
   created 2013
   by Ruben La Biunda
   CC-Licence
   
*/


// These constants won't change.  They're used to give names
// to the pins used:
const int analogInPin1 = A0;  // Analog input pin that the pressure sensor is attached to
const int analogInPin2 = A1;
const int analogOutPin1 = 5;  // Analog output pin that the LED is attached to
const int analogOutPin2 = 9;

/*  //1
    with multible sensors we need:
    a) the number of used analog-pins and 
    b) an array where to save the meassured values...
*/
const int numberOfSensors = 2;
int sensorValues[numberOfSensors];

//  c) and an array for the last meassured values...
int lastSensorValues[numberOfSensors];

//  d) and an array to store the difference for each sensor to its former value
int delta[numberOfSensors];





//2 VARIABLES FOR DEBOUNCE
long lastDebounce[numberOfSensors];  // the last time the delta was toggled
long debounceDelay = 180;    // the debounce time; increase if the output flickers



//3 VARIABLES FOR THE EFFECT-O-METER ---
// if "millis()" is the hourglass, "timer" is the depleted sand
long timer = 0;

// countdown is, how fast the sand depletes (ms)
long countdown = 10;

/* 
   instead of the zero point of an hourglass, "millis" is running on and on
   therefor this little time progress every loop needs to be calculated and
   adjusted to the timer
*/
long lastMillis = 0;  //value of the last loop
long deltaT = 0;      // difference to the last loop

// this is the remaining sand
long effectOmeter = 0;

//VARIABLES FOR THE EFFECT LEVELS
// LEVEL 0
int timeout = 2000;
int fadeLevel = 5;
int fadeAmount = 8;

// LEVEL 1
int counter = 0;


//-------------------------------------

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600); 
}

///////////////////////////////////////
// function for LV.0 - idle fade
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
// function for LV.1
void Level1(int level){
  //test effect: light up the led normal
  analogWrite(analogOutPin1, level);
}
/////////////////////////////////////// 



///////////////////////////////////////
// function for LV.2
void Level2(int level){
  level = (level-25) * 10;
  if(level > 255){
    level = 255;
  }
  analogWrite(analogOutPin1, 255);
  analogWrite(analogOutPin2, level);  
}
/////////////////////////////////////// 





void loop() {
  //1111111111111111111111111111111111//
  //meassure every sensor one after another
  for(int i = 0; i < numberOfSensors; i++){
    // read the analog in value for the current pin:
    sensorValues[i] = analogRead(i);
    
    //calculate the difference to the last value...
    delta[i] = sensorValues[i] - lastSensorValues[i];
    //and update the lastSensorValue to the current array slot
    lastSensorValues[i] = sensorValues[i];
    
    //wait some ms for the analog-digital-converter to settle
    delay(9);
    
    
    // print the results to the serial monitor:
    Serial.print("sensor " );
    Serial.print(i);
    Serial.print(" = " );
    Serial.print(sensorValues[i]);
    Serial.print("\t delta = " );                       
    Serial.println(delta[i]);  
  }
  //1111111111111111111111111111111111//
  
  //2222222222222222222222222222222222//
   // keep up the debounce time for each sensor to millis, if delta goes down
   for(int i = 0; i < numberOfSensors; i++){
    if(delta[i] <= 1){
      // reset the debouncing timer
      lastDebounce[i] = millis();
    }else if((millis() - lastDebounce[i]) >= debounceDelay){
      // else - if debounce high enough - throw some sand in hourglass (ms)
      timer = timer - 100;
    }
   }
  //2222222222222222222222222222222222//
  
  
  //3333333333333333333333333333333333//
  // (From here on the uniqe sonsors are unrelevant, so no need for loop-array-checking)
  // now lets deplete some sand
  
  //calculate the difference to the time of the last loop
  deltaT = millis() - lastMillis;
  lastMillis = millis();     
        Serial.print("lastMillis = ");
        Serial.println(lastMillis);
        
        Serial.print("deltaT = ");
        Serial.println(deltaT);
  
  //then add this difference to the timer, to balance time progress
  timer = timer + deltaT;
  
  //timer then moves towards millis (this reverse zero point)
  if(timer + countdown <= millis()){
    timer = timer + countdown;
  }
  
  //and finally we look for our effect-o-meter :)
  effectOmeter = millis() - timer;
        Serial.print("effectOmeter = ");
        Serial.println(effectOmeter);
  //3333333333333333333333333333333333//
  
  
  //4444444444444444444444444444444444//
  // here we evaluate the effect-o-meter
  counter = map(effectOmeter, 0 ,5000 ,0 ,255);
        Serial.println(counter);
  
  //and call the test function
  Level1(counter);
  
  //4444444444444444444444444444444444//
  
  Serial.println("whole loop over");
}








