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

    ret = avcodec_send_packet(dec_ctx, pkt);
    if (ret < 0) {
        return ret;
    }

    while (ret >= 0) {
        ret = avcodec_receive_frame(dec_ctx, frame);
        if (ret < 0) {
            return ret;
        }

        printf("saving frame %3d\n", dec_ctx->frame_number);
        fflush(stdout);

        /* the picture is allocated by the decoder. no need to
           free it */
        snprintf(buf, sizeof(buf), "%s-%d", filename, dec_ctx->frame_number);
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

    filename = argv[0];
    outfilename = argv[1];

    avcodec_register_all();
    pkt = av_packet_alloc();
    if (!pkt)
        exit(1);

    // 将 inbuf 从 INBUF_SIZE 到INBUF_SIZE + AV_INPUT_BUFFER_PADDING_SIZE 这一段的数据都设置为0(这确保了对损坏的MPEG流不会发生过读)
    /* set end of buffer to 0 (this ensures that no overreading happens for damaged MPEG streams) */
    memset(inbuf + INBUF_SIZE, 0, AV_INPUT_BUFFER_PADDING_SIZE);

    /* find the hevc video decoder */
    codec = avcodec_find_decoder_by_name("mpeg4");
    if (!codec) {
        ret = -1111;
        goto end;
    }

    parser = av_parser_init(codec->id);
    if (!parser) {
        ret = -1112;
        goto end;
    }

    c = avcodec_alloc_context3(codec);
    if (!c) {
        ret = -1113;
        goto end;
    }

    // 对于某些 codec ，例如 msmpeg4  和 mpeg4,宽和高需要被初始化，因为这些信息在 bitstream 中是无效的
    /* For some codecs, such as msmpeg4 and mpeg4, width and height
       MUST be initialized there because this information is not
       available in the bitstream. */

    /* open it */
    if ((ret = avcodec_open2(c, codec, NULL)) < 0) {
        goto end;
    }

    f = fopen(filename, "rb");
    if (!f) {
        ret = -1114;
        goto end;
    }

    frame = av_frame_alloc();
    if (!frame) {
        ret = -1115;
        goto end;
    }

    while (!feof(f)) {
        // 从原文件中读取4096个字节
        /* read raw data from the input file */
        data_size = fread(inbuf, 1, INBUF_SIZE, f);
        if (!data_size)
            break;

        // 将每次读出的4096个字节不断通过 parser 分割解析到 frame 中
        /* use the parser to split the data into frames */
        data = inbuf;
        while (data_size > 0) {
            // 从 data 中将数据解析成一个个 pkt 包
            if ((ret = av_parser_parse2(parser, c, &pkt->data, &pkt->size, data, data_size,
                                        AV_NOPTS_VALUE, AV_NOPTS_VALUE, 0)) < 0) {
                goto end;
            }
            data += ret;
            data_size -= ret;

            if (pkt->size) {
                // 将一个 pkt 包解析成 多个 frame
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
