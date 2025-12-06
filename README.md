# LY Serial Tool

一个基于Qt6的串口通信工具，集成了串口终端、串口收发器和IEC61850客户端功能，专为电子设备测试和通信协议分析而设计。

## 功能特性

### 串口终端 (Serial Terminal)
- 支持多种波特率、数据位、校验位和停止位配置
- 终端式界面，支持命令行交互
- 右键菜单支持复制、粘贴和清空操作
- 实时显示串口数据

### 串口收发器 (Serial Transceiver)
- 十六进制和ASCII格式发送/接收
- 自动发送功能，可配置发送间隔
- 帧计数和字节统计
- 时间戳显示选项
- 自定义帧分隔符
- 数据验证功能

### IEC61850客户端 (IEC61850 Client)
- 完整的IEC61850协议栈支持
- 数据模型浏览和导航
- 数据读写操作
- 数据集管理
- 报告控制
- 控制服务

## 系统要求

- **操作系统**: Linux (支持多架构: amd64, arm64, loong64, riscv64)
- **Qt版本**: Qt 6.x
- **编译器**: 支持C++17标准的编译器
- **依赖库**:
  - qt6-base-dev
  - qt6-l10n-tools
  - qt6-tools-dev
  - qt6-tools-dev-tools
  - qt6-svg-dev
  - qt6-serialport-dev

## 安装方法

### 从源码编译

```bash
git clone https://github.com/lhdjply/ly-serial-tool.git
cd ly-serial-tool
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
```

### 使用Debian包

从[Releases](https://github.com/lhdjply/ly-serial-tool/releases)页面下载适合您架构的.deb包，然后安装：

```bash
sudo dpkg -i ly-serial-tool_*.deb
```

## 使用说明

### 串口终端
1. 启动应用程序
2. 选择"串口终端"标签页
3. 配置串口参数（端口、波特率等）
4. 点击"打开"按钮连接串口
5. 在终端中输入命令，按回车发送

### 串口收发器
1. 选择"串口收发器"标签页
2. 配置串口参数并连接
3. 在发送区域输入数据，可选择十六进制格式
4. 点击"发送"按钮或启用自动发送
5. 接收区域将显示收到的数据

### IEC61850客户端
1. 选择"IEC61850客户端"标签页
2. 点击连接按钮，输入服务器地址和端口
3. 连接成功后，浏览左侧的数据模型树
4. 双击数据对象查看详情
5. 使用读取/写入按钮进行数据操作

## 界面语言

应用程序支持以下语言：
- 中文 (简体)
- English

语言会根据系统语言自动切换。

## 开发信息

### 项目结构
```
ly-serial-tool/
├── src/                    # 主程序源码
├── myresource/            # 资源文件和UI代码
│   ├── inc/               # 头文件
│   ├── src/               # 源文件
│   └── translations/      # 翻译文件
├── third_party/           # 第三方库
│   ├── libiec61850/       # IEC61850协议栈
│   └── qtermwidget/       # 终端控件
├── assets/                # 资源文件
│   ├── icons/             # 图标
│   └── desktop/           # 桌面文件
└── debian/                # Debian打包文件
```

### 编译选项
- `TRANSLATION_RESOURCE_EMBEDDING`: 启用翻译资源嵌入
- 支持C++17标准
- 自动MOC、RCC和UIC处理

## 贡献指南

欢迎提交Issue和Pull Request！

1. Fork 本仓库
2. 创建您的特性分支 (`git checkout -b feature/AmazingFeature`)
3. 提交您的更改 (`git commit -m 'Add some AmazingFeature'`)
4. 推送到分支 (`git push origin feature/AmazingFeature`)
5. 打开一个Pull Request

## 许可证

本项目采用MIT许可证 - 查看 [LICENSE](LICENSE) 文件了解详情。

## 作者

lhdjply <lhdjply@126.com>

## 致谢

- [Qt6](https://www.qt.io/) - 跨平台应用程序框架
- [libiec61850](https://github.com/mz-automation/libiec61850) - IEC61850协议栈
- [QTermWidget](https://github.com/lxqt/qtermwidget) - 终端控件
