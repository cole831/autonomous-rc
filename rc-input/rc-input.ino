#define LOG false

static const int in1 = 11, in2 = 10, in3 = 6, in4 = 5, r_throttle = 3, r_rudder = 9; //motor and throttle pin initalizition 
int r_rudderPulse = 0; float turnRatio = 0.0, throttleRatio; // 990-1970 range, ~1470-1490 is no turning, add buffer
int rd_lowBound = 980, rd_highBound = 2007, rd_lowThreshold = 1480, rd_highThreshold = 1500, rd_buffer = 20; // pulse bounding range and threshold for no turning + buffer
int r_throttlePulse = 0, motor_pwm = 0; // pin 3 and 0 duration default

void setup() {

  Serial.begin(9600);
  // motor pins
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  // controller pins
  pinMode(r_throttle, INPUT);
  pinMode(r_rudder, INPUT);

  // ensure motors are off
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);

  // ensure recieving signal
  digitalWrite(r_throttle, HIGH);
  digitalWrite(r_rudder, HIGH);
}

void LogSerial(String text, int value) {
  if(LOG){
    Serial.print(text);
    Serial.println(value);
  }
}
void LogSerial(String text, float value) {
  if(LOG){
    Serial.print(text);
    Serial.println(value);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  // delay(250);
  r_rudderPulse = pulseIn(r_rudder, HIGH);
  r_throttlePulse = pulseIn(r_throttle, HIGH);
  motor_pwm = map(r_throttlePulse, 1100, 2000, 0, 255);
  //LogSerial("rudder pulse: ", r_rudderPulse);
  
  if(r_rudderPulse > (rd_lowThreshold-rd_buffer) && r_rudderPulse < (rd_highThreshold + rd_buffer)) {
    turnRatio = 0.0f;
  } else if (r_rudderPulse < (rd_lowThreshold-rd_buffer)) {
    turnRatio = (float(r_rudderPulse-rd_lowBound)/float(rd_lowThreshold-rd_buffer-rd_lowBound));
  } else if (r_rudderPulse > (rd_highThreshold + rd_buffer)) {
    turnRatio = (float(r_rudderPulse-rd_highBound)/float(rd_lowThreshold-rd_buffer-rd_lowBound));
  } 
  
  LogSerial("turn ratio: ", turnRatio);

  if(motor_pwm < 0)
     motor_pwm = 0;

  if (r_throttlePulse > 1100) {
  // LogSerial("pwm motor: ", motor_pwm);
    if (motor_pwm > 0) {
      if(turnRatio == 0.0f) {
        analogWrite(in1, motor_pwm);
        // LogSerial("in1 pwm: ", motor_pwm);
        analogWrite(in3, motor_pwm);
        // LogSerial("in3 pwm: ", motor_pwm);
        digitalWrite(in2, LOW);
        digitalWrite(in4, LOW);
      } 
      else if (turnRatio > 0.0f) {
        analogWrite(in1, motor_pwm);
        // LogSerial("in1 pwm: ", motor_pwm);
        analogWrite(in3, int(motor_pwm * turnRatio));
        // LogSerial("in3 pwm: ", int(motor_pwm * turnRatio));
        digitalWrite(in2, LOW);
        digitalWrite(in4, LOW);
      } 
      else if (turnRatio < 0.0f) {
        analogWrite(in1, int(motor_pwm * (-1.0*turnRatio)));
        // LogSerial("in1 pwm: ", int(motor_pwm * (-1.0*turnRatio)));
        analogWrite(in3, motor_pwm);
        // LogSerial("in3 pwm: ", motor_pwm);
        digitalWrite(in2, LOW);
        digitalWrite(in4, LOW);
      }      
    }
  }
  else {
    if (turnRatio == 0.0f) {
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
      digitalWrite(in1, LOW);
      digitalWrite(in4, LOW);
    } 
    else if (turnRatio > 0.0f) {
      analogWrite(in1, int(255 * (1.0 - turnRatio)));
      // LogSerial("in1 pwm: ", motor_pwm);
      analogWrite(in4, int(255 * (1.0 - turnRatio)));
      // LogSerial("in3 pwm: ", motor_pwm);
      digitalWrite(in2, LOW);
      digitalWrite(in3, LOW);
     } 
      else if (turnRatio < 0.0f) {
        analogWrite(in2, int(255 * (1.0 + turnRatio)));
        // LogSerial("in1 pwm: ", motor_pwm);
        analogWrite(in3, int(255 * (1.0 + turnRatio)));
        // LogSerial("in3 pwm: ", motor_pwm);
        digitalWrite(in1, LOW);
        digitalWrite(in4, LOW);
     }
  }
}


