#ifndef HARDWARE_CORE_H
#define HARDWARE_CORE_H

#include "Config.h"
#include <HCSR04.h> // Sua lib de ultrassom

HCSR04 sensor(PIN_TRIG, new int[2]{PIN_ECHO_ESQ, PIN_ECHO_DIR}, 2); // Exemplo construtor

// Função auxiliar para controlar motores (apenas exemplo lógico)
void aplicarMotores() {
  if (obstaculoDetectado) {
     // Força parada independente do comando
     analogWrite(PIN_PWM_ESQ, 0);
     analogWrite(PIN_PWM_DIR, 0);
     return;
  }

  char cmd;
  // Lê o comando que veio do Núcleo 1 (Web)
  if (xQueueReceive(filaComandos, &cmd, 0) == pdTRUE) {
    switch(comandoWeb) {
      case 'F': 
        // Lógica de frente...
        analogWrite(PIN_PWM_ESQ, 200); 
        analogWrite(PIN_PWM_DIR, 200);
        break;
      case 'S':
        analogWrite(PIN_PWM_ESQ, 0);
        analogWrite(PIN_PWM_DIR, 0);
        break;
      // ... outros casos
    }
}

// === ESTA É A FUNÇÃO PRINCIPAL DO NÚCLEO 0 ===
void taskHardware(void * parameter) {
  
  // 1. SETUP DO NÚCLEO 0 (Configura pinos aqui)
  pinMode(PIN_MOTOR_ESQ_DIR, OUTPUT);
  // ... outros pinmodes
  
  // 2. LOOP INFINITO DO NÚCLEO 0
  for(;;) {
    
    // A. Ler Sensores
    float dist = sensor.dist(0); // Exemplo
    if (dist > 0 && dist < 20) {
      obstaculoDetectado = true;
    } else {
      obstaculoDetectado = false;
    }

    // B. Mover Robô
    aplicarMotores();

    // C. OBRIGATÓRIO: Respiro para o Watchdog
    // Se tirar isso, o ESP32 reinicia sozinho!
    vTaskDelay(10 / portTICK_PERIOD_MS); 
  }
}

#endif