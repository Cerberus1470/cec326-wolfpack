/* Autor: Bruce Noble
    Team: Wolf Pack
    Description:
    This file runs the autonomous driving and navigation code for "Roomba" in the robot chase game
*/

/* Note that for proper use, Prox1EN needs to be controlled. 

 * Chip enable is pulled high on the VL6180 daughter board. 

*/


#include <Wire.h>     // I2C library
#include <VL6180X.h>  // distance sensor library
#include <PCA95x5.h>  // I2C to GPIO library
#include <Servo.h>    // servo library

// motor assignments
#define AIN1 0
#define AIN2 1
#define BIN1 A2
#define BIN2 8

// servo assignment
#define SERVO_PIN 9

// to try different scaling factors, change the following define.
// valid scaling factors are 1, 2, or 3.
#define SCALING 3

// start/ stop buton
#define RIGHT_BUTTON_PIN A0


VL6180X sensor;  // distance sensor object
PCA9535 muxU31;  // MUX object for U31 PCA9535
Servo myservo;   // servo object for scans


void setup() {
  Serial.begin(115200);
  delay(3000);
  Serial.println("Starting...");
  Wire.begin();

  // I2C to GPIO chip setup
  muxU31.attach(Wire, 0x20);
  muxU31.polarity(PCA95x5::Polarity::ORIGINAL_ALL);
  muxU31.direction(0x1CFF);                             // 1 is input, see schematic to get upper and lower bytes
  muxU31.write(PCA95x5::Port::P11, PCA95x5::Level::H);  // enable VL6180 distance sensor

  // motor controls setup
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);

  // servo motor setup
  myservo.attach(SERVO_PIN);

  // Distance sensor setup
  sensor.init();
  sensor.configureDefault();
  sensor.setScaling(SCALING);
  sensor.setTimeout(100);

  stop = 0;
}

unsigned long lastTime = 0;
int rightButtonState;
int rightLastButtonState = 1;

void loop() {
  unsigned long curTime = millis();
  unsigned long dt = curTime - lastTime;
  Serial.print("dt = ");
  Serial.print(dt);
  Serial.print(", ");
  //Serial.print("(Scaling = ");
  //Serial.print(sensor.getScaling());
  //Serial.print("x) ");

  uint16_t dist = sensor.readRangeSingleMillimeters();
  Serial.print(dist);
  if (sensor.timeoutOccurred()) { Serial.print(" TIMEOUT"); }

  // Pseud Code
  //1. if stop signal is changed, start/stop the robot
  rightButtonState = digitalRead(RIGHT_BUTTON_PIN);  // get button state

  if (rightButtonState != rightLastButtonState && !rightButtonState) {  // debounce
    if (stop == 1) {
      stop = 0;
    } else if (stop == 0) {
      stop = 1;
    }  // stop control
  }

  while (stop == 1) {  // run program if buton has been pushed to start
    unsigned long curTime = millis();
    unsigned long dt = curTime - lastTime;
    Serial.print("dt = ");
    Serial.print(dt);
    Serial.print(", ");


    // check te buttons inside te loop too
    rightButtonState = digitalRead(RIGHT_BUTTON_PIN);                     // get button state
    if (rightButtonState != rightLastButtonState && !rightButtonState) {  // debounce
      if (stop == 1) {
        stop = 0;
      } else if (stop == 0) {
        stop = 1;
      }  // stop control
    }

    //2. read the distance continususly until it detects an object in front of it
    myservo.write(90);
    delay(15);

    //3. start driving in a straight line
    //4. once it detects an object, use PID to slow down to a stop
    //5. once stopped, scan to the left and to the right of the robot
    //    (start right or left randomly)
    //6. turn left, turn right, or turn around
    //    (Use the gyroscope to make precise 90 deg turns)
    //7. start driving again
    //8. at random intervals, stop to change dirctions
    //    (repeat steps 5-7)

    //9. repeat until a stop signal is recieved
    //  track buttonn state for debounce;
    rightLastButtonState = rightButtonState;

    Serial.println();
    lastTime = curTime;
  }


  //  track buttonn state for debounce;
  rightLastButtonState = rightButtonState;

  Serial.println();
  lastTime = curTime;
}

void turn() {
  // turn when it gets close
  analogWrite(AIN1, 0);
  analogWrite(BIN1, 0);

  delay(500);
  analogWrite(AIN2, 255);
  analogWrite(BIN2, 255);

  delay(500);
  analogWrite(AIN2, 0);
  analogWrite(BIN2, 0);

  delay(500);
  analogWrite(AIN1, 255 / 2);
  analogWrite(BIN2, 255 / 2);

  delay(1000);
  analogWrite(AIN1, 0);
  analogWrite(BIN2, 0);

  delay(500);
}