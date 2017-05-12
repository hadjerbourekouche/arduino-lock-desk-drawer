#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <Stepper.h>

// FINGERPRINT
int getFingerprintIDez();
SoftwareSerial mySerial(2, 3); // pin #2 is IN from sensor (GREEN wire) pin #3 is OUT from arduino  (WHITE wire)
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// MOTOR 28BYJ-48
#define STEPS_PER_MOTOR_REVOLUTION 32   
#define STEPS_PER_OUTPUT_REVOLUTION 32 * 64 // 2048
Stepper small_stepper(STEPS_PER_MOTOR_REVOLUTION, 8, 10, 9, 11);
int Steps2Take;

// BUZZER
int buzzer = 7;
int i;

void setup() {                
  Serial.begin(9600);
  finger.begin(57600);

  pinMode(buzzer,OUTPUT);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1);
  }
  Serial.println("Waiting for valid finger...");
}

void loop() {
  int idFinger = getFingerprintIDez();
  if (idFinger >= 0) {
     openDrawer();
     tone(buzzer, 1000, 150);
     delay(200);
     tone(buzzer, 1000, 150);
     delay(5000);
     closeDrawer();
  }
  delay(100);
}

void openDrawer() {
  Steps2Take  =  STEPS_PER_OUTPUT_REVOLUTION / 4; // Rotate clockwise 1/4 turn  
  small_stepper.setSpeed(700);  // 700 a good max speed??
  small_stepper.step(Steps2Take);
}

void closeDrawer() {
  Steps2Take  =  - STEPS_PER_OUTPUT_REVOLUTION / 4; // Rotate against clockwise 1/4 turn 
  small_stepper.setSpeed(700);
  small_stepper.step(Steps2Take);
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  // OK success!

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }
  
  // OK converted!
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }   
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence); 
}

// returns -1 if failed, otherwise returns ID #
int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  Serial.print("Found ID #"); Serial.print(finger.fingerID); 
  Serial.print(" with confidence of "); Serial.println(finger.confidence);
  return finger.fingerID; 
}
