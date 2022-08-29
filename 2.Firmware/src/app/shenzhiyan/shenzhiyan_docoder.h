#ifndef PLAYER_H
#define PLAYER_H

#include <SD.h>
#include "app/shenzhiyan/MjpegClass.h"

class PlayDocoderBase
{
public:
    virtual ~PlayDocoderBase(){};
    virtual bool my_video_start(){};
    virtual bool my_video_play_screen(){};
    virtual bool my_video_end(){};
};

class MjpegPlayDocoder_me : public PlayDocoderBase
{
public:
    File *m_pFile;
    static bool m_isUseDMA; // 是否使用DMA
    uint8_t *m_displayBuf;  // 显示的
    int32_t m_bufSaveTail;  // 指向 m_displayBuf 中所保存的最后一个数据所在下标
    uint8_t *m_jpegBuf;     // 用来给 jpeg 图片做缓冲，将此提交给jpeg解码器解码
    bool m_tftSwapStatus;   // 由于jpeg图片解码后需要互换高低位才可以使用tft_espi进行显示
    // 由此保存环境当前的高低位置换，以便退出视频播放的时候还原回去。
    static uint8_t *m_displayBufWithDma[2];
    static bool m_dmaBufferSel;

public:
    MjpegPlayDocoder_me(File *file, bool isUseDMA = false);
    virtual ~MjpegPlayDocoder_me();
    uint32_t readJpegFromFile(File *file);
    // bool static tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t *bitmap);
    int static jpegDrawCallback(JPEGDRAW* pDraw);
    virtual bool my_video_start();
    virtual bool my_video_play_screen();
    virtual bool my_video_end();
};


#endif