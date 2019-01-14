/* A program to automatically pump water from a rainwater tank to a toilet cistern.
 * Reads a level switch to determine water level and pumps until switch toggles again
 * or until a certain time limit is reached.
 * 
 * Hardware:
 *    Digital Pin 3: Motor drive (high = on)
 *    Digital Pin 5: Level switch input (to GND)
 *    Digital Pin 7: Status LED pin (resistor to GND)
 *    Digital Pin 2: Manual Pump Switch (to GND)
 */

 // Libraries and includes
 #include <avr/power.h>

// Hardware Definitions
#define motorDrivePin 5
#define levelSwitchPin 6
#define statusLEDPin 3
#define manualPumpPin 2
//#define timeOutDuration 10000
#define timeOutDuration 208000

// Enumerations
enum levelSwitchState{full, empty, unknown};
enum programStateType{filling, waiting, timedOut};

// Global variables
levelSwitchState tank = full;
programStateType programState = waiting;

// Function to read the level switch in the cistern
levelSwitchState readLevelSwitch()
{
  boolean pinState1, pinState2;
  pinState1 = digitalRead(levelSwitchPin);
  delay(100);
  pinState2 = digitalRead(levelSwitchPin);
  if(pinState1 == pinState2)
  {
    if(pinState1 == true)
    {
      return full;
    }
    else
    {
      return empty;
    }
  }
  else
  {
    return unknown;
  }
}

void manualPump()
{
  digitalWrite(motorDrivePin, HIGH);
  digitalWrite(statusLEDPin, HIGH);
  while(digitalRead(manualPumpPin) == LOW)
  {


  }
  digitalWrite(motorDrivePin, LOW);
  digitalWrite(statusLEDPin, LOW);
}

void setup()
{
  power_adc_disable();
  power_spi_disable();
  power_twi_disable();
  power_timer1_disable(); //Timer0 used by millis
  power_timer2_disable();
  pinMode(motorDrivePin, OUTPUT);
  pinMode(levelSwitchPin, INPUT_PULLUP);
  pinMode(statusLEDPin, OUTPUT);
  pinMode(manualPumpPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(manualPumpPin), manualPump, LOW);
}

void loop()
{
  switch(programState)
  {
    case waiting:
      digitalWrite(motorDrivePin, LOW);
      if(readLevelSwitch() == empty)
      {
        programState = filling;
        break;
      }
      else
      {
        digitalWrite(statusLEDPin, HIGH);
        delay(50);
        digitalWrite(statusLEDPin, LOW);
        delay(2450);
        programState = waiting;
      }
      break;
    case filling:
      unsigned long startFill;
      startFill = millis();
      digitalWrite(motorDrivePin, HIGH);
      programState = waiting;
      while(readLevelSwitch() == empty)
      {
        if(millis() >= startFill + timeOutDuration)
        {
          programState = timedOut;
          break;
        }
        digitalWrite(statusLEDPin, HIGH);
        delay(100);
        digitalWrite(statusLEDPin, LOW);
        delay(100);
      }
      digitalWrite(motorDrivePin, LOW);
      digitalWrite(statusLEDPin, LOW);
      break;
    case timedOut:
      digitalWrite(motorDrivePin, LOW);
      if(readLevelSwitch() == full)
      {
        programState = waiting;
      }
      else
      {
        digitalWrite(statusLEDPin, HIGH);
        delay(50);
        digitalWrite(statusLEDPin, LOW);
        delay(50);
      }
      break;
    default:
      break;
  }
}
