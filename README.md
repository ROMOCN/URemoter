# URemoter

-----

A remote conference software based on C++ / 一款基于C++的远程会议软件

## Developer Environment Requirements / 开发环境要求

* windows 10 or windows 7
* Qt Creator
* FFmpeg & Opencv

## This project realizes the following features / 功能实现

* Camera image capture based on the OpenCV / 基于OpenCV的摄像头画面捕获
* Audio capture based on the QAudio / 基于QT音频模块的麦克风捕获
* Screen capture / 屏幕捕获
* Establish a connection with the server through TCP / 通过tcp与服务器建立连接
* Establish a room by a ID receved from server / 根据从服务器收到的ID建立一个房间
* Push the captured picture by the udp / 通过UDP推送捕获的画面
* Large size pictures will be sent in subcontracting / 大尺寸的画面将会被分包发送
