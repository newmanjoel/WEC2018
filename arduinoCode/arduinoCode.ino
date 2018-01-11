template< typename T > void roll( T output[], T input[], int n, bool dir );
template< typename D> void Print(D input[], int n);
template< typename I, typename O> void average(O &output, I input[], int n, int taps, bool dir);

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

template< typename I, typename O> void average(O &output, I input[], int n, int taps, bool dir) {
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
  output = 0;
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
  output = (O)output / taps;
}

String inputString  = "";
void setup() {
  // put your setup code here, to run once:
  int values[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  Serial.begin(9600);
  Serial.println("Orignal");
  Print(values, 10);
  Serial.println("Roll True");
  roll(values, values, 10, true);
  Print(values, 10);
  Serial.println("Roll False");
  roll(values, values, 10, false);
  roll(values, values, 10, false);
  Print(values, 10);
  float result = 0;
  LPF(result, values, 10, 3, false);
  Serial.println("Results of LPF true with a float output");
  Serial.println(result);
  int result2 = 0;
  LPF(result2, values, 10, 3, false);
  Serial.println("Results of LPF true with a int output");
  Serial.println(result2);



}

void loop() {
  // put your main code here, to run repeatedly:
  delay(500);
}
void serialEvent() {
  /*
     This class is called at the end of the loop IF there is new serial data.
     This is usefull so you dont have to check each time during the loop if there is new data.
  */
  char first = (char)Serial.read();
  wholeThing();
  Serial.print(first);
  Serial.print(inputString);
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

