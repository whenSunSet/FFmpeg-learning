//
// Created by 何时夕 on 2018/10/21.
//

#include <jni.h>

//ffmpeg库

//打印日志
#include <android/log.h>
extern "C"
{
#include "libavformat/avformat.h"
}
#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"whensunset",FORMAT,##__VA_ARGS__);

extern "C"
JNIEXPORT void JNICALL
Java_com_example_whensunset_ffmpeg_1learning_FFmpegPlayer_playMyMedia(JNIEnv *env, jobject instance,
                                                                      jstring url_) {
        const char *url = (*env).GetStringUTFChars(url_, 0);

        LOGI("url:%s", url);

        av_register_all();
        AVCodec *c_temp = av_codec_next(NULL);
        while (c_temp != NULL)
        {
            switch (c_temp->type)
            {
                case AVMEDIA_TYPE_VIDEO:
                    LOGI("[Video]:%s", c_temp->name);
                    break;
                case AVMEDIA_TYPE_AUDIO:
                    LOGI("[Audio]:%s", c_temp->name);
                    break;
                default:
                    LOGI("[Other]:%s", c_temp->name);
                    break;
            }
            c_temp = c_temp->next;
        }
        // TODO

        (*env).ReleaseStringUTFChars(url_, url);
}