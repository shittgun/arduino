#include <SoftwareSerial.h>

#define BT_RXD 2
#define BT_TXD 3
SoftwareSerial hc06(BT_RXD, BT_TXD);

// 초기 시간 설정 (11:00:00)
unsigned long startMillis;
int startHour = 11;
int startMinute = 0;
int startSecond = 0;

int idx = 0;
bool is_seated = false;

void setup()
{
  Serial.begin(9600);
  hc06.begin(9600);

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

void loop()
{
  if (idx++ % 3 == 0) is_seated = !is_seated;

  String timeStr = getCurrentTimeString();
  String stateStr = is_seated ? "출석중" : "결석중";
  String output = "목/" + timeStr + "/" + stateStr;

  Serial.println(output);
  hc06.println(output);
  delay(1000);
}