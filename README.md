# ghostlight-board
Small board under a see-through miniature, to light it up.
## power supply
Circuit is exprcted to be build for 3V
### battery
CR2032 for start. It needs holder at the bottom [Keystone 3000](https://cz.mouser.com/ProductDetail/Keystone-Electronics/3000?qs=Mn60vILZNNbcLh3ZnzbtUw%3D%3D) TODO - nahradit, za větší průměr.
### regulator
[MCP1700T-3302E/TT](https://cz.mouser.com/ProductDetail/Microchip-Technology/MCP1700T-3302E-TTVAO?qs=5aG0NVq1C4y9x%252BiC%252BXtIqg%3D%3D)
[1kF](https://cz.mouser.com/ProductDetail/TAIYO-YUDEN/MBASG168AB7105KTNA01?qs=sGAEpiMZZMukHu%252BjC5l7YRGYzw8qFBFXWJL8eB%2Fc1Uk%3D)
[100nF](https://cz.mouser.com/ProductDetail/TAIYO-YUDEN/MLASU105SB7104KFNB25?qs=sGAEpiMZZMukHu%252BjC5l7YRGYzw8qFBFXza8LoeCVgcc%3D)
[4.7 kilo ohm](https://cz.mouser.com/ProductDetail/YAGEO/SR0603JR-074R7L?qs=TyBUSTk9pD8f6RpHbu07DQ%3D%3D)
## microcontroler
[STM32C011J4M6](https://cz.mouser.com/ProductDetail/511-STM32C011J4M6) (SO8 package)
### programming

## LED
### LED control
[LP5817](https://cz.mouser.com/new/texas-instruments/ti-lp5817-3-channel-rgb-led-driver/) seems to be a good way to do it.
It will connect to u-controler to microcontroler.
### LED component
While being small enough the [SB0606WC02-RGB](https://cz.mouser.com/ProductDetail/VCC/SB0606WC02-RGB?qs=rQFj71Wb1eX6WH%252BB20VtTw%3D%3D). Seems to be small and powered by 3V.
