# ifndef _WEBSERVER_H
# define _WEBSERVER_H

 void initWebServer();
 void webRoot(); //处理网站根目录"/"的访问请求
 void webNotFound(); //设置处理404情况的函数'webUserRequest'
 void webRead_fileManagement(); //文件管理-向网页显示文件系统里的文件
 void respondOK();
 void webFileUpload();
 void webRootPost();
 void webRead_enableBmp(); //启用Bmp文件
 void webRenameFile();
 void webRead_deleteFile();
 void webRead_stopBmp(); //停用Bmp文件
 void webRESET();
String getContentType(String filename);
 void webRead_updateButton(); //文件上传超过上限的按钮
 void webSetOutputPower(); //表单提交 当前发射功率
 void webRead_setOutputPower(); //获取当前发射功率
 void webRead_clockQSJG(); //获取全刷间隔
 void webClockJZJG(); //时钟校准间隔表单提交
 void webRead_clockJZJG(); //获取时钟校准间隔
 void webClockCompensate(); //时钟补偿值表单提交
 void webRead_clockCompensate(); //获取时钟补偿值
 void webPut_albumAuto(); //图片随机播放 写入状态
 void webRead_albumAuto(); //图片随机播放 获取状态
 void webPut_clockCalibrationState(); //切换时钟强制校准状态
 void webRead_clockCalibrationState(); //获取时钟强制校准状态
 void webPut_clockDisplayState(); //切换时钟显示模式-极简&显示日期
 void webRead_clockDisplayState(); //获取时钟显示模式-极简&显示日期
 void webPut_setRotation(); //切换屏幕旋转方向
 void webRead_setRotation(); //获取屏幕旋转方向
 void webPut_batDisplayType(); //切换电量显示类型
 void webRead_batDisplayType(); //获取电量显示类型
 void webPut_nightUpdata(); //切换夜间更新信息
 void webRead_nightUpdata(); //获取夜间更新信息
 void webPut_sdInit(); //SD卡挂载卸载
 void webRead_sdInit(); //SD卡挂载卸载
 void webRead_info(); //ajax请求系统信息
 void webRead_updataAddress(); //ajax固件更新地址
 void webRead_city(); //ajax请求天气
 void webRead_staNameIp(); //ajax请求STA名字和ip
 void webReadInAWord(); //读取当前的模式 0-联网获取 1-自定义 2-倒计时模式 3-B粉获取
 void webInAWord(); //自定义一句话的提交表单处理
 void webFileUpload();
 void webWeatherPost();
 void webRead_scanWifi(); //扫描周边wifi
 void webRootPost();
 void webRoot(); //处理网站根目录"/"的访问请求
 void webNotFound(); //设置处理404情况的函数'webUserRequest'
 void respondOK();
 void webClockQSJG(); //时钟全刷表单提交
 void webRead_fileSync(); //文件系统剩余内存
boolean getSystemFile(String fileName);

# endif