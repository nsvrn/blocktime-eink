#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#include "Arduino.h"
#include "TFT_eSPI.h" /* Please use the TFT library provided in the library. */
#include "pin_config.h"
#include "config.h"


// Root CA Cert for https://api.blockchair.com/bitcoin/stats
// valid until Mon, 04 Jun 2035 11:04:38 GMT
const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n" \
"MIIFazCCA1OgAwIBAgIRAIIQz7DSQONZRGPgu2OCiwAwDQYJKoZIhvcNAQELBQAw\n" \
"TzELMAkGA1UEBhMCVVMxKTAnBgNVBAoTIEludGVybmV0IFNlY3VyaXR5IFJlc2Vh\n" \
"cmNoIEdyb3VwMRUwEwYDVQQDEwxJU1JHIFJvb3QgWDEwHhcNMTUwNjA0MTEwNDM4\n" \
"WhcNMzUwNjA0MTEwNDM4WjBPMQswCQYDVQQGEwJVUzEpMCcGA1UEChMgSW50ZXJu\n" \
"ZXQgU2VjdXJpdHkgUmVzZWFyY2ggR3JvdXAxFTATBgNVBAMTDElTUkcgUm9vdCBY\n" \
"MTCCAiIwDQYJKoZIhvcNAQEBBQADggIPADCCAgoCggIBAK3oJHP0FDfzm54rVygc\n" \
"h77ct984kIxuPOZXoHj3dcKi/vVqbvYATyjb3miGbESTtrFj/RQSa78f0uoxmyF+\n" \
"0TM8ukj13Xnfs7j/EvEhmkvBioZxaUpmZmyPfjxwv60pIgbz5MDmgK7iS4+3mX6U\n" \
"A5/TR5d8mUgjU+g4rk8Kb4Mu0UlXjIB0ttov0DiNewNwIRt18jA8+o+u3dpjq+sW\n" \
"T8KOEUt+zwvo/7V3LvSye0rgTBIlDHCNAymg4VMk7BPZ7hm/ELNKjD+Jo2FR3qyH\n" \
"B5T0Y3HsLuJvW5iB4YlcNHlsdu87kGJ55tukmi8mxdAQ4Q7e2RCOFvu396j3x+UC\n" \
"B5iPNgiV5+I3lg02dZ77DnKxHZu8A/lJBdiB3QW0KtZB6awBdpUKD9jf1b0SHzUv\n" \
"KBds0pjBqAlkd25HN7rOrFleaJ1/ctaJxQZBKT5ZPt0m9STJEadao0xAH0ahmbWn\n" \
"OlFuhjuefXKnEgV4We0+UXgVCwOPjdAvBbI+e0ocS3MFEvzG6uBQE3xDk3SzynTn\n" \
"jh8BCNAw1FtxNrQHusEwMFxIt4I7mKZ9YIqioymCzLq9gwQbooMDQaHWBfEbwrbw\n" \
"qHyGO0aoSCqI3Haadr8faqU9GY/rOPNk3sgrDQoo//fb4hVC1CLQJ13hef4Y53CI\n" \
"rU7m2Ys6xt0nUW7/vGT1M0NPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNV\n" \
"HRMBAf8EBTADAQH/MB0GA1UdDgQWBBR5tFnme7bl5AFzgAiIyBpY9umbbjANBgkq\n" \
"hkiG9w0BAQsFAAOCAgEAVR9YqbyyqFDQDLHYGmkgJykIrGF1XIpu+ILlaS/V9lZL\n" \
"ubhzEFnTIZd+50xx+7LSYK05qAvqFyFWhfFQDlnrzuBZ6brJFe+GnY+EgPbk6ZGQ\n" \
"3BebYhtF8GaV0nxvwuo77x/Py9auJ/GpsMiu/X1+mvoiBOv/2X/qkSsisRcOj/KK\n" \
"NFtY2PwByVS5uCbMiogziUwthDyC3+6WVwW6LLv3xLfHTjuCvjHIInNzktHCgKQ5\n" \
"ORAzI4JMPJ+GslWYHb4phowim57iaztXOoJwTdwJx4nLCgdNbOhdjsnvzqvHu7Ur\n" \
"TkXWStAmzOVyyghqpZXjFaH3pO3JLF+l+/+sKAIuvtd7u+Nxe5AW0wdeRlN8NwdC\n" \
"jNPElpzVmbUq4JUagEiuTDkHzsxHpFKVK7q4+63SM1N95R1NbdWhscdCb+ZAJzVc\n" \
"oyi3B43njTOQ5yOf+1CceWxG1bQVs5ZufpsMljq4Ui0/1lvh+wjChP4kqKOJ2qxq\n" \
"4RgqsahDYVvTH9w7jXbyLeiNdd8XM2w9U/t7y0Ff/9yi0GE44Za4rF2LN9d11TPA\n" \
"mRGunUHBcnWEvgJBQl9nJEiU0Zsnvgc/ubhPgXRR4Xq37Z0j4r7g1SgEEzwxA57d\n" \
"emyPxgcYxn/eR44/KJ4EBs+lVDR3veyJm+kXQ99b21/+jh5Xos1AnX5iItreGCc=\n" \
"-----END CERTIFICATE-----\n";


/* The product now has two screens, and the initialization code needs a small change in the new version. The LCD_MODULE_CMD_1 is used to define the
 * switch macro. */
#define LCD_MODULE_CMD_1

TFT_eSPI tft = TFT_eSPI();

#if defined(LCD_MODULE_CMD_1)
typedef struct {
    uint8_t cmd;
    uint8_t data[14];
    uint8_t len;
} lcd_cmd_t;

lcd_cmd_t lcd_st7789v[] = {
    {0x11, {0}, 0 | 0x80},
    {0x3A, {0X05}, 1},
    {0xB2, {0X0B, 0X0B, 0X00, 0X33, 0X33}, 5},
    {0xB7, {0X75}, 1},
    {0xBB, {0X28}, 1},
    {0xC0, {0X2C}, 1},
    {0xC2, {0X01}, 1},
    {0xC3, {0X1F}, 1},
    {0xC6, {0X13}, 1},
    {0xD0, {0XA7}, 1},
    {0xD0, {0XA4, 0XA1}, 2},
    {0xD6, {0XA1}, 1},
    {0xE0, {0XF0, 0X05, 0X0A, 0X06, 0X06, 0X03, 0X2B, 0X32, 0X43, 0X36, 0X11, 0X10, 0X2B, 0X32}, 14},
    {0xE1, {0XF0, 0X08, 0X0C, 0X0B, 0X09, 0X24, 0X2B, 0X22, 0X43, 0X38, 0X15, 0X16, 0X2F, 0X37}, 14},
};
#endif


struct {
  int height;
  int price;
  int satsPerDlr;
  int fee;
  int hrate;
  int diff;
  int nodes;
  int nodeSize;
  String dtm;
  String moscowTime;
} stats;


void connectWifi()
{
  WiFi.begin(WIFI_SSID, WIFI_PW);
  delay(500);
  while (WiFi.status() != WL_CONNECTED){}
  Serial.println("Connected!");
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


void displayText(String msg = "", int textSize = 1)
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_GREEN);

    if (msg.isEmpty()){
      // Block Number
      tft.setTextSize(3);
      msg = "$" + fmtNum(stats.price);
      tft.drawString(msg, 12, 4, 4);
      // Price
      tft.setTextColor(TFT_CYAN);
      tft.setTextSize(2);
      msg = "#" + fmtNum(stats.height);
      tft.drawString(msg, 28, 82, 4);

      // Moscow time | Fee
      tft.setTextColor(TFT_WHITE);
      tft.setTextSize(1);
      
      msg = stats.moscowTime  + "  |  " 
            + fmtNum(stats.fee) + " s/vB  |  "
            + fmtNum(stats.hrate) + " EH/s";
      tft.drawString(msg, 0, 140, 4);

    } else {
      tft.setTextSize(textSize);
      tft.drawString(msg, 0, 0, 2);
    }
    delay(2000);
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
    jsonToObj(jsonDoc);
    displayText();
    https.end();
  }
  else{
    displayText("HTTP request error: " + String(httpCode) + ", re-trying in 2 min.....");
    delay(2 * 60 * 1000);
    refreshData();
  }
}

void jsonToObj(DynamicJsonDocument d)
{
  String result = "";
  String brk = "\n  ";

  stats.height = d["data"]["blocks"].as<int>();
  stats.price = d["data"]["market_price_usd"].as<int>();
  stats.satsPerDlr = int(100E6/stats.price);
  stats.fee = d["data"]["suggested_transaction_fee_per_byte_sat"].as<int>();
  stats.hrate = int(d["data"]["hashrate_24h"].as<float>()/pow(10, 18)); // exa=10^18
  stats.diff = int(d["data"]["difficulty"].as<float>()/pow(10, 12));  // tera=10^12
  stats.nodes = d["data"]["nodes"].as<int>();
  stats.nodeSize = int(d["data"]["blockchain_size"].as<float>()/(pow(1024, 3)));
  String spd = String(stats.satsPerDlr);
  stats.moscowTime = spd.substring(0, 2) + ":" + spd.substring(2, spd.length());
  stats.dtm = d["context"]["cache"]["since"].as<String>();
  
  if (stats.dtm.length() == 19){
    stats.dtm.remove(16, 3);
    stats.dtm.replace(" ", brk + "~");
    stats.dtm.replace("-", "/");
  }
}


void setup()
{
    pinMode(PIN_POWER_ON, OUTPUT);
    digitalWrite(PIN_POWER_ON, HIGH);
    Serial.begin(115200);
    tft.begin();

    #if defined(LCD_MODULE_CMD_1)
    
    for (uint8_t i = 0; i < (sizeof(lcd_st7789v) / sizeof(lcd_cmd_t)); i++) {
        tft.writecommand(lcd_st7789v[i].cmd);
        for (int j = 0; j < lcd_st7789v[i].len & 0x7f; j++) {
            tft.writedata(lcd_st7789v[i].data[j]);
        }

        if (lcd_st7789v[i].len & 0x80) {
            delay(120);
        }
    }
    #endif

    tft.setRotation(3);
    tft.setSwapBytes(true);
    delay(2000);

    ledcSetup(0, 2000, 8);
    ledcAttachPin(PIN_LCD_BL, 0);
    ledcWrite(0, 255);

    displayText("Connecting to " + String(WIFI_SSID) + ".......");
    connectWifi();
}

void loop(){
  Serial.println("In the loop!");
  refreshData();   
  delay(REFRESH_DELAY * 60 * 1000);
}

