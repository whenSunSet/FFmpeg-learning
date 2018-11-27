//
// Created by 何时夕 on 2018/11/10.
//

#ifndef FFMPEGLEARNING_FFMPEG_SAMPLE_ONE_H
#define FFMPEGLEARNING_FFMPEG_SAMPLE_ONE_H
int av_io_reading(char *argv[]);
int decode_video(char **argv);
int encode_video(char **argv);
int filter_video(char **argv, JNIEnv *env, jobject instance);
#endif //FFMPEGLEARNING_FFMPEG_SAMPLE_ONE_H
