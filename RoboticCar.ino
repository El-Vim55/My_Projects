#include <Servo.h>
#include <LiquidCrystal.h>

// General Pins
#define PIEZO 8
#define RLED 15
#define YLED 16
#define GLED 17
#define ECHOPIN 18
#define TRIGPIN 19

// Motor Pins
#define RIGHT_FORWARD 3
#define LEFT_FORWARD 4
#define RIGHT_BACKWARD 2
#define LEFT_BACKWARD 7
#define MOTOR_A 6
#define MOTOR_B 5

// Setup Servo
Servo myServo;

// UltraSonic Variables
float ping, targetDistance;

// LCD Pins
const int rs = 10, en = 0, d4 = 12, d5 = 13, d6 = 14, d7 = 11;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// int ledState = LOW;
// unsigned long currentmillis = millis();
// unsigned long previousMillis = 0;
// const long interval 1000;


// Setup
void setup() {
  // Motor PinMode
  pinMode(RIGHT_FORWARD, OUTPUT);
  pinMode(RIGHT_BACKWARD, OUTPUT);
  pinMode(LEFT_FORWARD, OUTPUT);
  pinMode(LEFT_BACKWARD, OUTPUT);
  pinMode(MOTOR_A, OUTPUT);
  pinMode(MOTOR_B, OUTPUT);

  // LED PinMode
  pinMode(RLED, OUTPUT);
  pinMode(YLED, OUTPUT);
  pinMode(GLED, OUTPUT);

  // LED Setup
  digitalWrite(RLED, LOW);
  digitalWrite(YLED, LOW);
  digitalWrite(GLED, LOW);

  // UltraSonic PinMode
  pinMode(ECHOPIN, INPUT);
  pinMode(TRIGPIN, OUTPUT);

  // Piezo PinMode
  pinMode(PIEZO, OUTPUT);

  // Servo PinMode & Setup
  myServo.attach(9);
  myServo.write(64);

  // LCD Setup
  lcd.begin(16,2);

  // Baud Rate
  Serial.begin(9600);
}


// LOOP
void loop() {
  Leds();
  Movement();
}


int Distance() {
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, LOW);
  ping = pulseIn(ECHOPIN, HIGH);
  targetDistance = ping / 29.1 / 2.0;  // in cm
  // Serial.println(targetDistance);
  delay(70);
  return targetDistance;
}


// LED and Piezo function
void Leds() {
  if (Distance() > 17) {
    digitalWrite(GLED, HIGH);
    digitalWrite(YLED, LOW);
    digitalWrite(RLED, LOW);
    noTone(PIEZO);
  }

  else if (Distance() >= 7 && Distance() <= 17) {
    digitalWrite(GLED, HIGH);
    digitalWrite(YLED, HIGH);
    digitalWrite(RLED, LOW);
    tone(PIEZO, 250);
  }

  else if (Distance() >= 0 && Distance() < 7) {
    digitalWrite(GLED, HIGH);
    digitalWrite(YLED, HIGH);
    digitalWrite(RLED, HIGH);
    tone(PIEZO, 550);
  }
}


int starttime = millis();
int endtime = starttime;

// Car Movement
void Car_Forward() {
  Stop();
  digitalWrite(RIGHT_FORWARD, HIGH);
  digitalWrite(LEFT_FORWARD, HIGH);
  digitalWrite(MOTOR_A, 60);
  digitalWrite(MOTOR_B, 60);
}

void Car_Back() {
  Stop();
  digitalWrite(RIGHT_BACKWARD, HIGH);
  digitalWrite(LEFT_BACKWARD, HIGH);
  digitalWrite(MOTOR_A, 15);
  digitalWrite(MOTOR_B, 15);
}

void Turn_Right() {
  do {
  digitalWrite(LEFT_FORWARD, HIGH);
  digitalWrite(RIGHT_BACKWARD, HIGH);
  digitalWrite(MOTOR_A, 255);
  digitalWrite(MOTOR_B, 65);
  } while(Distance() <= 17);
}

void Turn_Left() {
  do {
  digitalWrite(RIGHT_FORWARD, HIGH);
  digitalWrite(LEFT_BACKWARD, HIGH);
  digitalWrite(MOTOR_A, 65);
  digitalWrite(MOTOR_B, 255);
  } while(Distance() <= 17);
}

void Stop() {
  digitalWrite(RIGHT_FORWARD, LOW);
  digitalWrite(LEFT_FORWARD, LOW);
  digitalWrite(RIGHT_BACKWARD, LOW);
  digitalWrite(LEFT_BACKWARD, LOW);
  digitalWrite(MOTOR_A, 0);
  digitalWrite(MOTOR_B, 0);

}


// Servo Look and Turn Left, Right
void lookLeftRight() {
  Stop();
  int pos[] = {64, 150, 0, 64};
  float newDistance[3];

  while (Distance() >= 8 && Distance() <= 17) {
    for (int i = 0; i < 4; i++) {
      delay(1000);
      myServo.write(pos[i]);
      delay(5);
      newDistance[i-1] = Distance();
    }

    float dist1 = newDistance[1];
    float dist2 = newDistance[2];

    if ((dist1 + dist2) <= 15) {
      Car_Back();
    } else if (dist1 >= dist2) {
      Turn_Left();
    } else if (dist2 >= dist1) {
      Turn_Right();
    }else {
      Car_Back();
    }
  }
}


void Movement() {
  if (Distance() > 17) {
    Car_Forward();
  } else if (Distance() >= 7 && Distance() <= 17) {
    lookLeftRight();
  } else if (Distance() >= 0 && Distance() < 7) {
    Car_Back();
  }
