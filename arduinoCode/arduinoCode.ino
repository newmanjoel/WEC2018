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

int formatData(int data) {
  if (data < 10) {
    return 1000;
  }
  if (data > 1000) {
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
#define TRIG_PIN_2 10
#define ECHO_PIN_2 11
// PID
#define POT1 A0
#define POT2 A1
#define MOTOR1 8
#define MOTOR2 9
// array information
#define araryNum 50
#define averageAmount 5
#define DELAYMAX 50
#define BUTTONA 4
#define BUTTONB 5
#define BUTTONC 6
#define BUTTOND 7
// Ultrasonic Sensors
HCSR04 RawA(TRIG_PIN_1, ECHO_PIN_1, 20, 1000);
HCSR04 RawB(TRIG_PIN_2, ECHO_PIN_2, 20, 1000);

int valuesA[araryNum];
int valuesB[araryNum];
int valuesC[araryNum];
int valuesD[araryNum];
bool SensorA = false;
bool SensorB = false;
bool SensorC = false;
bool SensorD = false;

double SetpointX, InputX, OutputX;
double SetpointY, InputY, OutputY;
double Kp = 0.1, Ki = 5, Kd = 0;
PID pidx(&InputX, &OutputX, &SetpointX, Kp, Ki, Kd, DIRECT);
PID pidy(&InputY, &OutputY, &SetpointY, Kp, Ki, Kd, DIRECT);
int valuesPot1[araryNum] ;
int valuesPot2[araryNum] ;
int sensors[] = {0, 0, 0, 0};

int StateNumber = 1;
int timeout = 0;
int timeout_amount = 5000;
int offset1 = 0;
int offset2 = 0;
int delaynum = 0;

bool buttons = true;


Servo motorx;
Servo motory;

String inputString  = "";
void setup() {

  pinMode(ECHO_PIN_1, INPUT);
  pinMode(ECHO_PIN_2, INPUT);
  pinMode(BUTTONA, INPUT);
  pinMode(BUTTONB, INPUT);
  pinMode(BUTTONC, INPUT);
  pinMode(BUTTOND, INPUT);
  digitalWrite(BUTTONA,HIGH);
  digitalWrite(BUTTONB,HIGH);
  digitalWrite(BUTTONC,HIGH);
  digitalWrite(BUTTOND,HIGH);
  pinMode(POT1, INPUT);
  pinMode(POT2, INPUT);

  Serial.begin(9600);
  changeState(1);
  motorx.attach(MOTOR1);
  motory.attach(MOTOR2);
  pidx.SetMode(AUTOMATIC);
  pidy.SetMode(AUTOMATIC);
  pidx.SetOutputLimits(0, 180);
  pidy.SetOutputLimits(0, 180);
  Serial.println("SETUP IS DONE");
}

void loop() {


  roll(valuesA, valuesA, araryNum, false);
  if (buttons) {
    valuesA[araryNum - 1] = digitalRead(BUTTONA) ? 1000 : 30;
  }
  else {
    valuesA[araryNum - 1] = formatData(RawA.distanceInMillimeters());
  }

  delay(50);
  roll(valuesB, valuesB, araryNum, false);
  if (buttons) {
    valuesB[araryNum - 1] = digitalRead(BUTTONB) ? 1000 : 30;
  }
  else {
    valuesB[araryNum - 1] = formatData(RawB.distanceInMillimeters());
  }
  delay(50);
  roll(valuesPot1, valuesPot1, araryNum, false);
  valuesPot1[araryNum - 1] = map(analogRead(POT1) + offset1, 0, 1023, 0, 100);
  roll(valuesPot2, valuesPot2, araryNum, false);
  valuesPot2[araryNum - 1] = map(analogRead(POT2) + offset2, 0, 1023, 0, 100);

  if (buttons) {
    roll(valuesC, valuesC, araryNum, false);
    roll(valuesD, valuesD, araryNum, false);
    valuesC[araryNum - 1] = digitalRead(BUTTONC) ? 1000 : 30;
    valuesD[araryNum - 1] = digitalRead(BUTTOND) ? 1000 : 30;
  }


  InputX = average(valuesPot1, araryNum, 3, false);
  InputY = average(valuesPot2, araryNum, 3, false);

  SensorA = (average(valuesA, araryNum, averageAmount, true) < 40) ? true : false;
  SensorB = (average(valuesB, araryNum, averageAmount, true) < 40) ? true : false;
  SensorC = (average(valuesC, araryNum, averageAmount, true) < 40) ? true : false;
  SensorD = (average(valuesD, araryNum, averageAmount, true) < 40) ? true : false;
  sensors[0] = average(valuesA, araryNum, averageAmount, true);
  sensors[1] = average(valuesB, araryNum, averageAmount, true);
  sensors[2] = average(valuesC, araryNum, averageAmount, true);
  sensors[3] = average(valuesD, araryNum, averageAmount, true);
  Print(sensors, 4);
  Serial.print(" ");
  switch (StateNumber) {
    case 0:
    case 1:
      Serial.println("State 1");
      SetpointX = 50;
      SetpointY = 50;
      if (SensorA) {
        changeState(2);
      }
      if (SensorB) {
        changeState(5);
      }
      if (SensorC) {
        changeState(8);
      }
      if (SensorD) {
        changeState(11);
      }
      break;
    case 2:
      Serial.println("State 2");
      SetpointX = 75;
      SetpointY = 25;
      if (timeout > timeout_amount) {
        changeState(1);
      }
      if (SensorA) {
        changeState(3);
      }
      if (SensorC) {
        changeState(4);
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
        changeState(1);
      }
      // timeout
      break;
    case 4:
      Serial.println("State 4");
      SetpointX = 75;
      SetpointY = 0;
      if (timeout > timeout_amount) {
        changeState(1);
      }
      // timeout
      break;
    case 5:
      Serial.println("State 5");
      SetpointX = 25;
      SetpointY = 75;
      if (timeout > timeout_amount) {
        changeState(1);
      }
      if (SensorD) {
        changeState(6);
      }
      if (SensorB) {
        changeState(7);
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
        changeState(1);
      }
      // timeout
      break;
    case 7:
      Serial.println("State 7");
      SetpointX = 0;
      SetpointY = 75;
      if (timeout > timeout_amount) {
        changeState(1);
      }
      // timeout
      break;
    case 8:
      Serial.println("State 8");
      SetpointX = 75;
      SetpointY = 25;
      if (timeout > timeout_amount) {
        changeState(1);
      }
      if (SensorA) {
        changeState(9);
      }
      if (SensorC) {
        changeState(10);
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
        changeState(1);
      }
      // timeout
      break;
    case 10:
      Serial.println("State 10");
      SetpointX = 75;
      SetpointY = 0;
      if (timeout > timeout_amount) {
        changeState(1);
      }
      // timeout
      break;
    case 11:
      Serial.println("State 11");
      SetpointX = 25;
      SetpointY = 75;
      if (timeout > timeout_amount) {
        changeState(1);
      }
      if (SensorB) {
        changeState(12);
      }
      if (SensorD) {
        changeState(13);
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
        changeState(1);
      }
      // timeout
      break;
    case 13:
      Serial.println("State 13");
      SetpointX = 25;
      SetpointY = 100;
      if (timeout > timeout_amount) {
        changeState(1);
      }
      // timeouts
      break;
    default:
      Serial.println("IN THE DEFAULT CASE");

  };
  pidx.Compute();
  pidy.Compute();
  motorx.write(OutputX);
  motory.write(OutputY);
  timeout += 100;

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
  if (first == "Q") {
    offset1 = inputString.toInt();
  }
  if (first == "W") {
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

void changeState(int newState) {
  StateNumber = newState;
  timeout = 0;
  for (int i = 0; i < araryNum; i++) {
    valuesA[i] = 1000;
    valuesB[i] = 1000;
    valuesC[i] = 1000;
    valuesD[i] = 1000;
  }

}

