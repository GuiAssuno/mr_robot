#ifndef SENSORS_H
#define SENSORS_H

#include "Config.h"
#include <HCSR04.h> // Certifique-se de ter essa lib instalada

HCSR04 sensorEsq(TRIG_PIN, ECHO_ESQ);
HCSR04 sensorDir(TRIG_PIN, ECHO_DIR);

unsigned long lastSensorRead = 0;
bool obstaculoDetectado = false;

void setupSensors() {
  // Configura os pinos dos sensores como entrada
  pinMode(echo_esq, INPUT);
  pinMode(echo_dir, INPUT);
  pinMode(trig, OUTPUT);
  pinMode(ledSonarEsquerdo, OUTPUT);
  pinMode(ledSonarDireito, OUTPUT);

  // Estados inicial
  digitalWrite(ledSonarEsquerdo, LOW);
  digitalWrite(ledSonarDireito, LOW);
  stop();

  for (int i = 0; i < 5; i++) {
    pinMode(sensorPins[i], INPUT);
  }
}

// Retorna TRUE se precisar parar
bool checkSafety() {
  // Lê apenas a cada 50ms para não sobrecarregar
  if (millis() - lastSensorRead > 50) {
    lastSensorRead = millis();
    
    float d1 = sensorEsq.dist();
    float d2 = sensorDir.dist();
    
    // Filtro simples: ignora leituras 0 (erros) e valida distância segura
    bool perigoEsq = (d1 > 0 && d1 < DISTANCIA_SEGURA);
    bool perigoDir = (d2 > 0 && d2 < DISTANCIA_SEGURA);
    
    if (perigoEsq || perigoDir) {
      obstaculoDetectado = true;
      return true; // PERIGO!
    } else {
      obstaculoDetectado = false;
      return false; // Seguro
    }
  }
  return obstaculoDetectado; // Retorna o último estado conhecido
}

#endif