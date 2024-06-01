#### 将ffmpeg 编译为dll供外部调用

#### 创建qt应用程序，设置项目为lib

```
// ffmpeg-dll.pro
#TEMPLATE = app
#CONFIG += console
TEMPLATE = lib
```

#### 增加dll函数导出
```
// main.c
__declspec(dllexport) int __cdecl ffmain(int argc, char const *argv[]){
    return main(argc,argv);
}
```

#### 处理命令行参数，既ffmpeg的传入参数并不是以传入的为准，而是内部使用操作系统API又进行了替换，如果在某些系统出现兼容性问题，需要再次修改源代码

```
// cmdutils.c
// split_commandline
// 注释掉 785 行
// prepare_app_arguments(&argc, &argv);
```


#### 使用python 调用

```
```