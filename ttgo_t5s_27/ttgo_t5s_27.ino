#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <config.h>

#include <GxEPD2_BW.h>
#include <GxEPD2_3C.h>
#include <GxEPD2_7C.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include "GxEPD2_display_selection_new_style.h"
#include "GxEPD2_display_selection.h"
#include "GxEPD2_display_selection_added.h"


// Root CA Cert for https://api.blockchair.com/bitcoin/stats
// valid until Mon, 04 Jun 2035 11:04:38 GMT
const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"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\n" \
"-----END CERTIFICATE-----\n";



void setup()
{
  display.init(115200, true, 2, false);
}


void connectWifi()
{
  WiFi.begin(WIFI_SSID, WIFI_PW);
  delay(500);
  while (WiFi.status() != WL_CONNECTED){}
  Serial.println("Connected!");
}


void displayText(String msg)
{
  display.setRotation(2);
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.fillScreen(GxEPD_WHITE);
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.setCursor(40, 40);
    display.print(msg);
  }
  while (display.nextPage());
}


void refreshData()
{
  String dspText = "";
  String url = "https://api.blockchair.com/bitcoin/stats";
  WiFiClientSecure client;
  HTTPClient https;
  client.connect("https://api.blockchair.com/bitcoin/stats", 443);
  client.setCACert(rootCACertificate);
  https.begin(client, url);
  int httpCode = https.GET();
  if (httpCode > 0){
    DynamicJsonDocument jsonDoc(2048);
    deserializeJson(jsonDoc, https.getString());
    displayText(jsonToStr(jsonDoc));
    https.end();
  }
  else{
    displayText("HTTP request error:" + String(httpCode));
  }
}


String jsonToStr(DynamicJsonDocument d)
{
  String result = "";
  String brk = "\n  ";

  int height = d["data"]["blocks"].as<int>();
  int price = d["data"]["market_price_usd"].as<int>();
  int satsPerDlr = int(100E6/price);
  int fee = d["data"]["suggested_transaction_fee_per_byte_sat"].as<int>();
  int hrate = int(d["data"]["hashrate_24h"].as<float>()/pow(10, 18)); // exa=10^18
  int diff = int(d["data"]["difficulty"].as<float>()/pow(10, 12));  // tera=10^12
  int nodes = d["data"]["nodes"].as<int>();
  int nodeSize = int(d["data"]["blockchain_size"].as<float>()/(pow(1024, 3)));
    
  String dtm = d["context"]["cache"]["since"].as<String>();
  
  if (dtm.length() == 19){
    dtm.remove(16, 3);
    dtm.replace(" ", brk + "~");
    dtm.replace("-", "/");
  }

  result = "#"+ fmtNum(height) + "\n " 
           + "--------------" + brk 
           + "$" + fmtNum(price) + brk
           + fmtNum(satsPerDlr) + " s/$" + brk
           + fmtNum(fee) + " s/vB" + brk
           + fmtNum(hrate) + " EH/s" + brk
           + fmtNum(diff) + "T nonces" + brk
           + fmtNum(nodes) + " nodes" + brk
           + fmtNum(nodeSize) + " GB" + brk
           + dtm + " UTC";
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
  connectWifi();
  refreshData();
  
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  display.hibernate();

  delay(REFRESH_DELAY * 60 * 1000); 
}
