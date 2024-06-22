#include <SparkFun_TB6612.h>

#define AIN1 4
#define BIN1 6
#define AIN2 3
#define BIN2 7
#define PWMA 9
#define PWMB 10
#define STBY 5
String path;
const int offsetA = 1;
const int offsetB = 1;
Motor motor1 = Motor(AIN1, AIN2, PWMA, offsetA, STBY);
Motor motor2 = Motor(BIN1, BIN2, PWMB, offsetB, STBY);

const int Pin[8] = {A0, A1, A2, A3, A4, A5, A6, A7};
void setup() {
  Serial.begin(9600);
  for (int i = 0 ; i < 8 ; i++)
  {
    pinMode(Pin[i], INPUT);
  }
  pinMode(STBY, OUTPUT);
  digitalWrite(STBY, HIGH);
  delay(2000);
}
// A0 A1 A2   (A3 A4)   A5 A6 A7
void loop() {
  // moving forward [00011000]------------------------------------------------------------------------------------------------------
  if ( !digitalRead(A2) && digitalRead(A3) && digitalRead(A4) && !digitalRead(A5))
  {
    Serial.println("Forward");
    path += 'S';
    front();
  }
  //moving right [00011111]-------------------------------------------------------------------------------------------------------------
  if ( !digitalRead(A2) && digitalRead(A3) && digitalRead(A4) && digitalRead(A5) && digitalRead(A6) && digitalRead(A7))
  {
    Serial.println("RIGHT");
    path += 'R';
    right();
  }
  //moving left [11111000]-------------------------------------------------------------------------------------------------------------
  if (digitalRead(A0) && digitalRead(A1) && digitalRead(A2) && digitalRead(A3) && digitalRead(A4) && digitalRead(A5) && !digitalRead(A6))
  {
    if(path[0] == 'R'){right();}
    if(path[0] == 'S'){front();}
    if(path[0] == 'L'){left();}
    
    Serial.println("LEFT");
    path += 'L';
    left();
  }
  //moving back on dead end [00000000]------------------------------------------------------------------------------------------------------------------------------------------
  if (!digitalRead(A0) && !digitalRead(A1) && !digitalRead(A2) && !digitalRead(A3) && !digitalRead(A4) && !digitalRead(A5) && !digitalRead(A6) && !digitalRead(A7))
  {
    Serial.println("BACK");
    path += 'B';
    back();
  }
  // For T TUrn/ + turns [11111111]---------------------------------------------------------------------------------------------------------------------------------------
  if (digitalRead(A0) && digitalRead(A1) && digitalRead(A2) && digitalRead(A3) && digitalRead(A4) && digitalRead(A5) && digitalRead(A6) && digitalRead(A7))
  {
    Serial.println("Left");
    path += 'L';
    left();
  }
  //end position [01000010]----------------------------------------------------------------------------------------------------------------------------------------------
  if (digitalRead(A0) && !digitalRead(A1) && digitalRead(A2) && digitalRead(A3) && digitalRead(A4) && digitalRead(A5) && !digitalRead(A6) && digitalRead(A7))
  {
    halt();
  }
}
void front()
{
  motor1.drive(150);
  motor2.drive(150);
  delay(100);
  motor1.drive(0);
  motor2.drive(0);
}
void back()
{
  motor1.drive(150);
  motor2.drive(-150);
  delay(100);
  motor1.drive(0);
  motor2.drive(0);
}
void left()
{
  motor1.drive(150);
  motor2.drive(-150);
  delay(100);
  motor1.drive(0);
  motor2.drive(0);
}
void right()
{
  motor1.drive(-150);
  motor2.drive(150);
  delay(100);
  motor1.drive(0);
  motor2.drive(0);
}
void halt()
{
  motor1.drive(0);
  motor2.drive(0);
  delay(10);
}
String optimize()
{
  path.replace("LBL", "S");
  path.replace("LBS", "R");
  path.replace("RBL", "B");
  path.replace("SBS", "B");
  path.replace("SBL", "R");
  path.replace("LBR", "B");
  return path;
}
