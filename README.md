# Ghostlight
Small board under a see-through miniature, to light it up. The board is powered by a battery and the light is programmable.
## Power supply
Circuit is expected to be build for 3V
### Battery
CR2032 for start. It needs holder at the bottom [BAT-HLD-001](https://cz.mouser.com/ProductDetail/712-BAT-HLD-001)
### Miscelenious
[MCP1700T-3302E/TT](https://cz.mouser.com/ProductDetail/Microchip-Technology/MCP1700T-3302E-TTVAO?qs=5aG0NVq1C4y9x%252BiC%252BXtIqg%3D%3D)
[1kF](https://cz.mouser.com/ProductDetail/TAIYO-YUDEN/MBASG168AB7105KTNA01?qs=sGAEpiMZZMukHu%252BjC5l7YRGYzw8qFBFXWJL8eB%2Fc1Uk%3D)
[100nF](https://cz.mouser.com/ProductDetail/TAIYO-YUDEN/MLASU105SB7104KFNB25?qs=sGAEpiMZZMukHu%252BjC5l7YRGYzw8qFBFXza8LoeCVgcc%3D)
[10uF](https://cz.mouser.com/ProductDetail/Murata-Electronics/GRM155C80G106ME18D?qs=doiCPypUmgHaqNm%2FsBDkxw%3D%3D)
[4.7 kilo ohm](https://www.gme.cz/v/1496357/yageo-r0603-4k7-01w-5-smd-rezistor)
## microcontroler
[STM32C011J4M6](https://cz.mouser.com/ProductDetail/511-STM32C011J4M6) (SO8 package) or [STM32C011J6M6](https://cz.mouser.com/cs/ProductDetail/511-STM32G030J6M6).

## LED
### LED control
[LP5817](https://cz.mouser.com/new/texas-instruments/ti-lp5817-3-channel-rgb-led-driver/) seems to be a good way to do it.
It will connect to u-controler to microcontroler. Communicates on I2C. Needs 100nF between Vcc and GND.
### LED component
While being small enough the [SB0606WC02-RGB](https://cz.mouser.com/ProductDetail/VCC/SB0606WC02-RGB?qs=rQFj71Wb1eX6WH%252BB20VtTw%3D%3D). Seems to be small and powered by 3V.

## Accelerometer
[LIS3DHTR](https://cz.mouser.com/ProductDetail/511-LIS3DHTR) seems to be a grat opinion. Its povered by up to 3.6V so the LDO should supply sufficient and safe power. It also communicates over I2C alongside LP5817 and doesn't collide in address. It will take over last free pins for interrupts. It also provides three analog outpusts, which can substitute the pins on the programming Ring for debuginng.

## Program
### Programing 
The board is programmeable with [STLINK-V3](https://cz.mouser.com/ProductDetail/511-STLINK-V3MINIE) with connected battery or powersupply. The programming pins are connected to ring of pins (Adapter between ring, programmer and power supply is currently work in progress). 

Here are pin functions going clockwise from pin 1.
| Pin | Function |
| --- | --- |
| 1 | Vdd (Power detection) |
| 2 | ST_LINK_SWCLK |
| 3 | ST_LINK_SWDIO |
| 4 | Unused |
| 5 | Unused |
| 6 | RESET |
| 7 | V_BATT |
| 8 | GND |

Programmer connected this way allows programming using STM32CubeProgrammer.
V_BATT is a dirrect connection to + of the battery.
It is also possible to connect external 3.3V power supply here, but only, when battery is disconnected.

### Logic

Current program rotates thrue multiple modes.
Upon Startup the device shortly blinks red and than is switchis to first program, which is shifts over few colours as a lavalamp.

After start-up, the program also starts checking on accelerometer status. 
Program goes thrue two fases, so the color would not change on accident.
First the program checks, if the device is tilted 90 degrees using X and Y acceleration from the accelerometer.
Than it waits, till the board is fully flipped and than it changes color.
All measurements are triggered by change interrupt from the LI3DH.
When Program color change is noticed, the program cycles true colors in following order.

| Order | Color |
| --- | --- |
| 1. | Lavalamp (slowly shifting colors) |
| 2. | Red |
| 3. | Green |
| 4. | Blue |
| 5. | Magenta |
| 6. | Yellow |
| 7. | Cyan |
| 8. | While |
| 9. | Off |
| 10. | Color visualization of the accelerometer output |

When the program displays static color it waits for an interrupt, before even checking the accelerometer.


