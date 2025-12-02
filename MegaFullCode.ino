// === Pin Assignments ===
// Right Drive (BTS7960)
#define R_RPWM 7
#define R_LPWM 6
#define R_REN 30
#define R_LEN 31

// Left Drive (BTS7960)
#define L_RPWM 11
#define L_LPWM 10
#define L_REN 26
#define L_LEN 27

// Blade Motor (BTS7960)
#define B_RPWM 9
#define B_LPWM 8
#define B_REN 28
#define B_LEN 29

// Sonar Sensors
#define SONAR_RIGHT_TRIG 50
#define SONAR_RIGHT_ECHO 51
#define SONAR_LEFT_TRIG 22
#define SONAR_LEFT_ECHO 23
#define SONAR_FRONT_TRIG 24
#define SONAR_FRONT_ECHO 25
#define SONAR_BACK_TRIG 12
#define SONAR_BACK_ECHO 13

void setup() {
  Serial.begin(9600);

  // Motor pins
  int motorPins[] = {
    R_RPWM, R_LPWM, R_REN, R_LEN,
    L_RPWM, L_LPWM, L_REN, L_LEN,
    B_RPWM, B_LPWM, B_REN, B_LEN
  };
  for (int i = 0; i < 12; i++) pinMode(motorPins[i], OUTPUT);

  // Sonar pins
  pinMode(SONAR_RIGHT_TRIG, OUTPUT); pinMode(SONAR_RIGHT_ECHO, INPUT);
  pinMode(SONAR_LEFT_TRIG, OUTPUT);  pinMode(SONAR_LEFT_ECHO, INPUT);
  pinMode(SONAR_FRONT_TRIG, OUTPUT); pinMode(SONAR_FRONT_ECHO, INPUT);
  pinMode(SONAR_BACK_TRIG, OUTPUT);  pinMode(SONAR_BACK_ECHO, INPUT);

  stopAll();
}

void loop() {
  // Send sonar data to ESP every loop
  long r = readSonar(SONAR_RIGHT_TRIG, SONAR_RIGHT_ECHO);
  long l = readSonar(SONAR_LEFT_TRIG, SONAR_LEFT_ECHO);
  long f = readSonar(SONAR_FRONT_TRIG, SONAR_FRONT_ECHO);
  long b = readSonar(SONAR_BACK_TRIG, SONAR_BACK_ECHO);

  Serial.print("R:"); Serial.print(r);
  Serial.print(" L:"); Serial.print(l);
  Serial.print(" F:"); Serial.print(f);
  Serial.print(" B:"); Serial.println(b);

  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "FWD") forward();
    else if (cmd == "BWD") backward();
    else if (cmd == "LFT") turnLeft();
    else if (cmd == "RGT") turnRight();
    else if (cmd == "STP") stopAll();
    else if (cmd == "BON") bladeOn();
    else if (cmd == "BOF") bladeOff();
    else if (cmd == "AUTO") Serial.println("AUTO MODE ON"); // placeholder
    else if (cmd == "MANU") Serial.println("MANUAL MODE ON"); // placeholder
  }

  delay(100);
}

// === Motion Functions ===

void forward() {
  digitalWrite(L_REN, HIGH); digitalWrite(L_LEN, HIGH);
  digitalWrite(R_REN, HIGH); digitalWrite(R_LEN, HIGH);
  analogWrite(L_RPWM, 200); analogWrite(L_LPWM, 0);
  analogWrite(R_RPWM, 200); analogWrite(R_LPWM, 0);
}

void backward() {
  digitalWrite(L_REN, HIGH); digitalWrite(L_LEN, HIGH);
  digitalWrite(R_REN, HIGH); digitalWrite(R_LEN, HIGH);
  analogWrite(L_RPWM, 0); analogWrite(L_LPWM, 200);
  analogWrite(R_RPWM, 0); analogWrite(R_LPWM, 200);
}

void turnLeft() {
  digitalWrite(L_REN, HIGH); digitalWrite(L_LEN, HIGH);
  digitalWrite(R_REN, HIGH); digitalWrite(R_LEN, HIGH);
  analogWrite(L_RPWM, 0); analogWrite(L_LPWM, 200);
  analogWrite(R_RPWM, 200); analogWrite(R_LPWM, 0);
}

void turnRight() {
  digitalWrite(L_REN, HIGH); digitalWrite(L_LEN, HIGH);
  digitalWrite(R_REN, HIGH); digitalWrite(R_LEN, HIGH);
  analogWrite(L_RPWM, 200); analogWrite(L_LPWM, 0);
  analogWrite(R_RPWM, 0); analogWrite(R_LPWM, 200);
}

void stopAll() {
  analogWrite(L_RPWM, 0); analogWrite(L_LPWM, 0);
  analogWrite(R_RPWM, 0); analogWrite(R_LPWM, 0);
  analogWrite(B_RPWM, 0); analogWrite(B_LPWM, 0);
}

void bladeOn() {
  digitalWrite(B_REN, HIGH); digitalWrite(B_LEN, HIGH);
  analogWrite(B_RPWM, 200); analogWrite(B_LPWM, 0);
}

void bladeOff() {
  analogWrite(B_RPWM, 0); analogWrite(B_LPWM, 0);
}

// === Sonar Function ===
long readSonar(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000);
  long distance = duration * 0.0343 / 2;
  return distance;
}
