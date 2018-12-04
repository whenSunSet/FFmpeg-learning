package com.example.whensunset.ffmpeg_learning;

import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

public class GameActivity extends AppCompatActivity implements SurfaceHolder.Callback{
  
  SurfaceHolder surfaceHolder;
  SurfaceView mSurfaceView;
  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_game);
  
    mSurfaceView = (SurfaceView) findViewById(R.id.surface_view);
    surfaceHolder = mSurfaceView.getHolder();
    surfaceHolder.addCallback(this);
  }
  
  @Override
  protected void onPause() {
    super.onPause();
  }
  
  @Override
  protected void onResume() {
    super.onResume();
  }
  
  @Override
  public void surfaceCreated(SurfaceHolder holder) {
    new Thread(new Runnable() {
      @Override
      public void run() {
        FFmpegPlayer.play(surfaceHolder.getSurface());
      }
    }).start();
  }
  
  @Override
  public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
  
  }
  
  @Override
  public void surfaceDestroyed(SurfaceHolder holder) {
  
  }
}
