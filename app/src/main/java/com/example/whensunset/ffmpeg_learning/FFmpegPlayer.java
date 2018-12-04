package com.example.whensunset.ffmpeg_learning;

import android.support.annotation.Keep;

/**
 * Created by whensunset on 2018/10/21.
 */

public class FFmpegPlayer {
  static {
    System.loadLibrary("avutil-55");
    System.loadLibrary("swresample-2");
    System.loadLibrary("avcodec-57");
    System.loadLibrary("avformat-57");
    System.loadLibrary("swscale-4");
    System.loadLibrary("postproc-54");
    System.loadLibrary("avfilter-6");
    System.loadLibrary("avdevice-57");
    System.loadLibrary("myffmpeg");
  }
  
  public native void playMyMedia(String url);
  
  public native void initFfmpegLog();
  
  public native String urlProtocolInfo();
  
  public native String avFormatInfo();
  
  public native String avCodecInfo();
  
  public native String avFilterInfo ();
  
  public native String configurationInfo();
  
  public native int decode(String inputUrl, String outputUrl);
  
  public native int pushStream(String inputUrl, String outputUrl);
  
  public native String ffmpegSampleOne(String inputUrl);
  
  public native String ffmpegSampleTwo(String inputUrl, String outputUrl);
  
  public native String ffmpegSampleThree(String inputUrl);
  
  public native int ffmpegSampleFour(String inputUrl);
  
  public native String ffmpegSampleFive(String inputFile, String destVideoFile, String destAudioFile);
  
  public static native int play(Object surface);
  
  @Keep
  public static void showString(String showString, int y, int maxY) {
    MainActivity.showText(showString, y, maxY);
  }
}
