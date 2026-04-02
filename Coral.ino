/*
 * Coral Polyp Prototype
 *
 * Drives a 28BYJ-48 stepper motor through a ULN2003 driver
 * to open and close a polyp mechanism (like a flower blooming).
 *
 * WIRING (ULN2003 driver board → Mega 2560):
 *   IN1 → Pin 8
 *   IN2 → Pin 9
 *   IN3 → Pin 10
 *   IN4 → Pin 11
 *   VCC → 5V
 *   GND → GND
 *
 * The 28BYJ-48 has 2048 steps per full revolution (in half-step mode).
 * We use a portion of that range to open/close the polyp.
 */

#include <Stepper.h>

// The 28BYJ-48 has 2048 steps for a full 360° rotation.
// We only need a partial rotation to open/close the polyp.
const int STEPS_PER_REV = 2048;

// How far the polyp opens (in steps). 512 steps = 90 degrees.
// Adjust this to match your mechanism's range of motion.
const int POLYP_RANGE = 512;

// Speed in RPM — the 28BYJ-48 maxes out around 12-15 RPM.
const int MOTOR_SPEED = 10;

// Pause time (ms) when fully open or fully closed.
const int PAUSE_OPEN = 2000;
const int PAUSE_CLOSED = 1000;

// ULN2003 driver pins — note the pin order is IN1, IN3, IN2, IN4.
// This order matters! The 28BYJ-48 has a non-standard wiring sequence.
Stepper stepper(STEPS_PER_REV, 8, 10, 9, 11);

void setup() {
  Serial.begin(9600);
  stepper.setSpeed(MOTOR_SPEED);
  Serial.println("Coral Polyp Prototype");
  Serial.println("Opening and closing polyp...");
}

void loop() {
  // OPEN the polyp — rotate forward
  Serial.println("Opening...");
  stepper.step(POLYP_RANGE);
  delay(PAUSE_OPEN);

  // CLOSE the polyp — rotate backward
  Serial.println("Closing...");
  stepper.step(-POLYP_RANGE);
  delay(PAUSE_CLOSED);
}
