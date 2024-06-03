#### 将ffmpeg 编译为dll供外部调用

使用命令行工具时，在任务管理口可以看到ffmpeg的进程和参数，不太友好。因此将ffmpeg命令行编译为dll供外部使用。虽然重新用c/c++实现转码并不难，但是命令行工具已经能解决大多数的问题。重新实现耗时耗力，还不一定比命令行实现的更好。



基于ffmpeg7.0构建：https://github.com/GyanD/codexffmpeg/releases/tag/7.0.1 



#### 创建qt应用程序，设置项目为lib

```
// ffmpeg-dll.pro
#TEMPLATE = app
#CONFIG += console
TEMPLATE = lib
```

####

```c
// config.h 第四行，可以从ffmpeg -version 获取
FFMPEG_CONFIGURATION#define FFMPEG_CONFIGURATION "--enable-gpl --enable-version3 --enable-shared --disable-w32threads --disable-autodetect --enable-fontconfig --enable-iconv --enable-gnutls --enable-libxml2 --enable-gmp --enable-bzlib --enable-lzma --enable-libsnappy --enable-zlib --enable-librist --enable-libsrt --enable-libssh --enable-libzmq --enable-avisynth --enable-libbluray --enable-libcaca --enable-sdl2 --enable-libaribb24 --enable-libaribcaption --enable-libdav1d --enable-libdavs2 --enable-libuavs3d --enable-libxevd --enable-libzvbi --enable-librav1e --enable-libsvtav1 --enable-libwebp --enable-libx264 --enable-libx265 --enable-libxavs2 --enable-libxeve --enable-libxvid --enable-libaom --enable-libjxl --enable-libopenjpeg --enable-libvpx --enable-mediafoundation --enable-libass --enable-frei0r --enable-libfreetype --enable-libfribidi --enable-libharfbuzz --enable-liblensfun --enable-libvidstab --enable-libvmaf --enable-libzimg --enable-amf --enable-cuda-llvm --enable-cuvid --enable-dxva2 --enable-d3d11va --enable-d3d12va --enable-ffnvcodec --enable-libvpl --enable-nvdec --enable-nvenc --enable-vaapi --enable-libshaderc --enable-vulkan --enable-libplacebo --enable-opencl --enable-libcdio --enable-libgme --enable-libmodplug --enable-libopenmpt --enable-libopencore-amrwb --enable-libmp3lame --enable-libshine --enable-libtheora --enable-libtwolame --enable-libvo-amrwbenc --enable-libcodec2 --enable-libilbc --enable-libgsm --enable-libopencore-amrnb --enable-libopus --enable-libspeex --enable-libvorbis --enable-ladspa --enable-libbs2b --enable-libflite --enable-libmysofa --enable-librubberband --enable-libsoxr --enable-chromaprint"

```

#### 增加dll函数导出
```c
// main.c
__declspec(dllexport) int __cdecl ffmain(int argc, char const *argv[]){
    return main(argc,argv);
}
```

#### 处理命令行参数
既ffmpeg的传入参数并不是以传入的为准，而是内部使用操作系统API又进行了替换，如果在某些系统出现兼容性问题，需要再次修改源代码

```c
// cmdutils.c
// split_commandline
// 注释掉 785 行
// prepare_app_arguments(&argc, &argv);
```


#### 使用python 调用

```python
def ffmpeg():
    dll = ctypes.windll.LoadLibrary(
        "D:/cpp-sdk/FFmpeg-n7.0/ffmpeg-exe-dll/Release/release/ffmpeg-dll.dll"
    )

    ffmpeg_main = dll.ffmain

    ffmpeg_main.argtypes = [ctypes.c_int, ctypes.POINTER(ctypes.c_char_p)]
    ffmpeg_main.restype = ctypes.c_int
    argv = (ctypes.c_char_p * 10)()
    argv[0] = bytes("ffmpeg", "utf-8")
    argv[1] = bytes("-f", "utf-8")
    argv[2] = bytes("gdigrab", "utf-8")
    argv[3] = bytes("-i", "utf-8")
    argv[4] = bytes("desktop", "utf-8")
    argv[5] = bytes("-vcodec", "utf-8")
    argv[6] = bytes("libx264", "utf-8")
    argv[7] = bytes("-f", "utf-8")
    argv[8] = bytes("flv", "utf-8")
    argv[9] = bytes("rtmp://srs-push.qq829.cn:31935/live/557254322", "utf-8")
    ret = ffmpeg_main(10, argv)
    ctypes.windll.kernel32.FreeLibrary.argtypes = [ctypes.c_void_p]
    ctypes.windll.kernel32.FreeLibrary.restype = ctypes.c_int
    ctypes.windll.kernel32.FreeLibrary(dll._handle)
    return ret


ffmpeg()
```