#include <SparkFun_TB6612.h>

#define AIN1 4
#define BIN1 6
#define AIN2 3
#define BIN2 7
#define PWMA 9
#define PWMB 10
#define STBY 5
const int Pin[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
// these constants are used to allow you to make your motor configuration
// line up with function names like forward.  Value can be 1 or -1
const int offsetA = 1;
const int offsetB = 1;

// Initializing motors.  The library will allow you to initialize as many
// motors as you have memory for.  If you are using functions like forward
// that take 2 motors as arguements you can either write new functions or
// call the function more than once.

Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY);


int P, D, I, previousError, PIDvalue, error;
int lsp, rsp;
int lfspeed = 255;

float Kp = 0;
float Kd = 0;
float Ki = 0 ;
String path;

int minValues[8], maxValues[8], threshold[8];

void setup()
{
  Serial.begin(9600);
  pinMode(11, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  delay(2000);
  calibrate();
}


void loop()
{
  //delay(2000);
  while (1)
  {
    if (analogRead(Pin[0]) < threshold[0] && analogRead(Pin[7]) > threshold[7] )
    {
       path += 'R';
      lsp = 0; rsp = lfspeed;
      motor1.drive(0);
      motor2.drive(lfspeed);
      
    }
    
    else if (analogRead(Pin[1]) < threshold[1] && analogRead(Pin[6]) > threshold[6] )
    {
      lsp = 0; rsp = lfspeed/2;
      motor1.drive(0);
      motor2.drive(lfspeed);
    }

    else if (analogRead(Pin[2]) < threshold[2] && analogRead(Pin[5]) > threshold[5] )
    {
      lsp = 0; rsp = lfspeed/4;
      motor1.drive(0);
      motor2.drive(lfspeed);
    }

    else if (analogRead(Pin[5]) < threshold[5] && analogRead(Pin[2]) > threshold[2])
    { lsp = lfspeed/4; rsp = 0;
      motor1.drive(lfspeed);
      motor2.drive(0);
    }

    else if (analogRead(Pin[6]) < threshold[6] && analogRead(Pin[1]) > threshold[1])
    { lsp = lfspeed/2; rsp = 0;
      motor1.drive(lfspeed);
      motor2.drive(0);
    }

    else if (analogRead(Pin[7]) < threshold[7] && analogRead(Pin[0]) < threshold[0] )
    {
       path += 'L';
      lsp = lfspeed; rsp = 0;
      motor1.drive(0);
      motor2.drive(lfspeed);
      
    }
    
    else if (analogRead(Pin[3]) > threshold[3] || analogRead(Pin[4]) > threshold[4])
    {
       path += 'S';
      Kp = 0.002 * (1000 - analogRead(Pin[3]));
      Kd = 1 * Kp;
      Ki = 0.0006;
      linefollow();
    }

    else if (analogRead(Pin[0]) < threshold[0] && analogRead(Pin[1]) < threshold[1] && analogRead(Pin[2]) < threshold[2] && analogRead(Pin[3]) < threshold[3] && analogRead(Pin[4]) < threshold[4] && analogRead(Pin[5]) < threshold[5] && analogRead(Pin[6]) < threshold[6] && analogRead(Pin[7]) < threshold[7])
    {
      lsp = 0; rsp = 0;
      motor1.drive(0);
      motor2.drive(0);
    }
    else if (analogRead(Pin[0]) > threshold[0] && analogRead(Pin[1]) > threshold[1] && analogRead(Pin[2]) > threshold[2] && analogRead(Pin[3]) > threshold[3] && analogRead(Pin[4]) > threshold[4] && analogRead(Pin[5]) > threshold[5] && analogRead(Pin[6]) > threshold[6] && analogRead(Pin[7]) > threshold[7])
    {
      lsp = 0; rsp = 0;
      motor1.drive(0);
      motor2.drive(0);
    }
    else
    {
      lsp = 0; rsp = 0;
      motor1.drive(0);
      motor2.drive(0);
    }
    //moving back on dead end [00000000]------------------------------------------------------------------------------------------------------------------------------------------
//  if (!digitalRead(A0) && !digitalRead(A1) && !digitalRead(A2) && !digitalRead(A3) && !digitalRead(A4) && !digitalRead(A5) && !digitalRead(A6) && !digitalRead(A7))
//  {
//    Serial.println("BACK");
//    path += 'B';
//    motor1.drive(150);
//      motor2.drive(-150);
//  }
  }
}

void linefollow()
{
  int error = (analogRead(Pin[1]) - analogRead(Pin[6]));

  P = error;
  I = I + error;
  D = error - previousError;

  PIDvalue = (Kp * P) + (Ki * I) + (Kd * D);
  previousError = error;

  lsp = lfspeed - PIDvalue;
  rsp = lfspeed + PIDvalue;

  if (lsp > 255) {
    lsp = 255;
  }
  if (lsp < 0) {
    lsp = 0;
  }
  if (rsp > 255) {
    rsp = 255;
  }
  if (rsp < 0) {
    rsp = 0;
  }
  motor1.drive(lsp);
  motor2.drive(rsp);

}

void calibrate()
{
  for ( int i = 0; i < 8; i++)
  {
    minValues[i] = analogRead(Pin[i]);
    maxValues[i] = analogRead(Pin[i]);
  }
  
  for (int i = 0; i < 3000; i++)
  {
    motor1.drive(50);
    motor2.drive(-50);

    for ( int i = 0; i < 8; i++)
    {
      if (analogRead(Pin[i]) < minValues[i])
      {
        minValues[i] = analogRead(Pin[i]);
      }
      if (analogRead(Pin[i]) > maxValues[i])
      {
        maxValues[i] = analogRead(Pin[i]);
      }
    }
  }

  for ( int i = 0; i < 8; i++)
  {
    threshold[i] = (minValues[i] + maxValues[i]) / 2;
    Serial.print(threshold[i]);
    Serial.print("   ");
  }
  Serial.println();
  
  motor1.drive(0);
  motor2.drive(0);
}
String optimize()
{
  path.replace("LBL", "S");
  path.replace("LBS", "R");
  path.replace("RBL", "B");
  path.replace("SBS", "B");
  path.replace("SBL", "R");
  path.replace("LBR", "B");
  path.replace("SS", "S");
  return path;
}
