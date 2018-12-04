//
// Created by 何时夕 on 2018/10/21.
//

#include <jni.h>
#include "ffmpeg_sample.h"
extern "C"
{
#include "libavfilter/avfilter.h"
#include "libavformat/avformat.h"
}

#define LOGI(FORMAT,...) __android_log_print(ANDROID_LOG_INFO,"whensunset",FORMAT,##__VA_ARGS__);

#ifdef ANDROID

#include <android/log.h>

#ifndef LOG_TAG
#define  LOG_TAG    "FFMPEG"
#endif

#define  XLOGD(...)  __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define  XLOGE(...)  __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)

#else
#include <stdio.h>
#define XLOGE(format, ...)  fprintf(stdout, LOG_TAG ": " format "\n", ##__VA_ARGS__)
#define XLOGI(format, ...)  fprintf(stderr, LOG_TAG ": " format "\n", ##__VA_ARGS__)
#endif  //ANDROID

static void log_callback_null(void *ptr, int level, const char *fmt, va_list vl)
{
    static int print_prefix = 1;
    static int count;
    static char prev[1024];
    char line[1024];
    static int is_atty;

    av_log_format_line(ptr, level, fmt, vl, line, sizeof(line), &print_prefix);

    strcpy(prev, line);
    //sanitize((uint8_t *)line);

    if (level <= AV_LOG_WARNING)
    {
        XLOGE("%s", line);
    }
    else
    {
        XLOGD("%s", line);
    }
}


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
}extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_whensunset_ffmpeg_1learning_FFmpegPlayer_urlProtocolInfo(JNIEnv *env,
                                                                          jobject instance) {

    char info[40000]={0};
    av_register_all();

    struct URLProtocol *pup = NULL;
    //Input
    struct URLProtocol **p_temp = &pup;
    avio_enum_protocols((void **)p_temp, 0);
    while ((*p_temp) != NULL){
        sprintf(info, "%s[In ][%10s]\n", info, avio_enum_protocols((void **)p_temp, 0));
    }
    pup = NULL;
    //Output
    avio_enum_protocols((void **)p_temp, 1);
    while ((*p_temp) != NULL){
        sprintf(info, "%s[Out][%10s]\n", info, avio_enum_protocols((void **)p_temp, 1));
    }

    return (*env).NewStringUTF(info);
}extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_whensunset_ffmpeg_1learning_FFmpegPlayer_avFormatInfo(JNIEnv *env,
                                                                       jobject instance) {

    char info[40000] = { 0 };

    av_register_all();

    AVInputFormat *if_temp = av_iformat_next(NULL);
    AVOutputFormat *of_temp = av_oformat_next(NULL);
    //Input
    while(if_temp!=NULL){
        sprintf(info, "%s[In ][%10s]\n", info, if_temp->name);
        if_temp=if_temp->next;
    }
    //Output
    while (of_temp != NULL){
        sprintf(info, "%s[Out][%10s]\n", info, of_temp->name);
        of_temp = of_temp->next;
    }

    return env->NewStringUTF(info);
}extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_whensunset_ffmpeg_1learning_FFmpegPlayer_avCodecInfo(JNIEnv *env,
                                                                      jobject instance) {

    char info[40000] = { 0 };

    av_register_all();

    AVCodec* c_temp = av_codec_next(NULL);

    while(c_temp!=NULL){
        if (c_temp->decode!=NULL){
            sprintf(info, "%s[Dec]", info);
        }
        else{
            sprintf(info, "%s[Enc]", info);
        }
        switch (c_temp->type){
            case AVMEDIA_TYPE_VIDEO:
                sprintf(info, "%s[Video]", info);
                break;
            case AVMEDIA_TYPE_AUDIO:
                sprintf(info, "%s[Audio]", info);
                break;
            default:
                sprintf(info, "%s[Other]", info);
                break;
        }
        sprintf(info, "%s[%10s]\n", info, c_temp->name);


        c_temp=c_temp->next;
    }
    //LOGE("%s", info);
    return env->NewStringUTF(info);
}extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_whensunset_ffmpeg_1learning_FFmpegPlayer_avFilterInfo(JNIEnv *env,
                                                                       jobject instance) {
    char info[40000] = { 0 };
    avfilter_register_all();
    AVFilter *f_temp = (AVFilter *) avfilter_next(NULL);
    while (f_temp != NULL) {
        sprintf(info, "%s[%10s]\n", info, f_temp->name);
        f_temp = (AVFilter *) avfilter_next(f_temp);
    }
    return env->NewStringUTF(info);
}extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_whensunset_ffmpeg_1learning_FFmpegPlayer_configurationInfo(JNIEnv *env,
                                                                            jobject instance) {

    char info[10000] = { 0 };
    av_register_all();

    sprintf(info, "%s\n", avcodec_configuration());

    return env->NewStringUTF(info);
}extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_whensunset_ffmpeg_1learning_FFmpegPlayer_ffmpegSampleOne(JNIEnv *env,
                                                                          jobject instance,
                                                                          jstring inputUrl_) {
    const char *inputUrl = env->GetStringUTFChars(inputUrl_, 0);
    char *avgr[1] = {(char *) inputUrl};

    char *return_string = av_io_reading(avgr);

    return env->NewStringUTF(return_string);
}extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_whensunset_ffmpeg_1learning_FFmpegPlayer_ffmpegSampleTwo(JNIEnv *env,
                                                                          jobject instance,
                                                                          jstring inputUrl_,
                                                                          jstring outputUrl_) {
    const char *inputUrl = env->GetStringUTFChars(inputUrl_, 0);
    const char *outputUrl = env->GetStringUTFChars(outputUrl_, 0);

    char *avgr[2] = {(char *) inputUrl, (char *) outputUrl};

    char *return_string = decode_video(avgr);

    env->ReleaseStringUTFChars(inputUrl_, inputUrl);
    env->ReleaseStringUTFChars(outputUrl_, outputUrl);

    return env->NewStringUTF(return_string);
}extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_whensunset_ffmpeg_1learning_FFmpegPlayer_ffmpegSampleThree(JNIEnv *env,
                                                                            jobject instance,
                                                                            jstring inputUrl_) {
    const char *inputUrl = env->GetStringUTFChars(inputUrl_, 0);

    char *avgr[1] = {(char *) inputUrl};

    char *return_string = encode_video(avgr);

    env->ReleaseStringUTFChars(inputUrl_, inputUrl);

    return env->NewStringUTF(return_string);
}

extern "C"
JNIEXPORT jint JNICALL
Java_com_example_whensunset_ffmpeg_1learning_FFmpegPlayer_ffmpegSampleFour(JNIEnv *env,
                                                                           jobject instance,
                                                                           jstring inputUrl_) {


    const char *inputUrl = env->GetStringUTFChars(inputUrl_, 0);

    char *avgr[1] = {(char *) inputUrl};

    filter_video(avgr, env, instance);

    env->ReleaseStringUTFChars(inputUrl_, inputUrl);
    return 0;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_whensunset_ffmpeg_1learning_FFmpegPlayer_initFfmpegLog(JNIEnv *env,
                                                                        jobject instance) {
    av_log_set_callback(log_callback_null);
}extern "C"
JNIEXPORT jstring JNICALL
Java_com_example_whensunset_ffmpeg_1learning_FFmpegPlayer_ffmpegSampleFive(JNIEnv *env,
                                                                           jobject instance,
                                                                           jstring inputFile_,
                                                                           jstring destVideoFile_,
                                                                           jstring destAudioFile_) {
    const char *inputFile = env->GetStringUTFChars(inputFile_, 0);
    const char *destVideoFile = env->GetStringUTFChars(destVideoFile_, 0);
    const char *destAudioFile = env->GetStringUTFChars(destAudioFile_, 0);


    char *avgr[4] = {(char *) inputFile, (char *) destVideoFile, (char *) destAudioFile};

    char *return_string = demuxing_decoding(avgr);

    env->ReleaseStringUTFChars(inputFile_, inputFile);
    env->ReleaseStringUTFChars(destVideoFile_, destVideoFile);
    env->ReleaseStringUTFChars(destAudioFile_, destAudioFile);

    return env->NewStringUTF(return_string);
}extern "C"
JNIEXPORT jint JNICALL
Java_com_example_whensunset_ffmpeg_1learning_FFmpegPlayer_play(JNIEnv *env, jclass type,
                                                               jobject surface) {
    play(env, surface);
    return 0;
}