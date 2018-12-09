/*
 * Copyright (c) 2001 Fabrice Bellard
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @file
 * video decoding with libavcodec API example
 *
 * @example decode_video.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern "C" {
#include "libavcodec/avcodec.h"
}

#define INBUF_SIZE 4096

static void pgm_save(unsigned char *buf, int wrap, int xsize, int ysize,
                     const char *filename) {
    FILE *f;
    int i;

    f = fopen(filename, "w");
    fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);
    for (i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);
    fclose(f);
}

static int decode(AVCodecContext *dec_ctx, AVFrame *frame, AVPacket *pkt,
                  const char *filename) {
    char buf[1024];
    int ret;

    // 将一帧压缩图像传入解码器中
    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        return ret;
    }

    while (ret >= 0) {
        // 从解码器中取出刚刚传入的压缩图像被解码出来的图像，avcodec_send_packet 和 avcodec_receive_frame 一般是对应的。取出数据成功后，再去取时 ret 会小于0
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret < 0) {
            return ret;
        }

        av_log(NULL, AV_LOG_DEBUG, "saving frame %3d\n", dec_ctx->frame_number);
        fflush(stdout);

        /* the picture is allocated by the decoder. no need to
           free it */
        snprintf(buf, sizeof(buf), "%s-%d", filename, dec_ctx->frame_number);
        // ........**
        // ........**
        // ........**
        // ........**
        // ........**
        // ........**
        // ........**
        // 如上所示，点就是我们平时看见的一帧图像，*是无用数据。一般来说：width指的是一行点的数量，height指的是以列点的数量，linesize[0]指的是 width + *的数量。
        // data[0]中存放数据的方式则是这样：........**........**........**........**........**........**........**将一帧图像平铺。
        // 最终我们存到文件中的数据就是这样：........ ........ ........ ........ ........ ........ ........ 中间的空格文件中不存在，只是为了好看一点
        pgm_save(frame->data[0], frame->linesize[0],
                 frame->width, frame->height, filename);
    }
    return 0;
}

char *decode_video(char **argv) {
    const char *filename, *outfilename;
    const AVCodec *codec;
    AVCodecParserContext *parser;
    AVCodecContext *c = NULL;
    FILE *f;
    AVFrame *frame;
    uint8_t inbuf[INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE];
    uint8_t *data;
    size_t data_size;
    int ret;
    AVPacket *pkt;

    // 输入和输出文件的名称，输入文件是 c.mpeg4，输出文件是 c.yuv。
    filename = argv[0];
    outfilename = argv[1];


    // 注册所有的编解码器
    avcodec_register_all();

    // 为 AVPacket 进行初始化，AVPacket 用于一帧压缩后的图像的数据结构
    pkt = av_packet_alloc();
    if (!pkt)
        exit(1);

    // 将 inbuf 从 INBUF_SIZE 到INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE 这一段的数据都设置为0(这确保了对损坏的MPEG流不会发生过读)
    /* set end of buffer to 0 (this ensures that no overreading happens for damaged MPEG streams) */
    memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);

    // 根据名称来查找某个编解码器，这里我们使用输入文件的编解码器 mpeg4
    codec = avcodec_find_decoder_by_name("mpeg4");
    if (!codec) {
        ret = -1111;
        goto end;
    }

    // 根据编解码器的id，来找到一个 解析器，这个解析器可以用来解析出 mpeg4 文件流中的一帧压缩后的数据
    parser = av_parser_init(codec->id);
    if (!parser) {
        ret = -1112;
        goto end;
    }

    // 根据编解码器初始化 编码器的上下文 数据结构。
    c = avcodec_alloc_context3(codec);
    if (!c) {
        ret = -1113;
        goto end;
    }

    // 打来编解码器
    if ((ret = avcodec_open2(c, codec, NULL)) < 0) {
        goto end;
    }

    // 打开文件
    f = fopen(filename, "rb");
    if (!f) {
        ret = -1114;
        goto end;
    }

    // 初始化 AV_Frame 这个数据结构，它是用来储存一帧解码后的图像的数据结构
    frame = av_frame_alloc();
    if (!frame) {
        ret = -1115;
        goto end;
    }

    // 一直循环，直到输入文件被读到了最后
    while (!feof(f)) {
        // 从原文件中读取4096个字节
        data_size = fread(inbuf, 1, INBUF_SIZE, f);
        if (!data_size)
            break;

        // 4096 的字节中可能会包含多帧压缩后的图像，所以这里每次解析出一帧压缩图像数据，然后解码成一帧解码后图像数据，然后再循环，直至4096个字节被读取完毕。
        data = inbuf;
        while (data_size > 0) {
            // 从4096个字节中以 data 作为起点，解析出一帧压缩图像数据到 AV_Packet 中。返回值是压缩帧的byte大小
            if ((ret = av_parser_parse2(parser, c, &pkt->data, &pkt->size, data, data_size,
                                        AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0)) < 0) {
                goto end;
            }
            // 将 data 移动到新的起点
            data += ret;
            // 记录 4096 字节中剩下的可用字节大小
            data_size -= ret;

            // 如果 size 大于0表示刚刚读取数据成功
            if (pkt->size) {
                // 将一个 pkt 包解析成一个 frame
                decode(c, frame, pkt, outfilename);
            }

        }
    }

    /* flush the decoder */
    decode(c, frame, NULL, outfilename);

    fclose(f);

    end:
    av_parser_close(parser);
    avcodec_free_context(&c);
    av_frame_free(&frame);
    av_packet_free(&pkt);

    if (ret < 0) {
        char buf2[500] = {0};
        if (ret == -1111) {
            return (char *) "codec not found";
        } else if (ret == -1112) {
            return (char *) "parser not found";
        } else if (ret == -1113) {
            return (char *) "could not allocate video codec context";
        } else if (ret == -1114) {
            return (char *) "could not open input file";
        } else if (ret == -1115) {
            return (char *) "could not allocate video frame";
        }
        av_strerror(ret, buf2, 1024);
        return buf2;
    } else {
        return (char *) "解码成功";
    }
}
