#include <WiFi.h>
// #include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_7C.h>
#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
#include "GxEPD2_display_selection_new_style.h"
#include "GxEPD2_display_selection.h"
#include "GxEPD2_display_selection_added.h"

void setup()
{
  display.init(115200, true, 2, false);
  connectWifi();
}


void connectWifi()
{
  WiFi.begin(WIFI_SSID, WIFI_PW);
  delay(500);
  while (WiFi.status() != WL_CONNECTED){}
  Serial.println("Connected!");
}


void displayText(String msg, String msg2 = "")
{
  display.setRotation(3);
  display.setFont(&FreeMonoBold24pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.fillScreen(GxEPD_WHITE);
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.setCursor(20, 65);
    display.print(msg);
    display.setFont(&FreeMonoBold12pt7b);
    display.print(msg2);
  }
  while (display.nextPage());
}


void refreshData()
{
  String dspText = "";
  String url = "http://192.168.1.180:8090";
  WiFiClient client;
  HTTPClient http;
  client.connect("http://192.168.1.180", 8090);
  http.begin(client, url);
  int httpCode = http.GET();
  if (httpCode > 0){
    DynamicJsonDocument jsonDoc(2048);
    deserializeJson(jsonDoc, http.getString());
    String msg1 = jsonToStr(jsonDoc, false);
    String msg2 = jsonToStr(jsonDoc, true);
    displayText(msg1, msg2);
    http.end();
  }
  else{
    displayText("HTTP request error:" + String(httpCode));
  }
}


String jsonToStr(DynamicJsonDocument d, bool sub)
{
  String result = "";

  int height = d["blocks"].as<int>();
  int price = d["price"].as<int>();
  // int satsPerDlr = int(100E6/price);
  int fee = d["fee"].as<int>();
  int hrate = int(d["hashrate"].as<float>()/pow(10, 18)); // exa=10^18
  // int diff = int(d["difficulty"].as<float>()/pow(10, 12));  // tera=10^12
  // int nodeSize = int(d["node_size"].as<float>()/(pow(1024, 3)));

  result = "#"+ fmtNum(height) + "\n " 
           + "$" + fmtNum(price);
  
  if (sub){
    String lbrk = "\n ";
    String brk = "  ";
    result = "\n" + lbrk
          + fmtNum(fee) + " s/vB" + brk
          + fmtNum(hrate) + " EH/s" ;
  }
           
  return result;
}


String fmtNum(int num){ 
  String result = "";
  String strNum = String(num);
  int startIdx = strNum.length()-1;
  int counter = 0;
  for(int i=startIdx;i>=0;i--){
    if (i != startIdx && counter==3){
      result = strNum.charAt(i)+ String(",") + result;
      counter = 1;
    }
    else{
      result = strNum.charAt(i) + result;
      counter++;
    }
  }
  return result;
}


void loop() 
{
  // connectWifi();
  refreshData();
  
  // WiFi.disconnect();
  // WiFi.mode(WIFI_OFF);
  display.hibernate();

  delay(REFRESH_DELAY * 60 * 1000); 
}
