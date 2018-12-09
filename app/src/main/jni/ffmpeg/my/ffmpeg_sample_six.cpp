extern "C"
{
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"

};

#include <sys/time.h>
#include <unistd.h>
#include <pthread.h>
static AVFormatContext *pFormatCtx;
static AVCodecContext *pCodecCtx;
static int video_stream_index = -1;
static AVCodec *pCodec;
static int64_t last_pts = AV_NOPTS_VALUE;


static long getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

struct timeval now;
struct timespec outtime;
pthread_cond_t cond;
pthread_mutex_t mutex;
static void sleep(int nHm) {
    gettimeofday(&now, NULL);
    now.tv_usec += 1000 * nHm;
    if (now.tv_usec > 1000000) {
        now.tv_sec += now.tv_usec / 1000000;
        now.tv_usec %= 1000000;
    }

    outtime.tv_sec = now.tv_sec;
    outtime.tv_nsec = now.tv_usec * 1000;

    pthread_cond_timedwait(&cond, &mutex, &outtime);
}

static int open_input_file(const char *filename) {

    int ret;

    // 打开文件，确认文件的封装格式，然后将文件的信息写入 AVFormatContext 中
    if ((ret = avformat_open_input(&pFormatCtx, filename, NULL, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open input file\n");
        return ret;
    }

    // 从 AVFormatContext 中解析文件中的各种流的信息，比如音频流、视频流、字幕流等等
    if ((ret = avformat_find_stream_info(pFormatCtx, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find stream information\n");
        return ret;
    }

    // 找到根据传入参数，找到最适合的数据流，和该数据流的编解码器，这里传入 AVMEDIA_TYPE_VIDEO 表示需要找到视频流
    ret = av_find_best_stream(pFormatCtx, AVMEDIA_TYPE_VIDEO, -1, -1, &pCodec, 0);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot find a video stream in the input file\n");
        return ret;
    }
    // 将找到的视频流，的 index 暂存
    video_stream_index = ret;

    // 根据前面找到的视频流的编解码器，构造编解码器上下文
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (!pCodecCtx)
        return AVERROR(ENOMEM);
    // 使用视频流的信息来编解码器上下文的参数
    avcodec_parameters_to_context(pCodecCtx, pFormatCtx->streams[video_stream_index]->codecpar);

   // 打开编解码器
    if ((ret = avcodec_open2(pCodecCtx, pCodec, NULL)) < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot open video decoder\n");
        return ret;
    }

    return 0;
}

int play(JNIEnv *env, jobject surface) {
    int ret;
    char filepath[] = "/storage/emulated/0/av_test/b.mp4";

    // 初始化 libavformat 然后 注册所有的 封装器，解封装器 和 协议。
    av_register_all();
    if (open_input_file(filepath) < 0) {
        av_log(NULL, AV_LOG_ERROR, "can not open file");
        return 0;
    }

    // 初始化两个 储存解码后视频帧 的数据结构，pFrame 表示解码后的视频帧，pFrameRGBA 表示将 pFrame 转换成 RGBA 格式的 视频帧
    AVFrame *pFrame = av_frame_alloc();
    AVFrame *pFrameRGBA = av_frame_alloc();

    // 计算格式为 RGBA 的视频帧的 byte 大小，视频帧的长和宽在解封装的时候就确定了
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, pCodecCtx->width, pCodecCtx->height, 1);
    // 初始化一块内存，内存大小就是 格式为 RGBA 的视频帧的大小
    uint8_t *buffer = (uint8_t *) av_malloc(numBytes * sizeof(uint8_t));
    // 填充 buffer
    av_image_fill_arrays(pFrameRGBA->data, pFrameRGBA->linesize, buffer, AV_PIX_FMT_RGBA,
                         pCodecCtx->width, pCodecCtx->height, 1);

    // 由于解码出来的帧格式不是RGBA的,在渲染之前需要进行格式转换
    struct SwsContext *sws_ctx = sws_getContext(pCodecCtx->width,
                                                pCodecCtx->height,
                                                pCodecCtx->pix_fmt,
                                                pCodecCtx->width,
                                                pCodecCtx->height,
                                                AV_PIX_FMT_RGBA,
                                                SWS_BILINEAR,
                                                NULL,
                                                NULL,
                                                NULL);

    // 获取native window，即surface
    ANativeWindow *nativeWindow = ANativeWindow_fromSurface(env, surface);

    // 获取视频宽高
    int videoWidth = pCodecCtx->width;
    int videoHeight = pCodecCtx->height;

    // 设置native window的buffer大小,可自动拉伸
    ANativeWindow_setBuffersGeometry(nativeWindow, videoWidth, videoHeight,
                                     WINDOW_FORMAT_RGBA_8888);
    ANativeWindow_Buffer windowBuffer;

    av_dump_format(pFormatCtx, 0, filepath, 0);

    // 初始化 压缩视频帧 的数据结构
    AVPacket *packet = (AVPacket *) av_malloc(sizeof(AVPacket));

    while (1) {
        long start_time = getCurrentTime();

        // 从视频流中读取出一帧 压缩帧
        if ((ret = av_read_frame(pFormatCtx, packet)) < 0) {
            av_log(NULL, AV_LOG_DEBUG, "can not read frame");
            break;
        }

        // 如果 压缩帧 是从是 视频流中读出来的，那么就可以被解码
        if (packet->stream_index == video_stream_index) {
            // 解码
            ret = avcodec_send_packet(pCodecCtx, packet);
            if (ret < 0) {
                av_log(NULL, AV_LOG_ERROR, "Error while sending a packet to the decoder\n");
                break;
            }

            while (ret >= 0) {
                // 解码
                ret = avcodec_receive_frame(pCodecCtx, pFrame);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                } else if (ret < 0) {
                    av_log(NULL, AV_LOG_ERROR,
                           "Error while receiving a frame from the decoder\n");
                }

                ANativeWindow_lock(nativeWindow, &windowBuffer, 0);

                // 将 YUV 格式的数据转换为 RGBA 格式的数据
                sws_scale(sws_ctx, (uint8_t const *const *) pFrame->data,
                          pFrame->linesize, 0, pCodecCtx->height,
                          pFrameRGBA->data, pFrameRGBA->linesize);

                // 获取stride
                uint8_t *dst = (uint8_t *) windowBuffer.bits;
                int dstStride = windowBuffer.stride * 4;
                uint8_t *src = pFrameRGBA->data[0];
                int srcStride = pFrameRGBA->linesize[0];

                // 由于window的stride和帧的stride不同,因此需要逐行复制，逐行将图像帧的数据拷贝到 Surface 的缓冲流中。
                int h;
                for (h = 0; h < videoHeight; h++) {
                    memcpy(dst + h * dstStride, src + h * srcStride, srcStride);
                }

                // 为了保持 40毫秒一帧，如果解码时间很快，那么就 sleep一会儿
                int sleep_time = 40 - (getCurrentTime() - start_time);
                if (sleep_time > 0) {
                    sleep(sleep_time);
                }

                ANativeWindow_unlockAndPost(nativeWindow);
            }
        }

        av_packet_unref(packet);
    }

    if (sws_ctx) sws_freeContext(sws_ctx);
    av_frame_free(&pFrameRGBA);
    if (pFrame) av_frame_free(&pFrame);
    if (pCodecCtx) avcodec_close(pCodecCtx);
    if (pFormatCtx) avformat_close_input(&pFormatCtx);
    if (buffer) av_free(buffer);
    return 0;
}

