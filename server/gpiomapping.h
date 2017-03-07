#ifndef GPIOMAPPING_H
#define GPIOMAPPING_H


#define DEFAULT_NEMA_PAUSE 10000

// here are the various devices we control

#define ART1 0 
#define ART2 1
#define ART3 2
#define ART4 3
#define ART5 4
#define ART6 5
#define VISE 6
#define GRIPPER 7


// here is the mapping device -> motors (motor 1, motor 2, 0 = same, 1 = opposed)

static const short device_to_motors[][3] = {
  { 1, 0, 0 },
  { 2, 3, 0 },
  { 4, 0, 0 },
  { 5, 0, 0 },
  { 6, 7, 0 },
  { 6, 7, 1 },
  { 8, 0, 0 }
};


// and finally the mapping motor -> pins (dir, step)

static const short motor_to_pins[][2] = {
  { 12, 16 },
  { 27, 17 },
  { 5, 22},
  { 13, 6},
  { 26, 19},
  { 23, 24},
  { 20, 21},
  { 15, 14 }
};


#define MOTOR_1(__device__) device_to_motors[__device__ - 1][0]
#define MOTOR_2(__device__) device_to_motors[__device__ - 1][1]
#define DIFFERENTIAL(__device__, __dir__) (device_to_motors[__device__ - 1][2] * (1 - __dir__) + (1 - device_to_motors[__device__ - 1][2]) * __dir__)


#define DIRECTION_PIN(__motor__) motor_to_pins[__motor__ - 1][0]
#define STEP_PIN(__motor__) motor_to_pins[__motor__ - 1][1]



#endif


#define GPIO_ENABLE_PIN 10
#define PWM_PIN 18


#define MAX_PWM 1500
#define MIN_PWM 1010

#define PWM_STEP 20
