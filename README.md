cc2530duino
========

This is 6Lowpan library (Chip CC2530 802.15.4 soc) for arduino using SLIP protocol via Serial port

Installations
========
**1. Clone this project:**

```bash
git clone https://github.com/stevenliy/6lowpanduino
cd 6lowpanduino
```

**1. Program CC2530 Shield:**

**2. Import arduino library and run example:**

Example read DHT11 and send to [thingspeak.com](http://thingspeak.com)
=========
- ```espduino/examples/thingspeak/thingspeak.ino```
- Using DHT11 library from: [https://github.com/RobTillaart/Arduino](https://github.com/RobTillaart/Arduino)

![](images/thingspeak.png)

Example send pushbullet push notification:
=============
[http://tuanpm.net/pir-motion-detect-send-pushbullet-push-notification-with-esp8266/](http://tuanpm.net/pir-motion-detect-send-pushbullet-push-notification-with-esp8266/)


Example for MQTT client
=======
TBD

Authors:
=================
[Yong Li](steven.liyong@gmail.com)


LICENSE - "MIT License"
==================
Copyright (c) 2014-2015 [Tuan PM](https://twitter.com/tuanpmt)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
