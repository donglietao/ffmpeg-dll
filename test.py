import ctypes as ctypes
import time as time


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


while ffmpeg() < 0:
    print("ffmpeg error")
    time.sleep(5)

# while ffmpeg() < 0:
#     print("ffmpeg error")
#     time.sleep(5)

# 获取当前活动的窗口标题

# while True:
#     # 获取当前活动窗口的句柄
#     hwnd = ctypes.windll.user32.GetForegroundWindow()
#     # 获取窗口标题
#     length = ctypes.windll.user32.GetWindowTextLengthW(hwnd)
#     buf = ctypes.create_unicode_buffer(length + 1)
#     ctypes.windll.user32.GetWindowTextW(hwnd, buf, length + 1)
#     print(buf.value)
#     # 窗口的class name
#     length = 100
#     buf = ctypes.create_unicode_buffer(length + 1)
#     ctypes.windll.user32.GetClassNameW(hwnd, buf, length + 1)
#     print(buf.value)
#     # 休眠1秒
#     time.sleep(1)
