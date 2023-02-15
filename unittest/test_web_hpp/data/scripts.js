// Get the Sidebar
var mySidebar = document.getElementById("mySidebar");
// Get the DIV with overlay effect
var overlayBg = document.getElementById("myOverlay");
// Toggle between showing and hiding the sidebar, and add overlay effect
function w3_open() {
  if (mySidebar.style.display === "block") {
    mySidebar.style.display = "none";
    overlayBg.style.display = "none";
  } else {
    mySidebar.style.display = "block";
    overlayBg.style.display = "block";
  }
}
// Close the sidebar with the close button
function w3_close() {
  mySidebar.style.display = "none";
  overlayBg.style.display = "none";
}
// textarea = document.getElementById('esp32_log');
// ip_server = "ws://" + location.host + "/serial_esp32";
// if (!textarea) {
//   var textarea = document.getElementById('k210_log');
//   var ip_server = "ws://" + location.host + "/serial_k210";
// }
// console.log(ip_server);
// var socket1 = new WebSocket(ip_server);
// socket1.onmessage = function (event) {
//   console.log(event.data); 
//   let text = event.data.substring(event.data.indexOf("[") + 1, event.data.indexOf("]"));
//   let txt_js = "\"" + text + "\"";
//   console.log(text);
//   console.log(txt_js);
//   // let njson = JSON.parse(txt_js);
//   let njson = text.split(",");
//   console.log(njson);
//   if (textarea) {
//     textarea.value += event.data;
//   }
//   console.log(from_b64_to_blob(event.data))

//   var myTable = document.getElementById("dirList");
//   var row = myTable.insertRow();
//   var cell;
//   var perrow = 1; // 2 CELLS PER ROW
//   njson.forEach((value, i) => {
//     cell = row.insertCell();
//     cell.innerHTML = value.substring(event.data.indexOf("'"), event.data.indexOf("'", 1));
//     var next = i + 1;
//     if (next % perrow == 0 && next != njson.length) { row = myTable.insertRow(); }
//   });
// }
// function clear_fun() {
//   textarea.value = "";
//   console.log("log");
// }
// function stop_fun() {

// }

// function dwl_file(file_name) {

//   socket1.send(`dwl-file[${file_name}]`);

// }
// function myMap() {
//   var mapProp = {
//     center: new google.maps.LatLng(19.43576838254597, -70.67392840185293),
//     zoom: 16,
//   };
//   var map = new google.maps.Map(document.getElementById("googleMap"), mapProp);
//   const myLatLng = { lat: 19.43576838254597, lng: -70.67392840185293 };
//   new google.maps.Marker({
//     position: myLatLng,
//     map,
//     title: "Hello World!",
//   });
// }


function saveTextAsFile(textToWrite, fileNameToSaveAs) {
  var textFileAsBlob = new Blob([textToWrite], { type: 'text/plain' });
  var downloadLink = document.createElement("a");
  downloadLink.download = fileNameToSaveAs;
  downloadLink.innerHTML = "Download File";
  if (window.webkitURL != null) {
    // Chrome allows the link to be clicked
    // without actually adding it to the DOM.
    downloadLink.href = window.webkitURL.createObjectURL(textFileAsBlob);
  }
  else {
    // Firefox requires the link to be added to the DOM
    // before it can be clicked.
    downloadLink.href = window.URL.createObjectURL(textFileAsBlob);
    downloadLink.onclick = destroyClickedElement;
    downloadLink.style.display = "none";
    document.body.appendChild(downloadLink);
  }
  downloadLink.click();
}


