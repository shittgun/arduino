#include <HX711.h>
#include "BluetoothSerial.h"

BluetoothSerial SerialBT;
HX711 scale1;

#define DT1 32
#define SCK1 17

float last_weight = 0;
bool is_seated = false;

// 초기 시간 설정 (11:00:00)
unsigned long startMillis;
int startHour = 11;
int startMinute = 0;
int startSecond = 0;

void setup() {
  Serial.begin(9600);
  SerialBT.begin("ESP32-Chair");

  scale1.begin(DT1, SCK1);
  delay(3000);

  // if (scale1.is_ready()) {
  //   Serial.println("Scale ready");
  // } else {
  //   Serial.println("Scale not found.");
  //   while (1);
  // }

  scale1.set_scale();
  scale1.tare();

  last_weight = scale1.get_units(10);
  startMillis = millis();
}

String getCurrentTimeString() {
  unsigned long elapsedMillis = millis() - startMillis;
  unsigned long totalSeconds = elapsedMillis / 1000;

  int hour = startHour + (totalSeconds / 3600);
  int minute = startMinute + ((totalSeconds % 3600) / 60);
  int second = startSecond + (totalSeconds % 60);

  // 오버플로우 정리
  if (second >= 60) {
    minute += second / 60;
    second = second % 60;
  }
  if (minute >= 60) {
    hour += minute / 60;
    minute = minute % 60;
  }
  if (hour >= 24) {
    hour = hour % 24;
  }

  // 포맷팅된 시간 문자열 반환
  char buf[16];
  sprintf(buf, "%02d:%02d:%02d", hour, minute, second);
  return String(buf);
}

void loop() {
  float weight_now = scale1.get_units(10);
  float diff = weight_now - last_weight;
  last_weight = weight_now;

  float abs_diff = abs(diff);

  // 출석/결석 판단
  if (abs_diff >= 1000) {
    is_seated = true;
  } else if (abs_diff <= 90) {
    is_seated = false;
  }

  // 출력
  // Serial.print("W: ");
  // Serial.print(weight_now, 2);
  // Serial.print("  diff: ");
  // Serial.print(diff, 2);
  // Serial.print(" (|diff|: ");
  // Serial.print(abs_diff, 2);
  // Serial.print(")  상태: ");
  // Serial.println(is_seated ? "출석" : "결석");
  // SerialBT.println(is_seated ? "출석" : "결석");

  // 최종 출력 문자열 구성
  String timeStr = getCurrentTimeString();
  String stateStr = is_seated ? "출석중" : "결석중";
  String output = "목/" + timeStr + "/" + stateStr;

  Serial.println(output);
  SerialBT.println(output);

  delay(500);
}
