/*
 * Coral - Arduino Project
 *
 * This is the main sketch file. The two functions below are
 * required in every Arduino program:
 *
 *   setup() - runs once when the board powers on or resets
 *   loop()  - runs over and over forever after setup finishes
 */

void setup() {
  // Start serial communication at 9600 baud so we can
  // send messages back to the computer for debugging.
  Serial.begin(9600);
  Serial.println("Hello from Coral!");
}

void loop() {
  // Your repeating code goes here.
}
