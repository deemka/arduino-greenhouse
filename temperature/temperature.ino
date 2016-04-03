#include <OneWire.h>
#include <DallasTemperature.h>
#include <Servo.h>

#define TEMP_PIN   2
#define SERVO_PIN  5
#define RELAIS_PIN 6

OneWire           oneWire(TEMP_PIN);
DallasTemperature sensors(&oneWire);
Servo             servo;

float t_meas;
int const t_crit[] = {25, 27, 40};
int NUM_OF_BINS = 0;
int mode = -1;
int prev_mode = 0;

const char* modes_breadmachine[] = {"Heizung An", "Alles Aus", "Luftung An"};
    
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

void servo_handler(const int mode)
{
    servo.attach(SERVO_PIN);
    servo.write(mode * 60);
    delay(1000);
    servo.detach();
}

void relais_handler(const int mode)
{
    switch (mode) {
    case 0:
	digitalWrite(RELAIS_PIN, HIGH);
	break;
    case 1:
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
}

void loop(void)
{
    sensors.requestTemperatures();
    t_meas = sensors.getTempCByIndex(0);
    Serial.print("T = ");
    Serial.print(int(t_meas + .5));
    Serial.print(" C (Stufe ");

    prev_mode = mode;
    mode = get_bin_index(t_crit, int(t_meas + .5));
    Serial.print(mode);
    Serial.print("), ");
    Serial.println(modes_breadmachine[mode]);
    
    if (mode != prev_mode) {
	relais_handler(mode);
    }

    delay(1000);
}
