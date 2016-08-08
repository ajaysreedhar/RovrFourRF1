/**
 * RovrFourRF1 sketch
 * 
 * Copyright (c) 2016 Ajay Sreedhar
 * 
 * Controls movements of the mini rover RovrFour RF-1 according to
 * the directions received from the controller app over WiFi.
 * 
 * Requires AFMotor library (https://github.com/adafruit/Adafruit-Motor-Shield-library).
 * 
 * The circuit:
 *  - L293 based motor shield attached to digital pins 4, 5, 7, 8, 12.
 *  - ESP8266 WiFi module attached to pins 2 and 13.
 *  
 * Read more, https://hackaday.io/project/13059-rovrfour-rf1
 */
#include <SoftwareSerial.h>
#include <AFMotor.h>

#define DELAY 1000

/* Chars that specify directions. */
#define CHAR_FORWARD '='
#define CHAR_LEFT '<'
#define CHAR_RIGHT '>'
#define CHAR_BACKWARD '@'

/**
 * Set pins 2 and 13 as RX and TX respectively
 * for serial communiction with ESP8266 module.
 */
SoftwareSerial wifi(2, 13);

/**
 * Left motor is to be connected to port 3 
 * of the motor shield and right motor to port 4.
 */
AF_DCMotor motorLeft(3);
AF_DCMotor motorRight(4);

char c;

/**
 * Releases both the motors 
 * thus stopping the rover.
 */
void stopRover() {
  motorLeft.run(RELEASE);
  motorRight.run(RELEASE);
}

/**
 * Moves rover to either of the four directions
 * according to the character received.
 * 
 * @param d character that specifies the direction
 */
void moveRover(char d) {
  if ( d == CHAR_FORWARD ) {
    motorLeft.run(FORWARD);
    motorRight.run(FORWARD);
    return;
  }

  if (d == CHAR_BACKWARD) {
    motorLeft.run(BACKWARD);
    motorRight.run(BACKWARD);
    return;
  }

  if (d == CHAR_RIGHT) {
    motorLeft.run(FORWARD);
    motorRight.run(BACKWARD);
    return;
  }

  if (d == CHAR_LEFT) {
    motorLeft.run(BACKWARD);
    motorRight.run(FORWARD);
    return;
  }
}

void setup() {
  /* Set max motor speed. */
  motorLeft.setSpeed(255);
  motorRight.setSpeed(255);

  stopRover();

  /* Begin communication with ESP8266 module at baud 57600. */
  wifi.begin(57600);

  /* Switch to access point mode. */
  wifi.write("AT+CWMODE=2\r\n");
  delay(DELAY);

  /* Create a new access point with the name RovrFour-RF1. */
  wifi.write("AT+CWSAP=\"RovrFour-RF1\",\"\",5,0\r\n");
  delay(DELAY);

  /* Set connection mode to multiple connections. */
  wifi.write("AT+CIPMUX=1\r\n");
  delay(DELAY);

  /* Configure ESP8266 as a TCP server listening on port 8468. */
  wifi.write("AT+CIPSERVER=1,8468\r\n");
  delay(DELAY);

  /* Run the motors for 500 milliseconds to indicate the rover is ready. */
  moveRover(CHAR_FORWARD);
  delay(500);
  stopRover();

  /* Flush the software serial buffer. */
  while(wifi.available()) {
    wifi.read();
  }
}

void loop() {
  /* Read incoming characters from the ESP8266 module. */
  if (wifi.available()) {
    c = wifi.read();

    /* If the character is between ASCII codes 59 and 65, compare. */
    if (c > 59 && c < 65) {
      moveRover(c);
      delay(100);

      stopRover();
    }
  }
}

