//
// Created by 何时夕 on 2018/11/10.
//

#ifndef FFMPEGLEARNING_FFMPEG_SAMPLE_ONE_H
#define FFMPEGLEARNING_FFMPEG_SAMPLE_ONE_H
char *av_io_reading(char *argv[]);
char *decode_video(char **argv);
char *encode_video(char **argv);
int filter_video(char **argv, JNIEnv *env, jobject instance);
char *demuxing_decoding(char **argv);
int play(JNIEnv *env, jobject surface);
#endif //FFMPEGLEARNING_FFMPEG_SAMPLE_ONE_H
