#!/usr/bin/env bash
# exit 不注释的时候，表示 android 项目编译的时候不需要编译 ffmepg，注释的时候，表示 android 项目编译的时候要编译 ffmpeg。
 exit

# 执行 FFmpeg 源码项目中的编译脚本
sh /Users/whensunset/AndroidStudioProjects/KSVideoProject/ffmpeg/build_android.sh

# 当前项目的 so 文件的存放目录，需要改成自己的
so_path="/Users/whensunset/AndroidStudioProjects/KSVideoProject/FFmpeglearning/app/src/main/jni/ffmpeg/armeabi/"

# 所有 so 文件编译生成后的默认命名
libavcodec_name="libavcodec-57.so"
libavdeivce_name="libavdevice-57.so"
libavfilter_name="libavfilter-6.so"
libavformat_name="libavformat-57.so"
libavutil_name="libavutil-55.so"
libpostproc_name="libpostproc-54.so"
libswresample_name="libswresample-2.so"
libseacale_name="libswscale-4.so"

# 删除当前项目中的老的 so 文件删除
rm ${so_path}${libavcodec_name}
rm ${so_path}${libavdeivce_name}
rm ${so_path}${libavfilter_name}
rm ${so_path}${libavformat_name}
rm ${so_path}${libavutil_name}
rm ${so_path}${libpostproc_name}
rm ${so_path}${libswresample_name}
rm ${so_path}${libseacale_name}

# FFmpeg 源码项目中，编译好的 so 文件的路径，需要改成自己的
build_so_path="/Users/whensunset/AndroidStudioProjects/KSVideoProject/ffmpeg/android/arm/lib/"

# 将新编译的 so 文件拷贝到当前项目的 so 目录下
cd /Users/whensunset/AndroidStudioProjects/KSVideoProject/FFmpeglearning/app
cp ${build_so_path}${libavcodec_name} ${so_path}${libavcodec_name}
cp ${build_so_path}${libavdeivce_name} ${so_path}${libavdeivce_name}
cp ${build_so_path}${libavfilter_name} ${so_path}${libavfilter_name}
cp ${build_so_path}${libavformat_name} ${so_path}${libavformat_name}
cp ${build_so_path}${libavutil_name} ${so_path}${libavutil_name}
cp ${build_so_path}${libpostproc_name} ${so_path}${libpostproc_name}
cp ${build_so_path}${libswresample_name} ${so_path}${libswresample_name}
cp ${build_so_path}${libseacale_name} ${so_path}${libseacale_name}
