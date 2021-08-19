#ifndef WIFI_MANAGER_WEB_PAGE_H
#define WIFI_MANAGER_WEB_PAGE_H

const static char webpage[] PROGMEM = R"=====(
<html>
  <head>
    <script>
var connection = new WebSocket('ws://'+location.hostname+':81/');

connection.onopen = function () { 
    connection.send('Connected ' + new Date());
};

connection.onerror = function (error) { 
    console.log('WebSocket Error ', error);
};

connection.onmessage = function (e) { 
    console.log('Server: ', e.data);
};
  
function ReceiveCredentials() {
  var ssid = document.getElementById('ssidInput').value;
  var password = document.getElementById('passwordInput').value;
  var authentication = document.getElementById('authenticationInput').value;
  var fullCommand = '#{"ssid":"'+ ssid +'", "password":"' +password +'","authentication":"'+ authentication +'"}';
  connection.send(fullCommand);
  location.replace('http://'+location.hostname+'/submit');
}

  </script
  </head>
  <body>
  <label for="ssid">SSID Name:</label>
  <input type="text" id="ssidInput"><br><br>
  
  <label for="password">Password:</label>
  <input type="text" id="passwordInput"><br><br>
  
  <lable for="authentication">Auth Token:</label>
  <input type="text" id="authenticationInput"><br><br>
  
   <button type="button" onclick=ReceiveCredentials();>Submit</button>
</body>
</html>
)=====";

#endif //WIFI_MANAGER_WEB_PAGE_H
