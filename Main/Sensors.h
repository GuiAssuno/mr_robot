#ifndef SENSORS_H
#define SENSORS_H

#include "Config.h"
#include <HCSR04.h> 

HCSR04 sensorEsq(trig, echo_esq);
HCSR04 sensorDir(trig, echo_dir);

// [2] Linhas (0=Esq, 1=Dir)
// [5] Colunas (5 leituras)
float matrizHistorico[2][5]; 

int colunaAtual = 0; // Qual coluna estamos usando
unsigned long ultimaLeitura = 0;

void setupSensores() {
  // Sensor HCSR04
  pinMode(echo_esq, INPUT); 
  pinMode(echo_dir, INPUT);
  pinMode(trig, OUTPUT);

  // LEDs de aviso do sensor
  pinMode(ledSonarEsquerdo, OUTPUT);
  pinMode(ledSonarDireito, OUTPUT);

  // Começa desligado
  digitalWrite(trig, LOW);
  digitalWrite(ledSonarEsquerdo, LOW);
  digitalWrite(ledSonarDireito, LOW);

  // Preenche a matriz com valores apenas para começar 
  for(int c=0; c<5; c++) {
      matrizHistorico[0][c] = DISTANCIA_SEGURA + 20; // Linha 0 (Esq)
      matrizHistorico[1][c] = DISTANCIA_SEGURA + 20; // Linha 1 (Dir)
  }
}

// Função que calcula a média de uma LINHA da matriz
float calcularMediaLinha(int linha) {
    float soma = 0;
    int validos = 0;
    for(int c = 0; c < 5; c++) {
        float valor = matrizHistorico[linha][c];
        if (valor > 0.1 && valor < 400) { // Filtra erros
            soma += valor;
            validos++;
        }
    }
    if (validos == 0) return 100.0;
    return soma / validos;
}

bool checkSafety() {
  if (millis() - ultimaLeitura > 50) {
    ultimaLeitura = millis();
    
    // Le sensores
    float leituraEsq = sensorEsq.dist();
    float leituraDir = sensorDir.dist();
    
    // Guarda na coluna atual da Matriz 
    if (leituraEsq > 0) matrizHistorico[0][colunaAtual] = leituraEsq;
    if (leituraDir > 0) matrizHistorico[1][colunaAtual] = leituraDir;
    
    // Avança a coluna atual para a próxima
    colunaAtual++;
    if (colunaAtual >= 5) colunaAtual = 0; // Volta pro começo
    
    // Calcula média usando a matriz
    float mediaEsq = calcularMediaLinha(0); // Média da Linha 0
    float mediaDir = calcularMediaLinha(1); // Média da Linha 1
    
    // Check zona segura
    bool perigoEsq = (mediaEsq < DISTANCIA_SEGURA);
    bool perigoDir = (mediaDir < DISTANCIA_SEGURA);
    
    digitalWrite(ledSonarEsquerdo, perigoEsq ? HIGH : LOW);
    digitalWrite(ledSonarDireito, perigoDir ? HIGH : LOW);

    if (perigoEsq || perigoDir) {
      obstaculoDetectado = true;
      return true;
    } else {
      obstaculoDetectado = false;
      return false;
    }
  }
  return obstaculoDetectado;
}

#endif