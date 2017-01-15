# vario
Hanggliding/paragliding tiny variometer based on the STMicro LPS25H sensor.

### attiny85 code
Vario emits sounds only.

### Arduino code
It is possible to have debug on the serial port.

### example of I2C init sequence
s - start
S - stop
a - ack
n - nack

s 1011 1010a 0000 1111a s1011 1011a 1011 1101n S
s BAa 0fa s BBa BDn S

