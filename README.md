# 智能家居数据中心系统

STM32 下位机采集 + Qt5 上位机监控的智能家居系统：实时显示环境与姿态数据、越限告警、串口控制家电，并展示网络天气。

## 功能

**上位机（Qt5）**

- 串口通信（9600-8N1，QSerialPort），支持端口枚举与开关
- 按自定义协议解析数据帧，实时刷新温湿度与姿态角显示
- 阈值告警：温度高于 35℃ 高温预警、低于 10℃ 低温提醒、湿度高于 85% 提示，配合 LCD 变色与消息面板提示
- 通过串口下发控制指令，实现客厅灯、卧室灯、空调、火警警报的开关联动
- 通过 QNetworkAccessManager 异步请求网络气象 JSON 数据，展示天气、温度、湿度与风速
- 日历与当日记事本

**下位机（STM32F103C8Tx）**

- 基于 I2C 读取 MPU6050 原始加速度，用 atan2 解算俯仰角与横滚角
- 倾角超过阈值时触发蜂鸣器报警，并在上报帧中置位报警标志
- OLED（SSD1315）实时显示姿态角与四路设备状态
- GPIO 驱动四路负载，串口状态机解析上位机指令
- DHT11 温湿度驱动（单总线，PB8）

## 通信协议

上下位机使用 10 字节定长帧，帧头 `0xFF`、帧尾 `0xAA`，共 10 个字节：

| 字节 | 字段 | 说明 |
| --- | --- | --- |
| 0 | FRAME_HEAD | 固定 `0xFF` |
| 1 | TEMP | 温度 |
| 2 | HUMIDITY | 湿度 |
| 3 | ANGLE | 倾角报警标志，0 正常 / 45 越限 |
| 4 | LIGHT | 光照 |
| 5 | KETING_LIGHT | 客厅灯状态 |
| 6 | WOSHI_LIGHT | 卧室灯状态 |
| 7 | KONGTIAO_STA | 空调状态 |
| 8 | ALARM_STA | 火警警报状态 |
| 9 | FRAME_TAIL | 固定 `0xAA` |

上位机下发的控制指令（写入帧头之后的第 1 个字节）：

| 指令 | 含义 | 指令 | 含义 |
| --- | --- | --- | --- |
| `0x02` / `0x03` | 客厅灯 开 / 关 | `0x06` / `0x07` | 空调 开 / 关 |
| `0x04` / `0x05` | 卧室灯 开 / 关 | `0x08` / `0x09` | 火警警报 开 / 关 |

## 目录结构

```
qt-upper-computer/        # Qt5 上位机工程（.pro + widget.cpp / widget.h / widget.ui）
firmware/                 # STM32 下位机工程
  Core/Src、Core/Inc      # CubeMX 生成的主程序与外设初始化
  MDK-ARM/                # Keil 工程文件与自写驱动（DHT11、MPU6050、OLED、蜂鸣器、LED）
  YAO Demo.ioc            # STM32CubeMX 工程配置
```

## 快速开始

- **上位机**：用 Qt Creator 打开 `qt-upper-computer/SmartHome.pro`，构建后插入串口设备并在界面中选择端口。
- **下位机**：用 STM32CubeMX 打开 `firmware/YAO Demo.ioc` 重新生成 `Drivers/` 目录，再用 Keil MDK 打开 `firmware/MDK-ARM/YAO Demo.uvprojx` 编译下载。

## 说明

- 仓库不含 Keil/Qt 的编译产物；`firmware/Drivers/`（STM32 HAL 固件库，约 65MB）由 CubeMX 依据 `.ioc` 重新生成，未纳入仓库。
- 本项目为课程小组项目，仓库包含上位机与下位机两部分源码。
