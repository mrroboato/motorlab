#include <Servo.h> 
#include "QuickStats.h"

#define SERVO_PIN (2)
#define DC_L1 (9)
#define DC_L2 (10)
#define SWITCH_PIN (6)
#define STEP_PIN (12)
#define DIR_PIN (13)
#define LIGHT_PIN (A3)
#define SENSOR_PIN (A1)
#define POT_PIN (A0)
#define STATE_SENSOR (0)
#define STATE_GUI (1)
#define STATE_VEL (2)
#define STATE_POS (3)
int encoder0PinA = 3;
int encoder0PinB = 4;
int encoder0Pos = 0;

Servo ht_servo;

byte byteRead;

int motor_state = STATE_SENSOR;
int dc_state = STATE_VEL;
boolean update_dc = false;

int servo_input;
int dc_input;
int stepper_input;
int encoder0PinALast = LOW;
int n = LOW;
double pos_percent;

int stepper_run = 0;

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

  pinMode (encoder0PinA,INPUT);
  pinMode (encoder0PinB,INPUT);
  
  Serial.begin(115200);
}

void loop() {
    // put your main code here, to run repeatedly:
    checkSwitch();

    // See if anything is in serial buffer.
    static char buffer[80];
    if (readline(Serial.read(), buffer, 80) > 0) {
        
        char *motorName, *motorVal;

        motorName = strtok(buffer,":");
        motorVal = strtok(NULL,":");

//        Serial.println("*Received command >" + String(motorName) + ":" + String(motorVal) + "<#");
        
        if (strcmp(motorName, "state") == 0) {
            if (strcmp(motorVal, "0") == 0) {
                motor_state = STATE_SENSOR;
//                Serial.println("*Motor State: Sensor#");
            } else if (strcmp(motorVal, "1") == 0) {
                motor_state = STATE_GUI;
//                Serial.println("*Motor State: GUI#");
            } else if (strcmp(motorVal, "2") == 0) {
                dc_state = STATE_VEL;
            } else if (strcmp(motorVal, "3") == 0) {
                dc_state = STATE_POS;
            }
        } else if (motor_state == STATE_GUI) { // We read from serial, so might as well.
            if (strcmp(motorName, "servo") == 0) {
                servo_input = atoi(motorVal);
//                Serial.println("*Servo Input: " + String(motorVal) + "#");
            } else if (strcmp(motorName, "dc") == 0) {
                int received_dc_input = atoi(motorVal);
                if (received_dc_input != dc_input) {
                    dc_input = received_dc_input;
                    update_dc = true;
                }
//                Serial.println("*DC Input: " + String(motorVal) + "#");
            } else if (strcmp(motorName, "stepper") == 0) {
                int received_stepper_input = atoi(motorVal);
                if (received_stepper_input == 0) {
                    stepper_input = LOW;
                    stepper_run = 1;
                } else if (received_stepper_input == 2) {
                    stepper_input = HIGH;
                    stepper_run = 1;
                } else {
                    stepper_input = HIGH;
                    stepper_run = 0;
                }
//                Serial.println("*Stepper Input: " + String(motorVal) + "#");
            }
        }
    } 

    // Sensor Read
    if (motor_state == STATE_SENSOR) {
        readSensors();  
    } 

    // Control Motors
    servoControl(servo_input);
//    if (update_dc) {
        if (dc_state == STATE_VEL) {
            if (motor_state == STATE_SENSOR) {
                digitalMotorControl(dc_input);
            } else {
                digitalMotorControl(dc_input);
            }
        } else {
            pos_percent = ((double) encoder0Pos)*360/ 180.0;
            pcontrol(dc_input,pos_percent); 
        }
//        update_dc = false;
//    }

    if (stepper_run) {
        stepperControl(stepper_input);
    }
    delay(50);
    
}

void checkSwitch() {
    if(digitalRead(SWITCH_PIN) == HIGH)
    {
      while(digitalRead(SWITCH_PIN) == HIGH)
      {
        delay(100);
      }
      motor_state = !motor_state;
      Serial.println("*state-toggle#");
    }  
}


/* Motor Control Functions */

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


void analogMotorControl(int speed)
{

 if(speed > 0)
 {
   speed = min(speed,255);
   analogWrite(DC_L1, speed);
   digitalWrite(DC_L2, LOW);
 }
 else if(speed < 0)
 {
   speed = max(speed,-255);
   digitalWrite(DC_L1, LOW);
   analogWrite(DC_L2, -speed);
 }
 else
 {
   digitalWrite(DC_L1, LOW);
   digitalWrite(DC_L2, LOW);
 }
 Serial.println("*"+String(speed)+"#");
}


void digitalMotorControl(int speed)
{

 if(speed > 0)
 {
//   speed = min(speed,127);
   digitalWrite(DC_L1, speed);
   digitalWrite(DC_L2, LOW);
 }
 else if(speed < 0)
 {
//   speed = max(speed,-127);
   digitalWrite(DC_L1, LOW);
   digitalWrite(DC_L2, -speed);
 }
 else
 {
   digitalWrite(DC_L1, LOW);
   digitalWrite(DC_L2, LOW);
 }
}
void pcontrol(double set_point, double pos)
{
   //Serial.println (pos);
   double error = pos - set_point;
   int Gain = 10;
   int CE = Gain * ((int) error);
//   Serial.println ("*" + String(CE) + "#");
   analogMotorControl(-CE);
//   Serial.println ("*dddddd" + String(error) + "#");
}

/* Sensor Read functions */

void readSensors() {
  
    // Servo motor and potentiometer
    float raw_pot = analogRead(POT_PIN);
    float pot_val = (raw_pot / 1024) * 180;
    servo_input = pot_val;

    // DC motor and ir proximity sensor.
    int pwr = irDistance()*2;
    if(pwr < 40)
    {
      pwr = -pwr*3;
    }
    if (pwr != dc_input) {
        dc_input = pwr;
//        Serial.println(dc_input);
        update_dc = true;
    }

    n = digitalRead(encoder0PinA);
    if ((encoder0PinALast == LOW) && (n == HIGH)) {
      if (digitalRead(encoder0PinB) == LOW) {
        encoder0Pos--;
      } else {
        encoder0Pos++;
      }
    }

    // Stepper motor and Stepper motor.
    float raw = analogRead(LIGHT_PIN);
    float light_val = (raw/1024)*5;
    if(light_val > 4.7)
    {
        stepper_input = HIGH;
        stepper_run = 1;
    }
    else if(light_val < 3.5)
    {
        stepper_input = LOW;
        stepper_run = 1;
    } else {
        stepper_run = 0;
    }

    sendSensorData(servo_input, dc_input, light_val*100);
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
//  Serial.println(distance);
  return distance;
}

/* Utilities */
int readline(int readch, char *buffer, int len) {
  static int pos = 0;
  int rpos;
  
  if (readch > 0) {
    switch (readch) {
      case '\n': // Ignore new-lines
        break;
      case '\r': // Return on CR
        rpos = pos;
        pos = 0;  // Reset position index ready for next time
        return rpos;
      default:
        if (pos < len-1) {
          buffer[pos++] = readch;
          buffer[pos] = 0;
        }
    }
  }
  // No end of line has been found, so return -1.
  return -1;
}

void sendSensorData(int pot, int ir, int pho) {
    char messageBuf[50];
    sprintf(messageBuf, "*Pot:%d,Ir:%d,Pho:%d#", pot, ir, pho);
    Serial.write(messageBuf);
}

