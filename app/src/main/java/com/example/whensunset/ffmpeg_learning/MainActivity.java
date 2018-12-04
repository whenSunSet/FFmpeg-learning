package com.example.whensunset.ffmpeg_learning;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.annotation.RequiresApi;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
  
  FFmpegPlayer fFmpegPlayer;
  
  static TextView mShowText;
  Button mButton1;
  Button mButton2;
  Button mButton3;
  Button mButton4;
  Button mButton5;
  Button mButton6;
  Button mButton7;
  Button mButton8;
  Button mButton9;
  Button mButton10;
  
  static StringBuilder stringBuilder = new StringBuilder();
  @SuppressLint("HandlerLeak")
  static Handler mHandler = new Handler() {
    @Override
    public void handleMessage(Message msg) {
      super.handleMessage(msg);
      int code = msg.what;
      if (code == 111) {
        mShowText.setText((CharSequence) msg.obj);
      } else if (msg.what == 0) {
        mShowText.setText("success");
      } else if (msg.what == 2) {
        int y = msg.arg1;
        int maxY = msg.arg2;
        String showString = (String) msg.obj;
  
        if (y == (maxY - 1)) {
          mShowText.setLines(maxY);
          mShowText.setText(stringBuilder.toString());
          stringBuilder = new StringBuilder();
        } else {
          showString = showString + "\n";
          stringBuilder.append(showString);
        }
      } else {
        mShowText.setText("failed!");
      }
    }
  };
  @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
    mShowText = findViewById(R.id.show);
    mShowText.setSingleLine(false);
    mShowText.setLineSpacing(1,0.4f);
    mShowText.setLetterSpacing(0.4f);
    mButton1 = findViewById(R.id.hello_world);
    mButton2 = findViewById(R.id.decode_encode);
    mButton3 = findViewById(R.id.push);
    mButton4 = findViewById(R.id.texture_camera_preview);
    
    mButton5 = findViewById(R.id.av_io_reading);
    mButton6 = findViewById(R.id.decode_video);
    mButton7 = findViewById(R.id.encode_video);
    mButton8 = findViewById(R.id.filter_video);
    mButton9 = findViewById(R.id.demuxing_decoding_video);
    mButton10 = findViewById(R.id.play_video);
    
    fFmpegPlayer = new FFmpegPlayer();
    fFmpegPlayer.initFfmpegLog();
    
    mButton1.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        String content = fFmpegPlayer.urlProtocolInfo() + "\n" + fFmpegPlayer.avFormatInfo() + "\n" + fFmpegPlayer.avCodecInfo()
            + "\n" + fFmpegPlayer.avFilterInfo() + "\n" + fFmpegPlayer.configurationInfo();
        mShowText.setText(content);
      }
    });
    
    mButton2.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        new Thread(new Runnable() {
          @Override
          public void run() {
            int code = fFmpegPlayer.decode("/storage/emulated/0/av_test/b.mp4", "/storage/emulated/0/av_test/b.yuv");
            Message message = new Message();
            message.what = code;
            mHandler.sendMessage(message);
          }
        }).start();
      
      }
    });
    
    mButton3.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        new Thread(new Runnable() {
          @Override
          public void run() {
            int code = fFmpegPlayer.pushStream("/storage/emulated/0/av_test/moren2.mp4", "rtmp://47.106.145.211:1935/hls/room");
            Message message = new Message();
            message.what = code;
            mHandler.sendMessage(message);
          }
        }).start();
      }
    });
    
    mButton4.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        MainActivity.this.startActivity(new Intent(MainActivity.this, TextureCameraPreviewActivity.class));
      }
    });
    
    mButton5.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        new Thread(new Runnable() {
          @Override
          public void run() {
            String returnString = fFmpegPlayer.ffmpegSampleOne("/storage/emulated/0/av_test/b.mp4");
            Message message = new Message();
            message.obj = returnString;
            message.what = 111;
            mHandler.sendMessage(message);
          }
        }).start();
      }
    });
    
    mButton6.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        new Thread(new Runnable() {
          @Override
          public void run() {
            String returnString = fFmpegPlayer.ffmpegSampleTwo("/storage/emulated/0/av_test/c.mpeg4", "/storage/emulated/0/av_test/c.yuv");
            Message message = new Message();
            message.obj = returnString;
            message.what = 111;
            mHandler.sendMessage(message);
          }
        }).start();
      }
    });
  
    mButton7.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        new Thread(new Runnable() {
          @Override
          public void run() {
            String returnString = fFmpegPlayer.ffmpegSampleThree("/storage/emulated/0/av_test/c.mpeg4");
            Message message = new Message();
            message.obj = returnString;
            message.what = 111;
            mHandler.sendMessage(message);
          }
        }).start();
      }
    });
    
    mButton8.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
    new Thread(new Runnable() {
          @Override
          public void run() {
            int code = fFmpegPlayer.ffmpegSampleFour("/storage/emulated/0/av_test/f.mp4");
            Message message = new Message();
            message.what = code;
            mHandler.sendMessage(message);
          }
        }).start();
      }
    });
    
    mButton9.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        new Thread(new Runnable() {
          @Override
          public void run() {
            String returnString = fFmpegPlayer.ffmpegSampleFive("/storage/emulated/0/av_test/b.mp4", "/storage/emulated/0/av_test/c1.yuv" , "/storage/emulated/0/av_test/c.mp3");
            Message message = new Message();
            message.obj = returnString;
            message.what = 111;
            mHandler.sendMessage(message);
          }
        }).start();
      }
    });
    
    mButton10.setOnClickListener(new View.OnClickListener() {
      @Override
      public void onClick(View v) {
        MainActivity.this.startActivity(new Intent(MainActivity.this, GameActivity.class));
      }
    });
    
  }
  
  public static void showText(String s, int y, int maxY) {
    Message message = new Message();
    message.what = 2;
    message.obj = s;
    message.arg1 = y;
    message.arg2 = maxY;
    mHandler.sendMessage(message);
  }
}
