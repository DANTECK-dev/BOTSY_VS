#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <Arduino.h>
#include <EEPROM.h>
#include <SPI.h>
#include <SD.h>

#include "JQuery.h"

#define PORT      85
#define DNS_PORT  53

#define WAITING_TICKS 15000

/*
#define RED     5
#define GREEN   6
#define BLUE    7
*/

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

IPAddress APIP(192, 168, 1, 1);

File jQuery;
String jQuery_str;
String JQ;

String HTML_Page_Index();
String HTML_JS_Index(String SSIDs);
String HTML_Style_Index();
String HTML_Style_Errors();
String HTML_Error(int num_of_error);
String jQuery_sd();
//String jQuery();
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

    //digitalWrite(RED, HIGH);

    EEPROM.begin(4096);
    EEPROM.get(0, config);
    //EEPROM.put(0, config);

    server.on("/", headroot);
    server.on("/connect", handle_Connect);  // привязать функцию обработчика к URL-пути
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

    Serial.print("Initializing SD card...");

    if (!SD.begin(D8)) {
        Serial.println("initialization failed!");
    }
    else {
        Serial.println("initialization done.");
    }

    if (SD.exists("JQuery.js")) {
        Serial.println("JQuery.js exists.");
        //jQuery = SD.open("JQuery.js", FILE_READ);
        //for(size_t i = 0; i < jQuery.size(); i++){
        //    jQuery_str += (char)jQuery.read();
        //}
        //jQuery.close();
        //jQuery_str = jQuery_sd();
        //Serial.println(jQuery_str);
        //Serial.println("JQuery.js readed");
    }
    else {
        Serial.println("JQuery.js doesn't exists.");
    }

    Connecting_to_WiFi();

    //digitalWrite(RED, LOW);

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
    /*
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, LOW);
    */

    Serial.println("Trying connecting AP mode");

    //flicker_blue = true;

    WiFi.disconnect(true);

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

    //digitalWrite(BLUE, HIGH);

    server.enableCORS(false);

}

void Start_STA() {
    /*
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, LOW);
    */

    Serial.println("Trying connecting STA mode");

    //flicker_green = true;

    WiFi.disconnect(true);

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

    //digitalWrite(GREEN, HIGH);

    server.enableCORS(false);

}

void Start_AP_STA() {
    /*
    digitalWrite(RED, LOW);
    digitalWrite(GREEN, LOW);
    digitalWrite(BLUE, LOW);
    */

    Serial.println("Trying connecting AP & STA mode");

    WiFi.disconnect(true);

    WiFi.mode(WIFI_AP_STA);
    WiFi.enableAP(true);
    WiFi.enableSTA(true);
    WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));
    WiFi.softAP(config.ap_ssid, config.ap_pass);
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

    server.on("/", headroot);
    server.begin();
    */

    server.enableCORS(false);

    /*
    digitalWrite(GREEN, HIGH);
    digitalWrite(BLUE, HIGH);
    */
}

void Connecting_to_WiFi() {
    bool connected = false;
    int tick = 0;
    if (String(config.sta_ssid) == nullptr || String(config.sta_ssid).equals(""))
    {
        Serial.println("Нет данных о подключеной сети в системе");
    }
    else {
        Start_AP_STA();
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

            tick += 100;
        };
    }
    if (connected)
    {
        //успешно подключено, отправить страницу с доанными о WIFI и локальном ip + кнопку отключения
        EEPROM.put(0, config);
        EEPROM.commit();
        Serial.println("Connect success");
        server.send(200, "text/html", HTML_Page_Index());
    }
    else {
        //неудалось подключится, отправить странуцу с ошибкой

        /*
        WL_NO_SHIELD        = 255,   // for compatibility with WiFi Shield library
        WL_IDLE_STATUS      = 0,
        WL_NO_SSID_AVAIL    = 1,
        WL_SCAN_COMPLETED   = 2,
        WL_CONNECTED        = 3,
        WL_CONNECT_FAILED   = 4,
        WL_CONNECTION_LOST  = 5,
        WL_WRONG_PASSWORD   = 6,
        WL_DISCONNECTED     = 7
        */

        if (WiFi.status() == WL_NO_SHIELD)
        {
            //когда WiFi-сеть переключается с одного статуса на другой.
            Serial.print("для совместимости с библиотекой WiFi Shield");
        }
        if (WiFi.status() == WL_IDLE_STATUS)
        {
            //когда WiFi-сеть переключается с одного статуса на другой.
            Serial.print("WiFi-сеть переключается с одного статуса на другой");
        }
        if (WiFi.status() == WL_NO_SSID_AVAIL)
        {
            //если заданный SSID находится вне зоны доступа.
            Serial.print("Заданный SSID находится вне зоны доступа");
        }
        if (WiFi.status() == WL_SCAN_COMPLETED)
        {
            //если неправильный пароль.
            Serial.print("Сканирование сетей закончено");
        }
        if (WiFi.status() == WL_CONNECT_FAILED)
        {
            //если неправильный пароль.
            Serial.print("Ошибка подключения");
        }
        if (WiFi.status() == WL_CONNECTION_LOST)
        {
            //если неправильный пароль.
            Serial.print("Соединение потеряно");
        }
        if (WiFi.status() == WL_WRONG_PASSWORD)
        {
            //если неправильный пароль.
            Serial.print("Немправтлььный пароль");
        }
        if (WiFi.status() == WL_DISCONNECTED)
        {
            //если модуль не находится в режиме станции.
            Serial.print("Модуль не находится в режиме станции");
        }

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
    String page = HTML_Page_Index();
    Serial.println(page);
    server.send(200, "text/html", page);
}

void handle_Connect() {
    for (int i = 0; i < server.args(); i++)
        Serial.println(server.arg(i));
    DynamicJsonDocument parsed(1024);
    deserializeJson(parsed, server.arg(0));

    Serial.println(String(parsed["SSID"]));
    Serial.println(String(parsed["PASS"]));

    //String(parsed["SSID"]).toCharArray(config.sta_ssid, String(parsed["SSID"]).length);
    //String(parsed["PASS"]).toCharArray(config.sta_pass, String(parsed["PASS"]).length);

    strcpy(config.sta_ssid, ((String)parsed["SSID"]).c_str());
    strcpy(config.sta_pass, ((String)parsed["PASS"]).c_str());

    Connecting_to_WiFi();

    //server.send(200, "text/html", HTML_Page_Index());
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
    if (!SD.exists("JQuery.js")) {
        //Serial.println("JQuery.js doesn't exists.");
        //jQuery = SD.open("JQuery.js", FILE_READ).readString();
    }
    String SSIDs = "";
    String page = F("");
    page += F("<!DOCTYPE html>\n");
    page += "<html lang=\"en\">\n";
    page += "<head>\n";
    page += "    <meta charset=\"UTF-8\">\n";
    page += "    <title>BoTSy</title>\n";
    page += "    <link rel=\"icon\" href=\"https://img.icons8.com/cotton/256/wifi--v2.png 2x\">\n";
    page += HTML_Style_Index();
    page += "    </head>\n";
    page += "<body>\n";
    page += "<main>\n";
    page += "    <div class=\"success\">\n";
    page += "    </div>\n";
    page += "    <h1 class=\"visible\">Body Tracking System</h1>\n";
    page += "    <h1 class=\"hidden\">BoTSy</h1>\n";
    page += "    <div id=\"wifi_list\">\n";
    page += "        <h2>Список Wi-Fi точек</h2>\n";
    page += "        <ul id=\"wifi_list_list\">\n";
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
    page += "        </ul>\n";
    page += "        <div class=\"field\">\n";
    page += "            <div class=\"text-field text-field_floating-3\">\n";
    page += "                <input class=\"text-field__input\" type=\"text\" id=\"SSID\" name=\"SSID\" placeholder=\"SSID\">\n";
    page += "                <label class=\"text-field__label\" for=\"SSID\">Имя сети</label>\n";
    page += "            </div>\n";
    page += "        </div>\n";
    page += "        <div class=\"field\">\n";
    page += "            <div class=\"text-field text-field_floating-3\">\n";
    page += "                <input class=\"text-field__input\" type=\"text\" id=\"PASS\" name=\"PASS\" placeholder=\"PASS\">\n";
    page += "                <label class=\"text-field__label\" for=\"PASS\">Пароль</label>\n";
    page += "            </div>\n";
    page += "        </div>\n";
    page += "        <div class=\"center\">\n";
    page += "            <button id=\"Connect_Button\">Подключиться</button>\n";
    page += "        </div>\n";
    page += "        <div id=\"loader\" class=\"center\"></div>\n";
    page += "    </div>\n";
    page += "    </main>\n";

    page += HTML_JS_Index(SSIDs);

    //page += JQuery::js1;
    //page += jQuery();
    //jQuery = SD.open("JQuery.js", FILE_READ);
    //page += jQuery.readString();
    //jQuery.close();
    //page += jQuery_sd();
    /*
    jQuery = SD.open("JQuery_ajax_min.js", FILE_READ);
    for(size_t i = 0; i < jQuery.size(); i++) {
        char temp = (char)jQuery.read();
        page += String(temp);
        Serial.print(temp);
    }
    jQuery.close();
    */
    page += JQuery::JQ();
    page += "</body>\n";
    page += "</html>\n";
    return page;
}

String HTML_JS_Index(String SSIDs) {
    String js = F("");
    js += "<script>\n";
    js += "   document.getElementById(\"SSID\").value = localStorage.getItem(\"SSID\")\n";
    js += "   document.getElementById(\"PASS\").value = localStorage.getItem(\"PASS\")\n";
    js += "   function getRandom(min, max) {";
    js += "       return Math.floor(Math.random() * (max - min)) + min;";
    js += "   }\n";
    js += "   let loading_links = [";
    js += "       \"https://media.tenor.com/In8c2digq1MAAAAC/claudio-claudin.gif\",";
    js += "       \"https://media.tenor.com/dmw5C4_LUHAAAAAC/loading-girls.gif\",";
    js += "       \"https://media.tenor.com/zkEybmcArhoAAAAS/discord-loading.gif\",";
    js += "       \"https://media.tenor.com/6Pjav1LgBIAAAAAC/tenkahyakken-chatannakiri.gif\",";
    js += "       \"https://media.tenor.com/d2fZlKUq6-UAAAAC/discord.gif\",";
    js += "       \"https://media.tenor.com/tDb298oy_nEAAAAC/erodiyn-loading.gif\",";
    js += "       \"https://media.tenor.com/RVvnVPK-6dcAAAAC/reload-cat.gif\",";
    js += "       \"https://media.tenor.com/Ao1ftp1W0WAAAAAS/discord-notification.gif\",";
    js += "       \"https://media.tenor.com/VW6fRXVCok4AAAAC/inugami-korone.gif\",";
    js += "       \"https://media.tenor.com/U6FZAy_RGSIAAAAd/gaminglight-imperialrp.gif\",";
    js += "   ]\n";
    js += "   let APIP = \"http://"; js += WiFi.softAPIP().toString(); js += "/\"\n";
    js += "   let STAIP = \"http://"; js += WiFi.localIP().toString(); js += "/\"\n";
    js += "   let SSIDs = ["; js += SSIDs; js += "]\n";
    js += "   document.getElementById(\"Connect_Button\").addEventListener(\"click\", function (e) {\n";
    js += "       if(document.getElementById(\"SSID\").value == \"\") {\n";
    js += "           alert(\"Заполните поле \\\"Имя сети\\\" или выберите из списка доступных сетей\");\n";
    js += "           return\n";
    js += "       }\n";
    js += "       if(SSIDs.indexOf(document.getElementById(\"SSID\").value) == -1) {\n";
    js += "           alert(\"Такой сети не существует\")\n";
    js += "           return\n";
    js += "       }\n";
    js += "       localStorage.setItem(\"SSID\", document.getElementById(\"SSID\").value)\n";
    js += "       localStorage.setItem(\"PASS\", document.getElementById(\"PASS\").value)\n";
    js += "       $.ajax({\n";
    js += "           url: '/connect',\n";
    js += "           type: 'POST',\n";
    js += "           data: JSON.stringify({SSID:document.getElementById(\"SSID\").value, PASS:document.getElementById(\"PASS\").value}),\n";
    js += "           success: function(){alert(\"Successfully post\")},\n";
    js += "           contentType: \"application/json\",\n";
    js += "           dataType: 'json'\n";
    js += "       })\n";
    js += "       document.getElementById(\"loader\").innerHTML = `<img style=\"margin-bottom: 5px; border-radius: 5px;\" src=\"` + loading_links[getRandom(0, 10)] + `\" width=\"200\">`\n";
    js += "   })\n";
    js += "   for (let item of document.querySelectorAll('ul > li')) {item.addEventListener('click', function (e){\n";
    js += "       document.getElementById('SSID').value = item.querySelectorAll('div')[0].querySelectorAll('h4')[0].innerText\n";
    js += "       if(item.querySelectorAll('div')[2].querySelectorAll('h4')[0].innerText == \"Открытая сеть\"){\n";
    js += "           document.getElementsByClassName('field')[1].style = `display: none;`\n";
    js += "           document.getElementById(\"PASS\").value = \"\"\n";
    js += "       } else {\n";
    js += "           document.getElementsByClassName('field')[1].style = `display: block;`\n";
    js += "       }\n";
    js += "   },{})}\n";
    js += "</script>\n";
    return js;
}

String HTML_Style_Index() {
    String style = F("<style>");
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

String jQuery_link() {
    return "<script src=\"https://ajax.googleapis.com/ajax/libs/jquery/3.6.3/jquery.min.js\"></script>";
}

String jQuery_sd() {

    jQuery = SD.open("JQuery_ajax.js", FILE_READ);
    //for(size_t i = 0; i < jQuery.size(); i++) {
    //    char temp = (char)jQuery.read();
    //    js += String(temp);
    //    Serial.print(js);
    //}
    while (jQuery.available()) {
        JQ = jQuery.readStringUntil('\n');
        Serial.println(JQ);
    }
    jQuery.close();
    Serial.println(JQ);
    return JQ;
}