#include <Stepper.h>
#include <Wire.h>
#include <RTClib.h>
#include <SoftwareSerial.h>

//stepper motor
const int stepsPerRevolution = 200;
Stepper stepper = Stepper(stepsPerRevolution, 2, 3, 4, 5);
//steps needed for = (stepsPerRevolution / 360) * 15
int stepsToShiftOneCell = 8;

//rtc
DateTime now;
RTC_DS3231 rtc;

//firebase and arduino
SoftwareSerial nodemcu(A0, A1);
String data[5]; //d|8,12|1|2  //a|mo|8,20|2|1
// d -> daily , 8,12 -> timings , 1 -> moudle number , 2 -. amount

//electromagnet
int magnetSignal = 8;
int s0 = 9;
int s1 = 10;
int s2 = 11;
int s3 = 12;

//button & buzzer
int buttonPin = A2;
bool buttonPressed = false;
int buzzerPin = A3;


void setup() {
  Serial.begin(9600);
  stepper.setSpeed(10);

  //rtc
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC Module");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, lets set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  
  //magnets
  pinMode(magnetSignal, OUTPUT);
  pinMode(s0, OUTPUT);
  pinMode(s1, OUTPUT);
  pinMode(s2, OUTPUT);
  pinMode(s3, OUTPUT);

  //button & buzzer
  pinMode(buttonPin, INPUT);
  pinMode(buzzerPin, OUTPUT);

  data[0] = "d|2:08,12:40|1|2";
}

void loop() {
  now = rtc.now();

  long m = millis();

    Serial.print(now.hour());
    Serial.println(now.minute());

  if(now.hour() == 00 && now.minute() == 00){
    //getDispenseData();
  }

  //checks every minute what pills need to be dispensed
  if (now.second() == 00) {
    dispenseSequence();
  }
  
}

//nodemcu 
void getDispenseData(){
  nodemcu.write("dispenseData");
  while(!nodemcu.available()>0){
    continue;
  }

  String rawdata = nodemcu.readString(); // d|8,12|1|2;a|mo|8, 20|2|1;w|we|18|3|1

  int begin = 0;
  int idx = 0;
  for(int i=0;i<rawdata.length();i++){
    if(rawdata.charAt(i) == ';'){
      data[idx] = rawdata.substring(begin,i);
      idx++;
      rawdata = rawdata.substring(i+1);
      begin = i+1;
    }
  }
}

// dispense sequence
void dispenseSequence() {

  String dispenseLog[5];
  for(int i=0;i<5;i++){
    dispenseLog[i] = "empty";
  }
  int dispenseLogIdx = 0;

  for (int i = 0; i < 5; i++) {
    String s = data[i] + '|';

    //checks if its the day to dispense
    char mode = s.substring(0, s.indexOf('|')).charAt(0);
    s = s.substring(s.indexOf('|' + 1));
    Serial.println(mode);

    if (mode == 'd') { //d|8:15,12:09|1|2|
      String timing = s.substring(0, s.indexOf('|'));
      s = s.substring(s.indexOf('|' + 1));      
      timing += ',';

      int begin = 0;
      for (int c = 0; c < timing.length(); c++) {
        if (timing.charAt(c) == ',') {
          String s = timing.substring(begin, c);

          int hr = s.substring(0,s.indexOf(':')).toInt();
          int min = s.substring(s.indexOf(':'+1)).toInt();
          
          Serial.print(hr);
          Serial.print(" : ");
          Serial.println(min);

          if(now.hour() == hr && now.minute() == min){

            int module = s.substring(0,s.charAt('|')).toInt();
            s = s.substring(s.indexOf('|' + 1));

            int amount = s.substring(0,s.charAt('|')).toInt();
            s = s.substring(s.indexOf('|' + 1));

            String tmp = module+"";
            tmp += ",";
            tmp += amount; 

            Serial.print(module);
            Serial.print( " , ");
            Serial.print(amount);
            
            dispenseLog[dispenseLogIdx] = module+","+amount;
            dispenseLogIdx++;
            //dispenseMedicine(module, amount);
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
          String s = timing.substring(begin, c);

          int hr = s.substring(0,s.indexOf(':')).toInt();
          int min = s.substring(s.indexOf(':'+1)).toInt();

          if(now.hour() == hr && now.minute() == min){

              int module = s.substring(0,s.charAt('|')).toInt();
              s = s.substring(s.indexOf('|' + 1));

              int amount = s.substring(0,s.charAt('|')).toInt();
              s = s.substring(s.indexOf('|' + 1));
              
              dispenseLog[dispenseLogIdx] = module+","+amount;
              dispenseLogIdx++;
              //dispenseMedicine(module, amount);
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
          String s = timing.substring(begin, c);

          int hr = s.substring(0,s.indexOf(':')).toInt();
          int min = s.substring(s.indexOf(':'+1)).toInt();

          if(now.hour() == hr && now.minute() == min){

              int module = s.substring(0,s.charAt('|')).toInt();
              s = s.substring(s.indexOf('|' + 1));

              int amount = s.substring(0,s.charAt('|')).toInt();
              s = s.substring(s.indexOf('|' + 1));

              dispenseLog[dispenseLogIdx] = module+","+amount;
              dispenseLogIdx++;
              //dispenseMedicine(module, amount);
            }

            begin = c + 1;
          }
        }
      }
    }  
  }

  bool flag = false;
  for(int i=0;i<5;i++){
    if(!dispenseLog[i].equals("empty")){
      flag = true;
    }
  }

  if(flag){
    buzzerSequence();

    if(buttonPressed){
      //show display("Dispensing")

      for(int i=0;i<5;i++){

      if(!dispenseLog[i].equals("empty")){
        continue;
      }

        String module = dispenseLog[i].substring(0,dispenseLog[i].indexOf('|'));
        String amount = dispenseLog[i].substring(dispenseLog[i].indexOf('|')+1);

        dispenseMedicine(module.toInt(), amount.toInt());
      }
    }
  } 
}

void buzzerSequence(){

  int startTime = now.minute();

  while(now.minute() - startTime <= 5){
    
    if(digitalRead(buttonPin) == 1){
      buttonPressed = true;
      break;
    }
  }

}

void dispenseMedicine(int module, int amount) {

  activateElectromagnet(module);

    for (int i = 0; i < amount; i++) {
      stepper.step(stepsToShiftOneCell);
      //sendLog("timestamp:success");
      delay(1000);
    }

    deactivateElectromagnet(module);
    buttonPressed = false;

}

//other helper methods
void activateElectromagnet(byte module) {

  digitalWrite(s0, bitRead(module, 0));
  digitalWrite(s1, bitRead(module, 1));
  digitalWrite(s2, bitRead(module, 2));
  digitalWrite(s3, bitRead(module, 3));

  digitalWrite(magnetSignal, HIGH);
}

void deactivateElectromagnet(byte module) {
  digitalWrite(s0, bitRead(module, 0));
  digitalWrite(s1, bitRead(module, 1));
  digitalWrite(s2, bitRead(module, 2));
  digitalWrite(s3, bitRead(module, 3));

  digitalWrite(magnetSignal, LOW);
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
