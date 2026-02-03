#ifndef MOTORS_H
#define MOTORS_H

#include "Config.h"

// Variáveis de controle de velocidade
int velocidadeAtualEsq = 0;
int velocidadeAtualDir = 0;
int velocidadeAlvo = 0;
unsigned long ultima_aceleracao = 0;

void setupMotor() { // Nome no plural
  pinMode(motor_esquerdo, OUTPUT);
  pinMode(motor_direito, OUTPUT);
  pinMode(pwm_esquerdo, OUTPUT);
  pinMode(pwm_direito, OUTPUT);
}

// === FUNÇÕES DE MOVIMENTO ===

void Frente() {
  digitalWrite(motor_esquerdo, LOW); // Ajuste LOW/HIGH conforme seu driver
  digitalWrite(motor_direito, LOW);
  velocidadeAlvo = VELOCIDADE_MAX;
}

void Back() {
  digitalWrite(motor_esquerdo, HIGH);
  digitalWrite(motor_direito, HIGH);
  velocidadeAlvo = VELOCIDADE_MAX;
}

void Parar() {
  velocidadeAlvo = 0;

}

void Stop(){
  velocidadeAtualEsq = 0;
  velocidadeAtualDir = 0;
}

void Gira(bool esquerda) {
  velocidadeAlvo = VELOCIDADE_MAX; 

  if (modoGiro360) {
    // === MODO Giro no Eixo ===
    if (esquerda) {
      digitalWrite(motor_esquerdo, HIGH); // Trás
      digitalWrite(motor_direito, LOW);   // Frente
    } else {
      digitalWrite(motor_esquerdo, LOW);  // Frente
      digitalWrite(motor_direito, HIGH);  // Trás
    }
  } 
  else {
    // === MODO CURVA ===
    // força o PWM para pular a aceleração de um dos lados
    if (esquerda) {
      digitalWrite(motor_esquerdo, LOW); 
      digitalWrite(motor_direito, LOW);
      
      analogWrite(pwm_esquerdo, 0);
      analogWrite(pwm_direito, velocidadeAlvo); 

    } else {
      digitalWrite(motor_esquerdo, LOW);
      digitalWrite(motor_direito, LOW);
      
      analogWrite(pwm_esquerdo, velocidadeAlvo);
      analogWrite(pwm_direito, 0);
    }
    return; // Retorna para o loopMotors não sobrescrever imediatamente
  }
}

// === O LOOP DA MOVIMENTAÇÃO ===
// Isso roda no loop do HardwareCore
void loopMotors() {
  if (millis() - ultima_aceleracao > TEMPO_ACELERACAO) {
    ultima_aceleracao = millis();
    
    // Aceleração Suave
    if (velocidadeAtualEsq < velocidadeAlvo) velocidadeAtualEsq += ACELERACAO;
    if (velocidadeAtualEsq > velocidadeAlvo) velocidadeAtualEsq -= ACELERACAO;
    
    // Trava limites
    velocidadeAtualEsq = constrain(velocidadeAtualEsq, 0, VELOCIDADE_MAX);
    velocidadeAtualDir = velocidadeAtualEsq; // Sincroniza lados (para Frente/Tras/360)

    // Aplica aos motores
    analogWrite(pwm_esquerdo, velocidadeAtualEsq);
    analogWrite(pwm_direito, velocidadeAtualDir);
  }
}

#endif