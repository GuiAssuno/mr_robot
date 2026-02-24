#ifndef HARDWARE_CORE_H
#define HARDWARE_CORE_H

#include "Config.h"
#include "Sensores.h"
#include "Motores.h"
#include "GerServer.h"

// Variável para saber o último comando
char comandoAtual = 'S';


void aplicarMotores() {
  // Processa Fila de Comandos
  char cmd;
  if (xQueueReceive(filaComandos, &cmd, 0) == pdTRUE) {
      // Comandos de Configuração
      if (cmd == 'A') { modoGiro360 = !modoGiro360; return; }
      if (cmd == 'X') { modoSegurancaTotal = !modoSegurancaTotal; return; }
      
      // Se recebeu comando de Joystick ('J'), apenas atualiza o modo
      // As variáveis joyX e joyY já foram atualizadas pelo GerServer (volatile)
      comandoAtual = cmd; 
  }

  // Se detectar obstáculo, corta motor
  if (obstaculoDetectado) {
      if (modoSegurancaTotal) {
          Stop(); 
          return; 
      } 
      else {
          // Modo Destravado
          if (tempoObstaculoDetectado == 0) tempoObstaculoDetectado = millis();
          if (millis() - tempoObstaculoDetectado < 1000) {
              Stop();
              return;
          }
      }
  } else {
      tempoObstaculoDetectado = 0;
  }

  // Movimentação
  if (comandoAtual == 'J') {
      // ========== JOYSTICK =============
      // Passa os valores globais joyX/joyY para a função do Motors.h
      moverComJoystick(joyX, joyY);
  } 
  else {

      // Teclado
      switch(comandoAtual) {
          case 'F': Frente(); break;
          case 'B': Back(); break;
          case 'L': Gira(true); break;
          case 'R': Gira(false); break;
          case 'S': Parar(); break;
      }
      
      // aplica a aceleração suave se NÃO estiver em modo Joystick ou curva suave
      if (!(!modoGiro360 && (comandoAtual == 'L' || comandoAtual == 'R'))) {
          loopMotores();
      }
  }
}

// ========== Tarefa do Nucleo 0 (1 mudar nucleo depois par ver desempenho) ======== 
void taskHardware(void * parameter) {
  
  setupMotores();  
  setupSensores(); 
  
  for(;;) { // Para o loop infinito
    
    // Verifica safe
    checkSafe(); 
    
    // Controla motores
    aplicarMotores();

    // Watchdog
    vTaskDelay(10 / portTICK_PERIOD_MS); 
  }
}

#endif