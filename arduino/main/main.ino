#include <Servo.h> 
#define SERVO_PIN (2)
#define DC_L1 (9)
#define DC_L2 (10)
#define SWITCH_PIN (6)
#define STEP_PIN (12)
#define DIR_PIN (13)
#define LIGHT_PIN (A3)
#define SENSOR_PIN (A1)
#define POT_PIN (A0)
Servo ht_servo;
int motor_state;

#include "QuickStats.h"

QuickStats stats; 

void setup() {
  // put your setup code here, to run once:

  ht_servo.attach(SERVO_PIN);
  pinMode(DC_L1, OUTPUT);
  pinMode(DC_L2, OUTPUT);
  pinMode(SENSOR_PIN, INPUT);

  pinMode(STEP_PIN, OUTPUT);     
  pinMode(DIR_PIN, OUTPUT);
  digitalWrite(STEP_PIN, LOW);
  digitalWrite(DIR_PIN, LOW);
  pinMode(POT_PIN, INPUT);
  pinMode(LIGHT_PIN, INPUT);
  
  Serial.begin(9600);
}

void servoControl(int input) {
  ht_servo.write(input);
}

void stepperControl(int dir)
{
  digitalWrite(DIR_PIN, dir);  
  for (int i=0; i <= 2500; i++){
    digitalWrite(STEP_PIN, HIGH);
    delayMicroseconds(50);          
    digitalWrite(STEP_PIN, LOW); 
    delayMicroseconds(50);
  }
}

void motorControl(int speed)
{
  if(speed > 0)
  {
    digitalWrite(DC_L1, speed);
    digitalWrite(DC_L2, LOW);
  }
  else if(speed < 0)
  {
    digitalWrite(DC_L1, LOW);
    digitalWrite(DC_L2, -speed);
  }
  else
  {
    digitalWrite(DC_L1, LOW);
    digitalWrite(DC_L2, LOW);
  }
}

float irDistance()
{
  int n = 20; //window size
  float sig[n];
  
  for (int i=0; i<n; i++){
    sig[i] = analogRead(SENSOR_PIN);
  }

  float x = stats.median(sig,n);
  float distance = .00036008*x*x - .28975*x + 68.567;
  Serial.println(distance);
  return distance;
}

void loop() {
  // put your main code here, to run repeatedly:

  if(digitalRead(SWITCH_PIN) == HIGH)
  {
    while(digitalRead(SWITCH_PIN) == HIGH)
    {
      delay(100);
    }
    motor_state = !motor_state;
    Serial.print("Motor State: ");
    Serial.println(motor_state);
  }
  if (motor_state == 1)
  {
    float raw_pot = analogRead(POT_PIN);
    float pot_val = (raw_pot / 1024) * 180;
    servoControl(pot_val);
  }

  if(motor_state == 0)
  {
    Serial.println("Motor Off");
    motorControl(0);
  }
  else
  {
    int pwr = irDistance()*2;
    if(pwr < 40)
    {
      pwr = -pwr;
    }
    motorControl(pwr);
    delay(100);
  }
  
  float raw = analogRead(LIGHT_PIN);
  float light_val = (raw/1024)*5;
  if(light_val > 4.7)
  {
    stepperControl(HIGH);
  }
  else if(light_val < 4)
  {
    stepperControl(LOW);
  }
}
 
