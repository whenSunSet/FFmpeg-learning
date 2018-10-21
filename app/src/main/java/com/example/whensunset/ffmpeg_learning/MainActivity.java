package com.example.whensunset.ffmpeg_learning;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {
  
  FFmpegPlayer fFmpegPlayer;
  
  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_main);
    TextView textView = findViewById(R.id.sample_text);
    fFmpegPlayer = new FFmpegPlayer();
    fFmpegPlayer.playMyMedia("http://blog.csdn.net/ywl5320");
  }
  
}
