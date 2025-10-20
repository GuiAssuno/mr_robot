// (Adicione estas variáveis no início do seu código)
float Kp = 20;   // Ganho Proporcional - Ajuste empiricamente
float Kd = 15;   // Ganho Derivativo - Ajuste empiricamente
int lastError = 0;
int baseSpeed = 150; // Velocidade base dos motores

void followLine() {
  // Mapeia a posição do sensor (0 a 4000 é uma escala comum)
  int sensorPosition = 0;
  int sum = 0;
  int weightedSum = 0;
  int onLine = 0; // Flag para saber se algum sensor está na linha

  for (int i = 0; i < 5; i++) {
    // Lê o sensor (assumindo que 1 = na linha preta)
    int value = digitalRead(sensorPins[i]);
    if (value == 1) onLine = 1;

    weightedSum += value * (i * 1000); // Ex: Sensor 0->0, 1->1000, 2->2000, 3->3000, 4->4000
    sum += value;
  }

  if (onLine == 0) {
    // Se nenhum sensor vê a linha, use o último erro conhecido
    // ou execute uma ação de recuperação (e.g., girar procurando)
  } else {
    // Calcula a posição média
    sensorPosition = weightedSum / sum; // Ex: Se sensores 1 e 2 ativos: (1000+2000)/2 = 1500
  }

  // Calcula o erro (o centro desejado é 2000, no exemplo da escala acima)
  int error = sensorPosition - 2000;

  // Calcula a correção do motor usando controle PD
  int correction = Kp * error + Kd * (error - lastError);
  lastError = error;

  // Aplica a correção às velocidades dos motores
  int leftMotorSpeed = baseSpeed + correction;
  int rightMotorSpeed = baseSpeed - correction;

  // Garante que as velocidades estão dentro de limites válidos (e.g., 0-255)
  leftMotorSpeed = constrain(leftMotorSpeed, 0, 255);
  rightMotorSpeed = constrain(rightMotorSpeed, 0, 255);

  // Comanda os motores aqui (usando sua biblioteca de ponte H)
  // motorLeft.write(leftMotorSpeed);
  // motorRight.write(rightMotorSpeed);
}