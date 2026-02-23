#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "freertos/queue.h"

// ================ REDE =================
const char* ssid = "mr robot";     
const char* password = "robot1234";     

// =============== PINAGEM =================
// Motores
#define motor_esquerdo 16
#define motor_direito 17
#define pwm_esquerdo 18
#define pwm_direito 19

// Sensores HCSR04
#define trig 27
#define echo_esq 34
#define echo_dir 35

// LEDs
#define ledSonarEsquerdo 4
#define ledSonarDireito 25

// Constantes
const int ACELERACAO = 12;        // O quanto aumenta a velocidade 
const int TEMPO_ACELERACAO = 15;  // Tempo entre acelerações
const int DISTANCIA_SEGURA = 12;  // Distancia segura dos sensores

// ========== VARIÁVEIS VOLATILE ==============
// volatile é obrigatório para variáveis usadas por núcleos diferentes

// Comandos
volatile char comandoWeb = 'S'; 
volatile int velocidadeAtual = 0; 
volatile bool obstaculoDetectado = false;

// Joystick
volatile int joyX = 0;
volatile int joyY = 0;

// ============= FLAGS VOLATILE ===============
volatile bool modoSegurancaTotal = true; // true = trava | false = libera

volatile bool modoGiro = false; // true = Giro | false = Curva

// Contar tempo auxiliar
volatile unsigned long tempoObstaculoDetectado = 0;

// Velocidade maxima
volatile int limitePwmGlobal = 255;
// ===========================================

// Aqui é apenas para mostrar que ele existe em outros arquivos
extern QueueHandle_t filaComandos; 

#endif