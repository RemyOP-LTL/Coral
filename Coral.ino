/*
 * Coral Polyp Prototype
 *
 * Drives a 28BYJ-48 stepper motor through a ULN2003 driver
 * to open and close a polyp mechanism (like a flower blooming).
 *
 * Instead of using the Stepper library, we drive the motor
 * directly using a "half-step" sequence. A stepper motor works
 * by energizing coils in a specific order — each change moves
 * the shaft one tiny step. The 28BYJ-48 takes 4096 half-steps
 * for one full 360° rotation.
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

// Half-step sequence — each row is one step, telling which coils are ON.
// This gives smoother motion and double the resolution vs full-step.
const int halfStepSeq[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

// 4096 half-steps = one full 360° rotation for the 28BYJ-48.
// 1024 half-steps = 90 degrees. Adjust to fit your mechanism.
const int POLYP_RANGE = 1024;

// Delay between steps in microseconds — controls speed.
// Lower = faster. The 28BYJ-48 stalls below ~1000us.
const int STEP_DELAY_US = 1500;

// Pause time (ms) when fully open or fully closed.
const int PAUSE_OPEN = 2000;
const int PAUSE_CLOSED = 1000;

// Tracks our current position in the step sequence (0-7).
int stepIndex = 0;

// Move the motor a given number of steps.
// Positive = open (clockwise), negative = close (counter-clockwise).
void moveSteps(int steps) {
  int direction = (steps > 0) ? 1 : -1;
  steps = abs(steps);

  for (int i = 0; i < steps; i++) {
    stepIndex = (stepIndex + direction + 8) % 8;

    for (int pin = 0; pin < 4; pin++) {
      digitalWrite(motorPins[pin], halfStepSeq[stepIndex][pin]);
    }

    delayMicroseconds(STEP_DELAY_US);
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

  Serial.println("Coral Polyp Prototype");
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
