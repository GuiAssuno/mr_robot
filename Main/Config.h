#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include "freertos/queue.h"
// Handle da fila
extern QueueHandle_t filaComandos;

// ================= REDE (HOTSPOT DO NOTEBOOK) =================
const char* ssid = "mr robot";     // Nome do host
const char* password = "robot1234";     // Senha

// IP Fixo para o Robô (Controle)
IPAddress local_IP(192, 168, 15, 20);   // Ajuste conforme seu Hotspot (50.x para Linux, 137.x para Windows)
IPAddress gateway(192, 168, 50, 1);
IPAddress subnet(255, 255, 255, 0);

// ================== VARIÁVEIS COMPARTILHADAS ==================
// 'volatile' avisa ao compilador que essa variável pode mudar 
// a qualquer momento por outro núcleo.
volatile char comandoWeb = 'S'; // 'F', 'B', 'L', 'R', 'S'
volatile int velocidadeAtual = 0;
volatile bool obstaculoDetectado = false;

// ================= PINAGEM =================
#define motor_esquerdo 16
#define motor_direito 17
#define pwm_esquerdo 18
#define pwm_direito 19

// Sensores
#define trig 27
#define echo_esq 34
#define echo_dir 35

// Led
#define ledSonarEsquerdo 4
#define ledSonarDireito 25

// Sensor faixa



// Declaração de funções
void Back();
void stop();
void Parar();
void Frente();
void Direita();
int autosafe();
void Esquerda();
String getHTML();
void Gira(bool lado);
void comando(char inst);
char mapCommand(String cmd);

// Declarando as variaveis
HCSR04 distancia_esquerda(trig, echo_esq),  //variaveis de leitura do modulo HC-SR04
  distancia_direita(trig, echo_dir);        // trigger disparado juntos pelo mesmo pino

float M_distancia[7][2];
void (*fucao)();

enum { FRENTE = 0,
       BACK = 1,
       PARAR = 2 };

unsigned long espera = 0,
              Millis = 0;

int x = 0,
    y = 0,
    pwm = 0,
    velo_atual = 0;

bool modo = true,
     curva = false,
     parado = true,
     press_l = false,
     press_r = false,
     liberado = false,
     flag = false,
     direction = false;

const int safe = 12,
          marcha = 50,
          base_vira = 15,
          delay_millis = 100,
          velo_limite = 200;

#endif