#ifndef WEBPAGE_H
#define WEBPAGE_H

<<<<<<< Updated upstream
=======
#include <WebServer.h>
#include "Config.h"

extern WebServer server;

>>>>>>> Stashed changes
const char* html_page = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<<<<<<< Updated upstream
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
=======
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no">
    <title>Controle Robô</title>
    <style>
        /* CSS VISUAL MANTIDO */
        * { box-sizing: border-box; -webkit-tap-highlight-color: transparent; }
        body { font-family: Tahoma, sans-serif; background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); min-height: 100vh; display: flex; justify-content: center; align-items: center; margin: 0; padding: 5px; touch-action: manipulation; }
        .container { background: rgba(255, 255, 255, 0.95); border-radius: 12px; padding: 12px; box-shadow: 0 10px 20px rgba(0, 0, 0, 0.2); max-width: 600px; width: 100%; }
        .header { text-align: center; margin-bottom: 5px; }
        .header h1 { color: #333; font-size: 1.0rem; margin: 0; }
        .status { background: #dc3545; color: white; padding: 4px 12px; border-radius: 15px; font-size: 0.8rem; display: inline-block; transition: background 0.3s; }
        .status.connected { background: #28a745; }
        
        /* INPUT DE IP DA CÂMERA (NOVO) */
        .cam-config { text-align: center; margin-bottom: 10px; font-size: 0.8rem; }
        .cam-input { width: 120px; padding: 4px; border: 1px solid #ccc; border-radius: 4px; text-align: center; }
        .cam-btn { padding: 4px 8px; background: #6c757d; color: white; border: none; border-radius: 4px; cursor: pointer; }
        
        /* RESTO DO CSS IGUAL AO SEU */
        .video-placeholder, .video-desktop { background: #000; border-radius: 8px; display: flex; align-items: center; justify-content: center; color: white; aspect-ratio: 4/3; position: relative; }
        .video-placeholder { font-size: 0.8rem; margin: 5px 0; }
        .video-desktop { font-size: 0.9rem; margin: 10px 0; max-width: 320px; margin-left: auto; margin-right: auto; }
        .cameraStream { width: 100%; height: 100%; object-fit: cover; border-radius: 5px; }
        .video-status { position: absolute; background: rgba(0, 0, 0, 0.8); color: white; padding: 4px 8px; border-radius: 4px; font-size: 0.7rem; }
        .mode-display { text-align: center; margin: 10px 0; padding: 8px; background: #f8f9fa; border-radius: 6px; }
        .mode-text { font-size: 0.9rem; font-weight: bold; color: #495057; }
        .controls-section { display: grid; grid-template-columns: 1fr 2fr 1fr; gap: 10px; align-items: center; margin: 15px 0; }
        .controls-grid { display: grid; grid-template-columns: repeat(3, 1fr); grid-template-rows: repeat(2, 1fr); gap: 5px; max-width: 160px; margin: 0 auto; }
        .side-controls { display: flex; flex-direction: column; gap: 4px; align-items: center; }
        .control-btn { width: 45px; height: 45px; border: 2px solid #333; border-radius: 12px; font-size: 1rem; font-weight: bold; cursor: pointer; display: flex; justify-content: center; align-items: center; user-select: none; transition: all 0.15s ease; background: #fff; color: #333; }
        .control-btn:active, .control-btn.active { transform: scale(0.9); box-shadow: inset 0 2px 6px rgba(0, 0, 0, 0.2); }
        .btn-mode { background: #e91e63; color: white; border-color: #c2185b; }
        .btn-stop { background: #4caf50; color: white; border-color: #388e3c; }
        .btn-up, .btn-down, .btn-left, .btn-right { background: #2196f3; color: white; border-color: #1976d2; }
        .btn-up { grid-column: 2; grid-row: 1; }
        .btn-left { grid-column: 1; grid-row: 2; }
        .btn-right { grid-column: 3; grid-row: 2; }
        .btn-down { grid-column: 2; grid-row: 2; }
        .control-label { text-align: center; font-weight: bold; color: #666; margin-top: 3px; font-size: 0.7rem; }
        .joystick-container { position: relative; width: 90px; height: 90px; background: radial-gradient(circle, #f8f9fa 0%, #e9ecef 100%); border: 2px solid #adb5bd; border-radius: 50%; margin: 10px auto; box-shadow: inset 0 2px 8px rgba(0, 0, 0, 0.1); transition: border-color 0.2s ease; touch-action: none; }
        .joystick-container.active { border-color: #e91e63; box-shadow: inset 0 2px 8px rgba(0, 0, 0, 0.1), 0 0 10px rgba(233, 30, 99, 0.2); }
        .joystick-knob { position: absolute; width: 35px; height: 35px; background: radial-gradient(circle, #e91e63 0%, #c2185b 100%); border: 2px solid #fff; border-radius: 50%; cursor: pointer; box-shadow: 0 2px 6px rgba(0, 0, 0, 0.2); transition: all 0.2s ease; top: 50%; left: 50%; transform: translate(-50%, -50%); touch-action: none; }
        .joystick-knob:active { transform: translate(-50%, -50%) scale(1.1); }
        .joystick-label { text-align: center; margin-top: 8px; font-weight: 600; color: #495057; font-size: 0.75rem; }
        .speed-section { background: #f8f9fa; border-radius: 10px; padding: 10px; margin: 10px 0; text-align: center; }
        .speed-display { margin-bottom: 8px; }
        .speed-label { font-weight: bold; color: #495057; margin-right: 8px; font-size: 0.9rem; }
        .speed-value { font-size: 1rem; font-weight: bold; color: #e91e63; }
        .btn-speed { background: #17a2b8; color: white; border-color: #138496; width: 40px; height: 40px; font-size: 1.1rem; }
        .landscape-layout, .portrait-layout { display: none; }
        @media screen and (max-width: 768px) and (orientation: portrait) {
            .controls-section, .speed-section, .video-desktop { display: none; }
            .video-placeholder { max-width: none; }
            .portrait-layout { display: flex; flex-direction: column; gap: 12px; }
            .portrait-buttons { display: flex; justify-content: space-around; gap: 15px; margin: 8px 0; }
            .portrait-controls { display: flex; justify-content: center; }
        }
        @media screen and (max-width: 900px) and (orientation: landscape) {
            .controls-section, .speed-section, .video-desktop { display: none; }
            .landscape-layout { display: grid; grid-template-columns: 1fr 1.5fr 1fr; gap: 8px; align-items: center; }
            .landscape-controls { display: flex; flex-direction: column; gap: 10px; align-items: center; }
            .container { max-width: 100%; height: auto; min-height: 95vh; border-radius: 0; padding: 8px; }
            .joystick-container { width: 80px; height: 80px; margin: 8px auto; }
            .joystick-knob { width: 30px; height: 30px; }
            .video-placeholder { max-height: 460px; margin: 5px 0; }
            .control-btn { width: 38px; height: 38px; font-size: 0.9rem; }
            .control-label { font-size: 0.65rem; }
        }
    </style>
</head>
<body>
    <div class="container">        
        <div class="mode-display">
            <div class="header">
                <h1>Controle <span id="status" class="status">...</span></h1>
            </div>
            <div class="cam-config">
                IP Câmera: <input type="text" id="camIP" class="cam-input" placeholder="192.168.x.x">
                <button class="cam-btn" onclick="updateCamera()">OK</button>
            </div>
            <div class="mode-text"><span id="modeText">Curva</span></div>
        </div>
        
        <div class="video-desktop">
            <img id="cameraStreamDesktop" class="cameraStream" alt="Sem Sinal">
            <div id="videoStatusDesktop" class="video-status">Aguardando IP...</div>
        </div>
        
        <div class="speed-section">
            <div class="speed-display">
                <span class="speed-label">Velocidade:</span>
                <span id="speedValue" class="speed-value">70%</span>
            </div>
        </div>
        
        <div class="controls-section">
            <div class="side-controls">
                <button class="control-btn btn-mode" id="btnMode">M</button>
                <div class="control-label">MODO</div>
                <button class="control-btn btn-speed" id="btnSpeedUpAlt" onclick="changeSpeed(10)">⇧</button>
                <div class="control-label">ACELERAR</div>
            </div>
            <div class="controls-grid">
                <button class="control-btn btn-up" id="btnUp">↑</button>
                <button class="control-btn btn-left" id="btnLeft">←</button>
                <button class="control-btn btn-down" id="btnDown">↓</button>
                <button class="control-btn btn-right" id="btnRight">→</button>
            </div>
            <div class="side-controls">
                <button class="control-btn btn-stop" id="btnStop">X</button>
                <div class="control-label">DESBLOQ</div>
                <button class="control-btn btn-speed" id="btnSpeedDownAlt" onclick="changeSpeed(-10)">⇩</button>
                <div class="control-label">REDUZIR</div>
            </div>
        </div>
        
        <div class="portrait-layout">
            <div class="video-placeholder">
                <img id="cameraStreamPortrait" class="cameraStream" alt="Sem Sinal">
                <div id="videoStatusPortrait" class="video-status">Aguardando IP...</div>
            </div>
            <div class="portrait-buttons">
                <button class="control-btn btn-mode" id="btnModePortrait">M</button>
                <button class="control-btn btn-stop" id="btnStopPortrait">X</button>
            </div>
            <div class="portrait-controls">
                <div class="joystick-container" id="centerJoystick">
                    <div class="joystick-knob" id="centerKnob"></div>
                </div>
            </div>
        </div>
        
        <div class="landscape-layout">
            <div class="landscape-controls">
                <button class="control-btn btn-mode" id="btnModeLandscape">M</button>
                <div class="joystick-container" id="leftJoystick">
                    <div class="joystick-knob" id="leftKnob"></div>
                    <div class="joystick-label">Frente/Ré</div>
                </div>
            </div>
            <div class="video-placeholder">
                <img id="cameraStreamLandscape" class="cameraStream" alt="Sem Sinal">
                <div id="videoStatusLandscape" class="video-status">Aguardando IP...</div>
            </div>
            <div class="landscape-controls">
                <button class="control-btn btn-stop" id="btnStopLandscape">X</button>
                <div class="joystick-container" id="rightJoystick">
                    <div class="joystick-knob" id="rightKnob"></div>
                    <div class="joystick-label">Esq/Dir</div>
                </div>
            </div>
        </div>
>>>>>>> Stashed changes
    </div>
  </div>

<script>
<<<<<<< Updated upstream
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

=======
    // ===== WEBSOCKET =====
    var ws;
    var lastSend = 0;
    var currentSpeedPct = 70;
    var joyState = { center: { x: 0, y: 0 }, left: { x: 0, y: 0 }, right: { x: 0, y: 0 } };

    function initWS() {
        ws = new WebSocket('ws://' + location.hostname + ':81/');
        ws.onopen = function() {
            var s = document.getElementById('status');
            s.innerText = 'On';
            s.classList.add('connected');
        };
        ws.onclose = function() {
            var s = document.getElementById('status');
            s.innerText = 'Off';
            s.classList.remove('connected');
            setTimeout(initWS, 1000);
        };
    }

    function send(cmd) { if (ws && ws.readyState === WebSocket.OPEN) ws.send(cmd); }

    function changeSpeed(delta) {
        currentSpeedPct += delta;
        if (currentSpeedPct > 100) currentSpeedPct = 100;
        if (currentSpeedPct < 10) currentSpeedPct = 10;
        var display = document.getElementById('speedValue');
        if(display) display.innerText = currentSpeedPct + "%";
        if (ws && ws.readyState === WebSocket.OPEN) ws.send("V:" + currentSpeedPct);
    }

    function sendJoy(x, y) {
        var now = Date.now();
        if (now - lastSend > 50) { 
            if (ws && ws.readyState === WebSocket.OPEN) ws.send("J:" + Math.round(x) + ":" + Math.round(y));
            lastSend = now;
        }
    }

    // ===== LÓGICA DE CÂMERA MANUAL =====
    function updateCamera() {
        var ip = document.getElementById('camIP').value;
        if(ip.length > 7) { // Validação mínima
            var url = 'http://' + ip + ':81/stream';
            
            // Salva no navegador para não precisar digitar de novo
            localStorage.setItem('mrRobotCamIP', ip);
            
            // Atualiza as imagens
            var imgs = ['cameraStreamDesktop', 'cameraStreamPortrait', 'cameraStreamLandscape'];
            imgs.forEach(id => {
                var el = document.getElementById(id);
                if(el) {
                    el.src = url;
                    // Esconde o texto de status
                    var statusId = id.replace('cameraStream', 'videoStatus');
                    var st = document.getElementById(statusId);
                    if(st) st.style.display = 'none';
                }
            });
        }
    }

    // ===== JOYSTICK =====
    function setupJoystick(containerId, knobId, type) {
        const container = document.getElementById(containerId);
        const knob = document.getElementById(knobId);
        let touchId = null; 
        const maxDist = 35; 

        function update(clientX, clientY) {
            const rect = container.getBoundingClientRect();
            const centerX = rect.width / 2;
            const centerY = rect.height / 2;
            let dx = clientX - rect.left - centerX;
            let dy = clientY - rect.top - centerY;
            
            if (type === 'left') dx = 0;   
            if (type === 'right') dy = 0;  
            
            const distance = Math.sqrt(dx*dx + dy*dy);
            if (distance > maxDist) {
                dx = (dx / distance) * maxDist;
                dy = (dy / distance) * maxDist;
            }
            
            knob.style.transform = `translate(-50%, -50%) translate(${dx}px, ${dy}px)`;
            
            let valX = (dx / maxDist) * 100;
            let valY = -(dy / maxDist) * 100;
            joyState[type] = { x: valX, y: valY };
            
            if (window.innerHeight < window.innerWidth && window.innerWidth < 900) {
                 sendJoy(joyState.right.x, joyState.left.y);
            } else {
                 sendJoy(valX, valY);
            }
        }

        function reset() {
            touchId = null;
            knob.style.transform = `translate(-50%, -50%)`;
            joyState[type] = { x: 0, y: 0 };
            if (window.innerHeight < window.innerWidth && window.innerWidth < 900) {
                 sendJoy(joyState.right.x, joyState.left.y);
            } else {
                 sendJoy(0, 0);
            }
        }

        container.addEventListener('touchstart', function(e) {
            e.preventDefault();
            const touch = e.changedTouches[e.changedTouches.length - 1];
            touchId = touch.identifier;
            update(touch.clientX, touch.clientY);
            container.classList.add('active');
        }, {passive: false});

        container.addEventListener('touchmove', function(e) {
            e.preventDefault();
            for (let i = 0; i < e.changedTouches.length; i++) {
                if (e.changedTouches[i].identifier === touchId) {
                    update(e.changedTouches[i].clientX, e.changedTouches[i].clientY);
                    break;
                }
            }
        }, {passive: false});

        container.addEventListener('touchend', function(e) {
            for (let i = 0; i < e.changedTouches.length; i++) {
                if (e.changedTouches[i].identifier === touchId) { 
                    reset(); 
                    container.classList.remove('active');
                    break; 
                }
            }
        });
        
        let isMouseDown = false;
        container.addEventListener('mousedown', function(e) { isMouseDown = true; update(e.clientX, e.clientY); });
        window.addEventListener('mousemove', function(e) { if(isMouseDown) update(e.clientX, e.clientY); });
        window.addEventListener('mouseup', function(e) { if(isMouseDown) { isMouseDown = false; reset(); } });
    }

    // ===== INICIALIZAÇÃO =====
    window.onload = function() {
        initWS();
        
        // Carrega IP salvo anteriormente
        var savedIP = localStorage.getItem('mrRobotCamIP');
        if(savedIP) {
            document.getElementById('camIP').value = savedIP;
            updateCamera(); // Tenta conectar automático
        }

        setupJoystick('centerJoystick', 'centerKnob', 'center'); 
        setupJoystick('leftJoystick',   'leftKnob',   'left');   
        setupJoystick('rightJoystick',  'rightKnob',  'right');  

        // Botões
        function btnLink(id, press, release) {
            var el = document.getElementById(id);
            if(!el) return;
            el.addEventListener('touchstart', function(e) { e.preventDefault(); send(press); el.classList.add('active'); });
            el.addEventListener('touchend',   function(e) { e.preventDefault(); if(release) send(release); el.classList.remove('active'); });
            el.addEventListener('mousedown', function(e) { send(press); el.classList.add('active'); });
            el.addEventListener('mouseup',   function(e) { if(release) send(release); el.classList.remove('active'); });
        }
        function clickLink(id, cmd) {
            var el = document.getElementById(id);
            if(el) el.onclick = function() { send(cmd); };
        }

        btnLink('btnUp', 'F', 'S');
        btnLink('btnDown', 'B', 'S');
        btnLink('btnLeft', 'L', 'S');
        btnLink('btnRight', 'R', 'S');
        
        clickLink('btnMode', 'A');
        clickLink('btnStop', 'X');
        clickLink('btnModePortrait', 'A');
        clickLink('btnStopPortrait', 'X');
        clickLink('btnModeLandscape', 'A');
        clickLink('btnStopLandscape', 'X');
        
        document.addEventListener('keydown', function(e) {
            if(e.repeat) return;
            if(e.key === "ArrowUp") send('F');
            if(e.key === "ArrowDown") send('B');
            if(e.key === "ArrowLeft") send('L');
            if(e.key === "ArrowRight") send('R');
            if(e.key === " ") send('S');
        });
        document.addEventListener('keyup', function(e) { send('S'); });
    };
>>>>>>> Stashed changes
</script>
</body>
</html>
)rawliteral";

#endif