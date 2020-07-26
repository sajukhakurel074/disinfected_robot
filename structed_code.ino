#include "motor_init.h"
#include "bluetooth.h"
#include "steppermotor_init.h"
#include "proxy_init.h"


int mots[4] = {0, 0, 0, 0};
int mot[4] = {0, 0, 0, 0};
int s = 0;


void setup()
{
  Serial.begin(9600);
  motor_init(); 
  proxy_init();
  steppermotor_init();
  while(digitalRead(proxy[0].SIG))      //stepper_motor_yaxis
  {
    digitalWrite(dirPin1, LOW);
    digitalWrite(stepPin1, HIGH);
    delay(15);
    digitalWrite(stepPin1, LOW);
    delay(15);
  }
  while(digitalRead(proxy[2].SIG))
  {
    digitalWrite(dirPin, HIGH);
    digitalWrite(stepPin, HIGH);
    delay(1000);
    digitalWrite(stepPin, LOW);
    delay(1000);
  }
  
 Serial.println("ALL INITIALIZED");
    
}

void loop()
{  
  read_bluetooth();
   if(myStepper1ShouldRun)
   {
      digitalWrite(dirPin1, LOW);
      for (int i = 0; i < stepsPerRevolution; i++) 
      {
        digitalWrite(stepPin1, HIGH);
        delayMicroseconds(1000);
        digitalWrite(stepPin1, LOW);
        delayMicroseconds(1000);
      }
      delay(2000);
      digitalWrite(dirPin1, HIGH);
      for (int i = 0; i < stepsPerRevolution; i++) 
      {
        digitalWrite(stepPin1, HIGH);
        delayMicroseconds(1000);
        digitalWrite(stepPin1, LOW);
        delayMicroseconds(1000);
      }
      delay(2000);
    }
    else
    {
     digitalWrite(stepPin1, LOW); 
    }

  if(digitalRead(proxy[4].SIG) || digitalRead(proxy[5].SIG))
  {
    if(motor_dir == MOTOR_FORWARD)
    {
      v = 0;
      w = 0;
    }
  }
  if(digitalRead(proxy[6].SIG) || digitalRead(proxy[7].SIG))
  {
    if(motor_dir == MOTOR_BACKWARD)
    {
      v = 0;
      w = 0;
    }
  }
  
  calculate_velocity();
  set_velocity();
}

void calculate_velocity()
{
  float velocity[2] = {v, w};
  int coupling_matrix[4][2] = {{1, 1},
                               {1, -1},
                               {1, 1},
                               {1, -1}};
  for (int i = 0; i < 4; i++)
  {
    mot[i] = 0;
    for(int j = 0; j < 2; j++)
    {
      mot[i] += velocity[j] * coupling_matrix[i][j];                             
    }
  }
}

void set_velocity()
{
    
      for(int i = 0; i < 4; i++)
      {
        if(mot[i]>mots[i])
        {
          mots[i]++;
        }
        else if(mot[i]<mots[i])
        {
          mots[i]--;
        }
        int dutycycle = (mots[i]/vel_max)*255;
        //Serial.print("dc:");
        //Serial.print(dutycycle);
        //Serial.print("\t"); 
        digitalWrite( motor[i].IN1, mot[i] > 0 );
        digitalWrite( motor[i].IN2, mot[i] < 0 );
        analogWrite( motor[i].EN, dutycycle);
      }  
      //Serial.println("   ");     
}

