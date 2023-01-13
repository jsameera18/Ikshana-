#include <Stepper.h>

//stepper motor
const int stepsPerRevolution = 200;
Stepper stepper = Stepper(stepsPerRevolution, 2, 3, 4, 5);
//steps needed for = (stepsPerRevolution / 360) * 15
int stepsToShiftOneCell = 8;

void setup() {
  // put your setup code here, to run once:

  Serial.begin(9600);
  stepper.setSpeed(10);

}

void loop() {
  // put your main code here, to run repeatedly:
  stepper.step(8);  

  delay(1000);
}
