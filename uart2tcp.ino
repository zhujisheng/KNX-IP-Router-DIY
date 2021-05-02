#include <Preferences.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>

#define RXD2 16
#define TXD2 17
#define INITIALIZE_PIN 23
#define CONFIG_PIN 21
#define AP_PIN 22

#define MAX_FLAG 32
#define MAX_SSID 32
#define MAX_PWD 32
#define TCP_LISTEN_PORT 5120
#define MDNS_NAME "NCN5120"
#define DEFAULT_AP_SSID "ncn5120"
#define DEFAULT_AP_PASSWORD "ncn5120tcp"
#define FLAG_STRING "KNZ NCN V1.000"
#define BAUD_RATE 19200

bool is_config, is_ap;
WiFiServer tcp_server(TCP_LISTEN_PORT);
WebServer *p_web_server;

Preferences preferences;

void handleRoot() ;
void handleSetAp();
void handleSetSta();
void handleNotFound();
void handleRestart();
void handleReset();

void setup() {
  char flag[MAX_FLAG]="";
  char ssid[MAX_SSID]="";
  char password[MAX_PWD]="";
  char ap_ssid[MAX_SSID] = DEFAULT_AP_SSID;
  char ap_password[MAX_PWD]="";
  size_t required_size;

  Serial.begin(115200);
  pinMode(INITIALIZE_PIN, INPUT_PULLUP);
  pinMode(CONFIG_PIN, INPUT_PULLUP);
  pinMode(AP_PIN, INPUT_PULLUP);
  is_config = (digitalRead(CONFIG_PIN)==HIGH);
  is_ap = (digitalRead(AP_PIN)==HIGH);

  preferences.begin("iotsharing", false);
  preferences.getString("flag", flag, MAX_FLAG);
  
  Serial.print("flag:");
  Serial.println(flag);

  if(digitalRead(INITIALIZE_PIN)==LOW || strcmp(flag, FLAG_STRING)!=0){
    preferences.putString("flag", FLAG_STRING);
    preferences.putString("ap_passwd", DEFAULT_AP_PASSWORD);
    preferences.putString("wifi_ssid", "");
    preferences.putString("wifi_passwd", "");
    Serial.println("HARDWARE INITIALIZED.");
    delay(1000);
    ESP.restart();
  }

  preferences.getString("ap_passwd", ap_password, MAX_PWD);
  Serial.print("ap_password:");
  Serial.println(ap_password);
  preferences.getString("wifi_ssid", ssid, MAX_SSID);
  Serial.print("ssid:");
  Serial.println(ssid);
  preferences.getString("wifi_passwd", password, MAX_PWD);
  Serial.print("password:");
  Serial.println(password);

  if( is_ap ){
    Serial.println("wifi mode: AP");
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ap_ssid, ap_password);
    Serial.println("");
    Serial.print("AP started: ");
    Serial.println(ap_ssid);
  }
  else{
    Serial.println("wifi mode: STA");
    WiFi.disconnect(true);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    WiFi.setSleep(false);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("Wifi Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  
    if (MDNS.begin(MDNS_NAME)) {
      Serial.print("MDNS responder started: ");
      Serial.println(MDNS_NAME);
    }
  }

  if( is_config ){
    Serial.println("run mode: CONFIG");
    p_web_server = new WebServer(80);
    p_web_server->on("/", handleRoot);
    p_web_server->on("/set_ap", handleSetAp);
    p_web_server->on("/set_sta", handleSetSta);
    p_web_server->on("/restart", handleRestart);
    p_web_server->on("/reset", handleReset);
    p_web_server->onNotFound(handleNotFound);
  
    p_web_server->begin();
    Serial.println("HTTP server started");
  }
  else{
    Serial.println("run mode: PROXY");
    tcp_server.begin();
    Serial.println("TCP Listen Started.");
  }
}
 
void loop() {
  //status() sometimes return error status, so we introduce fails
  static int fails=0;
  if(fails++ > 100){
    ESP.restart();
  }
  
  if( !is_ap ){
    if(WiFi.status()!=WL_CONNECTED) {
      WiFi.reconnect();
      Serial.print(".");
      delay(1000);
      return;
    }
  }

  if( is_config ){
    fails=0;
    p_web_server->handleClient();
  }
  else{
    WiFiClient tcp_client = tcp_server.available();
    if(tcp_client){
      fails=0;
      Serial.println("");
      Serial.print("Connected from: ");
      Serial.println(tcp_client.remoteIP().toString().c_str());
   
      Serial2.begin(BAUD_RATE, SERIAL_8N1, RXD2, TXD2);
      Serial.println("UART Started.");
    }
    else{
      Serial.print("x");
      delay(1000);
      yield();
      return;
    }
  
    while(WiFi.status()==WL_CONNECTED && tcp_client.connected()){
      if(tcp_client.available()){
        Serial2.write(tcp_client.read());
      }
      if(Serial2.available() > 0){
        tcp_client.write(Serial2.read());
      }
      yield();
    }
    Serial2.end();
    tcp_client.stop();
  }
}

void handleRoot() {
  String message;
  message = "<html><head><meta http-equiv='Content-Type' content='text/html; charset=utf-8'><title>KNX IP 网络连接器</title></head><body align='left'><h1>KNX Bridge Board via WIFI<h1><hr><h2>AP Mode</h2><p id='ap_info'></p><table><tr><td>AP SSID:</td><td>ncn5120</td></tr><tr><td>AP password：</td><td><input type='text' id='ap_pwd' placeholder='password'></td></tr></table><br><button type='button' onclick='set_ap()'>submit</button><br><br><hr><h2>STA Mode</h2><p id='sta_info'></p><table><tr><td>SSID</td><td><input type='text' id='wifi_ssid' placeholder='SSID'></td></tr><tr><td>password:</td><td><input type='text' id='wifi_pwd' placeholder='password'></td></tr></table><br><button type='button' onclick='set_sta()'>submit</button><br><br><hr><br><br><p id='restart_info'></p><button type='button' onclick='restart()'>Restart</button> <button type='button' onclick='reset()'>Reset Origin Config</button></h1></h1></body><script>function restart(){var e=document.getElementById('restart_info'),t=new XMLHttpRequest;t.onreadystatechange=function(){if(4===t.readyState)return 200===t.status?void(e.innerHTML=t.responseText):void(e.innerHTML='request failed...')},t.open('POST','restart'),t.setRequestHeader('Content-Type','application/x-www-form-urlencoded'),t.send()}function reset(){var e=document.getElementById('restart_info'),t=new XMLHttpRequest;t.onreadystatechange=function(){if(4===t.readyState)return 200===t.status?void(e.innerHTML=t.responseText):void(e.innerHTML='request failed...')},t.open('POST','reset'),t.setRequestHeader('Content-Type','application/x-www-form-urlencoded'),t.send()}function set_sta(){var e=document.getElementById('wifi_ssid'),t=document.getElementById('wifi_pwd'),n=document.getElementById('sta_info'),a=new XMLHttpRequest;if(e.value){var r='ssid='+e.value+'&pwd='+t.value;a.onreadystatechange=function(){if(4===a.readyState)return 200===a.status?void(n.innerHTML=a.responseText):void(n.innerHTML='request failed...')},a.open('POST','set_sta'),a.setRequestHeader('Content-Type','application/x-www-form-urlencoded'),a.send(r)}else alert('SSID cannot be empty')}function set_ap(){var e=document.getElementById('ap_pwd'),t=document.getElementById('ap_info'),n=new XMLHttpRequest;if(e.value)if(e.value.length<8)alert('AP password too short');else{var a='ap_pwd='+e.value;n.onreadystatechange=function(){if(4===n.readyState)return 200===n.status?void(t.innerHTML=n.responseText):void(t.innerHTML='request failed...')},n.open('POST','set_ap'),n.setRequestHeader('Content-Type','application/x-www-form-urlencoded'),n.send(a)}else alert('AP password cannot be empty!')}</script></html>";
  p_web_server->send(200, "text/html", message);
}

void handleSetAp() {
  String message;
  char ap_pwd[MAX_FLAG];
  if(p_web_server->argName(0) == "ap_pwd"){
    strcpy(ap_pwd,p_web_server->arg(0).c_str());
    preferences.putString("ap_passwd", ap_pwd);
    message = "Success!";
  }
  else{
    message = "input error";
  }
  p_web_server->send(200, "text/plain", message);
}

void handleSetSta() {
  String message;
  char ssid[MAX_SSID], pwd[MAX_PWD];
  if(p_web_server->args()!=2){
    message = "The number of input parameters is error";
    p_web_server->send(200, "text/plain", message);
    return;
  }
  for (uint8_t i = 0; i < p_web_server->args(); i++) {
    if(p_web_server->argName(i) == "ssid"){
      strcpy(ssid, p_web_server->arg(i).c_str());
    }
    else if(p_web_server->argName(i) == "pwd"){
      strcpy(pwd, p_web_server->arg(i).c_str());
    }
    else{
      message = "input error";;
      p_web_server->send(200, "text/plain", message);
      return;
    }
  }

  Serial.printf("ssid: %s\n", ssid);
  Serial.printf("password: %s\n", pwd);
  preferences.putString("wifi_ssid", ssid);
  preferences.putString("wifi_passwd", pwd);
  message = "Set Successfully";

  p_web_server->send(200, "text/plain", message);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += p_web_server->uri();
  message += "\nMethod: ";
  message += (p_web_server->method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += p_web_server->args();
  message += "\n";
  for (uint8_t i = 0; i < p_web_server->args(); i++) {
    message += " " + p_web_server->argName(i) + ": " + p_web_server->arg(i) + "\n";
  }
  p_web_server->send(404, "text/plain", message);
}

void handleRestart() {
  String message;
  message = "restarting...refresh the page after 10 seconds";
  p_web_server->send(200, "text/plain", message);
  delay(1000);
  ESP.restart();
}

void handleReset() {
  String message;
  message = "Reset to origin config...refresh the page after 10 seconds";

  preferences.putString("flag", "something");

  p_web_server->send(200, "text/plain", message);
  delay(1000);
  ESP.restart();
}
