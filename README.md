# ghostlight-board
Small board under a see-through miniature, to light it up.
## power supply
Circuit is exprcted to be build for 3V
### battery
CR2032 for start. It needs holder at the bottom [Keystone 3000](https://cz.mouser.com/ProductDetail/Keystone-Electronics/3000?qs=Mn60vILZNNbcLh3ZnzbtUw%3D%3D)
### regulator
[MCP1700T-3302E/TTVAO](https://cz.mouser.com/ProductDetail/Microchip-Technology/MCP1700T-3302E-TTVAO?qs=5aG0NVq1C4y9x%252BiC%252BXtIqg%3D%3D) + 2 condensators [MBASG168AB7105KTNA01](https://cz.mouser.com/ProductDetail/TAIYO-YUDEN/MBASG168AB7105KTNA01?qs=sGAEpiMZZMukHu%252BjC5l7YRGYzw8qFBFXWJL8eB%2Fc1Uk%3D)
## microcontroler
STM32C011J4M6 (SO8 package) or the STM32C011F4P6
### programming

## LED
### LED control
[LP5817](https://cz.mouser.com/new/texas-instruments/ti-lp5817-3-channel-rgb-led-driver/) seems to be a good way to do it.
It will connect to u-controler to microcontroler.
### LED component
While being small enough the [SB0606WC02-RGB](https://cz.mouser.com/ProductDetail/VCC/SB0606WC02-RGB?qs=rQFj71Wb1eX6WH%252BB20VtTw%3D%3D). Seems to be small and powered by 3V.
