# airsoft-replica-bomb
 Replica airsoft bomb made with arduino, wire and code defuse, configurable gameplay options


## required libraries
1. adafruit keypad
2. Wire.h
3. LiquidCrystal_I2C.h
4. SPI.h

WIRING DIAGRAM COMING SOON

# REQUIRED COMPONENTS TO BUILD YOUR OWN:
## essentials
1. POWER SOURCE, mine is run with a 11.1v lipo with a barrel plug adapter to the board, make sure it's enough for the board and possible speakers/other elements you want to connect via mosfets

2. adafruit-style 3x4 phone keypad, any will do as long as it's supported by adafruit keypad library
  - https://www.adafruit.com/product/1824
3. 20x4 i2c adapter LCD
4. push buttons, optionally with LED rings (not implemented in current version, just for show but they do light up)
5. reset button or key switch
6. arduino mega (wire defuse) or uno/similar (without wire defuse)
7. some container for it, I use a waterproof generic carrying case with a polycarbonate insert that the components are mounted to, but for this you can go wild

wire defuse requires more GPIO so mega is the only option
for wire defuse you just need two connector blocks with 8 terminals each, one side connects to gpio pins defined in code, other side to ground (in pullup mode)

## good to have
1. two mosfets for controlling piezo speakers, one low volume and one high volume
2. voltage booster for a louder piezo speaker (optional but recommended)
3. two piezo speakers, one should be around 3-8v and the other somewhere in 20v+ so it can be heard in-game

