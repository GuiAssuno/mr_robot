// Defina os pinos do ESP32 conectados às saídas do sensor (após o conversor de nível)
const int sensorPins[5] = {13, 12, 14, 27, 26}; // Exemplo: GPIOs para os sensores 1 a 5
int sensorValues[5] = {0}; // Array para armazenar os valores lidos

void setup() {
  // Inicializa a comunicação serial
  Serial.begin(115200);

  // Configura os pinos dos sensores como entrada
  for (int i = 0; i < 5; i++) {
    pinMode(sensorPins[i], INPUT);
  }
}

void loop() {
  // Lê todos os sensores
  for (int i = 0; i < 5; i++) {
    sensorValues[i] = digitalRead(sensorPins[i]);
  }

  // Exibe os valores no Monitor Serial
  Serial.print("Sensores: ");
  for (int i = 0; i < 5; i++) {
    Serial.print(sensorValues[i]);
    Serial.print(" ");
  }
  Serial.println(); // Nova linha

  delay(100); // Pequeno atraso para estabilidade
}