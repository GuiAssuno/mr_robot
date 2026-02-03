#ifndef HARDWARE_CORE_H
#define HARDWARE_CORE_H

#include "Config.h"
#include "Sensors.h"
#include "Motors.h"
#include "GerServer.h"

// Variável local para saber o último comando
char comandoAtual = 'S';

void moverComJoystick(int joyX, int joyY) {
//   if (server.hasArg("x") && server.hasArg("y")) {
//     int x_val = server.arg("x").toInt();
//     int y_val = server.arg("y").toInt();
    
//     // Verificar se sistema está bloqueado
//     if (flag) {
//       server.send(200, "text/plain", "BLOCKED");
//       return;
//     }
    
//     // Filtro para evitar atualizações desnecessárias
//     if (abs(x_val - x) > joystick_filtro || abs(y_val - y) > joystick_filtro || (x_val == 0 && y_val == 0)) {
//     /*+====================================================================================================+
//       |      O que acontece dentro desse if é o seguinte.                                                  |
//       |      As variavel "x_val" e "y_val" recebem os valores de x e y que são as coordenadas              |
//       |      das posições onde se encontra o knob (bolinha do centro do joystick). O if serve para         |
//       |      desvalidar as atualizações desnecessariamentes pequenas, pegando a diferença entre a          |
//       |      posição anterior (variaveis "x = x_val","y = y_val") e se for maior que o joystick_filtro     | 
//       |      ele atualiza, assim diminui a precisão a custo de estabilidade                                | 
//       +====================================================================================================+  */
      
//       x = x_val;
//       y = y_val;
      
//       // Se joystick está centralizado, executa stop();
//       if (x == 0 && y == 0) {
//         digitalWrite(motor_esquerdo, last_dir);
//         digitalWrite(motor_direito, last_dir);
            
//         stop();
//         Serial.println("Joystick centralizado - parando");
//         server.send(200, "text/plain", "OK");
//         return;
//       }
      
//       // Mapeamento proporcional para velocidade (0-255)
//       int speed_base = ((abs(y)/100)*250);
//       int speed_curva = (speed_base-((abs(x)/100))*speed_base);
//       Serial.printf("Speed - Curva:%4d      Base:%4d\n", 
//                  speed_curva, speed_base);
      
//       int speed_left = speed_base;
//       int speed_right = speed_base;
      
//       // Ajuste para curva baseado no eixo X
//       if (x > 10) { // Virando para direita
//         speed_left = speed_base;
//         speed_right = speed_curva;
//         return;
//       } else if (x < -10) { // Virando para esquerda
//         speed_left = speed_curva;
//         speed_right = speed_base;
//         return;
//       }
      
//       // Definir direção baseada no eixo Y
//       if ((y > 0)){
//         // Aplicar direção e velocidade
//         digitalWrite(motor_esquerdo, FRENTE);
//         digitalWrite(motor_direito,  FRENTE);
//         analogWrite(pwm_esquerdo, speed_left);
//         analogWrite(pwm_direito, speed_right);  
//          last_dir = 0;
//         Serial.println("frente");
//       }else if ((y < 0)){
//           digitalWrite(motor_esquerdo, BACK);
//           digitalWrite(motor_direito,  BACK);
//           // Aplicar direção e velocidade
//           analogWrite(pwm_esquerdo, speed_left);  
//           analogWrite(pwm_direito, speed_right); 
//            last_dir = 1;
//             Serial.println("costas");
//       } else {
//             digitalWrite(motor_esquerdo, last_dir);
//             digitalWrite(motor_direito, last_dir);
//             // Aplicar direção e velocidade
//             analogWrite(pwm_esquerdo, speed_left);
//             analogWrite(pwm_direito, speed_right);
//             Serial.println("porra nenhuma");
//         }      
      
//       Serial.printf("Joystick - X:%4d Y:%4d   |   Motors L:%4d R:%4d Dir:%s\n", 
//                 x, y, speed_left, speed_right);
//     }
//     server.send(200, "text/plain", "OK");
//   } else {
//     server.send(400, "text/plain", "Missing parameters");
//   }
}


void aplicarMotores() {
  // 1. Processa Fila de Comandos (Vindo do Wi-Fi)
  char cmd;
  // pdTRUE significa que chegou algo novo
  if (xQueueReceive(filaComandos, &cmd, 0) == pdTRUE) {
      
      // === COMANDOS ESPECIAIS (Configuração) ===
      if (cmd == 'A') { // Botão M (Mode)
        modoGiro360 = !modoGiro360; 
        return; // Não muda o movimento, só o modo
      }
      if (cmd == 'X') { // Botão X (Unlock/Stop)
        modoSegurancaTotal = !modoSegurancaTotal;
        return;
      }
      
      // Se for comando de movimento, atualiza
      comandoAtual = cmd; 
  }

  // 2. Lógica de Segurança Inteligente
  if (obstaculoDetectado) {
      if (modoSegurancaTotal) {
          // TRAVA TOTAL
          Stop(); // (Função que zera PWM direto, crie no Motors.h se nao tiver, ou use analogWrite 0)
          analogWrite(pwm_esquerdo, 0);
          analogWrite(pwm_direito, 0);
          return; 
      } 
      else {
          // TRAVA TEMPORÁRIA (1 Segundo)
          if (tempoObstaculoDetectado == 0) tempoObstaculoDetectado = millis();
          
          if (millis() - tempoObstaculoDetectado < 1000) {
              analogWrite(pwm_esquerdo, 0);
              analogWrite(pwm_direito, 0);
              return;
          }
          // Se passou 1s, ele ignora o return e continua abaixo (liberando o motor)
      }
  } else {
      tempoObstaculoDetectado = 0; // Reseta cronômetro
  }

  // 3. Executa Movimento
  if (comandoAtual == 'J') {
      moverComJoystick(joyX, joyY);
  } 
  else {
      switch(comandoAtual) {
          case 'F': Frente(); break;
          case 'B': Back(); break;
          case 'L': Gira(true); break;  // Esquerda
          case 'R': Gira(false); break; // Direita
          case 'S': Parar(); break;
      }
      
      // Importante: Atualiza a rampa se não for curva suave (que já aplicou PWM direto)
      // Se for curva suave, o return lá no Gira() evitou chegar aqui?
      // Não, o Gira() configura. O loopMotors() aplica.
      // Se for modo Gira Suave, o Gira já deu analogWrite, então ok.
      // Se for Frente/Tras, o loopMotors faz a rampa.
      
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