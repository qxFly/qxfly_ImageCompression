### 已知问题
在clion中开发出现openssl错误No functional TLS backend was found，找在Qt安装目录，如 F:\\*\*\*\Qt\6.6.3\mingw_64\bin 下找到**windeployqt6.exe**
,进入cmd输入如下命令，参数为项目构建目录下项目可执行文件xxx.exe
```
windeployqt6.exe F:\Code\Qt\test\qxfly\cmake-build-debug\qxfly.exe
```
执行后即可正常使用