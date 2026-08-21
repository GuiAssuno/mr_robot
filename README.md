# Mr. Robot

Um projeto multidisciplinar de robótica envolvendo engenharia mecânica (impressão 3D), eletrônica e programação de sistemas embarcados (RTOS). Este protótipo é um robô de esteira projetado para operar tanto como seguidor de linha autônomo quanto como veículo controlado remotamente, contando com transmissão de vídeo em tempo real e sistemas de segurança anti-colisão.

---

## Resumo do Projeto

O objetivo deste projeto foi desenvolver um robô robusto e inteligente, superando os desafios clássicos de comunicação e processamento em sistemas embarcados. A evolução do projeto marcou a transição de uma arquitetura básica (Arduino + Bluetooth) para um ecossistema avançado baseado em **ESP32 e ESP32-CAM**, permitindo controle via rede sem fio própria, visão em primeira pessoa (FPV) e processamento paralelo para garantir a segurança do veículo mesmo em caso de perda de sinal.

---

## Principais Funcionalidades

* **Modos de Operação Híbridos:** Alternância fluida entre o modo Seguidor de Linha (autônomo) e o Controle Remoto (manual).
* **Interface Web Embarcada:** Controle do robô a partir de qualquer smartphone, tablet ou PC via navegador (HTML/JS), sem necessidade de instalar aplicativos de terceiros.
* **Visão em Tempo Real:** Módulo ESP32-CAM independente transmitindo vídeo ao vivo para a interface do piloto, permitindo manobras fora do campo de visão.
* **Frenagem de Emergência Autônoma:** Sensores ultrassônicos monitoram o ambiente constantemente, parando o robô automaticamente para evitar colisões.
* **Tolerância a Falhas de Conexão:** Graças ao processamento em múltiplos núcleos, se a conexão Wi-Fi cair, o robô mantém seus sistemas de segurança e leitura de linha ativos.

---

## Arquitetura de Software e Comunicação

A comunicação via Bluetooth apresentou limitações de alcance e estabilidade. Para solucionar isso, o sistema foi migrado para o **ESP32**, implementando:

* **Access Point (AP):** O robô cria sua própria rede Wi-Fi, tornando-se independente de roteadores externos.
* **Comunicação via WebSockets:** Garante a troca de comandos entre a interface web e o microcontrolador com baixíssima latência.
* **Processamento Dual-Core (FreeRTOS):** 
  * **Núcleo 0:** Dedicado exclusivamente a manter a infraestrutura de rede e a comunicação Wi-Fi.
  * **Núcleo 1:** Focado no tempo real rígido, processando a leitura da linha, atuando nos motores e lendo os sensores de obstáculo.

---

## Hardware e Eletrônica

O sistema elétrico foi dimensionado para lidar com motores potentes e microcontroladores sensíveis na mesma malha de energia:

* **Alimentação:** Bateria de Lítio de 12V.
* **Tração:** 2 Motores DC de alta performance acionados por um Driver Ponte H de 4 canais.
* **Regulação de Tensão:** Utilização de um módulo Step-Down LM2596 em conjunto com capacitores para fornecer a tensão estável e limpa exigida pelo ESP32 (3.3V/5V).
* **Filtragem de Ruído Eletromagnético:** Os motores geravam interferência nos sensores ultrassônicos (HC-SR04). O problema foi solucionado via hardware com a instalação de resistores/capacitores de cerâmica, atuando como filtros de ruído e garantindo leituras milimétricas confiáveis.

---

## Mecânica e Modelagem 3D

Para garantir que o hardware se encaixasse perfeitamente, o chassi e as peças estruturais não foram comprados prontos. O ciclo de desenvolvimento do produto incluiu:

* **Design e Modelagem:** Feitos do zero na plataforma Tinkercad, otimizando o espaço para os sensores, baterias e placas.
* **Fabricação:** Produção do chassi e suportes utilizando Impressão 3D, permitindo iterações rápidas e mecânica personalizada.

---

## Conclusão e Aprendizados

Este projeto evidencia um amadurecimento técnico notável da equipe. A transição do Arduino para a arquitetura ESP32 com FreeRTOS demonstra a capacidade de identificar gargalos (como a instabilidade do Bluetooth e atrasos no loop principal) e aplicar soluções de engenharia avançadas, como o processamento Dual-Core, redes WebSocket e tratamento de ruídos eletromagnéticos em circuitos de potência.
