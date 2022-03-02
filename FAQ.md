# FAQ

## KNX board

#### *Where can I buy a KNX Bridge board?*

You can [buy it on taobao](https://item.taobao.com/item.htm?spm=a21dvs.23580594.0.0.621e3d0dGzEzHR&ft=t&id=669724322918).

Or if you are not in mainland China, you can email your needs information to <q648993779@gmail.com>.

#### *What's the difference between KNX Bridge Board type A and type B?*

They communicate with `knxd` running on a raspberry pi (or computer) via different hardware interface.

Both HaChina's KNX board(shield) type A and type B can connected to raspberry pi via USB.

Type A can also be a shield plugged into [Nodemcu-32s](https://docs.ai-thinker.com/en/esp32/boards/nodemcu_32s)'s 38-pin connector, and communicates with `knxd` using TCP on the Wifi.

Type B is a shield which can be directly plugged into the 40-pin connector on raspberry pi(ZeroW/3B/3B+/4B) board.

#### *Can I make a KNX Bridge Board myself?*

Yes, of course. You can base it on ncn5120, or any other IC supporting KNX Bus.

#### *How can I enable the primary UART on PI OS?*

1. Start raspi-config: `sudo raspi-config`.
2. Select option 3 - Interface Options.
3. Select option P6 - Serial Port.
4. At the prompt `Would you like a login shell to be accessible over serial?` answer 'No'
5. At the prompt `Would you like the serial port hardware to be enabled?` answer 'Yes'
6. Exit raspi-config and reboot the Pi for changes to take effect.

When you have these done, `/dev/ttyS0` and `/dev/serial0` will be the primary UART.

#### *How can I enable the primary UART on HassOS?*

1. Edit file `config.txt`.

    It's on `hassos-boot` partition of the tf card. You can find it by pluging the tf card into a Windows computer, or find it under `/mnt/boot/` when you login to HassOS.

2. Uncomment `enable_uart=1` in `config.txt`, and save it.
3. Reboot the Pi for changes to take effect.

When you have these done, `/dev/ttyS0` and `/dev/serial0` will be the primary UART.

## knxd docker

#### *Must knxd runs on raspberry pi?*

No.

You can run it on different architecture computers including amd64/i386/armv7/aarch64.

#### *If my computer is not armv7, where can I get the docker image?*

The docker images are:
- `zhujisheng/armv7-addon-knxd`(It's for 32bit OS on RaspberryPi)
- `zhujisheng/armhf-addon-knxd`(It's for RaspberryPi Zero W)
- `zhujisheng/aarch64-addon-knxd`(It's for 64bit OS on RaspberryPi)
- `zhujisheng/i386-addon-knxd`
- `zhujisheng/amd64-addon-knxd`
It is easy to distinguish them from the name. Replace with appropriate images in `docker run` command.

#### *Must knxd runs as a docker container or HomeAssistant add-on?*

No.

Knxd is an open source software. You can compile it from source, and run it on any Linux host. 

## HomeAssistant Add-on Knxd

#### *What's the difference between knxd add-ons in `da-anda` and `zhujisheng`'s add-on repository?*

`zhujisheng`'s repository is forked from `da-anda`'s, and add an `image` config. 

If you install knx add-on from `zhujisheng`'s repository, you will **download** the add-on(docker) image. However, you will **build** the image locally from Dockerfile if you install knx add-on from  `da-anda`'s repository.

## Control KNX devices via knxd

#### *How can I control KNX devices in HomeAssistant?*

Once you have set up the IP Router properly, you can add KNX devices to HomeAssistant, by using [knx integration](https://www.home-assistant.io/integrations/knx/).

#### *How can I control KNX devices in Node-RED?*

Once you have set up the IP Router properly, you can control KNX devices by using [`node-red-contrib-knx-ultimate`](https://flows.nodered.org/node/node-red-contrib-knx-ultimate).
