#ifndef WEBPAGE_H
#define WEBPAGE_H

const char* html_page = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1, user-scalable=no">
  <style>
    body { background-color: #1a1a1a; color: white; font-family: Arial; text-align: center; overflow: hidden; }
    
    /* Área da Câmera */
    #cam-container { margin: 10px auto; width: 320px; height: 240px; background: #000; border: 2px solid #444; }
    img { width: 100%; height: 100%; object-fit: contain; }

    /* Controles (Escondidos por padrão) */
    .controls { display: none; margin-top: 20px; }
    
    /* Botões Mobile (Grandes e tocáveis) */
    .btn { width: 80px; height: 80px; margin: 5px; border-radius: 50%; border: none; font-size: 24px; font-weight: bold; background: #444; color: #fff; touch-action: manipulation; }
    .btn:active { background: #ff9800; transform: scale(0.95); }
    
    /* Layout Joystick D-PAD */
    .dpad { display: grid; grid-template-columns: repeat(3, auto); justify-content: center; gap: 5px; }
    .btn-up { grid-column: 2; grid-row: 1; }
    .btn-left { grid-column: 1; grid-row: 2; }
    .btn-stop { grid-column: 2; grid-row: 2; background: #c00; }
    .btn-right { grid-column: 3; grid-row: 2; }
    .btn-down { grid-column: 2; grid-row: 3; }

    /* CSS Responsivo: Se for Mobile, mostra botões. Se for PC, avisa do teclado. */
    @media (hover: none) and (pointer: coarse) {
        .controls { display: block; } /* Mostra botões no celular */
        #pc-msg { display: none; }
    }
    @media (min-width: 900px) {
        #pc-msg { display: block; font-size: 1.2rem; color: #aaa; }
    }
  </style>
</head>
<body>

  <h2>ROBÔ ESP32 - STATION MODE</h2>
  
  <div id="cam-container">
    <img src="http://192.168.50.21:81/stream" id="stream">
  </div>

  <div id="pc-msg">USE AS SETAS DO TECLADO</div>

  <div class="controls">
    <div class="dpad">
      <button class="btn btn-up" ontouchstart="cmd('F')" ontouchend="cmd('S')">▲</button>
      <button class="btn btn-left" ontouchstart="cmd('L')" ontouchend="cmd('S')">◄</button>
      <button class="btn btn-stop" ontouchstart="cmd('S')">■</button>
      <button class="btn btn-right" ontouchstart="cmd('R')" ontouchend="cmd('S')">►</button>
      <button class="btn btn-down" ontouchstart="cmd('B')" ontouchend="cmd('S')">▼</button>
    </div>
  </div>

<script>
  // Função OTIMIZADA sem delay artificial
  function cmd(c) {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", "/command?cmd=" + c, true);
    xhr.send();
  }

  // Detectar Teclado (PC)
  document.addEventListener('keydown', function(e) {
    if(e.repeat) return;
    if(e.key === "ArrowUp") cmd('F');
    if(e.key === "ArrowDown") cmd('B');
    if(e.key === "ArrowLeft") cmd('L');
    if(e.key === "ArrowRight") cmd('R');
    if(e.key === " ") cmd('S'); // Espaço para parar
  });

  document.addEventListener('keyup', function(e) {
    cmd('S'); // Soltou a tecla, para
  });

</script>
</body>
</html>
)rawliteral";

#endif