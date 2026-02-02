#ifndef MOTORS_H
#define MOTORS_H

#include "Config.h"

// Estados de movimento
enum Direcao { PARADO, FRENTE, TRAS, ESQUERDA, DIREITA };
Direcao estadoAtual = PARADO;

int velocidadeAtualEsq = 0;
int velocidadeAtualDir = 0;
int velocidadeAlvo = 0;
unsigned long lastRampa = 0;

void setupMotors() {
  pinMode(motor_esquerdo, OUTPUT);
  pinMode(motor_direito, OUTPUT);
  pinMode(pwm_esquerdo, OUTPUT);
  pinMode(pwm_direito, OUTPUT);
}

// Função para definir o desejo (comando), sem travar o código
void setMovimento(Direcao novaDirecao) {
  estadoAtual = novaDirecao;
  
  if (estadoAtual == PARADO) {
    velocidadeAlvo = 0;
  } else {
    velocidadeAlvo = VELOCIDADE_MAX;
    
    // Configura os pinos de direção IMEDIATAMENTE
    // 0 = Frente, 1 = Trás (Ajuste conforme seu driver)
    bool sentido = (estadoAtual == TRAS) ? HIGH : LOW; 
    
    // Lógica simples para curvas (pode refinar depois)
    if (estadoAtual == FRENTE || estadoAtual == TRAS) {
       digitalWrite(MOTOR_ESQ_DIR, sentido);
       digitalWrite(MOTOR_DIR_DIR, sentido);
    } 
    else if (estadoAtual == ESQUERDA) {
       digitalWrite(MOTOR_ESQ_DIR, HIGH); // Um motor vai pra tras
       digitalWrite(MOTOR_DIR_DIR, LOW);  // Outro pra frente
    }
    else if (estadoAtual == DIREITA) {
       digitalWrite(MOTOR_ESQ_DIR, LOW);
       digitalWrite(MOTOR_DIR_DIR, HIGH);
    }
  }
}

// Função chamada dentro do LOOP principal para suavizar a velocidade
void loopMotors() {
  if (millis() - lastRampa > RAMPA_DELAY) {
    lastRampa = millis();
    
    // Rampa de Aceleração/Desaceleração
    if (velocidadeAtualEsq < velocidadeAlvo) velocidadeAtualEsq += RAMPA_PASSO;
    if (velocidadeAtualEsq > velocidadeAlvo) velocidadeAtualEsq -= RAMPA_PASSO;
    
    // Limites
    velocidadeAtualEsq = constrain(velocidadeAtualEsq, 0, 255);
    velocidadeAtualDir = velocidadeAtualEsq; // Por enquanto, iguais

    // Aplica o PWM
    analogWrite(PWM_ESQ, velocidadeAtualEsq);
    analogWrite(PWM_DIR, velocidadeAtualDir);
  }
}

void pararImediatamente() {
  estadoAtual = PARADO;
  velocidadeAlvo = 0;
  velocidadeAtualEsq = 0;
  velocidadeAtualDir = 0;
  analogWrite(PWM_ESQ, 0);
  analogWrite(PWM_DIR, 0);
}

#endif