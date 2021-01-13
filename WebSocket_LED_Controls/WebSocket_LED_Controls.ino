#include <AsyncEventSource.h>
#include <AsyncJson.h>
#include <AsyncWebSocket.h>
#include <AsyncWebSynchronization.h>
#include <ESPAsyncWebServer.h> //The ESPAsyncWebServer library includes a WebSocket plugin that makes it easy to handle WebSocket connections.
#include <SPIFFSEditor.h>
#include <StringArray.h>
#include <WebAuthentication.h>
#include <WebHandlerImpl.h>
#include <WebResponseImpl.h>
#include <ArduinoJson.h>


#include <AsyncPrinter.h>
#include <async_config.h>
#include <DebugPrintMacros.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncTCPbuffer.h>
#include <SyncClient.h>
#include <tcp_axtls.h>

#include <ESP8266WiFi.h>

/*
 * Abdulsalam Bdeir 
 * 1/5/21
 * This small project was conducted to learn how to create a WebSocket with an ESP8266 with the intention of being used as reference for later 
 * in my project.
 * Project Source: https://randomnerdtutorials.com/esp8266-nodemcu-websocket-server-arduino/ 
 * NOTE: Password has been replaced for security purposes with "removed" 
*/

const char* ssid = "Jharnka";
const char* password = "removed";

bool ledState = 0; //hold the GPIO state
const int ledPin = 2; //refers to the GPIO that is controlled, (onboard LED is GPIO 2)


AsyncWebServer server(80); // Create AsyncWebServer object on port 80
AsyncWebSocket ws("/ws"); //object called ws handles the connections on the /ws path
/*
 * index_html variable contains the HTML, CSS, and JS for the client-server interactions using WebSocket protocal
 * if space is provided later after figuring out how to audio buffer, I could separated HTML, CSS and JavaScript files 
 * and then upload to the ESP8266 filesystem and reference them in the code using SPIFFS.
*/
const char index_html[] PROGMEM = R"rawliteral( 
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  h1 {
    font-size: 1.8rem;
    color: white;
  }
  h2{
    font-size: 1.5rem;
    font-weight: bold;
    color: #143642;
  }
  .topnav {
    overflow: hidden;
    background-color: #143642;
  }
  body {
    margin: 0;
  }
  .content {
    padding: 30px;
    max-width: 600px;
    margin: 0 auto;
  }
  .card {
    background-color: #F8F7F9;
    box-shadow: 2px 2px 12px 1px rgba(140,140,140,0.5);
    padding-top:10px;
    padding-bottom:20px;
  }
  .button {
    padding: 15px 50px;
    font-size: 24px;
    text-align: center;
    outline: none;
    color: #fff;
    background-color: #0f8b8d;
    border: none;
    border-radius: 5px;
    -webkit-touch-callout: none;
    -webkit-user-select: none;
    -khtml-user-select: none;
    -moz-user-select: none;
    -ms-user-select: none;
    user-select: none;
    -webkit-tap-highlight-color: rgba(0,0,0,0);
   }
   .button:active {
     background-color: #0f8b8d;
     box-shadow: 2 2px #CDCDCD;
     transform: translateY(2px);
   }
   .state {
     font-size: 1.5rem;
     color:#8c8c8c;
     font-weight: bold;
   }
  </style>
<title>ESP Web Server</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <div class="topnav">
    <h1>ESP WebSocket Server</h1>
  </div>
  <div class="content">
    <div class="card">
      <h2>Output - GPIO 2</h2>
      <p class="state">state: <span id="state">%STATE%</span></p> <!--%STATE% is a placeholder for the GPIO state-->
      <p><button id="button" class="button">Toggle</button></p>
    </div>
  </div>
<script>
  //JavaScript will initialize a WebSocket connection with the server after the web interface is loaded in browser 
  //and will handle data exchange through WebSockets.
  
  var gateway = `ws://${window.location.hostname}/ws`; //entry point to the WebSocket interface gets the current page address (the web server IP address).
  var websocket;

  /*
   * The onload() function calls the initWebSocket() function to initialize a WebSocket connection with the server
   * 
  */
  window.addEventListener('load', onLoad);
  function initWebSocket() { //initializes a WebSocket connection on the gateway
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage;   // <-- added this line
  }
  function onOpen(event) {
    console.log('Connection opened');
    websocket.send('Connection opened');
  }
  function onClose(event) { //if the WebSocket connection is closed, call the initWebSocket() function again after 2 seconds
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000); //calls a function or evaluates an expression after a specified number of milliseconds.
  }
  function onMessage(event) { //when it receives a new message, the ESP server will send 1 or 0, change html display accordingly 
    var state;
    if (event.data == "0"){
      state = "ON";
    }
    else{
      state = "OFF";
    }
    document.getElementById('state').innerHTML = state;
  }
  function onLoad(event) {
    initWebSocket();
    initButton();
  }
  function initButton() { //The initButton() function to add event listeners to the buttons.
    document.getElementById('button').addEventListener('click', toggle);
  }
  function toggle(){
    websocket.send('toggled button'); 
  }
  /*
   * the ESP8266 should handle what happens when it receives the toggle message –> toggle the current GPIO state.
  */
</script>
</body>
</html>
)rawliteral";

/*
   * server side webSocket handling
  */
void notifyClients() {
  ws.textAll(String(ledState)); //notifies all clients of the current LED state whenever there's a change. 
                                //textAll() is from AsyncWebSocket library and is a method for sending same message to call clients that are connected to the server at the same time
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) { //callback function that will run whenever it receives new data from the clients by the WebSocket protocol.
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    if (strcmp((char*)data, "toggled button") == 0) { //if 'toggled button' message is received from client
      ledState = !ledState; //toggle value of ledState
      notifyClients(); //notify all clients of change and update interface accordingly
    }
  }
}
//Configure the WebSocket server
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len) {
    switch (type) {
      case WS_EVT_CONNECT: //when a client has logged in
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
      case WS_EVT_DISCONNECT: //when a client has logged out
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
      case WS_EVT_DATA: //when a data packet is received from the client
        handleWebSocketMessage(arg, data, len);
        break;
      case WS_EVT_PONG: //in response to a ping request
      case WS_EVT_ERROR: //when an error is received from the client
        break;
  }
}

void initWebSocket() { //initializes the WebSocket protocol
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

/*
 * The processor() function is responsible for searching for placeholders on the HTML text 
 * it will replace the %STATE% placeholder with ON if the ledState is 1. 
 * Otherwise, replace it with OFF before sending the web page to the browser
 */
String processor(const String& var){
  Serial.println(var);
  if(var == "STATE"){
    if (ledState){
      return "ON";
    }
    else{
      return "OFF";
    }
  }
}

void setup(){
  Serial.begin(115200); // Serial port for debugging purposes

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); //when program starts ledpin is set to low
  
  WiFi.begin(ssid, password); //initialize Wi-Fi 
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println(WiFi.localIP()); // print the ESP8266 IP address on the Serial Monitor.

  initWebSocket(); //Initialize WebSocket protocol

  /*
   * Route for root / web page
   * Serve the text saved on the index_html variable when you receive a request on the root / URL  
   * must pass the processor function as an argument to replace the placeholders with the current GPIO state.
   */
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  
  server.begin();   // Start server
}

void loop() {
  /*
   * call the cleanupClients method, explanation from the ESPAsyncWebServer library GitHub page  -> 
   * Browsers sometimes do not correctly close the WebSocket connection, even when the close() function is called in JavaScript. 
   * This will eventually exhaust the web server’s resources and will cause the server to crash. 
   * Periodically calling the cleanupClients() function from the main loop()limits the number of clients by closing the oldest client 
   * when the maximum number of clients has been exceeded. 
   * This can be called every cycle, however, if you wish to use less power, then calling as infrequently as once per second is sufficient.
   */
  ws.cleanupClients();
  digitalWrite(ledPin, ledState);
}
