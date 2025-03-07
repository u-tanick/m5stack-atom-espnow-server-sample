// ==================================
// 全体共通のヘッダファイルのinclude
#include <Arduino.h>                         // Arduinoフレームワークを使用する場合は必ず必要
#include <Update.h>                          // 定義しないとエラーが出るため追加。
#include <Ticker.h>                          // 定義しないとエラーが出るため追加。
#include <M5Unified.h>                       // M5Unifiedライブラリ
// ================================== End

#define ARDUINO_M5STACK_ATOM
// #define ARDUINO_M5STACK_ATOMS3

// ==================================
// for LED
#ifdef ARDUINO_M5STACK_ATOM
  #include <FastLED.h>
  #define ATOM_LED_PIN  27      // ATOM Lite本体のLED用
  #define ATOM_NUM_LEDS 1       // Atom LED
  static CRGB atom_leds[ATOM_NUM_LEDS];
 
  // Atom本体のLEDを光らせる用の関数
  void setLed(CRGB color)
  {
    // change RGB to GRB
    uint8_t t = color.r;
    color.r = color.g;
    color.g = t;
    atom_leds[0] = color;
    FastLED.show();
  }
  
#endif
// ================================== End

// ==================================
// for ESPNow
#include <WiFi.h>  // ESPNOWを使う場合はWiFiも必要
#include <esp_now.h> // ESPNOW本体

// ESP-NOW受信時に呼ばれる関数
void OnDataReceived(const uint8_t *mac_addr, const uint8_t *data, int data_len) {

  char macStr[18];
  snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);

  Serial.print("Last Packet Recv from: "); Serial.println(macStr);
  Serial.print("Last Packet Recv Data: "); Serial.println(*data);

  #ifdef ARDUINO_M5STACK_ATOM
    if (data[0] == 1) {
      setLed(CRGB::Blue);
      delay(2000);
      setLed(CRGB::Black);
    } else {
      setLed(CRGB::Pink);
      delay(2000);
      setLed(CRGB::Black);
    }
  #endif

  #ifdef ARDUINO_M5STACK_ATOMS3
    if (data[0] == 1) {
      M5.Lcd.fillScreen(TFT_BLUE);  // 背景色変更
    } else {
      M5.Lcd.fillScreen(TFT_PINK);  // 背景色変更
    }
  #endif

}
// ================================== End

void setup() {
  // 設定用の情報を抽出
  auto cfg = M5.config();
  #ifdef ARDUINO_M5STACK_ATOMS3
    // Groveポートの出力をしない（m5atomS3用）
    cfg.output_power = true;
  #endif
  // M5Stackをcfgの設定で初期化
  M5.begin(cfg);

  #ifdef ARDUINO_M5STACK_ATOMS3
    // 液晶初期化
    M5.Lcd.init();                         // 初期化
    M5.Lcd.setTextWrap(false);             // テキストが画面からはみ出した時の折り返し無し
    M5.Lcd.setTextColor(TFT_WHITE);        // 文字色
    M5.Lcd.fillScreen(TFT_RED);
  #endif

  #ifdef ARDUINO_M5STACK_ATOM
    FastLED.addLeds<WS2811, ATOM_LED_PIN, RGB>(atom_leds, ATOM_NUM_LEDS);
    FastLED.setBrightness(5);
    for (int i = 0; i < 3; i++)
    {
      setLed(CRGB::Red);
      delay(500);
      setLed(CRGB::Black);
      delay(500);
    }
  #endif

  // WiFi初期化
  WiFi.mode(WIFI_STA);

  // ESP-NOWの初期化(出来なければリセットして繰り返し)
  if (esp_now_init() != ESP_OK) {
    return;
  }

  // ESP-NOW受信時に呼ばれる関数の登録
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataReceived));
}

void loop() {
}