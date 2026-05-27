#ifndef OASERVER_h
#define OASERVER_h

// #############################################################################
//
//       __ ________  _____  ____  ___   ___  ___
//      / //_/ __/\ \/ / _ )/ __ \/ _ | / _ \/ _ \
//     / ,< / _/   \  / _  / /_/ / __ |/ , _/ // /
//    /_/|_/___/_  /_/____/\____/_/_|_/_/|_/____/
//      / _ \/ _ | / _ \/_  __/ |/ / __/ _ \
//     / ___/ __ |/ , _/ / / /    / _// , _/
//    /_/  /_/ |_/_/|_| /_/ /_/|_/___/_/|_|
//
// ESP32-basierter FPGA-Loader mit Webinterface und LCD-MenÜ
// Für KBP MenuPanel LCD 16x2 mit I2C, aber auch mit anderen Displays kompatibel
// und als Basis für andere Projekte mit ähnlichen Anforderungen nutzbar
// (c) KeyboardPartner UG & C. Meyer 03/2026
//
// #############################################################################

#include <Arduino.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <WiFiClient.h>
// Server
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ElegantOTA.h> // https://docs.elegantota.pro/

#include "global_vars.h"
#include "fpga_config.h"
#include "dataflash.h"
#include "files.h"
#include "menu_items.h"
#include "drawing.h"

// #define DEBUG_JSON

#ifdef DEBUG_JSON
  #define DJSPRINT(...)    Serial.print(__VA_ARGS__)
  //OR, #define DJSPRINT(args...)    Serial.print(args)
  #define DJSPRINTLN(...)  Serial.println(__VA_ARGS__)
  #define DJSPRINTF(...)    Serial.print(F(__VA_ARGS__))
  #define DJSPRINTLNF(...) Serial.println(F(__VA_ARGS__)) //printing text using the F macro
#else
  #define DJSPRINT(...)     //blank line
  #define DJSPRINTLN(...)   //blank line
  #define DJSPRINTF(...)    //blank line
  #define DJSPRINTLNF(...)  //blank line
#endif

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
WiFiClient client;

unsigned long ota_progress_millis = 0;

int currentParamPage = 0;

// ##############################################################################
//
//    ##      ## #### ######## ####
//    ##  ##  ##  ##  ##        ##
//    ##  ##  ##  ##  ##        ##
//    ##  ##  ##  ##  ######    ##
//    ##  ##  ##  ##  ##        ##
//    ##  ##  ##  ##  ##        ##
//     ###  ###  #### ##       ####
//
// ##############################################################################

int wps_connect() {
  Serial.println(F("WPS Anmeldung..."));
  Serial.println(F("bitte warten"));
  drawMsg("WPS Anmeldung...", "bitte warten", DB_INFO_OK);
  delay(1000);
  return false;
}

void wifi_connect_ap() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP("HX3 Mainboard", ""); // access point, kein PW
  Serial.println(F("Access Point IP: 192.168.4.1"));
  drawMsg("Access Point IP:", "192.168.4.1", DB_INFO_OK);
  delay(3000);
}

wl_status_t wifi_connect_sta() {
  int i;
  wl_status_t status;
  char ssid[32]; // Router SSID
  String password = settings.password;
  Serial.print(F("Verbinde mit "));
  Serial.print(settings.ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(settings.ssid, settings.password);
  drawMsg("Verbinde mit ", settings.ssid, DB_INFO_OK);
  delay(350);
  i = 0;
  while ((WiFi.status() != WL_CONNECTED) && (i < 15)) {
    Serial.write('.');
    delay(350);
    i++;
  }
  Serial.println();

  status = WiFi.status();
  if (status == WL_CONNECTED) {
    drawMsg("Browser Config IP: ", WiFi.localIP().toString().c_str(), DB_INFO_OK);
    delay(MSG_DISPLAY_TIME);
    Serial.println(F("WLAN Verbindung erfolgreich!"));
    Serial.print(F("Browser Config IP: "));
    Serial.println(WiFi.localIP());
  } else {
    Serial.println(F("WLAN Fehler - nicht verbunden!"));
    drawMsg("WLAN Fehler", "Nicht verbunden!", DB_ERROR_OK);
    delay(MSG_DISPLAY_TIME);
    WiFi.disconnect(true);
  }
  return status;
}

// ##############################################################################
//
//     #######  ########    ###
//    ##     ##    ##      ## ##
//    ##     ##    ##     ##   ##
//    ##     ##    ##    ##     ##
//    ##     ##    ##    #########
//    ##     ##    ##    ##     ##
//     #######     ##    ##     ##
//
// ##############################################################################

void onOTAStart() {
  // Log when OTA has started
  Serial.println(F("OTA update started!"));
  // <Add your own code here>
}

void onOTAProgress(size_t current, size_t final) {
  // Log every 1 second
  if (millis() - ota_progress_millis > 1000) {
    ota_progress_millis = millis();
    Serial.printf("OTA Progress Current: %u bytes, Final: %u bytes\n", current, final);
  }
}

void onOTAEnd(bool success) {
  // Log when OTA has finished
  if (success) {
    Serial.println(F("OTA update finished successfully"));
  } else {
    Serial.println(F("Error during OTA update!"));
  }
  // <Add your own code here>
}

// ##############################################################################
//
//     ######  ######## ########  ##     ## ######## ########
//    ##    ## ##       ##     ## ##     ## ##       ##     ##
//    ##       ##       ##     ## ##     ## ##       ##     ##
//     ######  ######   ########  ##     ## ######   ########
//          ## ##       ##   ##    ##   ##  ##       ##   ##
//    ##    ## ##       ##    ##    ## ##   ##       ##    ##
//     ######  ######## ##     ##    ###    ######## ##     ##
//
// ##############################################################################
//
//    ##     ## ########  ##        #######     ###    ########  
//    ##     ## ##     ## ##       ##     ##   ## ##   ##     ## 
//    ##     ## ##     ## ##       ##     ##  ##   ##  ##     ## 
//    ##     ## ########  ##       ##     ## ##     ## ##     ## 
//    ##     ## ##        ##       ##     ## ######### ##     ## 
//    ##     ## ##        ##       ##     ## ##     ## ##     ## 
//     #######  ##        ########  #######  ##     ## ########  
//                                                     
// ##############################################################################

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  // handles uploads to File System (LittleFS)
  static int blockIndex = 0;
  static uint32_t start_time;
  if (!index) {
    // open the file on first call and store the file handle in the request object
    DPRINTF("Upload Start: ");
    DPRINTLN(filename.c_str());
    start_time = millis();
    blockIndex = 0;
    request->_tempFile = LittleFS.open("/" + filename, "w");
  }
  if (len) {
    // stream the incoming chunk to the opened file
    request->_tempFile.write(data, len);
    blockIndex++;
  }
  if (final) {
    // close the file handle as the upload is now done
    request->_tempFile.close();
    DPRINTF("Upload to FS complete, size: ");
    DPRINT(index + len);
    DPRINTF(", duration: ");
    DPRINT(millis() - start_time);
    DPRINTLNF(" ms");
  }
}

static bool dfUploadLastFileValid = true; // set at end of each /upload_df transfer; read by POST handler for redirect
static uint16_t dfUploadErrFlags = ERR_DF_OK;

void handleUpload_df(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  // handles uploads to external SAM QSPI (WFU/DFU File) or FPGA Dataflash
  static int chunkIndex;
  static bool file_valid, is_dfu;
  static uint32_t start_time;
  static bool isQSPIready = false; // Flag, ob QSPI-Flash bereit ist, wird bei jedem Zugriff auf die Weboberfläche aktualisiert
  static int16_t file_index = -1; // Index des empfangenen Files in der Liste der bekannten Dateien, -1 = unbekannt
  if (!index) {
    // erster Chunk empfangen, Datei- und Zielinformationen ermitteln
    dfUploadErrFlags = ERR_DF_OK;
    chunkIndex = 0;
    // filename.toUpperCase(); // Großschreibung für Vergleiche, z.B. mit ".WFU"
    is_dfu = filename.endsWith(".DFU") || filename.endsWith(".dfu"); // Bei DFU-Dateien müssen die Bytes paarweise getauscht werden, da sie im MSB-First-Format vorliegen
    file_valid = is_dfu || filename.endsWith(".bin") || filename.endsWith(".BIN");
    file_valid |= filename.endsWith(".dat") || filename.endsWith(".DAT");
    DPRINTF("Upload file ");
    DPRINT(filename);
    start_time = millis();
    if (!is_dfu) {
      // ermitteln, ob Update-fähiges File in der Liste der bekannten Dateien vorhanden ist, 
      // um Zielblocknummer und DFUDL-Block anlegen zu können
      file_index = lookupFileName(filename.c_str());
      file_valid &= (file_index >= 0);
    }
    if (file_valid) {
      // Ziel QSPI-Dataflash
      DPRINTF(" to QSPI Block ");
      DPRINTLN(BLOCK_DSP_DFU);
      isQSPIready = spi_xc_qspi_activate();
      // DFUDL für 1 Datei anlegen, damit Bootloader sie später als DFU-Update erkennen kann
    } else {
      DPRINTLNF(" is not valid, skipped");
    }
  }
  if (len || final) {
    // stream the incoming chunk to QSPI write 
    if (file_valid && isQSPIready) {
      dfUploadErrFlags |= qspi_flash_dfu_chunk(data, index, len, final, true); // Daten direkt auf QSPI flashen
    }
    chunkIndex++;
  }

  if (final) {
    // upload is now done
    if (file_valid) {
      if (!isQSPIready) {
        DPRINTLNF("QSPI was not ready, upload failed");
        dfUploadErrFlags |= ERR_DF_WRITE;
        spi_xc_qspi_deactivate(); // SPI für DF-Zugriff deaktivieren
      } else {
        DPRINTLNF("");
        DPRINTF("Upload to QSPI complete, size: ");
        DPRINT(index + len);
        DPRINTF(", duration: ");
        DPRINT(millis() - start_time);
        DPRINTLNF(" ms");
        // Bei Nicht-DFU-Dateien, z.B. .bin, wird zusätzlich ein DFUDL-Block mit Zielblocknummer angelegt, damit Bootloader die Datei später als DFU-Update erkennen kann
        qspi_flash_dfudl(file_index, false);
        dfUploadLastFileValid = file_valid; // signal result to POST completion handler
        spi_xc_qspi_deactivate(); // SPI für DF-Zugriff deaktivieren
        qspi_notify_update(file_index, is_dfu);
      }
    }
  }
}

// ##############################################################################
//
//    ########   #######   #######  ######## 
//    ##     ## ##     ## ##     ##    ##    
//    ##     ## ##     ## ##     ##    ##    
//    ########  ##     ## ##     ##    ##    
//    ##   ##   ##     ## ##     ##    ##    
//    ##    ##  ##     ## ##     ##    ##    
//    ##     ##  #######   #######     ##    
//                                                     
// ##############################################################################

String html_processor_root(const String &var) {
  // Werte für Webpage senden, so durch Platzhalter angefordert
  if (var == "STA_SSID") {
    return String(settings.ssid);
  } else if (var == "STA_PASSWORD") {
    return String(settings.password);
  } else if (var == "STA_MODE") {
    if (settings.wifiMode == 1)
      return "checked"; // oder gar nicht, "value" geht nicht!
    else
      return String();
  } else if (var == "DIRECTORY") {
    // Directory-Tabellenzeilen <tr></tr> schicken
    String dirList = "";
    File root = LittleFS.open("/");
    File file = root.openNextFile();
    String filename;
    while (file) {
      filename = file.name();
      // dirList = dirList + "<a href=\"" + file.name() + "\">" + file.name() + "</a> (" + file.size() + " Bytes)<br>";
      dirList = dirList + "<tr><td class=\"link\" style=\"width: 200px\"><a href=\"" + filename + "\">" + filename + "</a></td>";
      dirList = dirList + "<td style=\"width: 100px\"><small>" + file.size() + " Bytes</small></td><td>";
      if ((filename != "index.html") && (filename != "style.css.gz"))
        dirList = dirList + "<a href=\"/?delete=/" + filename + "\">Delete</a>";
      dirList = dirList + "</td></tr>\r\n";
      file = root.openNextFile();
    }
    return dirList;
  } else if (var == "FPGADATE") {
    // FPGA Date Register lesen, z.B. 0xDDMMYYYY als Hexadezimalzahl
    return String(fpgaDate, HEX);
  } else {
    return String();
  }
}


struct ParamInfo {
  char param[8]; // z.B. "1495"
  char desc[32]; // z.B. "Panel LED Brightness"
  char value[8]; // undefined
  char max[8]; // z.B. "127"
  char type[8]; // z.B. "trk"
};

int extractValues(String line, char separator, ParamInfo &paramInfo) {
  // CSV-String zerlegen und in ParamInfo-Struktur packen
  // Format des Strings: Param;Description;Value;Max;Type
  // Beispiel: 1495;Panel LED Brightness;15;trk
  // liefert TRUE wenn erfolgreich (alle 4 Felder vorhanden), FALSE wenn nicht
  int fieldCount = 0;
  String field = "";
  memset(&paramInfo, 0, sizeof(paramInfo)); // paramInfo zurücksetzen
  // Parse the line and split by separator
  for (int i = 0; i <= line.length(); i++) {
    // Check for separator or end of string
    if (i == line.length() || line[i] == separator) {   
      field.trim();
      // Process the field based on position
      if (fieldCount == 0) {
        field.toCharArray(paramInfo.param, sizeof(paramInfo.param));
      } else if (fieldCount == 1) {
        field.toCharArray(paramInfo.desc, sizeof(paramInfo.desc));
      } else if (fieldCount == 2) {
        field.toCharArray(paramInfo.max, sizeof(paramInfo.max));
      } else if (fieldCount == 3) {
        field.toCharArray(paramInfo.type, sizeof(paramInfo.type));
      }
      fieldCount++;
      field = "";
    } else {
      field += line[i];
    }
  }
  // return number of fields
  return fieldCount;
}

bool getLineFromFile(File &file, String &line) {
  char lineBuf[128]; // Hilfspuffer für Zeile aus Datei, z.B. für Parameterdatei
  line = String();
  int len = 0;
  while (file.available()) {
    len = file.readBytesUntil('\n', lineBuf, sizeof(lineBuf));
    if ((len > 0) && (len < sizeof(lineBuf))) {
      lineBuf[len] = '\0'; // Null-terminate the string
      line = String(lineBuf);
      return true;
    }
  }
  return false; // Return true if there's a last line without newline
}


// ###############################################################################
//
//    #### ##    ## #### ######## 
//     ##  ###   ##  ##     ##    
//     ##  ####  ##  ##     ##    
//     ##  ## ## ##  ##     ##    
//     ##  ##  ####  ##     ##    
//     ##  ##   ###  ##     ##    
//    #### ##    ## ####    ##    
//
// ###############################################################################

void notFound(AsyncWebServerRequest *request) {
  request->send(404, "text/plain", "NOT FOUND");
}

void init_server() {
  // In setup() aufrufen
  // Handler für Webseiten-Requests installieren
  DPRINTLNF("Installing Handlers for Server...");

  // Route for root / web page
  server.on("/saveinits", HTTP_GET, [](AsyncWebServerRequest *request) {
    DJSPRINTLNF("Index Get Req");
    String value, param, redirect;
    redirect = "/";
    // Handler fuer einzelne Parameter

    // Basisdaten
    if (request->hasParam("ssid_sta")) {
      value = request->getParam("ssid_sta")->value();
      value.toCharArray(settings.ssid, value.length() + 1);
      DPRINTF("Set SSID: ");
      DPRINTLN(settings.ssid);
    }

    if (request->hasParam("pass_sta")) {
      value = request->getParam("pass_sta")->value();
      value.toCharArray(settings.password, value.length() + 1);
  	  DPRINTF("Set Password: ");
      DPRINTLN(settings.password);
    }

    if (request->hasParam("param_page")) {
      value = request->getParam("param_page")->value();
      currentParamPage = value.toInt();
      DPRINTF("Set Param Page: ");
      DPRINTLN(currentParamPage);
    }

    if (request->hasParam("hascheckbox")) {
      DPRINTF("Set Switches");
      if(request->hasParam("auto_upload")) // wird gesendet oder gar nicht
        settings.autoUpload = 1;
      else
        settings.autoUpload = 0;

      if(request->hasParam("sta_mode")) // wird gesendet oder gar nicht
        settings.wifiMode = 1;
      else
        settings.wifiMode = 2; // AP mode
    }
    // alle Werte abspeichern
    saveCredentials();
    // Seite nochmal aktualisiert senden
    request->redirect(redirect); 
  });

  // run handleUpload function when any file is uploaded to SPIFFS
  server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
      DPRINTLNF("Index Upload done");
      request->redirect("/"); 
  }, handleUpload);

   // run handleUpload function when a DFU or BIN/DAT file is uploaded
  server.on("/upload_df", HTTP_POST, [](AsyncWebServerRequest *request) {
      if (!dfUploadLastFileValid) {
        DPRINTLNF("Uploaded file invalid");
        request->redirect("/?df_fileerr=1");
      } else if (dfUploadErrFlags != ERR_DF_OK) {
        DPRINTF("Errors during flash write, flags: 0x");
        DPRINTLN(dfUploadErrFlags, HEX);
        request->redirect("/?df_flasherr=1");
      } else {
        DPRINTLNF("Upload OK");
        request->redirect("/");
      }
  }, handleUpload_df);

   // Route for reset button
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request) {
      DPRINTLNF("Index Reset Req");
      resetRequested = true; // Flag setzen, um einen Reset anzufordern
      request->redirect("/"); 
  });

 // ------------------------------------------------------------------------------

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("delete")) {
      DPRINTLNF("Index Delete Req");
      String value = request->getParam("delete")->value();
      LittleFS.remove(value);
      request->redirect("/");
    } else {
      DPRINTLNF("Index Root Req");
      request->send(LittleFS , "/index.html", String(), false, html_processor_root);
    } 
  });

  // Route for panel web page
  server.on("/panel.html", HTTP_GET, [](AsyncWebServerRequest *request) {
      DPRINTLNF("Panel Req");
      request->send(LittleFS , "/panel.html", String(), false);
  });

  // Route for params web page
  server.on("/params.html", HTTP_GET, [](AsyncWebServerRequest *request) {
      DPRINTLNF("Params Req");
      request->send(LittleFS , "/params.html", String(), false);
  });


// ###############################################################################
// HX3 PARAM PAGES
// ###############################################################################

  // Standard GET requests from navigation buttons, input fields
  server.on("/navbutton", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("param_page")) {
      // Set current page for parameter editor
      int page = request->getParam("param_page")->value().toInt();
      if (page < 0 || page > 9) {
        request->send(400, "text/plain", "OUT OF RANGE");
        return;
      }
      currentParamPage = page;
    }
    if (request->hasParam("param_prev")) {
      // Previous page button clicked
      if (currentParamPage > 0) currentParamPage--;
    }
    if (request->hasParam("param_next")) {
      // Next page button clicked
      if (currentParamPage < 9) currentParamPage++;
    }
    DPRINTF("Navbutton Page=");
    DPRINTLN(currentParamPage);
    request->redirect("/params.html"); // reload same page
  });
 
  // Return current HX3 param values as JSON:
  server.on("/getparams_json", HTTP_GET, [](AsyncWebServerRequest *request) {
    ParamInfo paramInfo;
    DPRINTLNF("Params JSON req");
    String json = "[\r\n";
    String value_str = "0";
    String file_name = "/params_" + String(currentParamPage) + ".txt";
    String line = "";
    String param_str = "";
    File file = LittleFS.open(file_name, "r");
    do  {
      if (!getLineFromFile(file, line)) break; // keine Zeile mehr, abbrechen
      if (extractValues(line, ';', paramInfo) < 3) break; // ungültige Zeile, abbrechen
      if (paramInfo.param[0] == '#') {
        param_str = "\"#\""; // muss in "" stehen!
      } else { // kein Header
        param_str = String(paramInfo.param); // z.B. "1495"
        int paramIndex = atoi(paramInfo.param) - 1000; // z.B. 1495
        if (paramIndex < 0 || paramIndex > 999) {
          value_str = "0"; // kein Wert
        } else {
          value_str = String(hx3EditArray[paramIndex]); // aktueller Wert aus Array, nicht aus Datei, da er sich ja ändern kann 
        }
      }
      json += "{\"param\":" + param_str + ",\"desc\":\"" + String(paramInfo.desc) 
          + "\",\"value\":" + value_str + ",\"max\":" 
          + String(paramInfo.max) + ",\"type\":\"" + String(paramInfo.type) + "\"}";
      json += ",\r\n";
    } while (true);
    file.close();
    json += "\r\n{\"page\":" + String(currentParamPage) + "}\r\n]";
    DJSPRINTLN(json); // TEST
    request->send(200, "application/json", json); 
  });

// ###############################################################################

  // Return current HX3 param from hx3EditArray[] values as JSON:
  server.on("/getpanel_json", HTTP_GET, [](AsyncWebServerRequest *request) {
    int16_t upperVoicePreset = hx3EditArray[269] & 0x0F; // active upper preset, 0..11
    int16_t lowerVoicePreset = hx3EditArray[270] & 0x0F; // active lower preset, 0..11

    DPRINTLNF("Panel JSON req");
    // Array mit aktuellen Werten vom HX3 füllen, damit die Weboberfläche immer die aktuellen Werte anzeigt
    spi_xc_getEditArray(250); 
    String json = "{";
    for (int i = 0; i < 9; i++) {
      json += "\"db" + String(i) + "\":" + String(hx3EditArray[i]/15);
      json += ",";
    }
    for (int i = 16; i < 25; i++) {
      json += "\"db" + String(i) + "\":" + String(hx3EditArray[i]/15);
      json += ",";
    }
    for (int i = 128; i < 134; i++) {
      json += "\"sw" + String(i) + "\":" + String(hx3EditArray[i] != 0 ? 1 : 0); // Switches als 0 oder 1 zurückgeben
      json += ",";
    }
    json += "\"kn264\":" + String(hx3EditArray[264]);
    json += ",\"upper_voice\":" + String(upperVoicePreset);
    json += ",\"lower_voice\":" + String(lowerVoicePreset);
    json += "}";
    DJSPRINTLN(json); // TEST
    request->send(200, "application/json", json); 
  });

  server.on("/setparval", HTTP_GET, [](AsyncWebServerRequest *request) {
    // Set HX3 Board Parameter value
    if (!request->hasParam("param") || !request->hasParam("val")) {
      request->send(400, "text/plain", "BAD REQUEST");
      return;
    }
    int val = request->getParam("val")->value().toInt();
    int param = request->getParam("param")->value().toInt();
    int paramIndex = param - 1000; // z.B. 1495
    if (paramIndex < 0 || paramIndex > 999) {
      request->send(400, "text/plain", "OUT OF RANGE");
      return;
    }
    hx3EditArray[paramIndex] = val; // Wert im Array aktualisieren, damit er in der Weboberfläche sofort sichtbar ist, auch wenn er ja eigentlich erst an den HX3 gesendet werden muss, was z.B. durch einen Klick auf "Apply" geschehen könnte, damit
    Serial.printf("ParamPage Set edit %d to %d\n", param, val);
    spi_xc_binarycmd(param, val); // Switches als 0 oder 1 an HX3 senden, damit sie im HX3 als ON oder OFF ankommen
    request->send(200, "text/plain", "OK"); 
  });

  server.on("/setpanel", HTTP_GET, [](AsyncWebServerRequest *request) {
    // Set HX3 Panel value
    if (request->hasParam("sw") ) {
      int sw = request->getParam("sw")->value().toInt();
      int val = request->getParam("val")->value().toInt();
      Serial.printf("Panel sw %d=%d\n", sw, val);
      hx3EditArray[sw] = val != 0 ? 1 : 0; // Wert im Array aktualisieren, damit er in der Weboberfläche sofort sichtbar ist
      spi_xc_binarycmd(sw + 1000, hx3EditArray[sw]); // Switches als 0 oder 1 an HX3 senden, damit sie im HX3 als ON oder OFF ankommen
    }
    if (request->hasParam("kn") ) {
      int kn = request->getParam("kn")->value().toInt();
      int val = request->getParam("val")->value().toInt();
      Serial.printf("Panel knob %d=%d\n", kn, val);
      hx3EditArray[kn] = val; // Wert im Array aktualisieren, damit er in der Weboberfläche sofort sichtbar ist
      spi_xc_binarycmd(kn + 1000, val); // Knobs an HX3 senden
    }
    if (request->hasParam("db") ) {
      int db = request->getParam("db")->value().toInt();
      int val = request->getParam("val")->value().toInt();
      Serial.printf("Panel db %d=%d\n", db, val);
      val *= 15;
      hx3EditArray[db] = val; // Wert im Array aktualisieren, damit er in der Weboberfläche sofort sichtbar ist
      spi_xc_binarycmd(db + 1000, val); // Drawbars an HX3 senden
    }
    request->send(200, "text/plain", "OK"); 
    redrawOrganRequest = true; // nach jedem Setzen eines Werts Organ neu zeichnen, damit die Weboberfläche immer den aktuellen Zustand anzeigt, auch wenn er ja eigentlich erst durch einen Klick auf "Apply" an den HX3 gesendet werden könnte
  });

  server.on("/setpresetupr", HTTP_GET, [](AsyncWebServerRequest *request) {
    // Set active upper voice preset (0..11)
    if (!request->hasParam("pr")) {
      request->send(400, "text/plain", "BAD REQUEST");
      return;
    }
    int pr = request->getParam("pr")->value().toInt();
    if (pr < 0 || pr > 11) {
      request->send(400, "text/plain", "OUT OF RANGE");
      return;
    }
    hx3EditArray[269] = (pr & 0x0F);
    Serial.printf("Panel upper preset=%d\n", pr);
    spi_xc_binarycmd(1269, hx3EditArray[269]);
    request->send(200, "text/plain", "OK");
    redrawOrganRequest = true; // nach jedem Setzen eines Werts Organ neu zeichnen, damit die Weboberfläche immer den aktuellen Zustand anzeigt, auch wenn er ja eigentlich erst durch einen Klick auf "Apply" an den HX3 gesendet werden könnte
  });

  server.on("/setpresetlwr", HTTP_GET, [](AsyncWebServerRequest *request) {
    // Set active lower voice preset (0..11)
    if (!request->hasParam("pr")) {
      request->send(400, "text/plain", "BAD REQUEST");
      return;
    }
    int pr = request->getParam("pr")->value().toInt();
    if (pr < 0 || pr > 11) {
      request->send(400, "text/plain", "OUT OF RANGE");
      return;
    }
    hx3EditArray[270] = (pr & 0x0F);
    Serial.printf("Panel lower preset=%d\n", pr);
    spi_xc_binarycmd(1270, hx3EditArray[270]);
    request->send(200, "text/plain", "OK");
    redrawOrganRequest = true; // nach jedem Setzen eines Werts Organ neu zeichnen, damit die Weboberfläche immer den aktuellen Zustand anzeigt, auch wenn er ja eigentlich erst durch einen Klick auf "Apply" an den HX3 gesendet werden könnte
  });

    // Set a value immediately: /setpanel?sw=0&val=1
  server.on("/savepanel", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("pr") ) {
      int pr = request->getParam("pr")->value().toInt();
      DPRINTF("Save Preset to ");
      DPRINTLN(pr);
    }
    request->send(200, "text/plain", "OK"); 
  });

// ###############################################################################

  server.onNotFound(notFound);
  
  // andere Dateien, Grafiken
  server.serveStatic("/", LittleFS, "/");

  ElegantOTA.begin(&server); // Start ElegantOTA
  // ElegantOTA callbacks
  ElegantOTA.onStart(onOTAStart);
  ElegantOTA.onProgress(onOTAProgress);
  ElegantOTA.onEnd(onOTAEnd); // Start server
  server.begin();
  Serial.println(F("Done."));
}

#endif