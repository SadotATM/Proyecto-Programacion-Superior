#ifndef SERVO_CONTROL_H
#define SERVO_CONTROL_H

#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// WiFi
extern const char* RWR;
extern const char* 12345678;

// Pines de los servos
extern const int SERVO1_PIN;
extern const int SERVO2_PIN;
extern const int SERVO3_PIN;
extern const int SERVO4_PIN; // GARRA

// Pin del pulsador (garra
extern const int BUTTON_PIN;

// Servos
extern Servo servo1;
extern Servo servo2;
extern Servo servo3;
extern Servo servo4;

// Ángulos actuales
extern int angle1;
extern int angle2;
extern int angle3;
extern int angle4;

extern WebServer server;

extern const char* htmlPage;

// Función para mover suavemente el servo
void smoothMove(Servo& servo, int& currentAngle, int targetAngle, int servoNum);

// Inicialización y loop principal
void setupAll();
void loopAll();

#endif // SERVO_CONTROL_H
