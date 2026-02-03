#ifndef HARDWARE_CORE_H
#define HARDWARE_CORE_H

#include "Config.h"
#include "Sensors.h"
#include "Motors.h"
#include "GerServer.h"

// Variável local para saber o último comando
char comandoAtual = 'S';


void aplicarMotores() {
  // 1. Processa Fila de Comandos
  char cmd;
  if (xQueueReceive(filaComandos, &cmd, 0) == pdTRUE) {
      // Comandos de Configuração
      if (cmd == 'A') { modoGiro360 = !modoGiro360; return; }
      if (cmd == 'X') { modoSegurancaTotal = !modoSegurancaTotal; return; }
      
      // Se recebeu comando de Joystick ('J'), apenas atualiza o modo
      // As variáveis joyX e joyY já foram atualizadas pelo GerServer (volatile)
      comandoAtual = cmd; 
  }

  // 2. SEGURANÇA (Se detectar obstáculo, corta motor)
  if (obstaculoDetectado) {
      if (modoSegurancaTotal) {
          Stop(); // Zera PWM
          return; 
      } 
      else {
          // Modo Destravado: Para por 1s, depois libera
          if (tempoObstaculoDetectado == 0) tempoObstaculoDetectado = millis();
          if (millis() - tempoObstaculoDetectado < 1000) {
              Stop();
              return;
          }
      }
  } else {
      tempoObstaculoDetectado = 0;
  }

  // 3. EXECUÇÃO DE MOVIMENTO
  if (comandoAtual == 'J') {
      // === AQUI ACONTECE A MÁGICA DO JOYSTICK ===
      // Passa os valores globais joyX/joyY para a função do Motors.h
      moverComJoystick(joyX, joyY);
  } 
  else {
      // === MODO BOTÕES/TECLADO (Com Rampa) ===
      switch(comandoAtual) {
          case 'F': Frente(); break;
          case 'B': Back(); break;
          case 'L': Gira(true); break;
          case 'R': Gira(false); break;
          case 'S': Parar(); break;
      }
      
      // Só aplica a rampa suave se NÃO estiver em modo Joystick
      // e se não estiver numa curva suave manual
      if (!(!modoGiro360 && (comandoAtual == 'L' || comandoAtual == 'R'))) {
          loopMotors();
      }
  }
}



// === TAREFA DO NÚCLEO 0 ===
void taskHardware(void * parameter) {
  
  setupMotor();  
  setupSensores(); 
  
  for(;;) { // Loop Infinito
    
    // 1. Verifica Segurança (atualiza obstaculoDetectado)
    checkSafety(); 
    
    // 2. Controla Motores
    aplicarMotores();

    // 3. Respiro do Watchdog
    vTaskDelay(10 / portTICK_PERIOD_MS); 
  }
}

#endif