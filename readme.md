# Spirometer
use a pressure sensor to do spirometery

## MS4525DO Pressure Sensor 
### sensor type:
px4airspeedv1.1

### writings on sensor:
4525D 5AI
001D 1923
0118 12800

### link to sensor:
https://www.te.com/usa-en/product-CAT-BLPS0002.html#mdp-tabs-content

### link to pixhawk module:
https://hobbyking.com/en_us/pixhawk-digital-airspeed-sensor-w-pitot-tube.html
https://docs.px4.io/master/en/sensor/airspeed.html

### link to code:
https://os.mbed.com/users/epremeaux/code/MS4525_pitot_tube/

### some info:
- Output Interface  I²C or SPI
- Overpressure  300 psi
- Pressure (psi) 1, 2, 5, 15, 30, 50, 100, 150
- Output/Span  14 bit ADC
- Board Level Pressure Sensor Accuracy  ±0.25% Span
- Interface Type  I2C (Addr.0x28H), I2C (Addr.0x36H), I2C (Addr.0x46H), I2C (Addr.0x48H), I2C (Addr.0x49H), I2C (Addr.0x4AH), I2C (Addr.0x4BH), I2C (Addr.0x4CH), I2C (Addr.0x4DH), I2C (Addr.0x4EH), I2C (Addr.0x4FH), I2C (Addr.0x50H), I2C (Addr.0x51H), SPI
- high performance digital output pressure (14bit) and temperature (11bit) transducer 


## TODO
- [ ] use arduino i2c lib and wrappers
- [ ] test new code
- [ ] add venturi tube measurments
- [ ] use datasheet formulas to get psi as correctly as possible
- [ ] figure out a way to calibrate it
- [x] design a frame for system in CAD
- [ ] explore other methods such as using a fan 