# Screen_recording
qt+ffmpeg实现的录屏软件
# 编译环境
qt5.14.2 
# 实现功能
截屏/录屏
# 流程
截屏使用dxgi(windows下速度快)
图像使用ffmpeg+libyuv编码成h264格式
音频编码成aac
最后封装成mp4



