# Nessie

#### WORK IS IN PROGRESS at the moment, code will be available soon, please check back...

**Nessie** is a modchip for the **Nintendo Entertainment System** (AKA **NES**). It has the following features:

- **Reset-From-Pad** (AKA **In-Game-Reset** AKA **IGR**): Press **Select + Start + A + B**.
- **NESRGB on/off toggling and palette switching**:
  - Control your NESRGB add-on board: turn it on or off and set your palette of choice from the Player 1 controller pad: Press **Select + Start + Down** to turn it on/off or **Select + Start + Left/Right** to cycle through palettes.
  - Alternatively, palettes can be switched through the **Reset button: Keep pushed**.
  - The mod is **switchless**, so you don't need to modify the aesthetics of your console installing ugly switches.
  - The last used mode is saved automatically after 5 seconds and reused at power up.
  - Supports a single led, common-anode or common-cathode dual or RGB LEDs to indicate the current mode (Almost any color can be used when PWM pins are available).
- **CIC on/off toggling**: So you want to **play games from a different region**, but at you already fear nostalgia for that blinking power led? No problem, just turn your NES's CIC chip on and off through the **Reset button**, just quickly **Press it twice** or through the Player 1 controller: Press **Select + Start + Up**.
- Uses **cheap *Atmel AVR* microcontrollers**.
  - Can be **flashed on different chips** (ATtiny's, ATmega's, or **even a full Arduino** board), but please note that **not all features are supported on all chips**, depending on the number of available I/O pins, please read on for details.
- Even though default settings are recommended, **everything can be customized** to taste.
- Uses the popular **Arduino environment**, allowing for easy development, testing and modifications.
- Last but not least, it is **Open Source and Free Software**!
