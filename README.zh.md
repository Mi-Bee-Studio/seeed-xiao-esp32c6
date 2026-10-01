# Seeed XIAO ESP32C6（感知节点主板）

[English](README.md) | [中文文档](README.zh.md)

[![Build Firmware](https://github.com/Mi-Bee-Studio/seeed-xiao-esp32c6/actions/workflows/build.yml/badge.svg)](https://github.com/Mi-Bee-Studio/seeed-xiao-esp32c6/actions/workflows/build.yml)

主板目录规范的一块板。**本仓库按"主板为根"规范组织**：

```
seeed-xiao-esp32c6/
├── README.md          # 本文件：这块板的一切硬件信息
└── <project>/         # 每个用这块板做的项目一个目录（按能力命名）
    ├── CMakeLists.txt / main/ / sdkconfig.defaults / main/idf_component.yml
    └── README.md      # 项目说明 + 编译/烧录命令
```

规范要点：

- **主板目录名** = 板子名（kebab-case），根 README 只写硬件、不写业务；
- **项目目录独立可编译**：自带完整 ESP-IDF 工程三件套（顶层 CMakeLists、
  `main/`、`sdkconfig.defaults`），`cd <project> && idf.py build` 即出固件；
- 项目间不共享代码；需要共性时先拷贝，稳定后再考虑抽组件。

### 固件基线规范（全家桶强制）

1. **看门狗：必须启用**。任务订阅 ESP-IDF TWDT 按周期喂狗；
2. **Web/API 固件升级（OTA）：硬件允许则必须提供**。本板有 WiFi、4MB flash
   放得下 OTA 双槽，故每个项目都要带板端 web 刷机能力。

| 项目 | 看门狗 | Web/API OTA |
|------|--------|-------------|
| blink | ✅ 任务订阅 TWDT（5s panic） | ✅ OTA 双槽 + `POST /ota` 流式写槽 |

---

## 板子概要

| 项目 | 值 |
|------|-----|
| 芯片 | **ESP32-C6FH4** —— RISC-V 单核 160MHz（原理图元件型号，非模组） |
| Flash | 4MB（芯片片内） |
| PSRAM | 无（512KB SRAM） |
| 无线 | **WiFi 6** 2.4GHz（802.11 b/g/n/ax）+ Bluetooth 5 (LE)、Zigbee/Thread（802.15.4） |
| USB | 原生 USB Type-C（**USB-Serial-JTAG**：控制台/烧录同一口，无桥芯片） |
| 板载 LED | **User LED = GPIO15**（单色，wiki 描述橙色）+ 红色充电指示 |
| 按键 | BOOT = **GPIO9**（按住上电进下载）、RESET |
| 天线 | 内置陶瓷天线 + **U.FL 外接座**；开关脚 GPIO14（低=内置，默认），GPIO3 拉低使能开关 |
| 尺寸 | 21 × 17.8 mm（标准 XIAO 形制） |

## 引脚位置图（USB-C 朝上，正面/元件面视角；D 编号 = XIAO 丝印）

```
                 ┌─ USB-C ─┐
       5V ◎┬───┘          ├───┬◎ D0 = GPIO0  (ADC, LP_GPIO0)
      GND ◎│              │   ◎ D1 = GPIO1  (ADC, LP_GPIO1)
      3V3 ◎│  [USER LED]  │   ◎ D2 = GPIO2  (ADC, LP_GPIO2)
     D8 ◎  │   = GPIO15   │   ◎ D3 = GPIO21 (SDIO_DATA1)
     D9 ◎  │              │   ◎ D4 = GPIO22 (I2C SDA)
    D10 ◎  │  ESP32-C6FH4 │   ◎ D5 = GPIO23 (I2C SCL)
           │              │   ◎ D6 = GPIO16 (UART TX)
           └──────────────┘   ◎ D7 = GPIO17 (UART RX)
            左列（正面）    右列（正面）

  背面 JTAG 焊盘：MTDO=GPIO7 · MTDI=GPIO5 · MTCK=GPIO6 · MTMS=GPIO4
  （MTDI/MTCK/MTMS 兼 ADC）
```

要点（引脚映射出处：[Seeed wiki Pin Map](https://wiki.seeedstudio.com/xiao_esp32c6_getting_started/)）：

- **左列**自 USB 端向下：`5V · GND · 3V3 · D8=GPIO19(SPI SCK) · D9=GPIO20(SPI MISO) · D10=GPIO18(SPI MOSI)`；
- **右列**自 USB 端向下：`D0=GPIO0 · D1=GPIO1 · D2=GPIO2 · D3=GPIO21 · D4=GPIO22(SDA) · D5=GPIO23(SCL) · D6=GPIO16(TX) · D7=GPIO17(RX)`；
- **GPIO14/3 是天线开关脚**，勿挪用（GPIO14 拉低选内置天线=默认）；
- 物理排布以 Seeed 官方 pinout 图为准；本图按"USB-C 朝上"标准视角整理。

## 注意事项

- **无 USB-UART 桥**：串口即 C6 的 USB-Serial-JTAG（控制台/烧录/JTAG 同口）；
- BOOT = GPIO9，按住插 USB / 按 RESET 进下载模式；
- C6 同时是 **802.15.4（Zigbee/Thread） radio**——后续做 Matter/Thread 节点的硬件基础；
- 512KB SRAM 无 PSRAM——大缓冲方案要谨慎（同 esp32-c3 量级）。

## 项目索引

| 项目 | 说明 |
|------|------|
| [blink](blink/README.zh.md) | 基线工程/测试固件：USER LED（GPIO15）心跳闪烁 + BOOT 交互 + web 维护页（配网/OTA）+ TWDT 看门狗 |
