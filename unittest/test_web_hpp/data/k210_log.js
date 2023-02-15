
var textarea = document.getElementById('k210_log');
var ip_server = "ws://" + location.host + "/serial_k210";
console.log(ip_server);
try {
  var socket1 = new WebSocket(ip_server);
  socket1.onmessage = function (event) {
    console.log(event.data);
    if (textarea) {
      textarea.value += event.data;
      textarea.scrollTop = textarea.scrollHeight;
    }
  }
}
catch (err) {
  console.log(err.message);
}
function rts_fun() {
  textarea.value = "";
  console.log("log");
  socket1.send("cmd[restart_app]");
}
function clear_fun() {
  textarea.value = "";
  console.log("log");
  socket1.send("cmd[clear_log]");
}
function stop_start_fun() {
  let bt = document.getElementById("stop_bt");
  console.log(bt.innerHTML);
  if (bt.innerHTML == "Stop") {
    bt.innerHTML = "Start";
    socket1.send("cmd[stop_serial]");
    bt.className = "w3-button  w3-green"
  } else if (bt.innerHTML == "Start") {
    bt.innerHTML = "Stop";
    socket1.send("cmd[start_serial]");
    bt.className = "w3-button  w3-red"
  }
}
var cmd = document.getElementById("k210_cmd");
cmd.addEventListener("keypress", function(event) {
  // If the user presses the "Enter" key on the keyboard
  if (event.key === "Enter") {
    // Cancel the default action, if needed
    event.preventDefault();
    // Trigger the button element with a click
    document.getElementById("send_bt").click();
  }
});
function send_fun() {

  socket1.send("rpel[" + cmd.value + "]");
  cmd.value = "";
}
function readTextFile(file) {
  var rawFile = new XMLHttpRequest();
  rawFile.open("GET", "/log_k210.txt", false);
  rawFile.onreadystatechange = function () {
    if (rawFile.readyState === 4) {
      if (rawFile.status === 200 || rawFile.status == 0) {
        var allText = rawFile.responseText;
        let bt = document.getElementById("stop_bt");
        if (bt.innerHTML == "Stop") {
          textarea.value = allText;
        }
      }
    }
  }
  rawFile.send(null);
}
setInterval(readTextFile, 100);

