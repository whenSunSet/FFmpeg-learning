package com.example.whensunset.ffmpeg_learning;

import android.annotation.SuppressLint;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
  
  FFmpegPlayer fFmpegPlayer;
  
  TextView mShowText;
  Button mButton1;
  Button mButton2;
  Button mButton3;
  Button mButton4;
  
  @SuppressLint("HandlerLeak")
  Handler mHandler = new Handler() {
    @Override
    public void handleMessage(Message msg) {
      super.handleMessage(msg);
      if (msg.what == 0) {
        mShowText.setText("succeed!");
      } else {
        mShowText.setText("failed!");
      }
    }
  };
  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
    mShowText = findViewById(R.id.show);
    mButton1 = findViewById(R.id.hello_world);
    mButton2 = findViewById(R.id.decode_encode);
    mButton3 = findViewById(R.id.push);
    mButton4 = findViewById(R.id.trans_code);
    fFmpegPlayer = new FFmpegPlayer();
    
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
            int code = fFmpegPlayer.decode("/storage/emulated/0/av_test/a.mp4", "/storage/emulated/0/av_test/a.yuv");
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
            int code = fFmpegPlayer.pushStream("/storage/emulated/0/av_test/a.mp4", "rtmp://192.168.31.233:1935/live/rtmp");
            Message message = new Message();
            message.what = code;
            mHandler.sendMessage(message);
          }
        }).start();
      }
    });
  }
  
}
