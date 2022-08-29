#include "display.h"
#include "network.h"
#include "lv_port_indev.h"
#include "lv_demo_encoder.h"
#include "common.h"

#define LV_HOR_RES_MAX_LEN 10 // 24
static lv_disp_buf_t disp_buf;
static lv_color_t buf[LV_HOR_RES_MAX * LV_HOR_RES_MAX_LEN];

void my_print(lv_log_level_t level, const char *file, uint32_t line, const char *fun, const char *dsc)
{
    Serial.printf("%s@%d %s->%s\r\n", file, line, fun, dsc);
    Serial.flush();
}

void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p)
{
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft->startWrite();
    tft->setAddrWindow(area->x1, area->y1, w, h);
    // tft->writePixels(&color_p->full, w * h);
    tft->pushColors(&color_p->full, w * h, true);
    tft->endWrite();
    // Initiate DMA - blocking only if last DMA is not complete
    // tft->pushImageDMA(area->x1, area->y1, w, h, bitmap, &color_p->full);

    lv_disp_flush_ready(disp);
}

void Display::init(uint8_t rotation, uint8_t backLight)
{
    ledcSetup(LCD_BL_PWM_CHANNEL, 5000, 8);
    ledcAttachPin(LCD_BL_PIN, LCD_BL_PWM_CHANNEL);

    lv_init();

    lv_log_register_print_cb(my_print); /* register print function for debugging */

    setBackLight(0.0);  // 设置亮度 为了先不显示初始化时的"花屏"

    tft->begin(); /* TFT init */
    tft->fillScreen(TFT_BLACK);
    // tft->writecommand(GC9A01_DISPON); //Display on
    tft->writecommand(0x29); //Display on
    
    // tft->fillScreen(BLACK);

    // 尝试读取屏幕数据作为屏幕检测的依旧
    // uint8_t ret = tft->readcommand8(0x01, TFT_MADCTL);
    // Serial.printf("TFT read -> %u\r\n", ret);

    // 以下setRotation函数是经过更改的第4位兼容原版 高四位设置镜像
    // 正常方向需要设置为0 如果加上分光棱镜需要镜像改为4 如果是侧显示的需要设置为5
    tft->setRotation(rotation); /* mirror 修改反转，如果加上分光棱镜需要改为4镜像*/

    setBackLight(backLight / 100.0);  // 设置亮度

    lv_disp_buf_init(&disp_buf, buf, NULL, LV_HOR_RES_MAX * LV_HOR_RES_MAX_LEN);

    /*Initialize the display*/
    lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = LV_HOR_RES_MAX;
    disp_drv.ver_res = LV_VER_RES_MAX;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.buffer = &disp_buf;
    // 开启 LV_COLOR_SCREEN_TRANSP 屏幕具有透明和不透明样式
    lv_disp_drv_register(&disp_drv);

}

void Display::routine()
{
    lv_task_handler();
}

void Display::setBackLight(float duty)
{
    duty = constrain(duty, 0, 1);
    duty = 1 - duty;
    ledcWrite(LCD_BL_PWM_CHANNEL, (int)(duty * 255));
}
