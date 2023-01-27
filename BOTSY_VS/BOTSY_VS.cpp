#include <Arduino.h>
#include <EEPROM.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

#define PORT      85
#define DNS_PORT  53

#define WAITING_TICKS 10000
/*
#define RED     0
#define GREEN   1
#define BLUE    2*/

struct Config
{
    char ap_ssid[32] = "BoTSy";
    char ap_pass[32] = "12345678";
    char sta_ssid[32] = "0";
    char sta_pass[32] = "";
    //char sta_ssid[32] = "Oxygen";
    //char sta_pass[32] = "*4@B7|RPj4nO";
};

Config config;

DNSServer dnsServer;
HTTPClient http;
ESP8266WebServer server(PORT);

IPAddress APIP(192, 168, 0, 1);

String HTML_Page_Index();
String HTML_JS_Index(String SSIDs);
String HTML_Style_Index();
String HTML_Style_Errors();
String HTML_Error(int num_of_error);
void Connecting_to_WiFi();
void Start_AP_STA();
void Start_AP(); //hotspot
void Start_STA(); //client
void handle_Connect();
void headroot();
void handle_NotFound();
/*
bool red = false;
bool green = false;
bool blue = false;

bool flicker_red = false;
bool flicker_green = false;
bool flicker_blue = false;*/

//=======================================================================
//                     SETUP
//=======================================================================
void setup()
{
    Serial.println("begin setup");
    //flicker_red = true;

    Serial.begin(115200);
    /*
      pinMode(RED, OUTPUT);
      pinMode(GREEN, OUTPUT);
      pinMode(BLUE, OUTPUT);
      */
    EEPROM.begin(4096);
    EEPROM.get(0, config);
    //EEPROM.put(0, config);

    server.on("/", headroot);
    server.onNotFound(handle_NotFound);

    Serial.println("");

    /*WiFi.mode(WIFI_AP_STA);
    //WiFi.begin(config.sta_ssid, config.sta_pass);
    //for(int i =0; i<10;i++)
    //{
    //  delay(500);
    //  if(WiFi.status() == WL_CONNECTED) break;
    //}
    //if(WiFi.status() == WL_CONNECTED) Connecting_to_WiFi();

    WiFi.begin();
    String str = "\nAP SSID: ";
    str += config.ap_ssid;
    str += "\nAP PASS: ";
    str += config.ap_pass;
    str += "\nSTA SSID: ";
    str += config.sta_ssid;
    str += "\nSTA PASS: ";
    str += config.sta_pass;

    Serial.println(str);
    Serial.print("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    };
    Serial.print("Connected ");
    Serial.println(WiFi.localIP());

    server.on("/", headroot);
    server.begin();*/

    //flicker_red = false;
    //Start_AP();
    Connecting_to_WiFi();
    Serial.println("end setup");

}
//=======================================================================
//                        LOOP
//=======================================================================
void loop()
{
    //Serial.println("begin loop");
  /*
    if (red == true){
      digitalWrite(RED, HIGH);
    }
    if (green == true){
      digitalWrite(RED, HIGH);
    }
    if (blue == true){
      digitalWrite(RED, HIGH);
    }
    if (flicker_red == true){
      digitalWrite(RED, HIGH);
      delay(50);
      digitalWrite(RED, HIGH);
    }
    if (flicker_green == true){
      digitalWrite(GREEN, HIGH);
      delay(50);
      digitalWrite(GREEN, HIGH);
    }
    if (flicker_blue == true){
      digitalWrite(BLUE, HIGH);
      delay(50);
      digitalWrite(BLUE, HIGH);
    }*/
    server.handleClient(); //обработка текущих входящих HTTP-запросов 
    dnsServer.processNextRequest();
    delay(1);
    //Serial.println("end loop");

}

void Start_AP() {
    Serial.println("Trying connecting AP mode");

    //flicker_blue = true;

    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP(config.ap_ssid, config.ap_pass);

    //dnsServer.start(DNS_PORT, "*", APIP);
    /*
      String str = "\nAP SSID: ";
      str += config.ap_ssid;
      str += "\nAP PASS: ";
      str += config.ap_pass;
      str += "\nSTA SSID: ";
      str += config.sta_ssid;
      str += "\nSTA PASS: ";
      str += config.sta_pass;
      Serial.println(str);

      Serial.print("Connecting AP");
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      };
      Serial.print("Connected ");
      Serial.println(WiFi.softAPIP());
      Serial.println(WiFi.localIP());

      dnsServer.start(DNS_PORT, "*", APIP);
      server.begin();
    */
    //flicker_blue = false;
    //blue = true;
}

void Start_STA() {
    Serial.println("Trying connecting STA mode");

    //flicker_green = true;

    WiFi.mode(WIFI_STA);
    WiFi.enableAP(false);
    WiFi.enableSTA(true);
    WiFi.begin(config.sta_ssid, config.sta_pass);
    /*
      String str = "\nAP SSID: ";
      str += config.ap_ssid;
      str += "\nAP PASS: ";
      str += config.ap_pass;
      str += "\nSTA SSID: ";
      str += config.sta_ssid;
      str += "\nSTA PASS: ";
      str += config.sta_pass;
      Serial.println(str);

      Serial.print("Connecting STA ");
      while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
      };
      Serial.print("Connected ");
      Serial.println(WiFi.localIP());

      server.begin();
    */
    //flicker_green = false;
    //green = true;
}

void Start_AP_STA() {
    WiFi.mode(WIFI_AP_STA);
    WiFi.enableAP(true);
    WiFi.enableSTA(true);
    WiFi.begin(config.sta_ssid, config.sta_pass);

    String str = "\nAP SSID: ";
    str += config.ap_ssid;
    str += "\nAP PASS: ";
    str += config.ap_pass;
    str += "\nSTA SSID: ";
    str += config.sta_ssid;
    str += "\nSTA PASS: ";
    str += config.sta_pass;
    Serial.println(str);

    Serial.print("Connecting STA ");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    };
    Serial.print("Connected ");
    Serial.println(WiFi.localIP());

    server.on("/", headroot);
    server.begin();
}

void handle_Connect() {
    //server.send(200, "text/html", HTML_Page_Index());
}

void Connecting_to_WiFi() {
    bool connected = false;
    int tick = 0;
    if (String(config.sta_ssid) == nullptr || String(config.sta_ssid).equals(""))
    {
        Serial.println("Нет данных о подключеной сети в системе");
    }
    else {
        Start_STA();
        while (true)
        {
            Serial.print(".");
            delay(100);
            //int n = WiFi.scanNetworks();
            if (WiFi.status() == WL_CONNECTED)
            {
                connected = true;
                break;
            }
            if (tick >= WAITING_TICKS)
            {
                connected = false;
                break;
            }
            if (WiFi.status() == WL_IDLE_STATUS)
            {
                //когда WiFi-сеть переключается с одного статуса на другой.
                //Serial.print("WiFi-сеть переключается с одного статуса на другой");
            }
            else if (WiFi.status() == WL_NO_SSID_AVAIL)
            {
                //если заданный SSID находится вне зоны доступа.
                //Serial.print("Заданный SSID находится вне зоны доступа");
            }
            else if (WiFi.status() == WL_CONNECT_FAILED)
            {
                //если неправильный пароль.
                //Serial.print("Неправильный пароль");
            }
            else if (WiFi.status() == WL_DISCONNECTED)
            {
                //если модуль не находится в режиме станции.
                //Serial.print("Модуль не находится в режиме станции");
            }
            else
            {
                //просто ожиданние
            }
            tick += 100;
        };
    }
    if (connected)
    {
        //успешно подключено, отправить страницу с доанными о WIFI и локальном ip + кнопку отключения
        EEPROM.put(0, config);
        EEPROM.commit();
        Serial.println("Connect success");
    }
    else {
        //неудалось подключится, отправить странуцу с ошибкой
        Serial.println("Connect failed");
        Start_AP();
    }
    Serial.print("Connected ");
    Serial.println(WiFi.softAPIP());
    Serial.println(WiFi.localIP());
    String str = "\nAP SSID: ";
    str += config.ap_ssid;
    str += "\nAP PASS: ";
    str += config.ap_pass;
    str += "\nSTA SSID: ";
    str += config.sta_ssid;
    str += "\nSTA PASS: ";
    str += config.sta_pass;
    Serial.println(str);

    dnsServer.start(DNS_PORT, "*", APIP);
    server.begin();
}

void headroot() {
    server.send(200, "text/html", HTML_Page_Index());
}

void handle_NotFound() {
    server.send(404, "text/html", HTML_Error(404));
}

struct WiFi_Sort {
    String ssid;
    int strength;
    uint8_t secure;
    String MAC;
};

String HTML_Page_Index() {
    String SSIDs = "";
    String page = "";
    page += "<!DOCTYPE html>";
    page += "<html lang=\"en\">";
    page += "<head>";
    page += "    <meta charset=\"UTF-8\">";
    page += "    <title>BoTSy</title>";
    page += "    <link rel=\"icon\" href=\"https://img.icons8.com/cotton/256/wifi--v2.png 2x\">";
    page += HTML_Style_Index();
    page += "    </head>";
    page += "<body>";
    page += "<main>";
    page += "    <div class=\"success\">";
    page += "    </div>";
    page += "    <h1 class=\"visible\">Body Tracking System</h1>";
    page += "    <h1 class=\"hidden\">BoTSy</h1>";
    page += "    <div id=\"wifi_list\">";
    page += "        <h2>Список Wi-Fi точек</h2>";
    page += "        <ul id=\"wifi_list_list\">";
    int n = WiFi.scanNetworks();
    WiFi_Sort wifi_sort[n];
    if (n == 0) {
        page += "<li>";
        page += " <div class=\"align-items: center;\"><div class=\"row\"><h3>Не найдено ни одной сети</h3></div></div>";
        page += "</li>";
    }
    else {
        for (int i = 0; i < n; i++) {
            wifi_sort[i].ssid = WiFi.SSID(i);
            wifi_sort[i].strength = WiFi.RSSI(i);
            wifi_sort[i].secure = WiFi.encryptionType(i);
            wifi_sort[i].MAC = WiFi.BSSIDstr(i);
        }
        bool changed;
        do {
            changed = false;
            for (int i = 0; i < (n - 1); i++) {
                if (wifi_sort[i].strength < wifi_sort[i + 1].strength) {
                    changed = true;
                    WiFi_Sort temp = wifi_sort[i];
                    wifi_sort[i] = wifi_sort[i + 1];
                    wifi_sort[i + 1] = temp;
                };
            };;
        } while (changed);
        for (int i = 0; i < n; i++) {
            SSIDs += "\"";
            SSIDs += WiFi.SSID(i);
            SSIDs += "\",";
            page += "<li>";
            page += "<div class=\"row\"><h3 class=\"SSID\">Имя сети</h3><h4>"; page += wifi_sort[i].ssid; page += "</h4></div>";
            page += "<div class=\"row\"><h3 class=\"RSSI\">Сила сигнала</h3><h4>"; page += wifi_sort[i].strength; page += "</h4></div>";
            page += "<div class=\"row\"><h3 class=\"encryptionType\">Защита</h3><h4>";
            if (wifi_sort[i].secure == AUTH_OPEN) page += "Открытая сеть";
            else if (wifi_sort[i].secure == ENC_TYPE_WEP) page += "WEP";
            else if (wifi_sort[i].secure == ENC_TYPE_TKIP) page += "WPA / PSK";
            else if (wifi_sort[i].secure == ENC_TYPE_CCMP) page += "WPA2 / PSK";
            else if (wifi_sort[i].secure == ENC_TYPE_NONE) page += "Открытая сеть";
            else if (wifi_sort[i].secure == ENC_TYPE_AUTO) page += "WPA / WPA2 / PSK";
            else page += "Неизвестно";
            page += "</h4></div>";
            page += "<div class=\"row\"><h3 class=\"MAC\">MAC-Адрес</h3><h4>"; page += wifi_sort[i].MAC; page += "</h4></div>";
            page += "</li>";
        }
    }
    page += "        </ul>";
    page += "        <div class=\"field\">";
    page += "            <div class=\"text-field text-field_floating-3\">";
    page += "                <input class=\"text-field__input\" type=\"SSID\" id=\"SSID\" name=\"SSID\" placeholder=\"SSID\">";
    page += "                <label class=\"text-field__label\" for=\"SSID\">Имя сети</label>";
    page += "            </div>";
    page += "        </div>";
    page += "        <div class=\"field\">";
    page += "            <div class=\"text-field text-field_floating-3\">";
    page += "                <input class=\"text-field__input\" type=\"PASS\" id=\"PASS\" name=\"PASS\" placeholder=\"PASS\">";
    page += "                <label class=\"text-field__label\" for=\"PASS\">Пароль</label>";
    page += "            </div>";
    page += "        </div>";
    page += "        <div class=\"center\">";
    page += "            <button id=\"Connect_Button\">Подключиться</button>";
    page += "        </div>";
    page += "        <div id=\"loader\" class=\"center\"></div>";
    page += "    </div>";
    page += "    </main>";
    page += HTML_JS_Index(SSIDs);
    page += "</body>";
    page += "</html>";
    return page;
}

String HTML_JS_Index(String SSIDs) {
    String js = "";
    js += "<script>\n";
    js += " function getRandom(min, max) {";
    js += "   return Math.floor(Math.random() * (max - min)) + min;";
    js += " }\n";
    js += " let loading_links = [";
    js += "   \"https://media.tenor.com/In8c2digq1MAAAAC/claudio-claudin.gif\",";
    js += "   \"https://media.tenor.com/dmw5C4_LUHAAAAAC/loading-girls.gif\",";
    js += "   \"https://media.tenor.com/zkEybmcArhoAAAAS/discord-loading.gif\",";
    js += "   \"https://media.tenor.com/6Pjav1LgBIAAAAAC/tenkahyakken-chatannakiri.gif\",";
    js += "   \"https://media.tenor.com/d2fZlKUq6-UAAAAC/discord.gif\",";
    js += "   \"https://media.tenor.com/tDb298oy_nEAAAAC/erodiyn-loading.gif\",";
    js += "   \"https://media.tenor.com/RVvnVPK-6dcAAAAC/reload-cat.gif\",";
    js += "   \"https://media.tenor.com/Ao1ftp1W0WAAAAAS/discord-notification.gif\",";
    js += "   \"https://media.tenor.com/VW6fRXVCok4AAAAC/inugami-korone.gif\",";
    js += "   \"https://media.tenor.com/U6FZAy_RGSIAAAAd/gaminglight-imperialrp.gif\",";
    js += " ]\n";
    js += " let APIP = http://"; js += WiFi.softAPIP(); js += "/\n";
    js += " let STAIP = http://"; js += WiFi.localIP(); js += "/\n";
    js += " let SSIDs = ["; js += SSIDs; js += "]\n";
    js += " let http = new XMLHttpRequest()\n";
    js += " let params = APIP + '/SSID=' + localStorage.getItem('SSID') + '&PASS=' + localStorage.getItem('PASS')\n";
    js += " http.open('POST', APIP, true)\n";
    js += " http.setRequestHeader('Content-type', 'application/x-www-form-urlencoded')";
    js += " http.onreadystatechange = function() {\n";
    js += "     if(http.readyState == 4 && http.status == 200) {\n";
    js += "         alert(http.responseText)}}\n";
    js += " http.send(params);\n";
    js += "   document.getElementById(\"Connect_Button\").addEventListener(\"click\", function (e) {\n";
    js += "   if(document.getElementById(\"SSID\").value == \"\") {\n";
    js += "     alert(\"Заполните поле \"Имя сети\" или выберите из списка доступных сетей\");\n";
    js += "     return\n";
    js += "   }\n";
    js += "   if(SSIDs.indexOf(document.getElementById(\"SSID\").value) == -1) {\n";
    js += "       alert(\"Такой сети не существует\")\n";
    js += "       return\n";
    js += "   }\n";
    js += "   \\\\document.location = '/SSID=' + localStorage.getItem('SSID') + 'PASS=' + localStorage.getItem('PASS')\n";
    js += "   document.getElementById(\"loader\").innerHTML = `<img style=\"margin-bottom: 5px; border-radius: 5px;\" src=\"` + loading_links[getRandom(0, 10)] + `\" width=\"200\">`\n";
    js += "   })\n";
    js += "   for (let item of document.querySelectorAll('ul > li')) {item.addEventListener('click', function (e){\n";
    js += "     document.getElementById('SSID').value = item.querySelectorAll('div')[0].querySelectorAll('h4')[0].innerText\n";
    js += "     if(item.querySelectorAll('div')[2].querySelectorAll('h4')[0].innerText == \"Открытая сеть\"){\n";
    js += "         document.getElementsByClassName('field')[1].style = `display: none;`\n";
    js += "     } else {\n";
    js += "         document.getElementsByClassName('field')[1].style = `display: block;`\n";
    js += "     }\n";
    js += "   },{})}\n";
    js += "</script>\n";
    return js;
}

String HTML_Style_Index() {
    String style = "<style>";
    style += "*, *::before, *::after {box-sizing: border-box;}";
    style += "body {display: flex;justify-content: space-evenly;align-items: center;flex-wrap: nowrap;}";
    style += "main {background-color: rgb(253, 245, 217, 0.53);max-width: 750px;margin: 20px;min-width: 80%; padding: 10px;border-radius: 5px;}";
    style += "h1, h2, h3, h4, h5, h6, .text-field__input{font-family: \"Comic Sans MS\", \"Comic Sans\", cursive;color: #505050;}";
    style += "h1, h2 {text-align: center;}";
    style += "h3, h4 {margin: 5px 5px 0 5px;}";
    style += "h3 {color: rgba(251 134 204 / 95%);}";
    style += "h4 {color: #505050}";
    style += ".row {display: flex;}";
    style += "li {list-style-type: none;border-radius: 5px;background-color: lightcyan;margin: 10px;}";
    style += "li:hover {background-color: #bde4ff;}";
    style += "ul {margin-left: 0;padding-left: 0;}";
    style += "h3 {margin-left: 5px;}";
    style += "#wifi_list {border-radius: 5px;border: solid 1px rgba(128, 128, 128, 0.2);}";
    style += ".row {flex-direction: row;}";
    style += ".success {display: none;}";
    style += ".visible{display: block;}";
    style += ".hidden {display: none;}";
    style += "@media (max-width: 450px) {";
    style += ".visible{display: none;}";
    style += ".hidden {display: block;}";
    style += ".row {flex-direction: column;align-items: center;}}";
    style += ".field {text-align: -webkit-center;border-radius: 5px;}";
    style += ".text-field {margin: 10px;}";
    style += ".text-field__label {font-family: \"Comic Sans MS\", \"Comic Sans\", cursive;display: block;margin-bottom: 0.25rem;}";
    style += ".text-field__input {display: block;width: 100%; height: calc(2.25rem + 2px);padding: 0.375rem 0.75rem;font-size: 1rem;font-weight: 400;line-height: 1.5;color: #212529;background-color: #fff;background-clip: padding-box;border: 1px solid #bdbdbd;border-radius: 0.25rem;transition: border-color 0.15s ease-in-out, box-shadow 0.15s ease-in-out;-webkit-appearance: none;-moz-appearance: none;appearance: none;}";
    style += ".text-field__input[type=\"search\"]::-webkit-search-decoration,.text-field__input[type=\"search\"]::-webkit-search-cancel-button,.text-field__input[type=\"search\"]::-webkit-search-results-button,.text-field__input[type=\"search\"]::-webkit-search-results-decoration {-webkit-appearance: none;}";
    style += ".text-field__input::placeholder {color: #212529;opacity: 0.4;}";
    style += ".text-field__input:focus {color: #212529;background-color: #fff;border-color: #bdbdbd;outline: 0;box-shadow: 0 0 0 0.2rem rgba(158, 158, 158, 0.25);}";
    style += ".text-field__input:disabled,.text-field__input[readonly] {background-color: #f5f5f5;opacity: 1;}";
    style += ".text-field_floating-3 {position: relative;}";
    style += ".text-field_floating-3 .text-field__input {border: none;border-bottom: 1px solid #bdbdbd;border-radius: 0.25rem 0.25rem 0 0;height: calc(3.5rem + 2px);line-height: 1.25;padding: 1rem 0.75rem;background-color: #f5f5f5;}";
    style += ".text-field_floating-3 .text-field__input:focus {outline: 0;border-bottom: 1px solid #03a9f4;box-shadow: none;background-color: #eee;}";
    style += ".text-field_floating-3 .text-field__label {position: absolute;top: 0;left: 0;height: 100%; padding: 1rem .75rem;pointer-events: none;border: 1px solid transparent;transform-origin: 0 0;transition: color .15s ease-in-out, transform .15s ease-in-out;}";
    style += ".text-field_floating-3 .text-field__input::-moz-placeholder {color: transparent;}";
    style += ".text-field_floating-3 .text-field__input::placeholder {color: transparent;}";
    style += ".text-field_floating-3 .text-field__input:focus,";
    style += ".text-field_floating-3 .text-field__input:not(:placeholder-shown) {padding-top: 1.625rem;padding-bottom: .625rem;border-bottom: 1px solid #03a9f4;}";
    style += ".text-field_floating-3 .text-field__input:focus~.text-field__label,.text-field_floating-3 .text-field__input:not(:placeholder-shown)~.text-field__label {transform: scale(.85) translateY(-.75rem) translateX(.15rem);color: #03a9f4;}";
    style += ".loader {align-items: center;}";
    style += "button {color:#505050;background-color: lightcyan;border-radius: 5px;border: solid 1px rgba(128, 128, 128, 0.2);margin: 0 10px 10px 10px;font-size: 24px;padding: 15px 50px 15px 50px;}";
    style += ".center {text-align: center;align-content: center;align-items: center;align-self: center;justify-content: center;justify-items: center;justify-self: center;}";
    style += "</style>";
    return style;
}

String HTML_Style_Errors() {
    String style = "<style>";
    style += "html, body{height:100%; overflow:hidden;}";
    style += ".error-page{display:flex;align-items:center;justify-content:center;text-align:center;height:100%; font-family:Arial,\"Helvetica Neue\",Helvetica,sans-serif;}";
    style += ".error-page h1{font-size:20vh;font-weight:bold;position:relative;margin:-8vh00;padding:0;}";
    style += ".error-page h1:after{content:attr(data-h1);position:absolute;top:0;left:0;right:0;color:transparent;background:-webkit-repeating-linear-gradient(-45deg,#71b7e6,#69a6ce,#b98acc,#ee8176,#b98acc,#69a6ce,#9b59b6);-webkit-background-clip:text;-webkit-text-fill-color:transparent;background-size:400%; text-shadow:1px 1px 2px rgba(255,255,255,0.25);animation:animateTextBackground 10s ease-in-out infinite;}";
    style += ".error-page h1+p{color:#d6d6d6;font-size:8vh;font-weight:bold;text-align:center;margin:auto;line-height:10vh;/*max-width:800px;*/position:relative;}";
    style += ".error-page h1+p:after{content:attr(data-p);position:absolute;top:0;left:0;right:0;color:transparent;text-shadow:1px 1px 2px rgba(255,255,255,0.5);-webkit-background-clip:text;-moz-background-clip:text;background-clip:text;}";
    style += "#particles-js{position:fixed;top:0;right:0;bottom:0;left:0;}";
    style += "@keyframes animateTextBackground{0%{background-position:00;}";
    style += "25%{background-position:100% 0;}";
    style += "50%{background-position:100% 100%;}";
    style += "75%{background-position:0 100%;}";
    style += "100%{background-position:00;}}";
    style += "@media(max-width:767px){.error-page h1{font-size:18vw;}";
    style += ".error-page h1+p{font-size:8vw;line-height:10vw;max-width:70vw;}}";
    style += "a.back{position:fixed;right:40px;bottom:40px;background:-webkit-repeating-linear-gradient(-45deg,#71b7e6,#69a6ce,#b98acc,#ee8176);border-radius:5px;box-shadow:0 2px 10px rgba(0,0,0,0.2);color:#fff;font-size:16px;font-weight:bold;line-height:24px;padding:15px30px;text-decoration:none;transition:0.25s all ease-in-out;}";
    style += "a.back:hover{box-shadow:0 4px 20px rgba(0,0,0,0.4);}";
    style += "</style>";
    return style;
};

String HTML_Error(int num_of_error) {
    String page = "<!DOCTYPE html>";
    page += "<html lang=\"en\">";
    page += "<head>";
    page += "    <meta charset=\"UTF-8\">";
    page += "    <title>Error</title>";
    page += "    <link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/twitter-bootstrap/3.3.7/css/bootstrap.min.css\">";
    page += HTML_Style_Errors();
    page += "    <link rel=\"icon\" href=\"https://img.icons8.com/bubbles/100/null/cancel--v1.png\">";
    page += "</head>";
    page += "<body>";
    page += "    <div class=\"error-page\">";
    page += "        <div>";
    if (num_of_error == 400)
    {
        page += "            <h1 data-h1='400'>400</h1>";
        page += "            <p data-p='BAD REQUEST'>BAD REQUEST</p>";
    }
    else if (num_of_error == 401)
    {
        page += "            <h1 data-h1='401'>401</h1>";
        page += "            <p data-p='UNAUTHORIZED'>UNAUTHORIZED</p>";
    }
    else if (num_of_error == 403)
    {
        page += "            <h1 data-h1='403'>403</h1>";
        page += "            <p data-p='FORBIDDEN'>FORBIDDEN</p>";
    }
    else if (num_of_error == 404)
    {
        page += "            <h1 data-h1='404'>404</h1>";
        page += "            <p data-p='NOT FOUND'>NOT FOUND</p>";
    }
    else if (num_of_error == 500)
    {
        page += "            <h1 data-h1='500'>500</h1>";
        page += "            <p data-p='SERVER ERROR'>SERVER ERROR</p>";
    }
    else
    {
        page += "            <h1 data-h1='"; page += num_of_error; page += "'>"; page += num_of_error; page += "</h1>";
        page += "            <p data-p='We don't know what happened'>We don't know what happened</p>";
    };
    page += "        </div>";
    page += "    </div>";
    page += "    <div id=\"particles-js\">";
    page += "      <canvas class=\"particles-js-canvas-el\" width=\"381\" height=\"545\" style=\"width: 100%; height: 100%;\">";
    page += "      </canvas>";
    page += "    </div>";
    page += "    <!--a(href=\"#\").back GO BACK-->";
    page += "</body>";
    page += "</html>";
    return page;
}