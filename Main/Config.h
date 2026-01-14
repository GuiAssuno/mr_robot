#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ================= REDE (HOTSPOT DO NOTEBOOK) =================
const char* ssid = "ControlCenter";     // Nome do Hotspot do Linux/Windows
const char* password = "robot1234";     // Senha
// IP Fixo para o Robô (Controle)
IPAddress local_IP(192, 168, 50, 20);   // Ajuste conforme seu Hotspot (50.x para Linux, 137.x para Windows)
IPAddress gateway(192, 168, 50, 1);
IPAddress subnet(255, 255, 255, 0);

// ================= PINAGEM =================
#define MOTOR_ESQ_DIR 16  // Direção
#define MOTOR_DIR_DIR 17
#define PWM_ESQ 18        // Velocidade
#define PWM_DIR 19

#define TRIG_PIN 27
#define ECHO_ESQ 34
#define ECHO_DIR 35

// ================= AJUSTES =================
const int DISTANCIA_SEGURA = 20; // cm
const int VELOCIDADE_MAX = 200;  // 0 a 255
const int RAMPA_PASSO = 20;      // Quanto acelera por ciclo
const int RAMPA_DELAY = 15;      // ms entre incrementos

#endif