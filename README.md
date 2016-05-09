# NES++

#### WORK IS IN PROGRESS at the moment, code will be available soon, please check back...

**NES++** is a modchip for the **Nintendo Entertainment System** (AKA **NES**). It has the following features:

- **Reset-From-Pad** (AKA **In-Game-Reset** AKA **IGR**): Press **Select + Start + A + B**.
- **NESRGB on/off toggling and palette switching**:
  - Control your NESRGB add-on board: turn it on or off and set your palette of choice from the Player 1 controller pad: Press **Select + Start + Down** to turn it on/off or **Select + Start + Left/Right** to cycle through palettes.
  - Alternatively, palettes can be switched through the **Reset button: Keep pushed**.
  - The mod is **switchless**, so you don't need to modify the aesthetics of your console installing ugly switches.
  - The last used palette is automatically saved and reused at power up.
  - Supports a single led, common-anode or common-cathode dual or RGB LEDs to indicate the current mode (Almost any color can be used when PWM pins are available).
- **CIC on/off toggling**: So you want to **play games from a different region**, but you already feel nostalgic for that blinking power led? No problem, just turn your NES's CIC chip on or off at will through the **Reset button**, just quickly **press it twice** or through the Player 1 controller: Press **Select + Start + Up**.
- Uses **cheap *Atmel AVR* microcontrollers**.
  - Can be **flashed on different chips** (ATtiny's, ATmega's, or **even a full Arduino** board).
- Even though default settings are recommended, **everything can be customized** to taste.
- Uses the popular **Arduino environment**, allowing for easy development, testing and modifications.
- Last but not least, it is **Open Source and Free Software**!

*Note that there is also a [NES emulator named nes++](https://sourceforge.net/projects/nespp/), but it is a different project. I chose to use the same name in analogy with [my MegaDrive++ project](https://github.com/SukkoPera/MegaDrivePlusPlus), but since the emulator hasn't been update in a while, no confusion should arise.*
