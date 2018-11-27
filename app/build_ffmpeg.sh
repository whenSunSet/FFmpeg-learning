#!/usr/bin/env bash
 exit
so_path="/Users/whensunset/AndroidStudioProjects/KSVideoProject/FFmpeglearning/app/src/main/jni/ffmpeg/armeabi/"
libavcodec_name="libavcodec-57.so"
libavdeivce_name="libavdevice-57.so"
libavfilter_name="libavfilter-6.so"
libavformat_name="libavformat-57.so"
libavutil_name="libavutil-55.so"
libpostproc_name="libpostproc-54.so"
libswresample_name="libswresample-2.so"
libseacale_name="libswscale-4.so"
rm ${so_path}${libavcodec_name}
rm ${so_path}${libavdeivce_name}
rm ${so_path}${libavfilter_name}
rm ${so_path}${libavformat_name}
rm ${so_path}${libavutil_name}
rm ${so_path}${libpostproc_name}
rm ${so_path}${libswresample_name}
rm ${so_path}${libseacale_name}
sh /Users/whensunset/AndroidStudioProjects/KSVideoProject/ffmpeg/build_android.sh
build_so_path="/Users/whensunset/AndroidStudioProjects/KSVideoProject/ffmpeg/android/arm/lib/"
cd /Users/whensunset/AndroidStudioProjects/KSVideoProject/FFmpeglearning/app
cp ${build_so_path}${libavcodec_name} ${so_path}${libavcodec_name}
cp ${build_so_path}${libavdeivce_name} ${so_path}${libavdeivce_name}
cp ${build_so_path}${libavfilter_name} ${so_path}${libavfilter_name}
cp ${build_so_path}${libavformat_name} ${so_path}${libavformat_name}
cp ${build_so_path}${libavutil_name} ${so_path}${libavutil_name}
cp ${build_so_path}${libpostproc_name} ${so_path}${libpostproc_name}
cp ${build_so_path}${libswresample_name} ${so_path}${libswresample_name}
cp ${build_so_path}${libseacale_name} ${so_path}${libseacale_name}
