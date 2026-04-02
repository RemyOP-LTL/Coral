/*
 * Coral Polyp Prototype
 *
 * Drives a 28BYJ-48 stepper motor through a ULN2003 driver.
 * Two modes toggled by a tactile button:
 *   - POLYP MODE: opens and closes with ramping (default, LED off)
 *   - SPIN MODE: continuous clockwise rotation (LED on)
 *
 * WIRING:
 *
 *   ULN2003 driver board -> Mega 2560:
 *     IN1 -> Pin 8
 *     IN2 -> Pin 9
 *     IN3 -> Pin 10
 *     IN4 -> Pin 11
 *     VCC -> 5V
 *     GND -> GND
 *
 *   Tactile switch -> Mega 2560:
 *     One leg  -> Pin 2
 *     Other leg -> GND
 *     (No resistor needed — we use the Mega's built-in pull-up)
 *
 *   Mode indicator LED -> Mega 2560:
 *     Long leg (anode/+)  -> 220 ohm resistor -> Pin 13
 *     Short leg (cathode/-) -> GND
 *     (Pin 13 also has a built-in LED on the Mega board itself,
 *      so you'll see it there even without wiring an external LED)
 */

// Motor control pins
const int motorPins[4] = {8, 9, 10, 11};

// Button on pin 2 (has hardware interrupt support on the Mega).
const int BUTTON_PIN = 2;

// LED on pin 13 — also the Mega's built-in LED.
const int LED_PIN = 13;

// Full-step sequence — two coils energized at once for max torque.
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
const int SPEED_START = 2500;
const int SPEED_PEAK  = 1000;

// Number of steps to ramp up when entering spin mode.
const int SPIN_RAMP_STEPS = 200;

// Ramp fraction — 25% accel at start, 25% decel at end.
const float RAMP_FRACTION = 0.25;

// Pause time (ms) when fully open or fully closed.
const int PAUSE_OPEN = 2000;
const int PAUSE_CLOSED = 1000;

// Tracks our current position in the step sequence (0-3).
int stepIndex = 0;

// Mode toggle: false = polyp mode, true = spin mode.
volatile bool spinMode = false;

// Tracks whether spin mode has finished ramping up.
bool spinRamped = false;

// Debounce — ignore button presses within this window.
volatile unsigned long lastButtonPress = 0;
const unsigned long DEBOUNCE_MS = 250;

// Called automatically when button is pressed (interrupt).
void buttonISR() {
  unsigned long now = millis();
  if (now - lastButtonPress > DEBOUNCE_MS) {
    spinMode = !spinMode;
    lastButtonPress = now;
  }
}

// Take a single step in the given direction.
void singleStep(int direction) {
  stepIndex = (stepIndex + direction + NUM_STEPS) % NUM_STEPS;
  for (int pin = 0; pin < 4; pin++) {
    digitalWrite(motorPins[pin], fullStepSeq[stepIndex][pin]);
  }
}

// Move the motor with acceleration ramping.
void moveSteps(int steps) {
  int direction = (steps > 0) ? 1 : -1;
  int totalSteps = abs(steps);
  int rampSteps = (int)(totalSteps * RAMP_FRACTION);

  if (rampSteps < 10) rampSteps = 10;
  if (rampSteps * 2 > totalSteps) rampSteps = totalSteps / 2;

  for (int i = 0; i < totalSteps; i++) {
    if (spinMode) return;

    singleStep(direction);

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

  // Turn off all coils when done.
  for (int pin = 0; pin < 4; pin++) {
    digitalWrite(motorPins[pin], LOW);
  }
}

// Ramp up to full speed for spin mode.
void spinRampUp() {
  for (int i = 0; i < SPIN_RAMP_STEPS; i++) {
    if (!spinMode) return;
    singleStep(1);
    int stepDelay = SPEED_START - (long)(SPEED_START - SPEED_PEAK) * i / SPIN_RAMP_STEPS;
    delayMicroseconds(stepDelay);
  }
  spinRamped = true;
}

void setup() {
  Serial.begin(9600);

  for (int i = 0; i < 4; i++) {
    pinMode(motorPins[i], OUTPUT);
  }

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonISR, FALLING);

  Serial.println("Coral Polyp Prototype");
  Serial.println("Press button to toggle polyp/spin mode");
}

void loop() {
  // Update LED to reflect current mode.
  digitalWrite(LED_PIN, spinMode ? HIGH : LOW);

  if (spinMode) {
    // SPIN MODE: ramp up first time, then cruise.
    if (!spinRamped) {
      spinRampUp();
    }
    singleStep(1);
    delayMicroseconds(SPEED_PEAK);
  } else {
    // Reset ramp flag when leaving spin mode.
    spinRamped = false;

    // POLYP MODE: open and close with ramping.
    Serial.println("Opening...");
    moveSteps(POLYP_RANGE);
    if (spinMode) return;
    delay(PAUSE_OPEN);

    if (spinMode) return;
    Serial.println("Closing...");
    moveSteps(-POLYP_RANGE);
    if (spinMode) return;
    delay(PAUSE_CLOSED);
  }
}
