#ifndef ACCELEROMETER_H
#define ACCELEROMETER_H

typedef struct {
	double r;
	double theta;
	double alpha;
} Angle;

void Accel_setup();
void Accel_poll();
Angle Accel_getAngle();

#endif