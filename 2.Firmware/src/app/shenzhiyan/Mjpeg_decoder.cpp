#include "common.h"
#include "shenzhiyan_docoder.h"
static MjpegClass mjpeg;

#define MJPEG_FILENAME "/240_30fps.mjpeg"
#define MJPEG_BUFFER_SIZE (240 * 240 * 2 / 4)

#define VIDEO_WIDTH 240L
#define VIDEO_HEIGHT 240L
#define EACH_READ_SIZE 2500 // 每次获取的数据流大小
#define JPEG_BUFFER_SIZE 10000 // 储存一张jpeg的图像(240*240 10000大概够了，正常一帧差不多3000)
#define MOVIE_BUFFER_SIZE 20000 // 理论上是JPEG_BUFFER_SIZE的两倍就够了

#define DMA_BUFFER_SIZE 512 // (16*16*2)

// #define TFT_MISO -1
// #define TFT_MOSI 15
// #define TFT_SCLK 14
// #define TFT_CS 5 // Not connected
// #define TFT_DC 27
// #define TFT_RST 33 // Connect reset to ensure display initialises

bool MjpegPlayDocoder_me::m_isUseDMA = 0;
uint8_t* MjpegPlayDocoder_me::m_displayBufWithDma[2];
bool MjpegPlayDocoder_me::m_dmaBufferSel = false;

File mjpegFile;
uint8_t* mjpeg_buf;

// pixel drawing callback
int MjpegPlayDocoder_me::jpegDrawCallback(JPEGDRAW* pDraw)
{
    // tft->pushImage(pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight, pDraw->pPixels);
    if (m_isUseDMA) {
        // Double buffering is used, the bitmap is copied to the buffer by pushImageDMA() the
        // bitmap can then be updated by the jpeg decoder while DMA is in progress
        uint16_t* dmaBufferPtr;
        if (MjpegPlayDocoder_me::m_dmaBufferSel)
            dmaBufferPtr = (uint16_t*)MjpegPlayDocoder_me::m_displayBufWithDma[0];
        else
            dmaBufferPtr = (uint16_t*)MjpegPlayDocoder_me::m_displayBufWithDma[1];
        MjpegPlayDocoder_me::m_dmaBufferSel = !MjpegPlayDocoder_me::m_dmaBufferSel; // Toggle buffer selection
        //  pushImageDMA() will clip the image block at screen boundaries before initiating DMA
        tft->pushImageDMA(pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight, pDraw->pPixels, dmaBufferPtr); // Initiate DMA - blocking only if last DMA is not complete
                                                             // The DMA transfer of image block to the TFT is now in progress...
    } else {
        // Non-DMA blocking alternative
        tft->pushImage(pDraw->x, pDraw->y, pDraw->iWidth, pDraw->iHeight, pDraw->pPixels); // Blocking, so only returns when image block is drawn
    }
    return 1;
}

MjpegPlayDocoder_me::MjpegPlayDocoder_me(File* file, bool isUseDMA)
{
    m_pFile = file;
    m_isUseDMA = isUseDMA;
    m_displayBuf = NULL;
    m_bufSaveTail = 0;
    m_jpegBuf = NULL;
    m_displayBufWithDma[0] = NULL;
    m_displayBufWithDma[1] = NULL;
    m_dmaBufferSel = 0;
    // The colour byte order can be swapped by the decoder
    // using TJpgDec.setSwapBytes(true); or by the TFT_eSPI library:
    // m_tftSwapStatus = tft->getSwapBytes();
    // tft->setSwapBytes(true);

    // TJpgDec.setSwapBytes(true);
    // The decoder must be given the exact name of the rendering function above
    // SketchCallback callback = (SketchCallback)&MjpegPlayDocoder_me::tft_output; //强制转换func()的类型
    // TJpgDec.setCallback(callback);

    my_video_start();
}

MjpegPlayDocoder_me::~MjpegPlayDocoder_me(void)
{
    Serial.println(F("~MjpegPlayDocoder_me"));
    // 释放资源
    my_video_end();
}

bool MjpegPlayDocoder_me::my_video_start()
{
    if (m_isUseDMA) {

        mjpegFile = *m_pFile;
        if (!mjpegFile || mjpegFile.isDirectory()) {
            Serial.println(F("ERROR: Failed to open " MJPEG_FILENAME " file for reading"));
        } else {
            m_displayBuf = (uint8_t*)malloc(MJPEG_BUFFER_SIZE);
            if (!m_displayBuf) {
                Serial.println(F("mjpeg_buf malloc failed!"));
            } else {
                mjpeg.setup(
                    &mjpegFile, m_displayBuf, jpegDrawCallback, true /* useBigEndian */,
                    0 /* x */, 0 /* y */, 240 /* widthLimit */, 240 /* heightLimit */);
                // Read video
                if (mjpegFile.available()) {
                    // Read video
                    mjpeg.readMjpegBuf();

                    // Play video
                    mjpeg.drawJpg();
                }
            }
        }

        // m_displayBuf = (uint8_t*)malloc(MOVIE_BUFFER_SIZE);
        m_jpegBuf = (uint8_t*)malloc(JPEG_BUFFER_SIZE);
        m_displayBufWithDma[0] = (uint8_t*)heap_caps_malloc(DMA_BUFFER_SIZE, MALLOC_CAP_DMA);
        m_displayBufWithDma[1] = (uint8_t*)heap_caps_malloc(DMA_BUFFER_SIZE, MALLOC_CAP_DMA);
        tft->initDMA();
        // 使用DMA
        // DMADrawer::setup(MOVIE_BUFFER_SIZE, SPI_FREQUENCY, TFT_MOSI, TFT_MISO, TFT_SCLK, TFT_CS, TFT_DC);
    } else {
        // tft->setAddrWindow((tft->width() - VIDEO_WIDTH) / 2,
        //     (tft->height() - VIDEO_HEIGHT) / 2,
        //     VIDEO_WIDTH, VIDEO_HEIGHT);
        mjpegFile = *m_pFile;
        if (!mjpegFile || mjpegFile.isDirectory()) {
            Serial.println(F("ERROR: Failed to open " MJPEG_FILENAME " file for reading"));
        } else {
            m_displayBuf = (uint8_t*)malloc(MJPEG_BUFFER_SIZE);
            if (!m_displayBuf) {
                Serial.println(F("mjpeg_buf malloc failed!"));
            } else {
                mjpeg.setup(
                    &mjpegFile, m_displayBuf, jpegDrawCallback, true /* useBigEndian */,
                    0 /* x */, 0 /* y */, 240 /* widthLimit */, 240 /* heightLimit */);
                // Read video
                if (mjpegFile.available()) {
                    // Read video
                    mjpeg.readMjpegBuf();

                    // Play video
                    mjpeg.drawJpg();
                }
            }
        }
    }
    return true;
}

bool MjpegPlayDocoder_me::my_video_play_screen(void)
{

    if (m_isUseDMA) {
        if (mjpegFile.available()) {
            // Read video
            mjpeg.readMjpegBuf();

            // Play video
            mjpeg.drawJpg();

        } else {
            Serial.printf("jpeg error\n");
        }
    } else {
        //播放一帧数据
        if (mjpegFile.available()) {
            // Read video
            mjpeg.readMjpegBuf();

            // Play video
            mjpeg.drawJpg();

        } else {
            Serial.printf("jpeg error\n");
        }
    }
}

bool MjpegPlayDocoder_me::my_video_end(void)
{

    m_pFile = NULL;
    // 结束播放 释放资源
    if (NULL != m_displayBufWithDma[0]) {
        free(m_displayBufWithDma[0]);
        free(m_displayBufWithDma[1]);
        m_displayBufWithDma[0] = NULL;
        m_displayBufWithDma[1] = NULL;
    }
    if (NULL != m_jpegBuf) {
        free(m_jpegBuf);
        m_jpegBuf = NULL;
    }

    if (NULL != m_displayBuf) {
        free(m_displayBuf);
        m_displayBuf = NULL;
    }

    return true;
}