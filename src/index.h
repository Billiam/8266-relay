const char index_html[] PROGMEM = R"=====(
<!doctype html>

<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href='data:image/svg+xml;utf8,<svg viewbox="0 0 1024 1024" xmlns="http://www.w3.org/2000/svg"><path d="M433 45c50 134 24 207-32 265-61 64-156 112-223 206-89 125-104 400 217 472-135-71-164-277-18-406-38 125 32 205 119 176 85-29 141 32 139 102-1 48-20 89-69 112 209-37 293-210 293-342 0-174-155-198-77-344-93 8-125 69-116 169 6 66-63 111-114 81-41-25-40-73-4-109 77-76 107-251-115-382z"/></svg>'>
  <title>Fireplace</title>
  <style>
  body, html {
    height: 100%%;
    padding: 0;
    margin: 0;
  }
  body {
    background-color: #141414;
    font-family:'Gill Sans', 'Gill Sans MT', Calibri, 'Trebuchet MS', sans-serif;
    color: #aaa;
  }
  button {
    background: none;
    border: none;
    cursor: pointer;
  }
  svg {
    width: 100%%;
  }
  .current-temperature {
    font-weight: bold;
    font-size: 30vw;
  }
  .temp-buttons {
    display: flex;
    flex-direction: column;
    width: 60px;
    overflow: hidden;
    justify-content: space-between;
  }
  .temperature {
    display: flex;
    justify-content: space-between;
    align-items: center;
  }
  .target-temperature {
    text-align: center;
    text-transform: uppercase;
    font-size:14px;
  }
  .main {
    display: flex;
    justify-content: space-between;
    height: 100%%;
    width: 100%%;
    box-sizing: border-box;
    align-items: center;
    max-width: 800px;
    margin: 0 auto;
  }
  .fire {
    color: #333;
    height: 50vh;
    transition: color 0.2s;
  }
  .fire:hover {
    color: #444;
  }
  .fire.active {
    color: rgb(145, 24, 24);
  }
  .fire.active:hover {
    color: rgb(153, 31, 31);
  }
  .fire.heating {
    color: rgb(255, 147, 32);
  }
  .fire.heating:hover {
    color: rgb(252, 149, 39);
  }
  .fire svg {
    stroke-width: 4px;
    stroke: transparent;
    transition: stroke 0.2s;
  }
  .fire.active svg {
    stroke: rgb(145, 24, 24);
  }
  .fire.heating svg {
    stroke: rgb(255, 231, 18);
  }
  .temp-buttons button {
    padding: 0;
    color: #333;
    transition: color 0.2s;
  }
  .temp-buttons button:hover {
    color: #444;
  }
  .temp-buttons svg {
    width: 100%%;
  }
  svg {
    max-height: 100%%;
  }
  .loading {
    position: absolute;
    bottom: 10px;
    right: 10px;
    width: 20px;
    height: 20px;
  }
  .loading svg {
    animation: rotation 2s infinite linear;
  }
  .wrapper {
    position: relative;
  }
  .disconnected::before {
    content: "";
    position: absolute;
    top: 0;
    bottom: 0;
    left: 0;
    right: 0;
    background-color: rgba(0, 0, 0, 0.9);
    z-index: 100;
  }
  .reload {
    position: absolute;
    display: none;
    z-index: 150;
    font-size: 30px;
    transform: translateY(-50%%);
    left: 0;
    right: 0;
    text-align: center;
  }
  .disconnected .reload {
    display: block;
  }

  @keyframes rotation {
    from {
      transform: rotate(0deg);
    }
    to {
      transform: rotate(359deg);
    }
  }
  @media only screen and (max-width: 350px) {
    .main {
      flex-direction: column;
      justify-content: center;
      align-items: stretch;
    }
    .current-temperature {
      font-size: 50vw;
    }
  }
  @media only screen and (min-width: 900px) {
    .current-temperature {
      font-size: 270px;
    }
  }
</style>
</head>

<body class="wrapper">
  <section class="main">
    <button class="fire %HEATING_CLASS% %ACTIVE_CLASS%" onclick="toggle()">
      <svg fill="currentColor" viewbox="0 0 1024 1024" xmlns="http://www.w3.org/2000/svg">
        <path d="M433 45c50 134 24 207-32 265-61 64-156 112-223 206-89 125-104 400 217 472-135-71-164-277-18-406-38 125 32 205 119 176 85-29 141 32 139 102-1 48-20 89-69 112 209-37 293-210 293-342 0-174-155-198-77-344-93 8-125 69-116 169 6 66-63 111-114 81-41-25-40-73-4-109 77-76 107-251-115-382z" />
      </svg>
    </button>
    <div class="temperature">
      <span class="current-temperature" data-temperature="%TEMPERATURE%">%TEMPERATURE%??</span>

      <div class="temp-buttons">
        <button class="temp-up" onclick="changeTargetTemp(1)">
          <svg xmlns="http://www.w3.org/2000/svg" preserveAspectRatio="xMidYMid meet" viewBox="0 0 24 24"><path d="M12.354 8.854l5.792 5.792a.5.5 0 0 1-.353.854H6.207a.5.5 0 0 1-.353-.854l5.792-5.792a.5.5 0 0 1 .708 0z" fill="currentColor"/></svg>
        </button>

        <div class="target-temperature" data-target="%TARGET%"></div>

        <button class="temp-down" onclick="changeTargetTemp(-1)">
          <svg xmlns="http://www.w3.org/2000/svg" preserveAspectRatio="xMidYMid meet" viewBox="0 0 24 24"><path d="M11.646 15.146L5.854 9.354a.5.5 0 0 1 .353-.854h11.586a.5.5 0 0 1 .353.854l-5.793 5.792a.5.5 0 0 1-.707 0z" fill="currentColor"/></svg>
        </button>
      </div>
    </div>
    <div class="loading" style="display: none">
      <svg xmlns="http://www.w3.org/2000/svg" preserveAspectRatio="xMidYMid meet" viewBox="0 0 1200 1200"><rect x="0" y="0" width="1200" height="1200" fill="none" stroke="none" /><path d="M600 0C308.74 0 66.009 207.555 11.499 482.812h166.553C229.37 297.756 398.603 161.719 600 161.719c121.069 0 230.474 49.195 309.668 128.613l-192.48 192.48H1200V0l-175.781 175.781C915.653 67.181 765.698 0 600 0zM0 717.188V1200l175.781-175.781C284.346 1132.819 434.302 1200 600 1200c291.26 0 533.991-207.555 588.501-482.812h-166.553C970.631 902.243 801.396 1038.281 600 1038.281c-121.069 0-230.474-49.195-309.668-128.613l192.48-192.48H0z" fill="currentColor"/></svg>
    </div>
    <div class="reload">
      Disconnected, please reload
    </div>
  </section>
  <script>
  var tempElement = document.querySelector('.current-temperature')
  var targetElement = document.querySelector('.target-temperature')
  var flameElement = document.querySelector('.fire')
  var wrapperElement = document.querySelector('.wrapper')

  var currentTemperature = tempElement.getAttribute('data-temperature')
  var loading = document.querySelector('.loading')
  var heating = false
  var on = false
  currentTemperature = currentTemperature ? parseInt(currentTemperature) : null

  var targetTemperature = targetElement.getAttribute('data-target')
  targetTemperature = targetTemperature == '' ? null : parseInt(targetTemperature)
  var pendingTemperature = targetTemperature

  var debounceUpdateTargetTemp = debounce(updateTargetTemp, 200)
  var source
  if (!!window.EventSource) {
    source = new EventSource('/events')
    
    source.addEventListener('open', function(e) {
      console.log("Events Connected");
    }, false);
  
    source.addEventListener('error', function(e) {
      if (e.target.readyState != EventSource.OPEN) {
        console.log("Events Disconnected");
      }
    }, false)

    source.addEventListener('on', function(e) {
      on = e.data === 'true'
      renderOn()
    }, false)

    source.addEventListener('heating', function(e) {
      heating = e.data === 'true'
      renderHeating()
    }, false)

    source.addEventListener('current-temp', function(e) {
      // TODO: pass data via JSON
      currentTemperature = parseInt(e.data)
      renderTemperature()
    }, false)

    source.addEventListener('target-temp', function(e) {
      // TODO: pass data via JSON
      targetTemperature = pendingTemperature = parseInt(e.data)
      renderPendingTemperature()
    }, false)
  }

  function debounce(callback, timeout) {
    var timer
    return function () {
      clearTimeout(timer)
      timer = setTimeout(callback, timeout)
    }
  }

  function renderPendingTemperature() {
    if (pendingTemperature) {
      targetElement.textContent = pendingTemperature + '??'
    } else {
      targetElement.textContent = 'manual'
    }
  }

  function renderOn() {
    flameElement.classList.toggle('active', on)
  }

  function renderHeating() {
    flameElement.classList.toggle('heating', heating)
  }
  
  function renderTemperature() {
    if (currentTemperature) {
      tempElement.textContent = currentTemperature + '??'
    } else {
      tempElement.textContent = ''
    }
  }

  function setLoading() {
    loading.style.display = 'block'
  }

  function clearLoading() {
    loading.style.display = 'none'
  }

  function changeTargetTemp(offset) {
    if (!pendingTemperature) {
      if (currentTemperature) {
        pendingTemperature = currentTemperature
      } else {
        pendingTemperature = offset > 0 ? 55 : 0
      }
    } else {
      pendingTemperature += offset
    }

    if (pendingTemperature < 55) {
      pendingTemperature = 0
    }

    renderPendingTemperature()
    debounceUpdateTargetTemp()
  }

  function toggle() {
    setLoading()
    fetch('/toggle.json', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json'
      }
    }).then(function(result) {
      if (!result.ok) {
        console.error("bad request")
      }
    }).finally(function() {
      clearLoading()
    })    
  }

  function visibilityChangeEvent() {
    var hidden, visibilityChange
    if (typeof document.hidden !== "undefined") { // Opera 12.10 and Firefox 18 and later support
      hidden = "hidden";
      visibilityChange = "visibilitychange";
    } else if (typeof document.msHidden !== "undefined") {
      hidden = "msHidden";
      visibilityChange = "msvisibilitychange";
    } else if (typeof document.webkitHidden !== "undefined") {
      hidden = "webkitHidden";
      visibilityChange = "webkitvisibilitychange";
    }
    return [hidden, visibilityChange]
  }

  function initDisconnect() {
    var props = visibilityChangeEvent()
    var hidden = props[0]
    var event = props[1]
    var idleTimer

    document.addEventListener(event, function () {
      clearTimeout(idleTimer)
      if (document[hidden]) {
        idleTimer = setTimeout(function() {
          console.log("visibility timed out")
          if (source) {
            source.close()
          }
          renderReload()
        }, 60000)
      }
    }, false)
  }

  function renderReload() {
    wrapperElement.classList.add('disconnected')
  }
  
  function updateTargetTemp() {
    setLoading()
    fetch('/target.json', {
      method: 'POST',
      headers: {
        'Content-Type': 'application/json'
      },
      body: JSON.stringify({ temp: pendingTemperature })
    }).then(function(result) {
      if (!result.ok) {
        console.error("bad request")
        pendingTemperature = targetTemperature
        renderTemperature()
      }
    }).finally(function() {
      clearLoading()
    })
  }

  document.addEventListener('DOMContentLoaded', function() {
    initDisconnect()
    renderPendingTemperature()
  })
  </script>
</body>
</html>
)=====";
