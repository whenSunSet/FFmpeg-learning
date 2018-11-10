package com.example.whensunset.ffmpeg_learning;

import android.annotation.SuppressLint;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.os.Bundle;
import android.support.v7.app.AppCompatActivity;
import android.view.TextureView;

import java.io.IOException;

/**
 * 1.TextureView 是用来绘制画面数据流的，比如说视频又或是 opengl 的场景。
 * 2.这里的画面数据流可以来自本进程，也可以来自其他进程
 * 3.TextureView 只能使用硬件加速的 window，不可使用软件绘制的时候。
 * 4.不同于 SurfaceView，TextureView不会直接创建一个window，它的行为更像一个 view，比如说你可以像移动一个 view 一样移动他。
 * 5.使用 TextureView 是简单的，只需要获取到 TextureView 的 SurfaceTexture，这个 SurfaceTexture 可以用来绘制内容
 * 6.TextureView 获取SurfaceTexture 的方式有两种，一个是使用 getSurfaceTexture()，一个是使用 TextureView.SurfaceTextureListener
 * 7.SurfaceTexture 只有在绑定了 window之后才能使用，也就是执行了 onAttachedToWindow 之后。在这之后 SurfaceTexture 才是可用的。
 * 8.一个需要注意的事情是：TextureView 只能有一个生产者，比如如果你已经使用他来显示视频了，那么就不能使用 lockCanvas 来在上面绘制其他内容了
 */
public class TextureCameraPreviewActivity extends AppCompatActivity implements TextureView.SurfaceTextureListener {
  
  TextureView mTextureView;
  private Camera mCamera;
  
  @SuppressLint("WrongViewCast")
  @Override
  protected void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);
    setContentView(R.layout.activity_texture_camera_preview);
    mTextureView = findViewById(R.id.texture_camera_preview);
    mTextureView.setSurfaceTextureListener(this);
    
    setContentView(mTextureView);
  }
  
  public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
    mCamera = Camera.open();
    
    try {
      mCamera.setPreviewTexture(surface);
      mCamera.startPreview();
    } catch (IOException ioe) {
      // Something bad happened
    }
  }
  
  public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
    // Ignored, Camera does all the work for us
  }
  
  public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
    mCamera.stopPreview();
    mCamera.release();
    return true;
  }
  
  public void onSurfaceTextureUpdated(SurfaceTexture surface) {
    // Invoked every time there's a new Camera preview frame
  }
  
}
