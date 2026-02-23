#ifndef SENSORES_H
#define SENSORES_H

#include "Config.h"
#include <HCSR04.h> 

HCSR04 sensorEsq(trig, echo_esq);
HCSR04 sensorDir(trig, echo_dir);

// [2] Linhas (0=Esq | 1=Dir)
// [5] Colunas (5 leituras)
float matrix[2][5]; 

int colunaAtual = 0; // Coluna que está usando
unsigned long ultimaLeitura = 0; // Ultima leitura dos sensores

// Configurando os sensores
void setupSensores() {
  // Sensores HCSR04
  pinMode(echo_esq, INPUT); 
  pinMode(echo_dir, INPUT);
  // os Dois triggers dos sensores estão na mesma pinagem
  pinMode(trig, OUTPUT); 

  // LEDs dos sensor
  pinMode(ledSonarEsquerdo, OUTPUT);
  pinMode(ledSonarDireito, OUTPUT);

  // Começa desligado
  digitalWrite(trig, LOW);
  digitalWrite(ledSonarEsquerdo, LOW);
  digitalWrite(ledSonarDireito, LOW);

  // Preenche a matriz
  for(int c=0; c<5; c++) {
      matrix[0][c] = DISTANCIA_SEGURA + 20; // Linha 0 (Esq)
      matrix[1][c] = DISTANCIA_SEGURA + 20; // Linha 1 (Dir)
  }
}

// Função para média da matriz
float calcularMedia(int linha) {
    float soma = 0;
    int validos = 0;
    for(int c = 0; c < 5; c++) {
        float valor = matrix[linha][c];
        if (valor > 0.1 && valor < 400) { // Filtro de erros
            soma += valor;
            validos++;
        }
    }
    if (validos == 0) return 100.0;
    return soma / validos;
}

bool checkSafe() {
  if (millis() - ultimaLeitura > 50) {
    ultimaLeitura = millis();
    
    // Le os sensores
    float leituraEsq = sensorEsq.dist();
    float leituraDir = sensorDir.dist();
    
    // Guarda na matriz coluna atual
    if (leituraEsq > 0) matrix[0][colunaAtual] = leituraEsq;
    if (leituraDir > 0) matrix[1][colunaAtual] = leituraDir;
    
    // Avança para a próxima coluna
    colunaAtual++;
    if (colunaAtual >= 5) colunaAtual = 0; // Volta pro começo
    
    // Calcula media da matriz
    float mediaEsq = calcularMedia(0); // Linha 0
    float mediaDir = calcularMedia(1); // Linha 1
    
    // Verifica zona segura
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