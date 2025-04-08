const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<body>

<div id="demo">
<h1>CEC 326 Robot interface without refresh via AJAX</h1>
  <button type="button" onclick="sendData(1)">LED ON</button>
  <button type="button" onclick="sendData(0)">LED OFF</button><BR>
  <input type="range" min="1" max="180" value="0" class="slider" id="motorPower">
  <label for="motorPower">Motor Power</label><br><br>
  <input type="range" min="-90" max="90" value="0" class="slider" id="motorSteer">
  <label for="motorSteer">Motor Steering</label><br>
</div>

<div>
  Motor Power is : <span id="powerVal">0</span><br>
  Steering is : <span id="steerVal">0</span>
</div>
<script>

var power = document.getElementById("motorPower");
var steer = document.getElementById("motorSteer");
power.oninput = function() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("GET", "P="+power.value+","+steer.value, true);
  xhttp.send();
}
steer.oninput = function() {
  var xhttp = new XMLHttpRequest();
  xhttp.open("GET", "P="+power.value+","+steer.value, true);
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
