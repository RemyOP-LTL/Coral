/*
 * Coral Polyp Prototype
 *
 * Drives a 28BYJ-48 stepper motor through a ULN2003 driver
 * to open and close a polyp mechanism (like a flower blooming).
 *
 * Uses FULL-STEP drive for maximum torque and speed. Each step
 * energizes two coils at once, giving stronger holding force.
 * Full-step = 2048 steps per revolution (vs 4096 in half-step).
 * Less precise, but we don't need precision for this mechanism.
 *
 * Movement uses acceleration ramping for smooth starts/stops.
 *
 * WIRING (ULN2003 driver board -> Mega 2560):
 *   IN1 -> Pin 8
 *   IN2 -> Pin 9
 *   IN3 -> Pin 10
 *   IN4 -> Pin 11
 *   VCC -> 5V
 *   GND -> GND
 */

// Motor control pins
const int motorPins[4] = {8, 9, 10, 11};

// Full-step sequence — two coils energized at once for max torque.
// Only 4 steps in the sequence (vs 8 for half-step).
const int NUM_STEPS = 4;
const int fullStepSeq[4][4] = {
  {1, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 1},
  {1, 0, 0, 1}
};

// 2048 full-steps = one full 360° rotation for the 28BYJ-48.
// 340 degrees = 2048 * (340/360) ≈ 1934 steps.
const int POLYP_RANGE = 1934;

// Speed settings (in microseconds between steps).
// Full-step can go faster than half-step since each step
// covers more angle. Pushing peak to 600us.
const int SPEED_START = 1500;
const int SPEED_PEAK  = 600;

// Ramp fraction — 15% accel at start, 15% decel at end.
const float RAMP_FRACTION = 0.15;

// Pause time (ms) when fully open or fully closed.
const int PAUSE_OPEN = 2000;
const int PAUSE_CLOSED = 1000;

// Tracks our current position in the step sequence (0-3).
int stepIndex = 0;

// Move the motor with acceleration ramping.
void moveSteps(int steps) {
  int direction = (steps > 0) ? 1 : -1;
  int totalSteps = abs(steps);
  int rampSteps = (int)(totalSteps * RAMP_FRACTION);

  if (rampSteps < 10) rampSteps = 10;
  if (rampSteps * 2 > totalSteps) rampSteps = totalSteps / 2;

  for (int i = 0; i < totalSteps; i++) {
    stepIndex = (stepIndex + direction + NUM_STEPS) % NUM_STEPS;

    for (int pin = 0; pin < 4; pin++) {
      digitalWrite(motorPins[pin], fullStepSeq[stepIndex][pin]);
    }

    int stepDelay;
    if (i < rampSteps) {
      stepDelay = SPEED_START - (long)(SPEED_START - SPEED_PEAK) * i / rampSteps;
    } else if (i >= totalSteps - rampSteps) {
      int stepsFromEnd = totalSteps - 1 - i;
      stepDelay = SPEED_START - (long)(SPEED_START - SPEED_PEAK) * stepsFromEnd / rampSteps;
    } else {
      stepDelay = SPEED_PEAK;
    }

    delayMicroseconds(stepDelay);
  }

  // Turn off all coils when done to save power and reduce heat.
  for (int pin = 0; pin < 4; pin++) {
    digitalWrite(motorPins[pin], LOW);
  }
}

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 4; i++) {
    pinMode(motorPins[i], OUTPUT);
  }

  Serial.println("Coral Polyp Prototype - Full Step");
  Serial.println("Opening and closing polyp...");
}

void loop() {
  Serial.println("Opening...");
  moveSteps(POLYP_RANGE);
  delay(PAUSE_OPEN);

  Serial.println("Closing...");
  moveSteps(-POLYP_RANGE);
  delay(PAUSE_CLOSED);
}
