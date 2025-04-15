const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body>

<div id="demo">
<h1>CEC 326 Robot interface without refresh via AJAX</h1>
  <button type="button" onclick="sendData(1)">LED ON</button>
  <button type="button" onclick="sendData(0)">LED OFF</button>
  <button type="button" onclick="stop">STOP</button><br>
  <input type="range" min="-100" max="100" value="0" class="slider" id="motorA">
  <label for="motorPower">Motor Power</label><br><br>
  <input type="range" min="-100" max="100" value="0" class="slider" id="motorB">
  <label for="motorSteer">Motor Steering</label><br>
</div>

<div>
  Motor A is : <span id="powerA">0</span><br>
  Motor B is : <span id="powerB">0</span>
</div>
<script>

var powerA = document.getElementById("motorA");
var powerB = document.getElementById("motorB");
powerA.oninput = function() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("GET", "P="+powerA.value+","+powerB.value, true);
  xhttp.send();
}
powerB.oninput = function() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("GET", "P="+powerA.value+","+powerB.value, true);
  xhttp.send();
}

function stop() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("GET", "P=0,0", true);
  powerA.value = 0;
  powerB.value = 0;
  xhttp.send();
}

function sendData(led) {
  var xhttp = new XMLHttpRequest();
  if(led) {
    xhttp.open("GET", "H", true);
  } else {
    xhttp.open("GET", "L", true);
  }
  xhttp.send();
}
</script>
</body>
</html>
)=====";
