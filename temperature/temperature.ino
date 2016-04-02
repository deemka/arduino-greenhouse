#include <OneWire.h>
#include <DallasTemperature.h>
#include <Servo.h>

#define TEMP_PIN  2
#define SERVO_PIN 5

OneWire           oneWire(TEMP_PIN);
DallasTemperature sensors(&oneWire);
Servo             servo; 

float t_meas;
int const t_crit[] = {20, 30, 40};
int NUM_OF_BINS = 0;
int mode = -1;
int prev_mode = 0;

int get_bin_index(int const ranges[], int meas)
{
    int i;
    for (i=0; i<NUM_OF_BINS-1; i++) {
	if (meas <= ranges[i]) {
	    return i;
	}
    }
    return i;
}

void setup(void)
{
    Serial.begin(9600);
    sensors.begin();
    NUM_OF_BINS = sizeof(t_crit)/sizeof(t_crit[0]) + 1;

    servo.attach(SERVO_PIN);
}

void loop(void)
{ 
    sensors.requestTemperatures();
    t_meas = sensors.getTempCByIndex(0);
    Serial.print("T = ");
    Serial.print(t_meas);
    Serial.print(" C (Stufe ");

    prev_mode = mode;
    mode = get_bin_index(t_crit, int(t_meas+.5));
    Serial.print(mode);
    Serial.println(")");

    if (mode != prev_mode) {
	servo.attach(SERVO_PIN);
	servo.write(mode*60);
	delay(1000);
	servo.detach();
    }

    delay(1000);
}
