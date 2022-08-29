#ifndef COMMON_H
#define COMMON_H

#define AIO_VERSION "2.0.0"

#define LCD_BL_PWM_CHANNEL 0

#include "Arduino.h"
#include "driver/rgb_led.h"
#include "driver/flash_fs.h"
#include "driver/sd_card.h"
#include "driver/display.h"
#include "driver/ambient.h"
#include "driver/imu.h"
#include "network.h"

// MUP6050
#define IMU_I2C_SDA 18
#define IMU_I2C_SCL 23

// //MJPEG
// #include "driver/MjpegClass.h"
// static MjpegClass mjpeg;

// #define MJPEG_FILENAME "/240_30fps.mjpeg"
// #define MJPEG_BUFFER_SIZE (240 * 240 * 2 / 4)


extern IMU mpu; // 原则上只提供给主程序调用
extern SdCard tf;
extern Pixel rgb;
// extern Config g_cfg;       // 全局配置文件
extern Network g_network;  // 网络连接
extern FlashFS g_flashCfg; // flash中的文件系统（替代原先的Preferences）
extern Display screen;     // 屏幕对象
extern Ambient ambLight;   // 光纤传感器对象

boolean doDelayMillisTime(unsigned long interval,
                          unsigned long *previousMillis,
                          boolean state);

// 光感 (与MPU6050一致)
#define AMB_I2C_SDA 18
#define AMB_I2C_SCL 23

// TFT屏幕接口
// #define PEAK
#ifdef PEAK
#define LCD_BL_PIN 12
/* Battery */
#define CONFIG_BAT_DET_PIN 37
#define CONFIG_BAT_CHG_DET_PIN 38
/* Power */
#define CONFIG_POWER_EN_PIN 21
#define CONFIG_ENCODER_PUSH_PIN 27
#else
#define LCD_BL_PIN 22
#endif

struct SysUtilConfig
{
    String ssid_0;
    String password_0;
    String ssid_1;
    String password_1;
    String ssid_2;
    String password_2;
    uint8_t power_mode;           // 功耗模式（0为节能模式 1为性能模式）
    uint8_t backLight;            // 屏幕亮度（1-100）
    uint8_t rotation;             // 屏幕旋转方向
    uint8_t auto_calibration_mpu; // 是否自动校准陀螺仪 0关闭自动校准 1打开自动校准
    uint8_t mpu_order;            // 操作方向
};

#define GFX 0

#if GFX
#define MISO 2
#define SCK 14 // SCL
#define MOSI 15 // SDA
#define SD_CS 13
#define TFT_CS 5
#define TFT_BLK 22
#define TFT_DC 27
#define TFT_RST 33

#define GFX_BL 22
#include <Arduino_GFX_Library.h>
extern Arduino_DataBus* bus ;
extern Arduino_GFX* gfx ;

#else
#include <TFT_eSPI.h>
/*
TFT pins should be set in path/to/Arduino/libraries/TFT_eSPI/User_Setups/Setup24_ST7789.h
*/
extern TFT_eSPI *tft;
#endif

#endif

