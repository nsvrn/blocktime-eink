#### ₿locktime (for eink displays)
---

![alt blocktime-eink](blocktime-eink.png)



#### Setup
1. Hardware:
    - TTGO [T5s](https://www.lilygo.cc/products/t5s-2-7inch-e-paper) 2.7in/[T5](https://www.lilygo.cc/products/t5-v2-3-1) 2.13in (or any similar ESP32 based eink can work with some minor code tweaks from these)
    - Lithium-ion battery (OPTIONAL), [link](https://www.amazon.com/dp/B08T6QS58J?psc=1&ref=ppx_yo2ov_dt_b_product_details)
    - 3d printed case (OPTIONAL), [T5s](https://www.thingiverse.com/thing:4664052)/[T5](https://www.thingiverse.com/thing:4055993)(I had to print mine at 102% scale for better accuracy, silicone glue comes handy to glue the battery and seal any lose side of the case)
2. IDE and libraries:
    - [Arduino IDE](https://www.arduino.cc/en/software)
    - Add these URLs under - Preferences > Additional boards manager URLs)
        ```
            https://adafruit.github.io/arduino-board-index/package_adafruit_index.json
            https://dl.espressif.com/dl/package_esp32_index.json
        ```
    - Install these libraries from Arduino IDE (Tools > Manage Libraries): Arduinojson, GxEPD2, Adafruit GFX, Adafruit BusIO
3. Connect, configure and upload:
    - Connect your device with usb and make sure to run `sudo chmod 666 /dev/ttyACM0` (replace ACM0 with your correct device location)
    - Rename `config_sample.cpp` to `config.cpp` and uncomment+edit to add your WiFi SSID/password and refresh frequency
    - Compile and upload, phew!

<br />

---
#### For TFT/LCD and command line display, check this [repo](https://github.com/ns-xvrn/blocktime).
---