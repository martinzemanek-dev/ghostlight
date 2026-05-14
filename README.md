# ghostlight-board
Small board under a see-through miniature, to light it up.
## power supply
Circuit is exprcted to be build for 3V
### battery
CR2032 for start. It needs holder at the bottom [BAT-HLD-001](https://cz.mouser.com/ProductDetail/712-BAT-HLD-001)
### miscelenious
[1kF](https://cz.mouser.com/ProductDetail/TAIYO-YUDEN/MBASG168AB7105KTNA01?qs=sGAEpiMZZMukHu%252BjC5l7YRGYzw8qFBFXWJL8eB%2Fc1Uk%3D)
[100nF](https://cz.mouser.com/ProductDetail/TAIYO-YUDEN/MLASU105SB7104KFNB25?qs=sGAEpiMZZMukHu%252BjC5l7YRGYzw8qFBFXza8LoeCVgcc%3D)
[10uF](https://cz.mouser.com/ProductDetail/Murata-Electronics/GRM155C80G106ME18D?qs=doiCPypUmgHaqNm%2FsBDkxw%3D%3D)
[4.7 kilo ohm](https://www.gme.cz/v/1496357/yageo-r0603-4k7-01w-5-smd-rezistor)
### regulator
[MCP1700T-3302E/TT](https://cz.mouser.com/ProductDetail/Microchip-Technology/MCP1700T-3302E-TTVAO?qs=5aG0NVq1C4y9x%252BiC%252BXtIqg%3D%3D)
Needs 1kF on input and output.
## microcontroler
[STM32C011J4M6](https://cz.mouser.com/ProductDetail/511-STM32C011J4M6) (SO8 package).
Needs 100nF between VCC and GND and 2 4.7kOhm resitors for I2C.
### programming

## LED
### LED control
[LP5817](https://cz.mouser.com/new/texas-instruments/ti-lp5817-3-channel-rgb-led-driver/) seems to be a good way to do it.
It will connect to u-controler to microcontroler. Communicates on I2C. Needs 100nF between Vcc and GND.
### LED component
While being small enough the [SB0606WC02-RGB](https://cz.mouser.com/ProductDetail/VCC/SB0606WC02-RGB?qs=rQFj71Wb1eX6WH%252BB20VtTw%3D%3D). Seems to be small and powered by 3V.

## Accelerometer
For the control purposes I think the best solution will be an accelerometer. From the research the [LIS3DHTR](https://cz.mouser.com/ProductDetail/511-LIS3DHTR) seems to be a grat opinion. Its povered by up to 3.6V so the LDO should supply sufficient and safe power. It also communicates over I2C alongside LP5817 and doesn't collide in address. It will take over last free pins for interrupts. It also provides three analog outpusts, which can substitute the pins on the programming Ring for debuginng. Needs 100nF and 10uF between Vcc and GND.
