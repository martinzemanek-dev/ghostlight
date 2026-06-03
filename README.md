# Ghostlight
Small board under a see-through miniature, to light it up. The board is powered by a battery and the light is programmable.
## Power supply
Circuit is expected to be build for 3V
### Battery
CR2032 for start. It needs holder at the bottom [BAT-HLD-001](https://cz.mouser.com/ProductDetail/712-BAT-HLD-001)
### Regulator
[MCP1700T-3302E/TT](https://cz.mouser.com/ProductDetail/Microchip-Technology/MCP1700T-3302E-TTVAO?qs=5aG0NVq1C4y9x%252BiC%252BXtIqg%3D%3D)
[1kF](https://cz.mouser.com/ProductDetail/TAIYO-YUDEN/MBASG168AB7105KTNA01?qs=sGAEpiMZZMukHu%252BjC5l7YRGYzw8qFBFXWJL8eB%2Fc1Uk%3D)
[100nF](https://cz.mouser.com/ProductDetail/TAIYO-YUDEN/MLASU105SB7104KFNB25?qs=sGAEpiMZZMukHu%252BjC5l7YRGYzw8qFBFXza8LoeCVgcc%3D)
[4.7 kilo ohm](https://www.gme.cz/v/1496357/yageo-r0603-4k7-01w-5-smd-rezistor)
## microcontroler
[STM32C011J4M6](https://cz.mouser.com/ProductDetail/511-STM32C011J4M6) (SO8 package)
### programming

## LED
### LED control
[LP5817](https://cz.mouser.com/new/texas-instruments/ti-lp5817-3-channel-rgb-led-driver/) seems to be a good way to do it.
It will connect to u-controler to microcontroler.
### LED component
While being small enough the [SB0606WC02-RGB](https://cz.mouser.com/ProductDetail/VCC/SB0606WC02-RGB?qs=rQFj71Wb1eX6WH%252BB20VtTw%3D%3D). Seems to be small and powered by 3V.

## Program
### Programing 
The board is programmeable with [STLINK-V3](https://cz.mouser.com/ProductDetail/511-STLINK-V3MINIE) with connected battery or powersupply. The programming pins are connected to ring of pins (Adapter between ring, programmer and power supply is currently work in progress). 

Here are pin functions going clockwise from pin 1.
| Pin | Function |
| --- | --- |
| 1 | Vdd (Power detection) |
| 2 | ST_LINK_SWCLK |
| 3 | ST_LINK_SWDIO |
| 4 | GPIO |
| 5 | GPIO |
| 6 | RESET |
| 7 | Unused |
| 8 | GND |

While this programmer the program cannot run, beacuse I2C_SCK overlaps with ST_LINK_SWCLK connection. this will be fixed in future version. 

Programmer connected this way allows programming using STM32CubeProgrammer, while connecting under reset.

Because of the conflict with I2C debugging currently is not possible, so the program was not yet downsized to fit debugging flags.

### Logic

Upon restart program reads flash and sets a color of the LED for this cycle. The colors selected are blue, red and yellow, according the alignments in game talisman. Than program communicates over I2C to setup LP5817 and selects color.

#### I2C

I2C uses DMA to comunicate. For I2C the standart HAL library is used.

##### LP5817
The logic is split into two functions. 

```LP5817_init``` initializes all registers of the LP5817 to be able to display a color. This setup resets the driver, sets up the current limiters and than tells the driver to update its configuration.

```LP5817_setColor``` sets the channels according the input parameters.
