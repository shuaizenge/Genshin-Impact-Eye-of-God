#include "common.h"


IMU mpu;
SdCard tf;
Pixel rgb;
// Config g_cfg;       // 全局配置文件
Network g_network;  // 网络连接
FlashFS g_flashCfg; // flash中的文件系统（替代原先的Preferences）
Display screen;     // 屏幕对象
Ambient ambLight;   // 光纤传感器对象

boolean doDelayMillisTime(unsigned long interval, unsigned long *previousMillis, boolean state)
{
    unsigned long currentMillis = millis();
    if (currentMillis - *previousMillis >= interval)
    {
        *previousMillis = currentMillis;
        state = !state;
    }
    return state;
}

#if GFX

#include <Arduino_GFX_Library.h>

Arduino_DataBus* bus = new Arduino_ESP32SPI(27 /* DC */, 5 /* CS */, 14 /* SCK */, 15 /* MOSI */, -1 /* MISO */);
Arduino_GFX* gfx = new Arduino_GC9A01(bus, 33 /* RST */, 0, true);

#else
#include <TFT_eSPI.h>
/*
TFT pins should be set in path/to/Arduino/libraries/TFT_eSPI/User_Setups/Setup24_ST7789.h
*/
TFT_eSPI *tft = new TFT_eSPI(240, 240);
#endif