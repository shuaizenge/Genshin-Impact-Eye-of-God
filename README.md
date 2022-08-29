# 原神-神之眼 Genshin Impact-Eye of God
![pic1](6.Docs/image/pic1.jpg)
> **项目简介：**  
> 本项目是某原神玩家~~渴望获得神明的注视~~基于兴趣而诞生的神之眼~~获得~~制作教程  
> 
> **资料包含的部分：**  
> - 神之眼硬件设计的PCB设计源文件，提供立创EDA设计链接
> - 外壳设计结构文件
> - 神之眼paltfromio平台源码  
功能演示

---
## 一、项目说明
1.Hardware  
Hardware文件夹内是神之眼里面用到的所有电路的原理图和PCB文件，目前提供的是立创EDA格式的源文件以及Gerber格式的光绘文件用于提供给厂家进行直接加工。
另一个是[立创EDA的开源链接](https://oshwhub.com/zengshuaishuai/shen-zhi-yan-gua-jian-2-0_copy)，可以查看开源的硬件设计  
2.Firmware  
基于platfromio开发的固件，提供基本源码，需要自己去vscode上使用platfromio编译烧录 
3.Software  
别人写的一个上位机软件，包含一些辅助功能，如图片转换、视频转码等  
4.Tools  
tools主要是提供一些三方的工具软件  
5.3D Model  
文件夹里是神之眼用到的所有结构件的3D模型文件，包含蒙德和稻妻的外壳。  
6.Docs  
相关的参考文件，包括芯片的Datasheet等。  
7.Others  
其他一些辅助的文件，如放置在SD卡里面的文件
## 二、硬件说明
主要包含模块：
- ESP32 Pico D4 主控芯片，神之眼的灵魂部分，负责整体的控制
- MPU6050 姿态解算芯片，主要用来获取当前位置，从而对神之眼进行控制
- TP4055 充电芯片，控制充电电流和充电量控制
- EC190707 开关IC，2s长按来开关机
- RT9013-3.3 稳压LDO，降压到3.3V给芯片供电
- GC9A01 屏幕，圆形屏幕展示
## 三、软件说明
### 主要特点
1. 聚合多种APP，内置天气、时钟、相册、特效动画、视频播放、电脑投屏、web设置等等。（各APP具体使用参考说明书）
2. 开机无论是否插接tf卡、mpu6050是否焊接正常、是否连接wifi（一定要2.4G的wifi），都不影响系统启动。
3. 程序相对模块化，低耦合。
4. 提供web界面进行配网以及其他设置选项。注：具体操作参考 APP介绍
5. 提供web端连入除了支持ip访问，也支持域名直接访问 http://holocubic （部分浏览器可能支持不好）
6. 提供web端的文件上传到SD卡（包括删除），无需拔插SD来更新图片。
7. 提供全套上位机软件，并开源上位机源码。 https://github.com/ClimbSnail/HoloCubic_AIO_Tool
### app介绍
**网页配置服务（Web Server）**
1. 运行条件：无。注：wifi等信息是保存在flash中，内存卡完全不影响wifi功能的连接。
2. 启用后，会显示 Web Sever Start 。小电视开启AP模式，建立在 AP_IP 上（屏幕的服务界面有标注），AP模式的热点名为 HoloCubic_AIO 无密码。
3. 开始使用时，应让电脑与 HoloCubic 处于同一网络环境（同网段）。如果之前没连接过wifi则需要
使用电脑连接HoloCubic放出的热点名为 HoloCubic_AIO 无密码的wifi。
4. 在浏览器地址栏输入 Local_IP 或者 AP_IP （ http://192.168.4.2 也支持域名直接访问 http://holocubic ），即可进入管理设置后台。推荐使用 ip地址 访问。
5. 网页里可设置系统参数、天气APP参数、相册参数、播放器参数等等。

**文件管理器（File Manager）**
作用：通过无线网络管理内存卡上的文件。
1. 运行APP条件：必须是已经正常配置wifi。必须插内存卡。为避免wifi连接时，功率不够导致重启，请确保USB口供电充足。目前部分功能还在开发中。
2. 进入 Holocubic 文件管理器后会自动连接已配置的wifi，并显示出IP地址。
3. 未完成：在上位机的文件管理器软件中填入自己 Holocubic 的IP地址（端口可以不用改），点击连接。
注：目前文件管理器临时使用 windows资源管理器 ，在地址栏输入ftp://holocubic:aio@192.168.123.241 （192.168.123.241为我的小电视上显示的IP地址，如果提示开启访问，就开启）

**相册（Picture）**
1. 运行APP条件：必须插内存卡，内存卡的根目录下必须存在 image/ 目录（也可以使用 WebServer服务 APP 通过浏览器上传照片）， image/ 目录下必须要有图片文件（jpg或者bin）。
2. 将需要播放的图片转化成一定格式（.jpg或.bin），再保存在 image/ 目录中，图片文件名必须为英文字符或数字。
3. 使用固件进入相册APP后，将会读取 image/ 目录下的图片文件。关于图片转换：使用附带的上位机转化（分辨率随意，软件会自动压缩到指定分辨率）。常用的天气图片，转换为c数组，格式为Indexed 16 colors 选择 C array。
不常用的图片则可以转换成（True color with alpha 选择Binary RGB565）bin文件存储到SD卡中，这样可以省下一些程序存储空间用来增加功能。支持转化为jpg图片。

**神之眼播放（shenzhiyan）**
1. 运行APP条件：必须插内存卡，内存卡的根目录下必须存在 movie/ 目录。
2. 将所需要播放的视频（最好长宽比是1:1），使用本固件配套的使用转化工具转化为目标文件
（mjpeg或者rgb格式都可），存放在 movie/ 目录下，视频文件名必须为英文字符或数字。
3. 运行播放器APP后，将会读取 movie/ 目录下的视频文件。
4. 默认功率下，无任何动作90s后进入低功耗模式，120s后进入二级低功耗模式，具体表现为播放画面帧数下降。
5. 神之眼的动态视屏都是用mp4转化为mjpeg而成的，转化工具在software的上位机软件里面有

**天气、时钟（Weather）**
一共有两款天气时钟APP
新版 weather：
1. 新版由 PuYuuu 模仿了 misaka 的时钟界面。使用https://www.tianqiapi.com 天气API。
2. 运行APP条件：必须是已经联网状态，且设置好 tianqi_appid 、 tianqi_appsecret 、 tianqi城市名（中文） 。
3. 使用新版天气时钟，需要再"Web Server"服务中修改 tianqi_appid 、 tianqi_appsecret 。（申请地址 https://www.yiketianqi.com/user/login ）

旧版 weather old：
1. 旧版UI设计模仿了CWEIB
2. 运行APP条件：必须是已经联网状态，且设置好地点、weather_key。不插内存卡大多数情况能正常工作。
3. 一般情况下不插内存卡也可以工作，但部分天气图标是存在内存卡中（由于内部flash不够用）的，需要将固件附带的 weather/ 文件夹复制到tf卡根目录。
4. 使用旧版天气时钟，需要再"Web Server"服务中修改知心天气的 知心天气 城市名 、 key（私钥） 。（申请地址 https://seniverse.com 。程序默认使用的是v3版本的api）

注：即使断网后，时钟也依旧运行。（开机最好连接wifi，这样会自动同步时钟。使用中会间歇尝试同
步时钟）

**特效动画（Idea）**
1. 运行APP条件：无。内置的几种特效动画。

**2048 APP**
1. 2048 游戏由群友 AndyXFuture 编写并同意，由 ClimbSnail 合入AIO固件。原项目链接为https://github.com/AndyXFuture/HoloCubic-2048-anim
2. 运行APP条件：无。基本屏幕能亮就行。
3. 操作注意：游戏中 向上 和 向下 操作由于与原 进入 和 退出 为同一个动作，系统已操作时长为区分动作，游戏中 向上 和 向下 正常操作即可， 进入 和 退出 需要倾斜1秒中方可触发。

**BiliBili APP**
1. 运行APP条件：内存卡中必须要有名为 bilibili 的文件夹。必须是已经正常配置wifi。为避免wifi连接时，功率不够导致重启，请确保USB口供电充足。
2. UID 查看方法：电脑浏览器上打开B站并登入账号，之后浏览器打开一个空白页粘贴回车这个网址
https://space.bilibili.com/ ，网址尾巴会自动多出一串纯数字码，此即为UID。
3. 第一次使用之前，要先在 WebServer App 的网页上填写 UID 码。
4. 需要在内存卡中名为 bilibili 的文件夹里添加一张名为 avatar.bin 自己B站头像的图片，分辨率为 100*100 的 bin 文件（可以使用AIO上位机转换）。

### 程序架构介绍
![xmind](6.Docs/image/xmind.png)
AIO框架讲解链接 https://www.bilibili.com/video/BV1jh411a7pV?p=4  
关于UI的设计可以自行关注下 Edgeline 、 gui-guider 等工具。
### 代码结构介绍
- lib 包含主要要用的一些arduino库
- src 主要的控制代码
    - app 是各个APP实现的单独代码
    - driver 主要是一些驱动代码
    - sys app系统的控制代码
    main.cpp 是主程序  
    common.h 对应一些要调用的对象设置  
    config.h 对应内存中写的一些设置  
    message/network 对应信息管理/网络的代码  
    start.c 是一张开始界面的图片  
## 四、SDK设计&二次开发
> 感谢以下项目  
[HoloCubic_AIO (All in one for HoloCubic)](https://gitee.com/ClimbSnailQ/HoloCubic_AIO)  
[神之眼挂件V1.1](https://oshwhub.com/Myzhazha/esp32_pico-d4-video)
## 五、作者的话
- 既然不能从神明哪里获得神之心，何不用自己的双手来获得呢？~~好中二，害羞~~
- 作者也是第一次开源项目，有很多不足之处还请多多包容，可以在issue中提出，或者发邮件到1287666071@qq.com
- 我做着个项目的初衷还是想去复刻小渣渣的[神之眼挂件项目](https://oshwhub.com/Myzhazha/esp32_pico-d4-video)，发现功能比较单一，想着以前看到过稚晖君的HoloCubic也使用了ESP32-PICO-D4芯片，想是不是可以融合一下呢？就基于[HoloCubic_AIO这个项目](https://gitee.com/ClimbSnailQ/HoloCubic_AIO)的基础改写出了神之眼这个项目来。