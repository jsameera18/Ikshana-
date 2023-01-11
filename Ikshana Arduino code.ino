#include <Stepper.h>
#include <Wire.h>
#include <RTClib.h>

const int stepsPerRevolution = 200;
Stepper stepper = Stepper(stepsPerRevolution, 2,3,4,5);
//steps needed for = (stepsPerRevolution / 360) * 15
int stepsToShiftOneCell = 8;

DateTime now;
RTC_DS3231 rtc;

String[] data = "d|8,12|1|2";
// d -> daily , 8,12 -> timings , 1 -> moudle number , 2 -. amount


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  stepper.setSpeed(10);

  if (!rtc.begin()) 
  {
    Serial.println("Couldn't find RTC Module");
    while (1);
  }

  if (rtc.lostPower()) 
  {
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

}

void loop() {
  // put your main code here, to run repeatedly:
  now = rtc.now();


  if(now.minute() == 00 && now.second() == 00){
    dispenseSequence();
  }

}  

void dispenseSequence(){

  for(int i=0;i<1;i++){
      String s = data[i];

      //checks if its the day to dispense
      char mode = s.substring(s.indexOf('|'));
      s = s.substring(s.indexOf('|'+1));
      if(mode)
  }
}

void dispensePill(){

  //buzzer sequence
  //show display
  //get button confirmation
  //activate magent
  stepper.step(stepsToShiftOneCell);
  //deacvtivate magnet
  //show display
}
