// Pins moteur 1
const int RC1_PIN = 2;
const int STEP1_PIN = 9;
const int DIR1_PIN = 8;

// Pins moteur 2
const int RC2_PIN = 3;
const int STEP2_PIN = 11;
const int DIR2_PIN = 10;

// Variables RC canal 1
volatile unsigned long pulse1Start = 0;
volatile unsigned long pulse1Width = 1500;

// Variables RC canal 2
volatile unsigned long pulse2Start = 0;
volatile unsigned long pulse2Width = 1500;

// Fréquences actuelles pour rampes
float currentFreq1 = 0;
float currentFreq2 = 0;

// Directions actuelles
int currentDir1 = 0;
int currentDir2 = 0;

// Paramètres d'accélération
const float ACCEL_RATE = 200.0;

void setup() {
  pinMode(RC1_PIN, INPUT);
  pinMode(STEP1_PIN, OUTPUT);
  pinMode(DIR1_PIN, OUTPUT);
  
  pinMode(RC2_PIN, INPUT);
  pinMode(STEP2_PIN, OUTPUT);
  pinMode(DIR2_PIN, OUTPUT);
  
  attachInterrupt(digitalPinToInterrupt(RC1_PIN), readRC1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(RC2_PIN), readRC2, CHANGE);
  
  // Timer1 - Mode 14: Fast PWM, TOP = ICR1
  TCCR1A = (1 << COM1A1) | (1 << WGM11);
  TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);
  ICR1 = 65535;
  OCR1A = 0;
  
  // Timer2 - Arrêté au départ
  TCCR2A = 0;
  TCCR2B = 0;
  OCR2A = 255;
}

void readRC1() {
  if (digitalRead(RC1_PIN) == HIGH) {
    pulse1Start = micros();
  } else {
    pulse1Width = micros() - pulse1Start;
  }
}

void readRC2() {
  if (digitalRead(RC2_PIN) == HIGH) {
    pulse2Start = micros();
  } else {
    pulse2Width = micros() - pulse2Start;
  }
}

float rampFrequency(float current, float target, float rate) {
  if (current < target) {
    current += rate;
    if (current > target) current = target;
  } else if (current > target) {
    current -= rate;
    if (current < target) current = target;
  }
  return current;
}

void updateMotor1() {
  int rcValue = constrain(pulse1Width, 1000, 2000);
  int ecart = rcValue - 1500;
  
  float targetFreq = 0;
  int targetDir = currentDir1;
  
  if (abs(ecart) >= 50) {
    targetDir = (ecart > 0) ? 1 : -1;
    int ecartAbs = abs(ecart);
    targetFreq = map(ecartAbs, 50, 500, 200, 10000);
    targetFreq = constrain(targetFreq, 200, 10000);
  }
  
  // Si changement de direction, d'abord décélérer à 0
  if (targetDir != currentDir1 && currentFreq1 > 50) {
    targetFreq = 0;
  } else {
    currentDir1 = targetDir;
    digitalWrite(DIR1_PIN, currentDir1 > 0 ? HIGH : LOW);
  }
  
  currentFreq1 = rampFrequency(currentFreq1, targetFreq, ACCEL_RATE);
  
  if (currentFreq1 < 50) {
    OCR1A = 0;
    currentFreq1 = 0;
    return;
  }
  
  unsigned int top = 2000000UL / (unsigned long)currentFreq1;
  ICR1 = top;
  OCR1A = 10;
}

void updateMotor2() {
  int rcValue = constrain(pulse2Width, 1000, 2000);
  int ecart = rcValue - 1500;
  
  float targetFreq = 0;
  int targetDir = currentDir2;
  
  if (abs(ecart) >= 50) {
    targetDir = (ecart > 0) ? 1 : -1;
    int ecartAbs = abs(ecart);
    targetFreq = map(ecartAbs, 50, 500, 244, 10000);
    targetFreq = constrain(targetFreq, 244, 10000);
  }
  
  // Si changement de direction, d'abord décélérer à 0
  if (targetDir != currentDir2 && currentFreq2 > 50) {
    targetFreq = 0;
  } else {
    currentDir2 = targetDir;
    digitalWrite(DIR2_PIN, currentDir2 > 0 ? HIGH : LOW);
  }
  
  currentFreq2 = rampFrequency(currentFreq2, targetFreq, ACCEL_RATE);
  
  if (currentFreq2 < 50) {
    TCCR2B = 0;
    TCCR2A = 0;
    digitalWrite(STEP2_PIN, LOW);
    currentFreq2 = 0;
    return;
  }
  
  // Prescaler 64 pour fréquences plus élevées
  unsigned long ocr = 125000UL / (unsigned long)currentFreq2 - 1;
  ocr = constrain(ocr, 2, 255);
  
  TCNT2 = 0;
  OCR2A = ocr;
  TCCR2A = (1 << COM2A0) | (1 << WGM21);
  TCCR2B = (1 << CS22);
}

void loop() {
  updateMotor1();
  updateMotor2();
  delay(10);
}