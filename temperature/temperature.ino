#include <OneWire.h>
#include <DallasTemperature.h>
#include <Servo.h>
#include "stats.h"

#define TEMP_PIN   2
#define SERVO_PIN  5
#define RELAIS_PIN 6

OneWire           oneWire(TEMP_PIN);
DallasTemperature sensors(&oneWire);
Servo             servo;

float t_meas;
int const t_crit[] = {26, 28, 40};
int NUM_OF_BINS = 0;
int temp_level = -1;
int prev_temp_level = 0;
int mode = 0;
String mode_str[] = {"Heizung AN", "Heizung AUS"};
stats_t hist;

int get_bin_index(int const ranges[], int meas)
{
    int i;
    for (i = 0; i < NUM_OF_BINS - 1; i++) {
	if (meas < ranges[i]) {
	    return i;
	}
    }
    return i;
}

void servo_handler(const int temp_level)
{
    servo.attach(SERVO_PIN);
    servo.write(temp_level * 60);
    delay(1000);
    servo.detach();
}

void relais_handler(const int temp_level)
{
    switch (temp_level) {
    case 0:
	digitalWrite(RELAIS_PIN, HIGH);     /* T < Tmin */
	mode = 1;
	break;
    case 1:
	if (temp_level > prev_temp_level) {
	    mode = 1;
	    digitalWrite(RELAIS_PIN, HIGH); /* Tmin < T < Tmax, increasing, keep heating */
	} else {
	    mode = 0;
	    digitalWrite(RELAIS_PIN, LOW);  /* Tmin < T < Tmax, decreasing, stop heating */
	}
	break;
    case 2:
	mode = 0;
	digitalWrite(RELAIS_PIN, LOW);
	break;
    }
}

void setup(void)
{
    Serial.begin(9600);
    sensors.begin();
    NUM_OF_BINS = sizeof(t_crit) / sizeof(t_crit[0]) + 1;

    servo.attach(SERVO_PIN);
    pinMode(RELAIS_PIN, OUTPUT);

    int i;
    for (i=0; i<8; i++) {
	hist.hist[i] = 0;
    }
}

void loop(void)
{
    sensors.requestTemperatures();
    t_meas = sensors.getTempCByIndex(0);

    prev_temp_level = temp_level;
    temp_level = get_bin_index(t_crit, int(t_meas + .5));

    if (temp_level != prev_temp_level) {
	relais_handler(temp_level);
    }

    increment_stats(&hist, mode);
    
    Serial.write(27);       // ESC command
    Serial.print("[2J");    // clear screen command
    Serial.write(27);
    Serial.print("[H");     // cursor to home command

    Serial.print("T = ");
    Serial.print(int(t_meas + .5));
    Serial.print(" C, ");
    Serial.print(mode_str[mode]);
    Serial.print(" AN / AUS [s] : ");
    Serial.print(hist.hist[1]);
    Serial.print(" / ");
    Serial.println(hist.hist[0]);
        
    delay(1000);
}
