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
  digitalWrite(motor_esquerdo, LOW);
  digitalWrite(motor_direito, LOW);
  velocidadeAlvo = limitePwmGlobal; 
}

void Back() {
  digitalWrite(motor_esquerdo, HIGH);
  digitalWrite(motor_direito, HIGH);
  velocidadeAlvo = limitePwmGlobal;
}

void Parar() {
  velocidadeAlvo = 0;

}

void Stop(){
  velocidadeAtualEsq = 0;
  velocidadeAtualDir = 0;
  analogWrite(pwm_esquerdo, 0);
  analogWrite(pwm_direito, 0);
}

void Gira(bool esquerda) {
  velocidadeAlvo = limitePwmGlobal; 

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
    velocidadeAtualEsq = constrain(velocidadeAtualEsq, 0, limitePwmGlobal);
    velocidadeAtualDir = velocidadeAtualEsq; // Sincroniza lados (para Frente/Tras/360)

    // Aplica aos motores
    analogWrite(pwm_esquerdo, velocidadeAtualEsq);
    analogWrite(pwm_direito, velocidadeAtualDir);
  }
}

void moverComJoystick(int x, int y) {
  // x e y chegam do site como porcentagem (-100 a +100)
  
  // 1. Calcula o Fator de Conversão baseado no Limite Global
  // Ex: Se limitePwmGlobal for 128 (50%), então entrada 100 vira PWM 128.
  float fator = (float)limitePwmGlobal / 100.0;
  
  // 2. Converte entrada para escala de PWM alvo
  int pwmY = y * fator; // Componente Frente/Tras
  int pwmX = x * fator; // Componente Giro
  
  // 3. Mistura os Eixos (Algoritmo Arcade Drive)
  // Esquerda = Frente + Giro
  // Direita  = Frente - Giro
  int motEsq = pwmY + pwmX;
  int motDir = pwmY - pwmX;
  
  // 4. Clamping (Garante que não estoure o limite máximo permitido)
  motEsq = constrain(motEsq, -limitePwmGlobal, limitePwmGlobal);
  motDir = constrain(motDir, -limitePwmGlobal, limitePwmGlobal);
  
  // 5. Aplica aos Motores (Lidando com a direção e valor positivo)
  
  // MOTOR ESQUERDO
  if (motEsq >= 0) {
    digitalWrite(motor_esquerdo, LOW); // Frente (ajuste se seu motor for invertido)
    analogWrite(pwm_esquerdo, motEsq);
  } else {
    digitalWrite(motor_esquerdo, HIGH); // Trás
    analogWrite(pwm_esquerdo, abs(motEsq)); // PWM tem que ser positivo
  }
  
  // MOTOR DIREITO
  if (motDir >= 0) {
    digitalWrite(motor_direito, LOW); // Frente
    analogWrite(pwm_direito, motDir);
  } else {
    digitalWrite(motor_direito, HIGH); // Trás
    analogWrite(pwm_direito, abs(motDir));
  }

  // 6. Atualiza variáveis de estado (Opcional, para debug)
  velocidadeAtualEsq = abs(motEsq);
  velocidadeAtualDir = abs(motDir);
}

#endif