# vario
Hanggliding/paragliding tiny variometer based on the STMicro LPS25H sensor.

### attiny85 code
Vario emits sounds only.

### Arduino code
It is possible to have debug on the serial port.

### KiCad files

I'm not very experienced in exporting kicad files, something may be missing.
Please file a bug or send me an email if something need to be added/removed, thanks.

### ToDo

se il device si inizializza male, es quando la tensione sale lentamente, allora resta bloccato fino a reset, che non essendo collegato non funziona.
- pulsante a reset, e resetta tutti i device

### example of I2C init sequence
s - start
S - stop
a - ack
n - nack

s 1011 1010a 0000 1111a s1011 1011a 1011 1101n S
s BAa 0fa s BBa BDn S

