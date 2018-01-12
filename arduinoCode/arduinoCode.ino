template< typename T > void roll( T output[], T input[], int n, bool dir );
template< typename D> void Print(D input[], int n);
template< typename I> int average( I input[], int n, int taps, bool dir);

template<typename T> void roll(T output[], T  input[], int n, bool dir = true) {
  /*
     This function "spins" the array by 1 unit.
     [1,2,3,4] would result in [4,1,2,3] if dir is true
     [1,2,3,4] would result in [2,3,4,1] if dir is false

     This function will work with any data type as long as input and output are the same
  */
  if (dir) {
    T first = input[0];
    for (int i = 1; i < n; i++) {
      output[i - 1] = input[i];
    }
    output[n - 1] = first;
  }
  else {
    T last = input[n - 1];
    for (int i = n - 1; i > 0; i--) {
      output[i] = input[i - 1];
    }
    output[0] = last;
  }
}

template <typename D> void Print(D input[], int n) {
  /*
     This function prints the values of an array (of any type) to the serial in CSV format.
     This will NOT end with a new line character
  */
  for (int i = 0; i < n; i++) {
    Serial.print(input[i]);
    if (i == n - 1) {
    }
    else {
      Serial.print(",");
    }
  }
}

template< typename I> int average(I input[], int n, int taps, bool dir) {
  /*
     This function calcuates the average of the input array from an end.
     Example:
     float result = 0;
     int values[] = [1,2,3,4,5];
     average(result, values, 5, 3,true)
     //result will be 2
     average(result, values, 5, 3,false)
     // result will be 4

  */
  float output = 0;
  if (dir) {
    for (int i =  0; i < taps; i++) {
      output += input[i];
    }
  }
  else {
    for (int i =  n - 1; ((n - 1) - i) < taps; i--) {
      output += input[i];
    }
  }
  return (int)output / taps;
}

int formatData(int data){
  if(data < 10){
    return 1000;
  }
  if(data > 1000){
    return 1000;
  }
  return data;
}

#include <hcsr04.h>
#include <PID_v1.h>
#include <Servo.h>

// Sonar 1
#define TRIG_PIN_1 12
#define ECHO_PIN_1 13
// Sonar 2
#define TRIG_PIN_2 12
#define ECHO_PIN_2 13
// PID
#define POT1 A0
#define POT2 A1
#define MOTOR1 5
#define MOTOR2 6
// array information
#define araryNum 4
// Ultrasonic Sensors
HCSR04 RawA(TRIG_PIN_1, ECHO_PIN_1, 20, 1000);
HCSR04 RawB(TRIG_PIN_2, ECHO_PIN_1, 20, 1000);

int valuesA[] = {1000, 1000, 1000, 1000};
int valuesB[] = {1000, 1000, 1000, 1000};
int valuesC[] = {1000, 1000, 1000, 1000};
int valuesD[] = {1000, 1000, 1000, 1000};
bool SensorA = false;
bool SensorB = false;
bool SensorC = false;
bool SensorD = false;

double SetpointX, InputX, OutputX;
double SetpointY, InputY, OutputY;
double Kp = 0.1, Ki = 5, Kd = 0;
PID pidx(&InputX, &OutputX, &SetpointX, Kp, Ki, Kd, DIRECT);
PID pidy(&InputY, &OutputY, &SetpointY, Kp, Ki, Kd, DIRECT);
int valuesPot1[] = {1000, 1000, 1000, 1000};
int valuesPot2[] = {1000, 1000, 1000, 1000};

int StateNumber = 1;
int timeout = 0;
int timeout_amount = 5000;
int offset1 = 0;
int offset2 = 0;

Servo motorx;
Servo motory;

String inputString  = "";
void setup() {
  Serial.begin(9600);
  motorx.attach(MOTOR1);
  motory.attach(MOTOR2);
  pidx.SetMode(AUTOMATIC);
  pidy.SetMode(AUTOMATIC);
  pidx.SetOutputLimits(0, 180);
  pidy.SetOutputLimits(0, 180);
}

void loop() {
  roll(valuesA, valuesA, araryNum, false);
  valuesA[araryNum - 1] = formatData(RawA.distanceInMillimeters());
  roll(valuesB, valuesB, araryNum, false);
  valuesB[araryNum - 1] = RawB.distanceInMillimeters();
  roll(valuesPot1, valuesPot1, araryNum, false);
  valuesPot1[araryNum - 1] = map(analogRead(POT1)+offset1, 0, 1023, 0, 100);
  roll(valuesPot2, valuesPot2, araryNum, false);
  valuesPot2[araryNum - 1] = map(analogRead(POT2)+offset2, 0, 1023, 0, 100);

  InputX = average(valuesPot1, araryNum, 3, false);
  InputY = average(valuesPot2, araryNum, 3, false);
  
  SensorA = (average(valuesA, araryNum, 3, false) < 40) ? true : false;
  SensorB = (average(valuesB, araryNum, 3, false) < 40) ? true : false;
  SensorC = (average(valuesC, araryNum, 3, false) < 40) ? true : false;
  SensorD = (average(valuesD, araryNum, 3, false) < 40) ? true : false;


  switch (StateNumber) {
    case 1:
      Serial.println("State 1");
      SetpointX = 50;
      SetpointY = 50;
      if (SensorA) {
        StateNumber = 2;
      }
      if (SensorB) {
        StateNumber = 5;
      }
      if (SensorC) {
        StateNumber = 8;
        timeout = 0;
      }
      if (SensorD) {
        StateNumber = 11;
      }
      break;
    case 2:
      Serial.println("State 2");
      SetpointX = 75;
      SetpointY = 25;
      if (timeout > timeout_amount) {
        StateNumber = 1;
        timeout = 0;
      }
      if (SensorA) {
        StateNumber = 3;
        timeout = 0;
      }
      if (SensorC) {
        StateNumber = 4;
        timeout = 0;
      }
      // timeout
      // state 3
      // state 4
      break;
    case 3:
      Serial.println("State 3");
      SetpointX = 100;
      SetpointY = 25;
      if (timeout > timeout_amount) {
        StateNumber = 1;
        timeout = 0;
      }
      // timeout
      break;
    case 4:
      Serial.println("State 4");
      SetpointX = 75;
      SetpointY = 0;
      if (timeout > timeout_amount) {
        StateNumber = 1;
        timeout = 0;
      }
      // timeout
      break;
    case 5:
      Serial.println("State 5");
      SetpointX = 25;
      SetpointY = 75;
      if (timeout > timeout_amount) {
        StateNumber = 1;
        timeout = 0;
      }
      if (SensorD) {
        StateNumber = 6;
        timeout = 0;
      }
      if (SensorB) {
        StateNumber = 7;
        timeout = 0;
      }
      // timeout
      // state 6
      // state 7
      break;
    case 6:
      Serial.println("State 6");
      SetpointX = 25;
      SetpointY = 100;
      if (timeout > timeout_amount) {
        StateNumber = 1;
        timeout = 0;
      }
      // timeout
      break;
    case 7:
      Serial.println("State 7");
      SetpointX = 0;
      SetpointY = 75;
      if (timeout > timeout_amount) {
        StateNumber = 1;
        timeout = 0;
      }
      // timeout
      break;
    case 8:
      Serial.println("State 8");
      SetpointX = 75;
      SetpointY = 25;
      if (timeout > timeout_amount) {
        StateNumber = 1;
        timeout = 0;
      }
      if (SensorA) {
        StateNumber = 9;
        timeout = 0;
      }
      if (SensorC) {
        StateNumber = 10;
        timeout = 0;
      }
      // timeout
      // state 9
      // state 10
      break;
    case 9:
      Serial.println("State 9");
      SetpointX = 100;
      SetpointY = 25;
      if (timeout > timeout_amount) {
        StateNumber = 1;
        timeout = 0;
      }
      // timeout
      break;
    case 10:
      Serial.println("State 10");
      SetpointX = 75;
      SetpointY = 0;
      if (timeout > timeout_amount) {
        StateNumber = 1;
        timeout = 0;
      }
      // timeout
      break;
    case 11:
      Serial.println("State 11");
      SetpointX = 25;
      SetpointY = 75;
      if (timeout > timeout_amount) {
        StateNumber = 1;
        timeout = 0;
      }
      if (SensorB) {
        StateNumber = 12;
        timeout = 0;
      }
      if (SensorD) {
        StateNumber = 13;
        timeout = 0;
      }
      // timeout
      // state 12
      // state 13
      break;
    case 12:
      Serial.println("State 12");
      SetpointX = 0;
      SetpointY = 75;
      if (timeout > timeout_amount) {
        StateNumber = 1;
        timeout = 0;
      }
      // timeout
      break;
    case 13:
      Serial.println("State 13");
      SetpointX = 25;
      SetpointY = 100;
      if (timeout > timeout_amount) {
        StateNumber = 1;
        timeout = 0;
      }
      // timeouts
      break;

  };
  pidx.Compute();
  pidy.Compute();
  motorx.write(OutputX);
  motory.write(OutputY);

}
void serialEvent() {
  /*
     This class is called at the end of the loop IF there is new serial data.
     This is usefull so you dont have to check each time during the loop if there is new data.
  */
  char first = (char)Serial.read();
  wholeThing();
  if (first == "C") {
    roll(valuesC, valuesC, araryNum, false);
    valuesC[araryNum - 1] = inputString.toInt();
  }
  if (first == "D") {
    roll(valuesD, valuesD, araryNum, false);
    valuesD[araryNum - 1] = inputString.toInt();
  }
  if(first =="Q"){
    offset1 = inputString.toInt();
  }
  if(first =="W"){
    offset2 = inputString.toInt();
  }
}

void wholeThing() {
  /* Helper function for getting a whole line of data from the serial line */
  inputString = "";
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
  }
}

