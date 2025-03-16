#include <Arduino.h>
#include <TaskScheduler.h>

#define BOARD_RATE 9600

// LED On 시간
struct LightTimes {
  int red;
  int yellow;
  int green;
  int greenBlink;
  int greenBlinkInterval;
};

const LightTimes TIMES = {/*red*/2000, /*yellow*/500, /*green*/2000, /*greenBlink*/1000, /*greenBlinkInterval*/160};

// LED 핀
const int redPin = 11;
const int yellowPin = 10;
const int greenPin = 9;


// 가변저항 핀
const int resPin = A0;

// 버튼 핀
const int emergencyBtnPin = 6;
const int allBlinkBtnPin = 5;
const int onOffBtnPin = 4;

int brightness = 0;

// 노란불 교통신호가 끝났는지 확인
bool isEnd = false;
// 비상 버튼이 눌렸는지 확인
bool isEmergency = false;
// 모든 신호등 깜빡임 버튼이 눌렸는지 확인
bool isAllBlink = false;
// on/off 버튼이 눌렸는지 확인
bool isOn = false;
// 버튼 더블 클릭 확인
unsigned long prevPressTime = 0;
unsigned long currentTime = 0;

// 함수 선언
bool redOE();
void redOD();

bool yellowOE();
void yellowOD();

bool greenOE();
void greenOD();

bool greenBlinkOE();
void greenBlinkCB();
void greenBlinkOD();

bool allBlinkOE();
void allBlinkCB();
void allBlinkOD();

void emergencyBtnPressed();
void allBlinkBtnPressed();
void onOffBtnPressed();

void allLedStop();
bool isDoublepressed();


// TaskScheduler 객체 생성
Scheduler ts;

Task red(TIMES.red, TASK_ONCE, NULL, &ts, false, redOE, redOD);
Task yellow(TIMES.yellow, TASK_ONCE, NULL, &ts, false, yellowOE, yellowOD);
Task green(TIMES.green, TASK_ONCE, NULL, &ts, false, greenOE, greenOD);
Task greenBlink(TIMES.greenBlinkInterval, TIMES.greenBlink / TIMES.greenBlinkInterval, greenBlinkCB, &ts, false, greenBlinkOE, greenBlinkOD);

Task allBlink(TIMES.yellow, TASK_FOREVER, allBlinkCB, &ts, false, allBlinkOE, allBlinkOD);


// setup 함수
void setup() {
  Serial.begin(BOARD_RATE);

  // 핀 모드 설정
  pinMode(redPin, OUTPUT);
  pinMode(yellowPin, OUTPUT);
  pinMode(greenPin, OUTPUT);  

  pinMode(resPin, INPUT);

  pinMode(emergencyBtnPin, INPUT_PULLUP);
  pinMode(allBlinkBtnPin, INPUT_PULLUP);
  pinMode(onOffBtnPin, INPUT_PULLUP);

  // 인터럽트 설정
  attachInterrupt(digitalPinToInterrupt(emergencyBtnPin), emergencyBtnPressed, FALLING);
  attachInterrupt(digitalPinToInterrupt(allBlinkBtnPin), allBlinkBtnPressed, FALLING);
  attachInterrupt(digitalPinToInterrupt(onOffBtnPin), onOffBtnPressed, FALLING);

  // 신호등 시작
  red.restartDelayed();
}


// loop 함수
void loop() {
  ts.execute();

  // 버튼 입력 확인
  int emergencyBtn = digitalRead(emergencyBtnPin);
  int blinkBtn = digitalRead(allBlinkBtnPin);
  int onOffBtn = digitalRead(onOffBtnPin);

  // 비상 버튼이 눌렸을 때 시간 측정해서 더블 클릭 확인
  if (!emergencyBtn) {
    currentTime = millis();
    if (!isDoublepressed()) {
      emergencyBtnPressed();
      prevPressTime = currentTime;
    }
  }

  // 모든 신호등 깜빡임 버튼이 눌렸을 때 시간 측정해서 더블 클릭 확인
  if (!blinkBtn) {
    currentTime = millis();
    if (!isDoublepressed()) {
      allBlinkBtnPressed();
      prevPressTime = currentTime;
    }
  }

  // on/off 버튼이 눌렸을 때 시간 측정해서 더블 클릭 확인
  if (!onOffBtn) {
    currentTime = millis();
    if (!isDoublepressed()) {
      onOffBtnPressed();
      prevPressTime = currentTime;
    }
  }

  // 가변저항 값 확인
  int resValue = analogRead(resPin);
  if (resValue - brightness > 1 || brightness - resValue > 1) {
    brightness = resValue;
    Serial.println(String(brightness) + '\n');
  }
}

// 시리얼 데이터 받기
void serialEvent() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');

    String color = input.substring(0, 1);
    String time = input.substring(1);
    
    int period = time.toInt();

    if (period > 0) {
      if (color == "r") {
        red.setInterval(period);
      } else if (color == "y") {
        yellow.setInterval(period);
      } else if (color == "g") {
        green.setInterval(period);
      }
    }
  }
}


// 신호등 콜백 함수
// 빨간불 켜기
bool redOE() {
  analogWrite(redPin, brightness);
  Serial.println("r\n");
  return true;
}
void redOD() {
  analogWrite(redPin, LOW);
  yellow.restartDelayed();
}

// 노란불 켜기
bool yellowOE() {
  analogWrite(yellowPin, brightness);
  Serial.println("y\n");
  return true;
}
void yellowOD() {
  analogWrite(yellowPin, LOW);
  if (isEnd) {
    red.restartDelayed();
  } else {
    green.restartDelayed();
  }
  isEnd = !isEnd;
}

// 초록불 켜기
bool greenOE() {
  analogWrite(greenPin, brightness);
  Serial.println("g\n");
  return true;
}
void greenOD() {
  analogWrite(greenPin, LOW);
  greenBlink.restartDelayed();
}

// 초록불 깜빡임
bool greenState;
bool greenBlinkOE() {
  analogWrite(greenPin, LOW);
  Serial.println("o\n");
  greenState = LOW;
  return true;
}
void greenBlinkCB() {
  greenState = greenState ^ 1;
  analogWrite(greenPin, greenState);
  if (greenState == HIGH) {
    Serial.println("g\n");
  } else {
    Serial.println("o\n");
  }
}
void greenBlinkOD() {
  analogWrite(greenPin, LOW);
  red.restartDelayed();
}

// 비상 버튼 눌렀을 때
void emergencyBtnPressed() {
  isEmergency = !isEmergency;
  Serial.println("e\n");
  if (isEmergency) {
    allLedStop();
    analogWrite(redPin, brightness);
  } else {
    isEnd = false;
    red.restartDelayed();
  }
}

// 모든 신호등 깜빡임
bool allState = false;
void allBlinkCB() {
  allState = allState ^ 1;
  analogWrite(redPin, allState);
  analogWrite(yellowPin, allState);
  analogWrite(greenPin, allState);
  if (allState == HIGH) {
    Serial.println("a\n");
  }
  else {
    Serial.println("o\n");
  }
}
bool allBlinkOE() {
  analogWrite(redPin, LOW);
  analogWrite(yellowPin, LOW);
  analogWrite(greenPin, LOW);
  Serial.println("o\n");
  allState = LOW;
  return true;
}
void allBlinkOD() {
  analogWrite(redPin, LOW);
  analogWrite(yellowPin, LOW);
  analogWrite(greenPin, LOW);
}

// 모든 신호등 깜빡임 버튼 눌렀을 때
void allBlinkBtnPressed() {
  isAllBlink = !isAllBlink;
  if (isAllBlink) { 
    allLedStop();
    allBlink.restartDelayed();
  } else {
    allBlink.abort();
    red.restartDelayed();
  }
}

// on/off 버튼 눌렀을 때
void onOffBtnPressed() {
  isOn = !isOn;
  if (isOn) {
    allLedStop();
    Serial.println("o\n");
  } else {
    red.restartDelayed();
  }
}


// 모든 LED 끄기
void allLedStop() {
  analogWrite(redPin, LOW);
  analogWrite(yellowPin, LOW); 
  analogWrite(greenPin, LOW);

  red.abort();
  yellow.abort();
  green.abort();
  greenBlink.abort();
}

// 더블 클릭 확인
bool isDoublepressed() {
  return (currentTime - prevPressTime) < 200 ? true : false;
}