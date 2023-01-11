#include <Stepper.h>
#include <Wire.h>
#include <RTClib.h>

const int stepsPerRevolution = 200;
Stepper stepper = Stepper(stepsPerRevolution, 2, 3, 4, 5);
//steps needed for = (stepsPerRevolution / 360) * 15
int stepsToShiftOneCell = 8;

DateTime now;
RTC_DS3231 rtc;

String data[] = {"d|8,12|1|2"};
// d -> daily , 8,12 -> timings , 1 -> moudle number , 2 -. amount


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  stepper.setSpeed(10);

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC Module");
    while (1)
      ;
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
}

void loop() {
  // put your main code here, to run repeatedly:
  now = rtc.now();


  if (now.minute() == 00 && now.second() == 00) {
    dispenseSequence();
  }
}

void dispenseSequence() {

  for (int i = 0; i < 1; i++) {
    String s = data[i] + '|';

    //checks if its the day to dispense
    char mode = s.substring(0, s.indexOf('|')).charAt(0);
    s = s.substring(s.indexOf('|' + 1));


    if (mode == 'd') { //d|8,12|1|2|
      String timing = s.substring(0, s.indexOf('|'));
      s = s.substring(s.indexOf('|' + 1));      
      timing += ',';

      int begin = 0;
      for (int c = 0; c < timing.length(); c++) {
        if (timing.charAt(c) == ',') {
          int tmp = timing.substring(begin, c).toInt();

          if(now.hour() == tmp){

            int module = s.substring(0,s.charAt('|')).toInt();
            s = s.substring(s.indexOf('|' + 1));

            int amount = s.substring(0,s.charAt('|')).toInt();
            s = s.substring(s.indexOf('|' + 1));

            dispenseMedicine(module, amount);
          }

          begin = c + 1;
        }
      }
    }

    if(mode == 'a'){ //"mo|2,20|3|1"

      int day = parseDay(s.substring(0,s.indexOf('|')));
      int check;
      if(day == 6){
        check = 1;
      } else if(day == 7){
        check = 2;
      }else{
        check = day;
      }
      if(now.dayOfTheWeek() == check){
        String timing = s.substring(0, s.indexOf('|'));
        s = s.substring(s.indexOf('|' + 1));      
        timing += ',';

        int begin = 0;
        for (int c = 0; c < timing.length(); c++) {
          if (timing.charAt(c) == ',') {
            int tmp = timing.substring(begin, c).toInt();

            if(now.hour() == tmp){

              int module = s.substring(0,s.charAt('|')).toInt();
              s = s.substring(s.indexOf('|' + 1));

              int amount = s.substring(0,s.charAt('|')).toInt();
              s = s.substring(s.indexOf('|' + 1));

              dispenseMedicine(module, amount);
            }

            begin = c + 1;
          }
        }
      }
    }           

    if(mode == 'w'){ //"mo|2,20|3|1"
      int day = parseDay(s.substring(0,s.indexOf('|')));
      if(now.dayOfTheWeek() == day){
        String timing = s.substring(0, s.indexOf('|'));
        s = s.substring(s.indexOf('|' + 1));      
        timing += ',';

        int begin = 0;
        for (int c = 0; c < timing.length(); c++) {
          if (timing.charAt(c) == ',') {
            int tmp = timing.substring(begin, c).toInt();

            if(now.hour() == tmp){

              int module = s.substring(0,s.charAt('|')).toInt();
              s = s.substring(s.indexOf('|' + 1));

              int amount = s.substring(0,s.charAt('|')).toInt();
              s = s.substring(s.indexOf('|' + 1));

              dispenseMedicine(module, amount);
            }

            begin = c + 1;
          }
        }
      }
    }  

  }
}

void dispenseMedicine(int module, int amount) {

  //buzzer sequence
  //show display
  //get button confirmation
  //activate magent
  stepper.step(stepsToShiftOneCell);
  //deacvtivate magnet
  //show display

}

int parseDay(String s){
  if(s.equals("mo")){
    return 1;
  }
  else if(s.equals("tu")){
    return 2;
  }
  else if(s.equals("we")){
    return 3;
  }
  else if(s.equals("th")){
    return 4;
  }
  else if(s.equals("fr")){
    return 5;
  }
  else if(s.equals("sa")){
    return 6;
  }
  else if(s.equals("su")){
    return 7;
  } else{
    return -1;
  }  
}
