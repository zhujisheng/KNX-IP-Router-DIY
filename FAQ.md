# FAQ

## KNX board

####  *Where can I buy a KNX Bridge board?*

You can [buy it on taobao](https://item.taobao.com/item.htm?spm=a2oq0.12575281.0.0.50111debqg4Tao&ft=t&id=599299965979).

Or if you are not in mainland China, you can email your needs information to <zhujisheng@gmail.com>.

#### *What's the difference between KNX Bridge Board type A and type B?*

They communicate with `knxd` running on a raspberry pi (or computer) via different hardware interface.

HaChina's KNX board(shield) type A can connected to raspberry pi via USB. Or it can be a shield plugged into [Nodemcu-32s](https://docs.ai-thinker.com/en/esp32/boards/nodemcu_32s)'s 38-pin connector, and communicates with `knxd` using TCP on the Wifi.

HaChina's KNX board(shield) type B is a shield which can be directly plugged into the 40-pin connector on raspberry pi(ZeroW/3B/3B+/4B) board.

#### *Can I make a KNX Bridge Board myself?*

Yes, of course. You can base it on ncn5120, or any other IC supporting KNX Bus.

## knxd docker

#### *Must knxd runs on raspberry pi?*

No.

You can run it on different architecture computers including amd64/i386/armv7/aarch64.

#### *If my computer is not armv7, where can I get the docker image?*

The docker images are:
- `zhujisheng/armv7-addon-knxd`
- `zhujisheng/aarch64-addon-knxd`
- `zhujisheng/i386-addon-knxd`
- `zhujisheng/amd64-addon-knxd`
It is easy to distinguish them from the name. Replace with appropriate images in `docker run` command.

#### *Must knxd runs as a docker container or HomeAssistant add-on?*

No.

Knxd is an open source software. You can compile it from source, and run it on any Linux host. 

## HomeAssistant Add-on Knxd

#### *What's the different between knxd add-ons in `da-anda` and `zhujisheng`'s add-on repository?*

`zhujisheng`'s repository is forked from `da-anda`'s, and add an `image` config. 

If you install knx add-on from `zhujisheng`'s repository, you will **download** the add-on(docker) image. However, you will **build** the image locally from Dockerfile if you install knx add-on from  `da-anda`'s repository.

#### *How can I config KNX devices in homeassistant?*

Once you have set up the IP Router properly, you can add KNX devices to HomeAssistant， by using [knx integration](https://www.home-assistant.io/integrations/knx/).
