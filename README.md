# AMPLIFIER_TDA7468_TDA7293

The amplifier is assembled on a TDA7468D running Atmega 328 (Arduino). 2 TDA7294 microcircuits work as a power amplifier.
The library for the audio processor and the control sketch program were written by Alexander Liman.
Link to his project page: http://rcl-radio.ru/?p=58165
In addition, on his website you will find many other interesting projects!
In the second part you will see what this amplifier consists of. Interesting? Follow the link:
https://youtu.be/hVsfGNAJudA

Amplifier specifications:
Dimensions: 27 X 12 X 7 cm (L.W.H.)
Weight: 1475 (without remote control)
Output power 2X18 Watt (at 8 Ohm load)
Number of inputs - 3 [RCA - 1 pc. and 3.5 mm jack - 2 pcs.]
Nonlinear Distortion Ratio no more than: 0.05% (at a frequency of 1 kHz and a power of 2X5 Watts)
Signal-to-noise ratio: 85 dB
Channel separation: 88 dB
Input Separation: 89 dB
Power Consumption: 47 Watts (Maximum)
Volume adjustment from -62 to 0 dB in 1 dB steps
Tone control: -14…+14 dB in 2 dB steps
Separate preamplifier for each input, with sensitivity adjustment from 0 to 14 dB in 2 dB steps
Balance adjustment: ±62 dB
Supply voltage: 200 - 265 Volts.
Clock accuracy: 2-3 sec/year

After 4 years of operation, the Chinese solid-state relay failed, the linear power supply turned on, the transformer (the only element in the domestically produced circuit) turned on at idle! overheated, several turns of the primary winding shorted, the winding resistance dropped and went on and on until the fuse blew out. When disassembling the transformer, I was struck by the extremely low quality of manufacture of the transformer manufactured by "ТОРЭЛ"; there was practically no insulation between the primary and the secondary (if office tape can even be considered as such. I did not dare to rewind the primary on a toroidal transformer, there were no materials, no devices, and there was no experience in such manipulations. I bought a similar one on AliExpress, and assembled the relay on BTA16 and MOC3063. At the same time, I completely rewrote the control firmware, providing it with a huge number of comments from which it is clear how it works. I eliminated unused functions, in particular, I removed the signal spectrum analyzer from all menus, leaving only 3.
