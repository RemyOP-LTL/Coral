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
 * Movement uses acceleration ramping: starts slow, speeds up
 * to peak, then slows down again at the end. This is gentler
 * on the mechanism and lets us push the peak speed higher than
 * a cold start would allow.
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
// 340 degrees = 4096 * (340/360) ≈ 3868 steps.
const int POLYP_RANGE = 3868;

// Speed settings (in microseconds between steps).
// Lower = faster. We ramp between these two values.
const int SPEED_START = 2000;  // slow start — easy for the motor to get going
const int SPEED_PEAK  = 800;   // aggressive peak — ramping lets us push past 1000

// What fraction of the move is spent ramping up / ramping down.
// 0.15 = first 15% accelerating, last 15% decelerating, 70% at peak.
const float RAMP_FRACTION = 0.15;

// Pause time (ms) when fully open or fully closed.
const int PAUSE_OPEN = 2000;
const int PAUSE_CLOSED = 1000;

// Tracks our current position in the step sequence (0-7).
int stepIndex = 0;

// Move the motor with acceleration ramping.
// Positive steps = open (clockwise), negative = close (counter-clockwise).
void moveSteps(int steps) {
  int direction = (steps > 0) ? 1 : -1;
  int totalSteps = abs(steps);
  int rampSteps = (int)(totalSteps * RAMP_FRACTION);

  // Ensure we have at least a few ramp steps.
  if (rampSteps < 10) rampSteps = 10;

  // If the move is so short that ramps would overlap, just split evenly.
  if (rampSteps * 2 > totalSteps) rampSteps = totalSteps / 2;

  for (int i = 0; i < totalSteps; i++) {
    stepIndex = (stepIndex + direction + 8) % 8;

    for (int pin = 0; pin < 4; pin++) {
      digitalWrite(motorPins[pin], halfStepSeq[stepIndex][pin]);
    }

    // Calculate delay for this step using linear interpolation.
    int stepDelay;
    if (i < rampSteps) {
      // Ramp UP: linearly decrease delay from SPEED_START to SPEED_PEAK.
      stepDelay = SPEED_START - (long)(SPEED_START - SPEED_PEAK) * i / rampSteps;
    } else if (i >= totalSteps - rampSteps) {
      // Ramp DOWN: linearly increase delay from SPEED_PEAK back to SPEED_START.
      int stepsFromEnd = totalSteps - 1 - i;
      stepDelay = SPEED_START - (long)(SPEED_START - SPEED_PEAK) * stepsFromEnd / rampSteps;
    } else {
      // Cruising at peak speed.
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
