constexpr int ECHO_PIN {9};
constexpr int TRIG_PIN {10};

long duration {};

void setup() {
  Serial.begin(9600);
  
  pinMode(TRIG_PIN, OUTPUT); 
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);

  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  Serial.println(duration);

  delay(10);
}



