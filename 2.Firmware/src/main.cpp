/*
 * @Author: Your Name email@example.com
 * @Date: 2022-05-16 21:57:44
 * @LastEditors: Your Name email@example.com
 * @LastEditTime: 2022-08-26 15:46:38
 * @FilePath: \ESP32_PICO_YuanShen\src\main.cpp
 * 因为MPU6050出问题，所以将程序简化
 */
#include "common.h"
#include "driver/lv_port_fatfs.h"
#include "driver/lv_port_indev.h"
#include "sys/app_controller.h"
#include "app/bilibili_fans/bilibili.h"
#include "app/file_manager/file_manager.h"
#include "app/game_2048/game_2048.h"
#include "app/idea_anim/idea.h"
#include "app/media_player/media_player.h"
#include "app/picture/picture.h"
#include "app/screen_share/screen_share.h"
#include "app/server/server.h"
#include "app/settings/settings.h"
#include "app/shenzhiyan/shenzhiyan.h"
#include "app/weather/weather.h"
#include "app/weather_old/weather_old.h"

#include <SPIFFS.h>
#include <esp32-hal.h>

#define TFT_GREY 0x5AEB // New colour

/*** Component objects **7*/
ImuAction* act_info; // 存放mpu6050返回的数据
AppController* app_controller; // APP控制器

TaskHandle_t handleTaskLvgl;

void TaskLvglUpdate(void* parameter)
{
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    for (;;) {
        screen.routine();
        // lv_task_handler();

        delay(5);
    }
}

void setup()
{
    Serial.begin(115200);
    Serial.println("yuan shen v2.0\n");

    Serial.println(F("\nYUANSHEN (All in one) version " AIO_VERSION "\n"));
    // MAC ID可用作芯片唯一标识
    Serial.print(F("ChipID(EfuseMac): "));
    Serial.println(ESP.getEfuseMac());
    // flash运行模式
    // Serial.print(F("FlashChipMode: "));
    // Serial.println(ESP.getFlashChipMode());
    // Serial.println(F("FlashChipMode value: FM_QIO = 0, FM_QOUT = 1, FM_DIO = 2, FM_DOUT = 3, FM_FAST_READ = 4, FM_SLOW_READ = 5, FM_UNKNOWN = 255"));

    app_controller = new AppController(); // APP控制器

    // 需要放在Setup里初始化
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed");
        return;
    }

    // config_read(NULL, &g_cfg);   // 旧的配置文件读取方式
    app_controller->read_config(&app_controller->sys_cfg);
    app_controller->read_config(&app_controller->mpu_cfg);
    app_controller->read_config(&app_controller->rgb_cfg);

    /*** Init screen ***/
    screen.init(app_controller->sys_cfg.rotation,
        app_controller->sys_cfg.backLight);
    // Serial.printf("rotation=%d\n", app_controller->sys_cfg.rotation);

    /*** Init micro SD-Card ***/
    tf.init(); 
    lv_fs_if_init();

    app_controller->init();
    // 将APP"安装"到controller里
    app_controller->app_install(&shenzhiyan_app);
    app_controller->app_install(&picture_app);
    // app_controller->app_install(&screen_share_app);
    app_controller->app_install(&file_manager_app);
    app_controller->app_install(&server_app);
    app_controller->app_install(&idea_app);
    app_controller->app_install(&bilibili_app);
    app_controller->app_install(&settings_app);
    // app_controller->app_install(&game_2048_app);
    app_controller->app_install(&weather_app);
    // app_controller->app_install(&weather_old_app);

    // 优先显示屏幕 加快视觉上的开机时间
    LV_IMG_DECLARE(start);
    lv_obj_t* img1 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img1, &start);
    lv_obj_align(img1, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_size(img1, 240, 240);
    screen.routine();

    /*** Init IMU as input device ***/
    lv_port_indev_init();
    mpu.init(app_controller->sys_cfg.mpu_order,
        app_controller->sys_cfg.auto_calibration_mpu,
        &app_controller->mpu_cfg); // 初始化比较耗时

    lv_obj_del(img1);//关闭初始图像
}

void loop()
{
    screen.routine();
    act_info = mpu.update(200);
    app_controller->main_process(act_info); // 运行当前进程

    // put your main code here, to run repeatedly:
}
