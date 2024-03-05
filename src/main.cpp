#include <Arduino.h>
#include <Ultrasonic.h>
#include <Servo.h>

#define TRIG  6
#define ECHO  7
#define SERVO 3

#define SERVO_MIN 0   // closed mouth
#define SERVO_MAX 60  // open mouth

#define DIST_MAX  40  // cm
#define DIST_MIN  5

#define FILTER_STEP 4

Ultrasonic ultrasonic(TRIG, ECHO);
unsigned int distance_old = DIST_MAX;
unsigned int dst[5] = {DIST_MAX};
uint8_t i = 0;

Servo servo;

void setup() {
  servo.attach(SERVO);
  servo.write(0);
  Serial.begin(115200);
}

int compare(const void* a, const void* b) {
     int int_a = * ( (int*) a );
     int int_b = * ( (int*) b );

     if ( int_a == int_b ) return 0;
     else if ( int_a < int_b ) return -1;
     else return 1;
}

unsigned int get_filtered(void) {
  qsort(dst, (sizeof(dst) / sizeof(dst[0])), sizeof(unsigned int), compare);
  return dst[(sizeof(dst) / sizeof(dst[0])) / 2];
}

void loop() {
  unsigned int distance = ultrasonic.read(); // raw
  distance = constrain(distance, DIST_MIN, DIST_MAX);

  dst[i++] = distance;
  if(i > (sizeof(dst) / sizeof(dst[0]))) {
    i = 0;
  }

  distance = get_filtered();

  if(abs(distance_old - distance) > FILTER_STEP) {
    distance = distance_old + ((distance > distance_old)?FILTER_STEP:(FILTER_STEP*(-1)));
  }
  distance_old = distance;

  uint16_t val = (uint16_t)map(distance, DIST_MIN, DIST_MAX, SERVO_MAX, SERVO_MIN);
  servo.write(val);
  
  // Serial.print("Distance in CM: ");
  // Serial.println(distance);
  delay(50);
}
