#pragma once

// ---------------- HTML ----------------
const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="de">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>AlpenZeit</title>
  <style>
    body {
      margin: 0;
      font-family: Arial, sans-serif;
      background: #f3f4f6;
      color: #111827;
      display: flex;
      align-items: center;
      justify-content: center;
      min-height: 100vh;
    }
    .card {
      background: white;
      padding: 32px;
      border-radius: 20px;
      box-shadow: 0 10px 30px rgba(0,0,0,0.12);
      text-align: center;
      min-width: 340px;
    }
    h1 {
      margin-top: 0;
      margin-bottom: 18px;
      font-size: 28px;
    }
    #timer {
      font-size: 56px;
      font-weight: bold;
      letter-spacing: 1px;
      margin: 18px 0 10px;
      font-variant-numeric: tabular-nums;
    }
    #status {
      font-size: 16px;
      color: #374151;
      margin-top: 10px;
    }
    .hint {
      margin-top: 18px;
      color: #6b7280;
      font-size: 14px;
      line-height: 1.5;
    }
    .dot {
      display: inline-block;
      width: 10px;
      height: 10px;
      border-radius: 50%;
      margin-right: 8px;
      background: #9ca3af;
      vertical-align: middle;
    }
    .running { background: #16a34a; }
    .stopped { background: #dc2626; }
  </style>
</head>
<body>
  <div class="card">
    <h1>Alpenzeit</h1>
    <div id="timer">00:00.000</div>
    <div id="status"><span class="dot stopped"></span>gestoppt</div>
    <div class="hint">
      Taster: kurz = Start/Stop, 3 s halten = Reset<br>
      Seriell: 1 = Start, 0 = Stop, r = Reset
    </div>
  </div>

  <script>
    let ws;
    let running = false;
    let elapsedAtSync = 0;
    let perfSync = 0;
    let lastRendered = -1;

    function formatTime(ms) {
      const minutes = Math.floor(ms / 60000);
      const seconds = Math.floor((ms % 60000) / 1000);
      const millis = Math.floor(ms % 1000);

      const mm = String(minutes).padStart(2, '0');
      const ss = String(seconds).padStart(2, '0');
      const mmm = String(millis).padStart(3, '0');
      return `${mm}:${ss}.${mmm}`;
    }

    function currentElapsed() {
      if (running) {
        return elapsedAtSync + (performance.now() - perfSync);
      }
      return elapsedAtSync;
    }

    function render() {
      const nowMs = currentElapsed();
      const rounded = Math.floor(nowMs);

      if (rounded !== lastRendered) {
        document.getElementById("timer").textContent = formatTime(rounded);
        lastRendered = rounded;
      }

      requestAnimationFrame(render);
    }

    function updateStatus() {
      const status = document.getElementById("status");
      if (running) {
        status.innerHTML = '<span class="dot running"></span>läuft';
      } else {
        status.innerHTML = '<span class="dot stopped"></span>gestoppt';
      }
    }

    function applyState(data) {
      running = !!data.running;
      elapsedAtSync = Number(data.elapsed || 0);
      perfSync = performance.now();
      updateStatus();
    }

    function connectWebSocket() {
      const wsUrl = `ws://${location.hostname}:81/`;
      ws = new WebSocket(wsUrl);

      ws.onmessage = (event) => {
        try {
          const data = JSON.parse(event.data);
          applyState(data);
        } catch (e) {
          console.log("Ungültige WS-Nachricht", e);
        }
      };

      ws.onclose = () => {
        setTimeout(connectWebSocket, 1000);
      };

      ws.onerror = () => {
        ws.close();
      };
    }

    connectWebSocket();
    updateStatus();
    requestAnimationFrame(render);
  </script>
</body>
</html>
)rawliteral";