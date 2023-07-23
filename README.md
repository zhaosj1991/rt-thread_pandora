[RT-Thread工具手册](https://www.bookstack.cn/books/RT-Thread-tool)

https://www.bookstack.cn/read/RT-Thread-tool/0.md



## 开发软件安装

1. keil
2. rthread env： https://www.rt-thread.org/download.html#download-rt-thread-env-tool



## keil + vscode 开发环境搭建

### 配置环境

最佳实践：

- keil用于创建、配置、编译、下载、单步调试等；
- vscode负责编辑代码、走读代码等；



1. keil正常安装；

2. vscode安装 “Keil Assistant” 插件，配置其中的 “MDK UV4.exe path” 参数，如图

   ![image-20230723105638311](C:\Users\zhaosj\Desktop\pandora\README.assets\image-20230723105638311.png)

   Keil Assistant安装后Exporer部分会多一个分栏 “Keil uVision Project“，点击右侧按钮可以打开 .uvprojx keil工程文件。

3. 


### 问题

1. vscode打开后无法识别头文件？

   vscode通过openfolder打开所需所有文件的根目录，注意不是uvprojx文件所在目录；然后再通过keil uvision project分区打开uvprojx项目；

2. 



## rt-thread软件开发实践（基于stm32l475-atk-pandora）

### wifi应用软件

1. 下载最新的rt-thread软件包：https://gitee.com/rtthread/rt-thread，并解压

2. 进入`rt-thread-v4.1.0\bsp\stm32\stm32l475-atk-pandora`目录，右键打开env终端工具 `ConEmu Here`。执行 `menuconfig` ，然后添加 wifi 模块配置。

   ![image-20230723123953443](C:\Users\zhaosj\Desktop\pandora\README.assets\image-20230723123953443.png)

3. 然后执行 `scons --target=mdk5` ，生成MDK工程；

4. keil打开 `rt-thread-v4.1.0\bsp\stm32\stm32l475-atk-pandora\project.uvprojx` 工程

   ![image-20230723124244472](C:\Users\zhaosj\Desktop\pandora\README.assets\image-20230723124244472.png)

5. 参考 `keil + vscode 开发环境搭建` 章节，通过vscode打开项目，修改 `Application/main.c` 进行软件开发；

