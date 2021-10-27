#include <Arduino.h>
#include <EEPROM.h>

const int NUM_SLIDERS = 5;
int time,pev_time,analogMaster[NUM_SLIDERS],current_program = 0,pev_analog=0,stop_update=1,ps_analog=0;
int adrEEP=5;
int percentage_cutoff = 10, delay_time = 200,led_current_loop=0,led_pev_time = 0;
boolean led_active = false;
void updateSliderValues();
void updateCurrentProgram();
void sendSliderValues();
void printSliderValues();
void Test();
void saveData();
void recallData();
void led_blink_without_delay(boolean setting,int pin_led,int max_loop,int blink_time);
void setup() {
  // put your setup code here, to run once:
  pinMode(5,INPUT);
  pinMode(7,INPUT);
  pinMode(6,OUTPUT);
  Serial.begin(115200);
  time = millis();
  pev_time = time;
  pev_analog = analogRead(A0);
  recallData();
}

void loop() {
  // put your main code here, to run repeatedly:
  updateSliderValues();
  updateCurrentProgram();
  sendSliderValues(); // Actually send data (all the time)
  saveData();
  // printSliderValues(); // For debug
  time = millis();
  led_blink_without_delay(false,6,current_program,delay_time);
  delay(10);
}

void updateSliderValues() {
  ps_analog = analogRead(A0);
  if(stop_update == 1){
    while(((abs(pev_analog - ps_analog)*100)/pev_analog) < percentage_cutoff){
      ps_analog=analogRead(A0);
      // Serial.print("error percentage: ");
      // Serial.println(((abs(pev_analog - ps_analog)*100)/pev_analog));
      time = millis();
      led_blink_without_delay(false,6,current_program,delay_time);
      updateCurrentProgram();
      delay(50);
    }
    stop_update = 0;
  }
  else{analogMaster[current_program] = analogRead(A0);}
}

void updateCurrentProgram(){
  if(digitalRead(7)==1){
    while (digitalRead(7)==1)
    {
      // Serial.println("release button");
      delay(10);
    }
    analogMaster[current_program] = analogRead(A0);
    if(current_program == 0){current_program = NUM_SLIDERS-1;}
    else{current_program --;}
    led_blink_without_delay(true,6,current_program,delay_time);
    pev_analog=analogRead(A0);
    stop_update = 1;
  }
  else if (digitalRead(5)==1){
    while (digitalRead(5)==1)
    {
      // Serial.println("release button");
      delay(10);
    }
    analogMaster[current_program] = analogRead(A0);
    if(current_program == NUM_SLIDERS-1){current_program = 0;}
    else{current_program ++;}
    led_blink_without_delay(true,6,current_program,delay_time);
    pev_analog=analogRead(A0);
    stop_update = 1;
  }
  if(current_program > NUM_SLIDERS-1){current_program = 0;}
  else if(current_program < 0){current_program = NUM_SLIDERS;}
}

void sendSliderValues() {
  String builtString = String("");
  for (int i = 0; i < NUM_SLIDERS; i++) {
    builtString += String((int)analogMaster[i]);

    if (i < NUM_SLIDERS - 1) {
      builtString += String("|");
    }
  }
  
  Serial.println(builtString);
}

void printSliderValues() {
  Serial.print("Current Program: ");
  Serial.print(current_program);
  Serial.print("   |   ");
  for (int i = 0; i < NUM_SLIDERS; i++) {
    String printedString = String("Slider #") + String(i + 1) + String(": ") + String(analogMaster[i]) + String(" mV");
    Serial.write(printedString.c_str());

    if (i < NUM_SLIDERS - 1) {
      Serial.write(" | ");
    } else {
      Serial.write("\n");
    }
  }
}

void Test(){
  time = millis();
  if(time - pev_time >= 100){
    Serial.print("A0: ");
    Serial.print(analogRead(A0));
    Serial.print(" Digital 5 and 7: ");
    Serial.print(digitalRead(7));
    Serial.print(", ");
    Serial.println(digitalRead(5));
    pev_time = millis();
  }
}

void saveData(){
  for(int x = 0; x<= NUM_SLIDERS-1 ; x++){
    EEPROM.put(adrEEP,analogMaster[x]);
    adrEEP += sizeof(analogMaster[x]);
  }
  EEPROM.put(adrEEP,current_program);
  adrEEP=5;
}

void recallData(){
  for(int x = 0; x<= NUM_SLIDERS-1 ; x++){
    EEPROM.get(adrEEP,analogMaster[x]);
    // Serial.print("Data number ");
    // Serial.print(x);
    // Serial.print(" : ");
    // Serial.println(analogMaster[x]);
    adrEEP += sizeof(analogMaster[x]);
  }
  EEPROM.get(adrEEP,current_program);
  // Serial.print("Current Programe ");
  // Serial.print(" : ");
  // Serial.println(current_program);
  adrEEP=5;
}

void led_blink_without_delay(boolean setting,int pin_led,int max_loop,int blink_time){
  max_loop = (max_loop*2)+3;
  if(setting == true){
    led_active = true;
    digitalWrite(pin_led,LOW);
  }
  else if(setting == false&& led_active == true){
    if(time - led_pev_time >= blink_time){
      digitalRead(pin_led) == HIGH ? digitalWrite(pin_led,LOW) : digitalWrite(pin_led,HIGH);
      led_pev_time = time;
      led_current_loop = led_current_loop +1;
    }
    if(led_current_loop >= max_loop){
      digitalWrite(pin_led,LOW);
      led_current_loop = 0;
      led_active = false;
    }
  }
  else{
    digitalWrite(pin_led,LOW);
  }
}