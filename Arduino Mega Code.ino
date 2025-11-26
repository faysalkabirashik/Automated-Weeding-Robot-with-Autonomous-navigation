// ---------- MOTOR PINS ----------
int motorLF = 26;  // Left Front motor
int motorLB = 27;  // Left Back motor
int motorRF = 30;  // Right Front motor
int motorRB = 31;  // Right Back motor

// ---------- BLADE MOTOR ----------
int bladeMotor = 28;

// ---------- ULTRASONIC SENSOR PINS ----------
#define TRIG1 52
#define ECHO1 53

#define TRIG2 22
#define ECHO2 23

#define TRIG3 34
#define ECHO3 25

#define TRIG4 12
#define ECHO4 13

// ---------- MODE ----------
bool autoMode = false;

void setup() {
  Serial.begin(9600);

  // Motor pins
  pinMode(motorLF, OUTPUT);
  pinMode(motorLB, OUTPUT);
  pinMode(motorRF, OUTPUT);
  pinMode(motorRB, OUTPUT);

  pinMode(bladeMotor, OUTPUT);

  // Sonar pins
  pinMode(TRIG1, OUTPUT);  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);  pinMode(ECHO2, INPUT);
  pinMode(TRIG3, OUTPUT);  pinMode(ECHO3, INPUT);
  pinMode(TRIG4, OUTPUT);  pinMode(ECHO4, INPUT);

  Stop();
  digitalWrite(bladeMotor, LOW);
}

// ----------- SONAR READ FUNCTION ----------
long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 30000); // 30ms timeout
  long distance = duration * 0.034 / 2;
  if (distance == 0) distance = 999; // Out of range
  return distance;
}

// ----------- MOTOR FUNCTIONS ----------
void Forward() {
  digitalWrite(motorLF, HIGH);
  digitalWrite(motorLB, LOW);
  digitalWrite(motorRF, HIGH);
  digitalWrite(motorRB, LOW);
}

void Backward() {
  digitalWrite(motorLF, LOW);
  digitalWrite(motorLB, HIGH);
  digitalWrite(motorRF, LOW);
  digitalWrite(motorRB, HIGH);
}

void Left() {
  digitalWrite(motorLF, LOW);
  digitalWrite(motorLB, HIGH);
  digitalWrite(motorRF, HIGH);
  digitalWrite(motorRB, LOW);
}

void Right() {
  digitalWrite(motorLF, HIGH);
  digitalWrite(motorLB, LOW);
  digitalWrite(motorRF, LOW);
  digitalWrite(motorRB, HIGH);
}

void Stop() {
  digitalWrite(motorLF, LOW);
  digitalWrite(motorLB, LOW);
  digitalWrite(motorRF, LOW);
  digitalWrite(motorRB, LOW);
}

// ---------- AUTO MODE ----------
void autoDrive(long s1, long s2, long s3, long s4) {
  long minDist = min(min(s1, s2), min(s3, s4));

  if (minDist < 20) {
    Stop();
    delay(100);
    Backward();
    delay(300);
    Stop();
    delay(200);
    Right();
    delay(300);
    Stop();
  } else {
    Forward();
  }
}

void loop() {
  // ---- READ 4 SENSORS ----
  long s1 = getDistance(TRIG1, ECHO1);
  long s2 = getDistance(TRIG2, ECHO2);
  long s3 = getDistance(TRIG3, ECHO3);
  long s4 = getDistance(TRIG4, ECHO4);

  // ---- SEND TO ESP8266 ----
  Serial.print("S1:"); Serial.print(s1);
  Serial.print(" S2:"); Serial.print(s2);
  Serial.print(" S3:"); Serial.print(s3);
  Serial.print(" S4:"); Serial.println(s4);

  // ---- RECEIVE COMMAND FROM ESP8266 ----
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    if (cmd == "FWD") Forward();
    else if (cmd == "BWD") Backward();
    else if (cmd == "LFT") Left();
    else if (cmd == "RGT") Right();
    else if (cmd == "STP") Stop();
    else if (cmd == "BON") digitalWrite(bladeMotor, HIGH);
    else if (cmd == "BOF") digitalWrite(bladeMotor, LOW);
    else if (cmd == "AUTO") autoMode = true;
    else if (cmd == "MANU") autoMode = false;
  }

  // ---- AUTO MODE IF ENABLED ----
  if (autoMode) {
    autoDrive(s1, s2, s3, s4);
  }

  delay(70); // smooth data rate
}
