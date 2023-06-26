
/* CODE REFERENCED FROM https://www.technolabcreation.com/heartbeat-sensor-using-arduino-heart-rate-monitor/ */
#include <LiquidCrystal_I2C.h>  // External Library
#include <Wire.h>
#include <SPI.h>
#include <SoftwareSerial.h>
#include <Vector.h>             // External Library
#include <SD.h>
#include "low_pass_filter.h"

// pins
const uint8_t pulsePin = A0;
const uint8_t CS = 10;
const uint8_t rxPin = 7;
const uint8_t txPin = 6;
const uint8_t rLED = 5;
const uint8_t piezo = 2;
 
// heart pulse data
const uint8_t numSamples = 10;
volatile Vector<uint16_t> rateVector[numSamples];
volatile int16_t  rate[numSamples];          // hold the last ten values of the IBI
volatile int16_t  IBI = 600;                 // Measures the interval between beats
volatile int16_t  BPM;                       // represents the heart rate in beats per minute
volatile uint16_t Signal;                    // Holds incoming raw analog data
volatile uint16_t Crest = 512;               // This is the peak of the pulse wave, the default value is 512
volatile uint16_t Trough = 512;              // The trough of the pulse wave, default value of 512
volatile uint16_t AMP = 100;                 // The amplitude of the pulse, Crest - Trough
volatile uint16_t Threshold = 525;           // Finds the instant moment the pulse is found

volatile boolean Pulse = false;       // If Pulse detected, changes to True
volatile boolean firstBeat = true;    // used to seed the rate array so the startup has reasonable BPM
volatile boolean secondBeat = false;  // used to seed the rate array so the startup has reasonable BPM
volatile boolean beatFound = false;   // Will be true when arduino finds the beat

volatile int32_t sampleCounter = 0;   // used to determine the timing of the pulse
volatile int32_t lastBeatTime = 0;    // used to find IBI
uint16_t *bpmData;                    // Pointer to dynamically allocate and store data

// lcd address
const uint8_t lcdAddress = 0x27;      // represents I2C address of LCD
const uint8_t lcdColumns = 16;
const uint8_t lcdRows =    2;
LiquidCrystal_I2C lcd(lcdAddress, lcdColumns, lcdRows);

// bluetooth functionality
SoftwareSerial bluetooth(txPin, rxPin);

// SD card File
File dataFile;




void setup() 
{
  Serial.begin(9600);
  bluetooth.begin(9600);
  SD.begin(CS);
  pinMode(rLED, OUTPUT);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  interruptSetup();
}


void loop() 
{
  BlueToothFunc();
  if (beatFound) 
  {
    if(BPM <= 100)
    {
      printDataToLCD(BPM);
    }
    beatFound = false;
  }
  delay(20);
}


void printDataToSerial(const char *c, uint16_t data) 
{
  Serial.print(c);
  Serial.println(data);
}


void printDataToLCD(uint16_t data) 
{
  lcd.setCursor(0, 0);
  lcd.print("Heart-Beat Found!");
  lcd.setCursor(0, 1);
  lcd.print("BPM: ");
  lcd.setCursor(5, 1);
  lcd.print(data);
  delay(300);
  lcd.clear();
}


// Bluetooth on android is not currently working, rarely it will work, must be a bluetooth issue or 
void BlueToothFunc() 
{
  while (Serial.available() > 0) 
  {
    char receivedData = Serial.read();

    switch (receivedData) 
    {
      case 'A':
        saveDataToSD(BPM);
        break;
      case 'B':
        readDataFromSD();
        break;
      case 'C':
        printDataToSerial("BPM ", BPM);
        break;
      case 'Q':
        loop();
        break;

      default:
        Serial.println("Please enter the number 'A', 'B' or 'C' and if you want to exit then 'Q'... \n");
        break;
    }
  }
}


void saveDataToSD(uint16_t data)
{
  bpmData = static_cast<uint16_t *>(malloc(sizeof(uint16_t)));  // Dynamically allocate data
  if (bpmData != NULL) {
    *bpmData = data;   // Assign BPM to dynamic variable
  }

  dataFile = SD.open("BPM.txt", FILE_WRITE);  // Open and create file in SD for writing
  if (dataFile)
  {
    Serial.println("\nWriting data to SD... \n");
    dataFile.print("Value written to SD: ");
    dataFile.println(*bpmData);  // write value to SD card
    Serial.println("Success... ");
    dataFile.close();
  }

  free(bpmData);  // free heap memory
  // delay(2000);
}


void readDataFromSD() 
{
  dataFile = SD.open("BPM.txt", FILE_READ);  // Open file for reading
  uint32_t fileSize = dataFile.size();        // Filesize
  if (dataFile) 
  {
    Serial.println("\nReading all values in SD... ");
    for (size_t i = 0; i <= fileSize; i++)  // for size in file
    {
      Serial.write(dataFile.read());  // reads all data
    }
    dataFile.close();

    Serial.println("\nData read successfully...");

  } else {
    Serial.println("\nError in reading data... ");
  }
  delay(200);
}


void interruptSetup() 
{
  // Initializes Timer2 to throw an interrupt every 2mS.
  TCCR2A = 0x02;  // DISABLE PWM ON DIGITAL PINS 3 AND 11, AND GO INTO CTC MODE
  TCCR2B = 0x06;  // DON'T FORCE COMPARE, 256 PRESCALER
  OCR2A =  0X7C;  // SET THE TOP OF THE COUNT TO 124 FOR 500Hz SAMPLE RATE
  TIMSK2 = 0x02;  // ENABLE INTERRUPT ON MATCH BETWEEN TIMER2 AND OCR2A
  sei();          // MAKE SURE GLOBAL INTERRUPTS ARE ENABLED
}


ExponentialSmoothing Low_Pass_Filter;

ISR(TIMER1_COMPA_vect)  // Interrupt Service Routine | "TIMER2" is the most suitable for arduino boards, Uses Vectors, counts to 124
{
  cli();                                       // Disable interrupts
  Signal = analogRead(pulsePin);               // read the Pulse Sensor data
  // Signal = Low_Pass_Filter.Smoothing(Signal);  // apply expoential smoothing
  sampleCounter += 2;                          // keep track of the time in mS with this variable
  size_t Beat = sampleCounter - lastBeatTime;  // monitor the time since the last beat to avoid noise

  if (Signal < Threshold && Signal < Trough) 
  {
    Trough = Signal;  // keep track of lowest point in pulse wave
  }

  if (Signal > Threshold && Signal > Crest) 
  {
    Crest = Signal;  // Keep track of the peak in the pulse wave
  }

  if (Beat > 250)    // Looking for heart beat
  {
    if ((Signal > Threshold) && (Pulse == false)) 
    {
      Pulse = true;                        // set pulse to True, when we think there may be a pulse
      digitalWrite(rLED, HIGH); 
      IBI = sampleCounter - lastBeatTime;  // measure time between beats
      lastBeatTime = sampleCounter;        // keep track for next pulse

      if (firstBeat)
      {
        firstBeat = false;
        secondBeat = true;
        sei();              // enable interrupts again
        return;             // Exit ISR prematurely as the first beat is inaccurate
      }

      if (secondBeat) 
      {
        secondBeat = false;
        for (size_t i = 0; i < numSamples; i++)  // seed the values to get a realistic BPM
        {
          rate[i] = IBI;  // required for updating the BPM
        }
      }

      uint16_t runningTotal = 0;
      for (size_t i = 0; i < numSamples - 1; i++) 
      {
        rate[i] = rate[i+1];          // remove oldest IBI value
        runningTotal += rate[i];      // add up the 9 IBI values
      }

      rate[9] = IBI;               // add latest IBI
      runningTotal += rate[9];     // add the latest IBI to runningTotal
      runningTotal /= 10;          // average the last 10 IBI values
      BPM = 60000 / runningTotal;  // how many beats can fit into a minute? that's BPM!
      beatFound = true;            // beat was found
    }
  }

  if (Signal < Threshold && Pulse == true)  // as the values are reducing, the beat has stopped
  {
    Pulse = false;                   // The pulse has stopped
    digitalWrite(rLED, LOW);
    AMP = Crest - Trough;            // Find the amplitude
    Threshold = (AMP / 2) + Trough;  // Set the threshold to 50% of the amplitude
    Crest = Threshold;               // Reset these for next time
    Trough = Threshold;
  }

  if (Beat > 3000)  // if 3 seconds go by without a beat
  {                 // set values to default
    Crest = 512;
    Trough = 512;
    Threshold = 525;
    lastBeatTime = sampleCounter;
    firstBeat = true;
    secondBeat = false;
  }
}




