#define ENA 6//25  // Enable pin for motor A
#define IN1 7//32  // Control pin 1 for motor A
#define IN2 4//33  // Control pin 2 for motor A
#define ENB 5//14  // Enable pin for motor B
#define IN3 2//27  // Control pin 1 for motor B
#define IN4 3//26  // Control pin 2 for motor B
const int Pin[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
// these constants are used to allow you to make your motor configuration
// line up with function names like forward.  Value can be 1 or -1

// Initializing motors.  The library will allow you to initialize as many
// motors as you have memory for.  If you are using functions like forward
// that take 2 motors as arguements you can either write new functions or
// call the function more than once.

int P, D, I, previousError, PIDvalue, error;
int lsp, rsp;
int lfspeed = 150;

float Kp = 0;
float Kd = 0;
float Ki = 0 ;


int minValues[8], maxValues[8], threshold[8];

void setup()
{
  Serial.begin(9600);
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(8, INPUT_PULLUP);
  pinMode(9, INPUT_PULLUP);

}


void loop()
{
  //  delay(2000);
  //  while (1)
  if (digitalRead(8) == LOW)
  {
    Serial.println("calibrating");
    delay(1000);
    calibrat();
  }
  if(digitalRead(9) == LOW)
  {
    delay(1000);
    strt();
  }
  
  
}
void strt()
{
  {
    if (analogRead(Pin[0]) < threshold[0] && analogRead(Pin[7]) > threshold[7] )
    {
      lsp = 0; rsp = lfspeed;
      motor2(0);
      motor1(lfspeed);
    }

    else if (analogRead(Pin[1]) < threshold[1] && analogRead(Pin[6]) > threshold[6] )
    {
      lsp = 0; rsp = lfspeed / 2;
      motor2(0);
      motor1(lfspeed);
    }

    else if (analogRead(Pin[2]) < threshold[2] && analogRead(Pin[5]) > threshold[5] )
    {
      lsp = 0; rsp = lfspeed / 4;
      motor2(0);
      motor1(lfspeed);
    }

    else if (analogRead(Pin[5]) < threshold[5] && analogRead(Pin[2]) > threshold[2])
    { lsp = lfspeed / 4; rsp = 0;
      motor2(lfspeed);
      motor1(0);
    }

    else if (analogRead(Pin[6]) < threshold[6] && analogRead(Pin[1]) > threshold[1])
    { lsp = lfspeed / 2; rsp = 0;
      motor2(lfspeed);
      motor1(0);
    }

    else if (analogRead(Pin[7]) < threshold[7] && analogRead(Pin[0]) > threshold[0] )
    {
      lsp = lfspeed; rsp = 0;
      motor2(0);
      motor1(lfspeed);
    }

    else if (analogRead(Pin[3]) > threshold[3] || analogRead(Pin[4]) > threshold[4])
    {
      Kp = 0.002 * (1000 - analogRead(Pin[3]));
      Kd = 10 * Kp;
      Ki = 0.0006;
      linefollow();
    }

    else if (analogRead(Pin[0]) < threshold[0] && analogRead(Pin[1]) < threshold[1] && analogRead(Pin[2]) < threshold[2] && analogRead(Pin[3]) < threshold[3] && analogRead(Pin[4]) < threshold[4] && analogRead(Pin[5]) < threshold[5] && analogRead(Pin[6]) < threshold[6] && analogRead(Pin[7]) < threshold[7])
    {
      lsp = 0; rsp = 0;
      motor1(0);
      motor2(0);
    }
    else if (analogRead(Pin[0]) > threshold[0] && analogRead(Pin[1]) > threshold[1] && analogRead(Pin[2]) > threshold[2] && analogRead(Pin[3]) > threshold[3] && analogRead(Pin[4]) > threshold[4] && analogRead(Pin[5]) > threshold[5] && analogRead(Pin[6]) > threshold[6] && analogRead(Pin[7]) > threshold[7])
    {
      lsp = 0; rsp = 0;
      motor1(0);
      motor2(0);
    }
    else
    {
      lsp = 0; rsp = 0;
      motor1(0);
      motor2(0);
    }
  }
  strt();
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
  motor1(lsp);
  motor2(rsp);

}

void calibrat()
{
  for ( int i = 0; i < 8; i++)
  {
    minValues[i] = analogRead(Pin[i]);
    maxValues[i] = analogRead(Pin[i]);
  }

  for (int i = 0; i < 3000; i++)
  {
    motor1(90);
    motor2(-90);

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
    Serial.print("Threshold :  ");
    Serial.print(threshold[i]);
    Serial.print("   ");
  }
  Serial.println();
  delay(1000);
  motor1(0);
  motor2(0);
}
void motor1(int Speed)
{
  if (Speed >= 0)
  {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, Speed);
  }
  else if (Speed < 0)
  {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENB, Speed);
  }
}
void motor2(int Speed)
{
  if (Speed >= 0)
  {
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    analogWrite(ENB, Speed);
  }
  else if (Speed < 0)
  {
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    analogWrite(ENB, Speed);
  }

}

