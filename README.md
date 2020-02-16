# GitLack renamed from FanController
This is a simple project of an enclosure for a 3D printer which controls the fan speed and some lights. But why the name GitLack? It's a German acronym and it means "Gehäuse Innen Temperatur & LED An/Aus Controller Krempel". Word by word translated in English it mean "Enclosure Inside Temperature & LED On/Off Controller Junk". On the other hand, the first syllable Git I want to share my project with you and I was stupid to built direct the right cabinet for my 3D printer. The last syllable for the name which I made the enclosure.

![](https://github.com/calliconfused/GitLack/blob/master/Pictures/20200127_221351.jpg)

Why this project?

So, during my experience with 3D printers I got on Xmas 2019 my new Anycubic 3D Mega. My old CTC printer got some issues during the time but more about this on a later stage. For the old printer I built a mixture of some cabinets out from the bargain basement of the well known Swedish furniture manufacturer. Cheap and already mounted, price was round about of 30 Euro with doors plus 4 LED strips (SMD 5050 with a very easy controller). The problem was that the alignment of the angle wasn't 90 degrees and if you put some weights on top of the cabinet and ... okay, the door was not really parallel opened.

So for the new printer I checked the internet for some collections of the IKEA Lack table. It will look very good in my new office room at home. BUT I want to have a very smart enclosure with a controller of the temperature inside.

My target

For the controller I wanted to use some of the parts what I already had at home and bought just new parts which I really need. So I found in some boxes 1 Arduino Uno R3 clone with some additional pins and SMD, 1 Arduino Nano close, IKEA DIODER LED strips, 12 Volt transformer, PCB plates, resistors, MOSFETs IRLZ34N, screw terminals and kilometer of wires. The parts which I just ordered during the planning phase are 2.8" TFT touchscreen display, temperature sensor GY-BME280, fan 120 mm with 4 pins, XL4015 DC to DC converter (12V to 9V for power supply of the Arduinos).

![](https://github.com/calliconfused/GitLack/blob/master/Pictures/20200129_192654.jpg)

![](https://github.com/calliconfused/GitLack/blob/master/Pictures/20200129_192650.jpg)

The MAIN problem of material

During my research how to handle an automatically temperature inside I found a big issue about the material handle. So it's really an advantage if you cool the inside if you print with PLA and if you work with ABS then you will promote warping. This is still in development to add in the program an adjustment for each material and it's own temperature range. The other problem what I have is I put all Arduinos, the Raspberry PI in the closed cabinet not to waste area outside of the enclosure. Just the 12V transformer is under the base plate. The planning is still in progress.

The other MAIN problem of the Arduino UNO

You see: 2.8" TFT touchscreen display and an Arduino Uno is a nightmare of save memory. As I started with the programming and put the my first sketch to build up the "boot" screen I reached the used memory of 60% ...........
Next problem: the touchscreen will took a lot of pins and all PWM pins are blocked for that. Okay, the card reader is not really necessary for my project but maybe for further updates. We will see.
Someone will say now: hey, why do you don't order a MEGA 2560 clone or similar with more space, speed and pin?
Yeah, you're correct! That's an easier way but my spirit is to use that parts which I already have at home. That's my personal milestone to start with I2C and put the second Arduino in the circuit (in my case the Nano). The master will collect all values from the user, pardon, from the pressed touchscreen and from the temperature sensor and give the values to the slave which will handle the logic of the fan speed and control the LEDs.

![](https://github.com/calliconfused/GitLack/blob/master/Pictures/20200127_221432.jpg)

![](https://github.com/calliconfused/GitLack/blob/master/Pictures/20200127_221713.jpg)

Okay ... currently from my side and I will give you in the next days/weeks some updates! Cheers!
