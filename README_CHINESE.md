
# 自己动手制作KNX IP Router

## 摘要

KNX的IP Router设备能够连接KNX网络与IP网络，从而在IP网络上接收和发送KNX总线上的信号。

<img src="images/iproute.png" width="70%">

但IP Router设备比较昂贵；并且一些IP Router不具备Wifi连接功能，造成工程部署上的不方便。本项目利用开源的软件与简单的硬件，自己搭建一个方便、稳定、便宜的IP Router设备。

## 思路

<img src="images/iproutediy.png" width="90%">

设备的主体硬件，选择使用[树莓派](https://www.raspberrypi.org/)，在上面运行开源软件[knxd](https://github.com/knxd/knxd)。

树莓派上没有直连KNX总线的接口，因此，我们还需要制作一块硬件板，一方面能连接到KNX总线，另一方面连接到树莓派上，使树莓派可以收发KNX总线上的信号。

## KNX连接板

<img src="images/knxboard.png" width="25%">

KNX连接板基于[ncn5120芯片](https://www.onsemi.com/pdf/datasheet/ncn5120-d.pdf)完成与KNX总线的通讯。

另一方面，KNX板可以与树莓派通讯，可以采用三种连接方式：

1. 对插模式
 
    KNX板作为树莓派的扩展板，直接对插到树莓派的40针GPIO引脚（HAChina KNX扩展版type B）

    **这种连接方式，你必须在操作系统上打开primary UART.**

    *[How can I enable the primary UART on PI OS?](FAQ.md#how-can-i-enable-the-primary-uart-on-pi-os)*

    *[How can I enable the primary UART on HassOS?](FAQ.md#how-can-i-enable-the-primary-uart-on-hassos)*

2. USB模式

    KNX板通过USB口，连接到树莓派（HAChina KNX扩展版type A）

3. TCP模式

    KNX板作为NodeMCU-32s的扩展版，对插到NodeMCU32s上；与树莓派直接通过WIFI网络通讯（HAChina KNX扩展版type A）

*[Where can I buy a KNX Bridge Board?](FAQ.md#where-can-i-buy-a-knx-bridge-board)*

*[What's the difference between KNX Bridge Board type A and type B?](FAQ.md#whats-the-difference-between-knx-bridge-board-type-a-and-type-b)*

*[Can I make a KNX Bridge Board myself?](FAQ.md#can-i-make-a-knx-bridge-board-myself)*

## 在树莓派上运行`knxd`

推荐使用：
- Raspberry Pi ZeroW
- Raspberry Pi 3B
- Raspberry Pi 3B+
- Raspberry Pi 4B

#### docker运行模式

<img src="images/docker.png" width="20%">

- 安装docker
    + 操作系统： `Pi OS`
    + 操作用户： `pi`
    + 安装docker：

        `curl -fsSL get.docker.com | sudo sh`

    + 将pi用户加入docker执行组

        `sudo usermod -aG docker pi`

- 配置文件`/etc/knxd.ini`

    `knxd.ini`文件完整格式说明参见knxd项目的[相关说明](https://github.com/knxd/knxd/blob/main/doc/inifile.rst)

    ```ini
    # an example of knxd.ini
    [main]
    addr = 0.0.1
    client-addrs = 0.0.2:10
    connections = server,A.tcp,interface
    logfile = /dev/stdout

    [A.tcp]
    server = knxd_tcp

    [server]
    server = ets_router
    tunnel = tunnel
    router = router
    discover = true
    name = knxd

    [interface]
    driver = ncn5120

    # USB link to KNX Board
    device = /dev/ttyUSB0
    baudrate = 19200

    # 40-pin link to KNX Board
    #device = /dev/serial0
    #baudrate = 19200

    # TCP link to KNX Board
    #ip-address = 192.168.1.3
    #dest-port = 5120
    ```

    *当你使用3种不同的方式连接KND板时，配置方式参见样例中`[interface]`部分的注释*

- 在后台运行docker `zhujisheng/armv7-addon-knxd`

    ```
    docker run --rm -d \
    --name knxd --network="host" \
    --tmpfs /etc/cont-init.d \
    --device=/dev/ttyUSB0 \
    -v /etc/knxd.ini:/etc/knxd.ini:ro \
    zhujisheng/armv7-addon-knxd 
    ```

    如果KNX板采用对插模式与树莓派连接，修改运行参数为`--device=/dev/serial0`；如果采用TCP连接模式，不必设置`--device`运行参数

*[Must knxd runs on raspberry pi?](FAQ.md#must-knxd-runs-on-raspberry-pi)*

*[If my computer is not armv7, where can I get the docker image?](FAQ.md#if-my-computer-is-not-armv7-where-can-i-get-the-docker-image)*

*[Must knxd runs as a docker container or HomeAssistant add-on?](FAQ.md#must-knxd-runs-as-a-docker-container-or-homeassistant-add-on)*

#### HomeAssistant add-on运行模式

<img src="images/homeassistant.png" width="20%">

- 安装

    增加add-on仓库`https://github.com/da-anda/hass-io-addons`或`https://github.com/zhujisheng/hass-io-addons`

    安装其中的add-on：`KNXD daemon`

    *[What's the difference between knxd add-ons in da-anda and zhujisheng's add-on repository?](FAQ.md#whats-the-difference-between-knxd-add-ons-in-da-anda-and-zhujishengs-add-on-repository)*

- 配置

    ```yaml
    address: 0.0.1
    client_address: '0.0.2:8'
    interface: ncn5120
    device: /dev/ttyAMA0
    log_error_level: warning
    usb_filters: single
    custom_config: |-
      [main]
      addr = 0.0.1
      client-addrs = 0.0.2:10
      connections = server,A.tcp,interface
      logfile = /dev/stdout

      [A.tcp]
      server = knxd_tcp

      [server]
      server = ets_router
      tunnel = tunnel
      router = router
      discover = true
      name = knxd

      [interface]
      driver = ncn5120

      # USB link to KNX Board
      device = /dev/ttyUSB0
      baudrate = 19200

      # 40-pin link to KNX Board
      #device = /dev/serial0
      #baudrate = 19200

      # TCP link to KNX Board
      #ip-address = 192.168.1.3
      #dest-port = 5120
    ```
    *配置有效部分为`custom_config`，其余没有作用但必须保留（add-on将它们设置成了必配项）。*

    *当使用3种不同的方式连接KND板时，配置方式参见样例中`[interface]`部分的注释*

    *[How can I config KNX devices in homeassistant?](FAQ.md#how-can-i-config-knx-devices-in-homeassistant)*