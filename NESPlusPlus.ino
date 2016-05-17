/*
 * (S)NES Protocol Information:
 * - http://www.gamesx.com/controldata/snesdat.htm
 * - http://www.gamefaqs.com/snes/916396-super-nintendo/faqs/5395
 * - https://kalshagar.wikispaces.com/Around+a+SNES
 */

/*******************************************************************************
 * PLATFORM SELECTION
 ******************************************************************************/

/* In all diagrams below, outer numbers are physical pins, inner numbers
 * are Arduino pins, using the ATtiny core by DrAzzy:
 * https://github.com/SpenceKonde/ATTinyCore
 */
#if defined __AVR_ATmega328__ || defined __AVR_ATmega328P__ || defined __AVR_ATmega168__
/*
 * Arduino Uno/Nano/Micro/Whatever, use a convenience #define
 */
#define ARDUINO328

/*
 * On a full Arduino board all features are supported.
 *
 *                    ,-----_-----.
 *                    |1     A5 28|
 *                    |2   0 A4 27|
 *                    |3   1 A3 26|
 *                    |4   2 A2 25| NESRGB Pad 3
 *            LED Red |5   3 A1 24| NESRGB Pad 2
 *          CIC Pin 4 |6   4 A0 23| NESRGB Pad 1
 *                +5V |7        22| GND
 *                GND |8        21| +5V
 *                    |9        20| +5V
 *                    |10    13 19| Controller Clock
 *          LED Green |11  5 12 18| Reset In
 *           LED Blue |12  6 11 17| Controller Data
 *         Single Led |13  7 10 16| GND (Very important!)
 *          Reset Out |14  8  9 15| Pad Led (Optional)
 *                    `-----------'
 */

#define PIN_CIC 4
#define PIN_RESET_IN 12		// Since we don't need this as MISO, we can use it as an input
#define PIN_RESET_OUT 8

#define PIN_NESRGB_PALETTE_1 A0
#define PIN_NESRGB_PALETTE_2 A1
#define PIN_NESRGB_PALETTE_3 A2

#define PIN_LED_R 3		// PWM
#define PIN_LED_G 5		// PWM
#define PIN_LED_B 6		// PWM
#define PIN_LED_SINGLE 7
#define PIN_LED_PAD 9

#define ENABLE_SERIAL_DEBUG

#elif defined __AVR_ATtinyX4__
/*
 * On ATtinyX4 we support all features.
 *
 * Note that ATtiny24A doesn't have enough flash for all features, so
 * either use a bigger chip or trim down some features.
 *
 *                  ,-----_-----.
 *              +5V |1        14| GND
 *        CIC Pin 4 |2   0 10 13| NESRGB Pad 3
 *          LED Red |3   1  9 12| NESRGB Pad 2
 *                  |4      8 11| NESRGB Pad 1
 *        LED Green |5   2  7 10| Reset Out
 *         LED Blue |6   3  6  9| Controller Clock
 *  Controller Data |7   4  5  8| Reset In
 *                  `-----------'
 */
#define PIN_LED_R 1
#define PIN_LED_G 2		// PWM
#define PIN_LED_B 3		// PWM
#define PIN_CIC 0
#define PIN_RESET_IN 5			// This is MISO but is unused as such
#define PIN_RESET_OUT 7
#define PIN_NESRGB_PALETTE_1 8
#define PIN_NESRGB_PALETTE_2 9
#define PIN_NESRGB_PALETTE_3 10

#elif defined __AVR_ATtinyX5__
/*
 * On ATtinyX5 we support Reset-From-Pad and CIC management, both from
 * pad and reset button.
 *
 *                  ,-----_-----.
 *                  |1         8| +5V
 *        CIC Pin 4 |2  3   2  7| Controller Clock
 *        Reset Out |3  4   1  6| Reset In
 *              GND |4      0  5| Controller Data
 *                  `-----------'
 */
#define PIN_CIC 3
#define PIN_RESET_IN 1			// This is MISO but is unused as such
#define PIN_RESET_OUT 4

#else
  #error "Unsupported platform!"
#endif


/*******************************************************************************
 * BUTTON COMBO SETTINGS
 ******************************************************************************/

// DON'T TOUCH THIS! Just look at it for the button names you can use below!
enum {
	BTN_A =      1 << 7,
	BTN_B =      1 << 6,
	BTN_SELECT = 1 << 5,
	BTN_START =  1 << 4,
	BTN_UP =     1 << 3,
	BTN_DOWN =   1 << 2,
	BTN_LEFT =   1 << 1,
	BTN_RIGHT =  1 << 0
};

/* Button combo that enables the other combos
 *
 * Note: That vertical bar ("pipe") means that the buttons must be pressed
 *       together.
 */
#define COMBO_TRIGGER (BTN_START | BTN_SELECT)


/* Button combos to perform other actions. These are to be considered in
 * addition to COMBO_TRIGGER.
 *
 * If you don't need them, just don't #define them.
 */
#define COMBO_RESET (BTN_A | BTN_B)
#define COMBO_NEXT_PALETTE BTN_RIGHT
#define COMBO_PREV_PALETTE BTN_LEFT
#define COMBO_NESRGB_OFF BTN_DOWN
#define COMBO_TOGGLE_CIC BTN_UP


/*******************************************************************************
 * ADVANCED SETTINGS
 ******************************************************************************/

/*******************************************************************************
 * WARNING - ACHTUNG - ATTENZIONE - ATTENTION - ATENCION - WAARSCHUWING - UWAGA!
 *******************************************************************************
 *
 * Please BE CAREFUL here, as you risk DESTROYING YOUR NESRGB!
 *
 * Only proceed if you understand what you are doing!
 *
 * Our chip works at 5V, while the Altera FPGA on the NESRGB uses 3.3V, so
 * we must do some kind of level shifting.
 *
 * Experience suggests that the NESRGB has internal pull-ups on the palette
 * pins, so emulating an open-collector output should allow us to interface
 * directly. Basically, to set a pin to 1 we switch the pin to INPUT mode,
 * it will go to high impedance mode and the FPGA will see 3.3V because of
 * its internal pull-up. To set a pin to 0 we switch the pin to OUTPUT mode.
 * The AVR architecture guarantees that the pin will be at the LOW level. If
 * you want to use this strategy, leave the following #define COMMENTED.
 *
 * See http://forum.arduino.cc/index.php?topic=335689.0
 *
 * Alternatively, proper level shifting can be put in place. Some possibilities:
 * - 74LVX125 buffer with 5V-tolerant inputs
 * - CD4504BE CMOS-to-CMOS level shifter
 * - SparkFun Logic Level Converter: https://www.sparkfun.com/products/12009
 * - 2 MOSFETs: http://www.hobbytronics.co.uk/mosfet-voltage-level-converter
 * - 2-3 series diodes
 * - 3.3V Zener (BZX55C3V3) + series resistor (220 ohm should be fine)
 *
 * If you have used one of these solutions, UNCOMMENT the following #define, but
 * please please please MAKE SURE THAT NO MORE THAN 3.3V ARE PUT ON THE NESRGB
 * PALETTE INPUTS!
 */
//~ #define USE_LEVEL_SHIFTER

/* The reset line must be released some time after power is applied, since there
 * is a capacitor on it that needs to be charged (milliseconds)
 */
//~ #define INITIAL_DELAY 50

/* Offset in the EEPROM at which the CIC toggle status should be saved. Undefine
 * to disable CIC status saving
 */
#define CIC_ROM_OFFSET 24

/* Offset in the EEPROM at which the current mode should be saved. Undefine to
 * disable palette saving.
 */
#define PALETTE_ROM_OFFSET 42

// Time to wait before saving changed settings to EEPROM (milliseconds)
#define EEPROM_SAVE_DELAY 5000


/* Colors to use to indicate the video mode, in 8-bit RGB componentes. You can
 * use any value here if your led is connected to PWM-capable pins, otherwise
 * values specified here will be interpreted as either fully off (if 0) or fully
 * on (if anything else).
 */
//~ #define PALETTE_LED_OFF_COLOR       {0xFF, 0xFF, 0xFF}	// White
//~ #define PALETTE_LED_NATURAL_COLOR   {0xFF, 0xFF, 0x00}	// Yellow
//~ #define PALETTE_LED_GARISH_COLOR    {0x00, 0xFF, 0xFF}	// Light blue
//~ #define PALETTE_LED_IMPROVED_COLOR  {0xFF, 0x00, 0xFF}	// Purple
#define PALETTE_LED_OFF_COLOR       {0x00, 0x00, 0x00}
#define PALETTE_LED_NATURAL_COLOR   {0xFF, 0x00, 0x00}
#define PALETTE_LED_GARISH_COLOR    {0xFF, 0x00, 0xFF}
#define PALETTE_LED_IMPROVED_COLOR  {0x00, 0x00, 0xFF}

// Define this if your led is common-anode, comment out for common-cathode
//~ #define RGB_LED_COMMON_ANODE

/* Use a single led to indicate the video mode. This is blinked 1-3 times
 * according to which palette is set. This does NOT disable the
 * multi-colored led and can be used together with it, provided that you
 * have enough pins. Otherwise please disable it manually.
 */
//~ #define PIN_LED_SINGLE 3

/* Debounce duration for the reset button. This might need increasing,
 * since NES's and their buttons are old and boooouncy. Note that if you
 * do so, you might need to increase DBLCLICK_TIMEOUT as well, for
 * double clicks to be detected properly.
 */
#define DEBOUNCE_MS 70

/* Two consecutive presses of the reset button within this amount of
 * milliseconds will toggle the CIC chip status.
 */
#define DBLCLICK_TIMEOUT 250

/* Keeping the reset button pressed longer than this amount of
 * milliseconds will switch to the next palette, shorter (single)
 * presses will reset the console.
 */
#define LONGPRESS_LEN 700

// Duration of the reset pulse (milliseconds)
#define RESET_LEN 250


/*******************************************************************************
 * FEATURE MACROS
 ******************************************************************************/

/* Feature macros: these get enabled automatically if the pins and other macros
* they depend on are defined above, but can be freely undefined if the feature
* is not desired.
*
* Note that PAD snooping also depends implicitly on hardware SPI, which we
* assume to always be available.
*/

#if defined PIN_RESET_IN && defined PIN_RESET_OUT && \
	defined COMBO_TRIGGER && defined COMBO_RESET
	#define ENABLE_RESET_FROM_PAD
#endif

#if defined PIN_NESRGB_PALETTE_1 && defined PIN_NESRGB_PALETTE_2 && \
	defined PIN_NESRGB_PALETTE_3 && defined COMBO_TRIGGER && \
	(defined COMBO_NEXT_PALETTE || defined COMBO_PREV_PALETTE || \
	 defined COMBO_NESRGB_OFF)
	#define ENABLE_PALETTE_FROM_PAD
#endif

#if defined COMBO_TRIGGER && defined COMBO_TOGGLE_CIC && defined PIN_CIC
	#define ENABLE_CIC_FROM_PAD
#endif

#if (defined PIN_LED_PAD)
	#define ENABLE_PAD_LED
#else
	#warning Pad led disabled
#endif

#if defined PIN_RESET_IN && defined PIN_RESET_OUT && \
	defined PIN_NESRGB_PALETTE_1 && defined PIN_NESRGB_PALETTE_2 && \
	defined PIN_NESRGB_PALETTE_3
	#define ENABLE_PALETTE_FROM_RESET
#endif

#if defined PIN_RESET_IN && defined PIN_RESET_OUT && defined PIN_CIC
	#define ENABLE_CIC_FROM_RESET
#endif

#if (defined PIN_LED_R || defined PIN_LED_G || defined PIN_LED_B)
	#define ENABLE_RGB_LED
#else
	#warning RGB led disabled
#endif

#if (defined PIN_LED_SINGLE)
	#define ENABLE_SINGLE_LED
#else
	#warning Single led disabled
#endif

// The following are automagic commodity defines that should not be touched

// Defined if we need to eavesdrop for gamepad data
#if (defined ENABLE_RESET_FROM_PAD || \
	 defined ENABLE_PALETTE_FROM_PAD || defined ENABLE_CIC_FROM_PAD)
	 #define ENABLE_PAD
#else
	#warning Pad snooping disabled
#endif

// Defined if we need to manage the reset line (both IN and OUT)
#if (defined ENABLE_PALETTE_FROM_RESET || \
	 defined ENABLE_CIC_FROM_RESET || defined ENABLE_RESET_FROM_PAD)
	#define ENABLE_RESET
#else
	#warning Reset button management disabled
#endif

// Defined if we need to set the NESRGB palette
#if defined ENABLE_PALETTE_FROM_PAD || defined ENABLE_PALETTE_FROM_RESET
	#define ENABLE_PALETTE
#else
	#warning Palette control disabled
#endif

// Defined if we need to control the CIC chip
#if (defined ENABLE_CIC_FROM_PAD || defined ENABLE_CIC_FROM_RESET)
	#define ENABLE_CIC
#else
	#warning CIC control disabled
#endif

/*******************************************************************************
 * END OF SETTINGS
 ******************************************************************************/


#ifdef ENABLE_SERIAL_DEBUG
	#define debug(...) Serial.print (__VA_ARGS__)
	#define debugln(...) Serial.println (__VA_ARGS__)
	#warning DEBUG ENABLED
#else
	#define debug(...)
	#define debugln(...)
#endif

#if defined CIC_ROM_OFFSET || defined PALETTE_ROM_OFFSET
	#include <EEPROM.h>
#endif

#ifdef ENABLE_PALETTE

#define PALETTE_POSITIONS 4

byte palette_led_colors[][PALETTE_POSITIONS] = {
	PALETTE_LED_OFF_COLOR,
	PALETTE_LED_NATURAL_COLOR,
	PALETTE_LED_GARISH_COLOR,
	PALETTE_LED_IMPROVED_COLOR
};

/*
 * Palette-Switch Info
 * Taken from NESRGB-IGR by Peter Bartmann <peter.bartmann@gmx.de>
 * 0 = shorted to ground. 1 = unconnected.
 *
 * V1.0,1.3
 * 123 - PALETTE
 * 000 - off         <- used by this code
 * 001 - off
 * 010 - off
 * 011 - garish      <- used by this code
 * 100 - off
 * 101 - improved    <- used by this code
 * 110 - natural     <- used by this code
 * 111 - off
 *
 * NESRGB V1.4+
 * 123 - PALETTE
 * 000 - off         <- used by this code
 * 001 - off
 * 010 - garish
 * 011 - garish      <- used by this code
 * 100 - improved
 * 101 - improved    <- used by this code
 * 110 - natural     <- used by this code
 * 111 - off
 */
byte palette_nesrgb_map[][PALETTE_POSITIONS] = {
	{0, 0, 0},	// NESRGB Off
	{1, 1, 0},  // Natural
	{1, 0, 1},  // Improved
	{0, 1, 1}   // Garish
};


byte palette;
unsigned long palette_last_changed_time;

// Palette that was is use before NESRGB was turned off
byte palette_before_off;


void set_palette_pin (byte pin, byte val) {
#ifdef USE_LEVEL_SHIFTER
	/* Proper level shifting is in place (Right???), just switch the pin
	 * HIGH/LOW as necessary
	 */
	digitalWrite (pin, val);
#else
	// Emulate open-collector output
	if (val) {
		// Make pin hi-Z, pull-up internal to the FPGA will do the rest
		pinMode (pin, INPUT);
	} else {
		// Short pin to ground
		pinMode (pin, OUTPUT);
	}
#endif
}

void set_palette (int new_palette, boolean flash = true) {
	if (new_palette > PALETTE_POSITIONS)
		new_palette = 0;

	debug ("Setting new palette: ");
	debugln (new_palette);

	byte *p = palette_nesrgb_map[new_palette];
	set_palette_pin (PIN_NESRGB_PALETTE_1, p[0]);
	set_palette_pin (PIN_NESRGB_PALETTE_2, p[1]);
	set_palette_pin (PIN_NESRGB_PALETTE_3, p[2]);

	palette = new_palette;
	led_on ();

	/* Flash single led
	 * WARNING: This loop must be reasonably shorter than LONGPRESS_LEN
	 * in the worst case!
	 */
	if (flash) {
#ifdef ENABLE_SINGLE_LED
		for (int i = 0; i < palette; ++i) {
			digitalWrite (PIN_LED_SINGLE, LOW);
			delay (40);
			digitalWrite (PIN_LED_SINGLE, HIGH);
			delay (80);
		}
#endif
	}

	palette_last_changed_time = millis ();
}

void change_palette (int increment) {
	// This also loops in [0, PALETTE_POSITIONS) backwards
	int new_palette = (palette + increment + PALETTE_POSITIONS) % PALETTE_POSITIONS;
	palette_last_changed_time = millis ();
	set_palette (new_palette);
}

void next_palette () {
	if (palette == 0) {
		// Turn back on with same palette
		nesrgb_on_off ();
	} else if (palette == PALETTE_POSITIONS - 1) {
		// Don't turn off
		change_palette (+2);
	} else {
		change_palette (+1);
	}
}

void prev_palette () {
	if (palette == 0) {
		nesrgb_on_off ();
	} else if (palette == 1) {
		change_palette (-2);
	} else {
		change_palette (-1);
	}
}

void save_palette () {
#ifdef PALETTE_ROM_OFFSET
	if (palette_last_changed_time > 0 && millis () - palette_last_changed_time >= EEPROM_SAVE_DELAY) {
		debugln ("Saving palette to EEPROM");
		byte saved_palette = EEPROM.read (PALETTE_ROM_OFFSET);
		if (palette != saved_palette) {
			EEPROM.write (PALETTE_ROM_OFFSET, palette);
		} else {
			debugln ("Palette unchanged, not saving");
		}
		palette_last_changed_time = 0;    // Don't save again

		// Blink led to tell the user that mode was saved
		led_off ();
		delay (500);
		led_on ();
	}
#endif
}

void nesrgb_on_off () {
	if (palette == 0) {
		// Turn back on, making sure we don't stay off
		change_palette (palette_before_off != 0 ? palette_before_off : 1);
		palette_before_off = 0;
	} else {
		// Turn off
		palette_before_off = palette;
		change_palette (-palette);
	}
}

#endif // ENABLE_PALETTE


/* LED legend:
 *
 * RGB led:
 * - Color is set as specified above
 * - 1 long flash: Current palette has been saved
 * - 6 rapid flashes: CIC disabled
 * - 10 rapid flashes: CIC enabled
 *
 * Single led:
 * - All of the above
 * - 1 rapid flash: Natural palette selected
 * - 2 rapid flashes: Garish palette selected
 * - 3 rapid flashes: Improved palette selected
 */
void led_off () {
#ifdef ENABLE_RGB_LED
	byte c = 0;

#ifdef RGB_LED_COMMON_ANODE
	c = 255 - c;
#endif

#ifdef PIN_LED_R
	analogWrite (PIN_LED_R, c);
#endif

#ifdef PIN_LED_G
	analogWrite (PIN_LED_G, c);
#endif

#ifdef PIN_LED_B
	analogWrite (PIN_LED_B, c);
#endif

#endif  // ENABLE_RGB_LED

#ifdef ENABLE_SINGLE_LED
	digitalWrite (PIN_LED_SINGLE, LOW);
#endif
}

void led_on () {
#ifdef ENABLE_RGB_LED
	byte *colors = palette_led_colors[palette];
	byte c;

#ifdef PIN_LED_R
	c = colors[0];
#ifdef RGB_LED_COMMON_ANODE
	c = 255 - c;
#endif
	analogWrite (PIN_LED_R, c);
#endif

#ifdef PIN_LED_G
	c = colors[1];
#ifdef RGB_LED_COMMON_ANODE
	c = 255 - c;
#endif
	analogWrite (PIN_LED_G, c);
#endif

#ifdef PIN_LED_B
	c = colors[2];
#ifdef RGB_LED_COMMON_ANODE
	c = 255 - c;
#endif
	analogWrite (PIN_LED_B, c);
#endif

#endif	// ENABLE_RGB_LED

#ifdef ENABLE_SINGLE_LED
	digitalWrite (PIN_LED_SINGLE, HIGH);
#endif
}


#ifdef ENABLE_CIC

#ifdef CIC_ROM_OFFSET
boolean cic_enabled;
unsigned long cic_last_toggled_time = 0;
#endif

void save_cic_status () {
#ifdef CIC_ROM_OFFSET
	if (cic_last_toggled_time > 0 && millis () - cic_last_toggled_time >= EEPROM_SAVE_DELAY) {
		debugln ("Saving CIC status to EEPROM");
		byte saved_cicst = EEPROM.read (CIC_ROM_OFFSET);
		if (cic_enabled != saved_cicst) {
			EEPROM.write (CIC_ROM_OFFSET, cic_enabled);
		} else {
			debugln ("CIC status unchanged, not saving");
		}
		cic_last_toggled_time = 0;    // Don't save again
	}
#endif
}

void set_cic (boolean enabled, boolean flash = true) {
	debug ("Setting CIC status: ");
	debugln (enabled ? "ENABLED" : "DISABLED");

	digitalWrite (PIN_CIC, enabled);

	cic_enabled = enabled;

	// Flash leds to signal CIC status: 6 flashes = OFF, 10 = ON
	if (flash) {
		for (int i = 0; i < (enabled ? 10 : 6); i++) {
			led_off ();
			delay (40);
			led_on ();
			delay (80);
		}
	}

	cic_last_toggled_time = millis ();
}

void toggle_cic () {
	set_cic (!cic_enabled);
}

#endif		// ENABLE_CIC

#ifdef ENABLE_RESET
enum ResetButtonState {
	RST_IDLE,				// Button not pressed
	RST_1ST_PRESS,			// Button pressed first time
	RST_WAIT_2ND,			// Button released after first press
	RST_LONGPRESS,			// Button kept pressed at first press
	RST_2ND_PRESS,			// Button pressed second time
	RST_LONG_2ND_PRESS		// Button kept pressed at second press
};

void handle_reset_button () {
	static byte pressed_before = LOW, debounce_state = HIGH;
	static long last_int = 0, last_pressed = 0, last_released;
	static unsigned int hold_cycles = 0;
	static ResetButtonState state = RST_IDLE;

	byte reset_pressed_now = digitalRead (PIN_RESET_IN);
	if (reset_pressed_now != debounce_state) {
		// Reset debouncing timer
		last_int = millis ();
		debounce_state = reset_pressed_now;
	} else if (millis () - last_int > DEBOUNCE_MS) {
		// OK, button is stable, see if it has changed
		bool just_pressed = reset_pressed_now && !pressed_before;
		bool just_released = !reset_pressed_now && pressed_before;

		switch (state) {
			case RST_IDLE:
				if (just_pressed) {
					// Button was just pressed
					last_pressed = millis ();
					state = RST_1ST_PRESS;
				}
				break;
			case RST_1ST_PRESS:
				if (just_released) {
					// Button was just released after 1st click
					state = RST_WAIT_2ND;
					last_released = millis ();
				} else if (millis () - last_pressed >= LONGPRESS_LEN) {
					/* First long press hit. No need to trigger action
					 * here, it will be called immediately at the next
					 * iteration in case RST_LONGPRESS.
					 */
					state = RST_LONGPRESS;
				}
				break;
			case RST_WAIT_2ND:
				if (reset_pressed_now) {
					// Button quickly pressed twice
					last_pressed = millis ();

					state = RST_2ND_PRESS;
				} else if (millis () - last_released >= DBLCLICK_TIMEOUT) {
					// Double click timeout expired, so this was a single click
					debugln ("Reset button clicked");
					reset_console ();

					state = RST_IDLE;
				}
				break;
			case RST_LONGPRESS:
				if (!reset_pressed_now) {
					// Button finally released
					state = RST_IDLE;
				} else {
					// Reset has been held for a while
					debugln ("Reset button held");
					reset_console_adlibitum ();

					// Remain in this state
				}
				break;
			case RST_2ND_PRESS:
				if (just_released) {
					// Button released again, so this was a double click
					debugln ("Reset button double-clicked");

#ifdef ENABLE_CIC_FROM_RESET
					toggle_cic ();
#endif

					state = RST_IDLE;
				} else if (millis () - last_pressed >= LONGPRESS_LEN) {
					/* First "long double press" hit. No need to trigger action
					 * here, it will be called immediately at the next
					 * iteration in case RST_LONG_2ND_PRESS.
					 */
					hold_cycles = 0;

					state = RST_LONG_2ND_PRESS;
				}
				break;
			case RST_LONG_2ND_PRESS:
				if (!reset_pressed_now) {
					// Button finally released
					state = RST_IDLE;
				} else if ((millis () - last_pressed) / LONGPRESS_LEN >= (hold_cycles + 1)) {
					// This is a "Long double click"
					debugln ("Reset button double-held");
					++hold_cycles;

#ifdef ENABLE_PALETTE_FROM_RESET
					next_palette ();
#endif
					// Remain in this state
				}
				break;
		}

		pressed_before = reset_pressed_now;
	}
}

// Resets the console as long as the reset button is pressed
void reset_console_adlibitum () {
	debugln ("Resetting console ad libitum");

	disable_pad ();

	digitalWrite (PIN_RESET_OUT, HIGH);
	while (digitalRead (PIN_RESET_IN))
		;
	digitalWrite (PIN_RESET_OUT, LOW);

	init_pad ();
}

void reset_console () {
	debugln ("Resetting console");

	disable_pad ();

	digitalWrite (PIN_RESET_OUT, HIGH);
	delay (RESET_LEN);
	digitalWrite (PIN_RESET_OUT, LOW);

	init_pad ();
}
#endif		// ENABLE_RESET


#ifdef ENABLE_PAD

#define COMBO_IGNORE_MS LONGPRESS_LEN

void handle_pad () {
	static unsigned long last_combo_time = 0;

	byte pad_status = read_pad ();

#if 0
	debug ("Pad status = ");
	debugln (pad_status);
#endif

#ifdef ENABLE_PAD_LED
	digitalWrite (PIN_LED_PAD, pad_status);
#endif

	if ((pad_status & COMBO_TRIGGER) == COMBO_TRIGGER && millis () - last_combo_time > COMBO_IGNORE_MS) {
		if (0) {
			// Just a code gimmick to allow the #ifdef stuff
#ifdef ENABLE_RESET_FROM_PAD
		} else if ((pad_status & COMBO_RESET) == COMBO_RESET) {
			debugln ("Reset combo detected");
			reset_console ();
			pad_status = 0;     // Avoid continuous reset (pad_status will keep the last value during reset!)
			last_combo_time = millis ();
#endif
#if defined ENABLE_PALETTE_FROM_PAD && defined COMBO_NEXT_PALETTE
		} else if ((pad_status & COMBO_NEXT_PALETTE) == COMBO_NEXT_PALETTE) {
			debugln ("Next palette combo detected");
			next_palette ();
			last_combo_time = millis ();
#endif
#if defined ENABLE_PALETTE_FROM_PAD && defined COMBO_PREV_PALETTE
		} else if ((pad_status & COMBO_PREV_PALETTE) == COMBO_PREV_PALETTE) {
			debugln ("Previous palette combo detected");
			prev_palette ();
			last_combo_time = millis ();
#endif
#if defined ENABLE_PALETTE_FROM_PAD && defined COMBO_NESRGB_OFF
		} else if ((pad_status & COMBO_NESRGB_OFF) == COMBO_NESRGB_OFF) {
			debugln ("NESRGB OFF combo detected");
			nesrgb_on_off ();
			last_combo_time = millis ();
#endif
#ifdef ENABLE_CIC_FROM_PAD
		} else if ((pad_status & COMBO_TOGGLE_CIC) == COMBO_TOGGLE_CIC) {
		  debugln ("CIC toggle combo detected");
		  toggle_cic ();
		  last_combo_time = millis ();
#endif
		}
	}
}

#endif	// ENABLE_PAD

void setup () {
#ifdef ENABLE_SERIAL_DEBUG
	Serial.begin (9600);
#endif

	debugln ("Starting up...");

	// Enable reset
#ifdef ENABLE_RESET
	pinMode (PIN_RESET_OUT, OUTPUT);
	digitalWrite (PIN_RESET_OUT, HIGH);
	pinMode (PIN_RESET_IN, INPUT);
#endif

	// Setup leds
#ifdef PIN_LED_R
	pinMode (PIN_LED_R, OUTPUT);
#endif

#ifdef PIN_LED_G
	pinMode (PIN_LED_G, OUTPUT);
#endif

#ifdef PIN_LED_B
	pinMode (PIN_LED_B, OUTPUT);
#endif

#ifdef ENABLE_SINGLE_LED
	pinMode (PIN_LED_SINGLE, OUTPUT);
#endif

#ifdef ENABLE_PAD_LED
	pinMode (PIN_LED_PAD, OUTPUT);
#endif

	// Init palette
#ifdef ENABLE_PALETTE
	// Initialize output pins
#ifdef USE_LEVEL_SHIFTER
	pinMode (PIN_NESRGB_PALETTE_1, OUTPUT);
	pinMode (PIN_NESRGB_PALETTE_2, OUTPUT);
	pinMode (PIN_NESRGB_PALETTE_3, OUTPUT);
#else
	/* Pins are in input (i.e. high Z) mode by default, so this isn't strictly
	 * necessary. It's here mainly in case some Arduino core goes crazy and
	 * changes the initial pin status.
	 */
	pinMode (PIN_NESRGB_PALETTE_1, INPUT);
	digitalWrite (PIN_NESRGB_PALETTE_1, LOW);		// Even less necessary

	pinMode (PIN_NESRGB_PALETTE_2, INPUT);
	digitalWrite (PIN_NESRGB_PALETTE_2, LOW);

	pinMode (PIN_NESRGB_PALETTE_3, INPUT);
	digitalWrite (PIN_NESRGB_PALETTE_3, LOW);
#endif

	// NESRGB OFF by default
	palette = 0;
	palette_before_off = 0;
#ifdef PALETTE_ROM_OFFSET
	palette = EEPROM.read (PALETTE_ROM_OFFSET);
	debug ("Loaded palette from EEPROM: ");
	debugln (palette);
	if (palette >= PALETTE_POSITIONS) {
		// Palette EEPROM value is out of range
		palette = 0;
	}
#endif
	set_palette (palette, false);	 // Don't flash at startup
	palette_last_changed_time = 0;   // No need to save what we just loaded
#endif

#ifdef ENABLE_CIC
	pinMode (PIN_CIC, OUTPUT);

#ifdef CIC_ROM_OFFSET
	cic_enabled = EEPROM.read (CIC_ROM_OFFSET);
	debug ("Loaded CIC status from EEPROM: ");
	debugln (cic_enabled ? "ENABLED" : "DISABLED");
	set_cic (cic_enabled, false);
	cic_last_toggled_time = 0;
#endif

#endif	// ENABLE_CIC

	// Prepare to read pad
	init_pad ();

#ifdef ENABLE_RESET
	// Finally release the reset line
#ifdef INITIAL_DELAY
	delay (INITIAL_DELAY);
#endif
	digitalWrite (PIN_RESET_OUT, LOW);
#endif
}

void loop () {
#ifdef ENABLE_RESET
	handle_reset_button ();
#endif

#ifdef ENABLE_PAD
	handle_pad ();
#endif

#ifdef ENABLE_PALETTE
	save_palette ();
#endif

#ifdef ENABLE_CIC
	save_cic_status ();
#endif
}


void init_pad () {
#ifdef ARDUINO328
	SPDR = 0;

	// Turn on SPI in slave mode, SPI Mode 2
	SPCR |= (1 << SPE) | (1 << CPOL);
#elif defined _AVR_ATtinyX4__
	// Only setup MOSI pin, no need for MISO
	DDRA &= ~((1 << PA6) | (1 << PA4));
#elif defined __AVR_ATtinyX5__
	// Ditto
	DDRB &= ~((1 << PB2) | (1 << PB0));
#endif

#if defined __AVR_ATtinyX4__ || defined __AVR_ATtinyX5__
	// This is common for all ATtiny's with USI
	USIDR = 0;

	USICR = (1 << USIWM0)   // USIWM1, USIWM0: Wire Mode
		  | (1 << USICS1)   // USICS1, USICS0: Clock Source Select
		  | (0 << USICS0)	// SPI mode
		  ;
#endif
}

byte read_pad () {
	static byte pad_status = 0;

#ifdef ARDUINO328
	if (SPSR & (1 << SPIF)) {
		pad_status = ~SPDR;
	}
#elif defined __AVR_ATtinyX4__ || defined __AVR_ATtinyX5__
	if (USISR & (1 << USIOIF)) {
		pad_status = ~USIBR;	// Use buffer register

		// Clear flag
		USISR |= (1 << USIOIF);
	}
#endif

	return pad_status;
}

void disable_pad () {
#ifdef ARDUINO328
	SPCR &= ~(1 << SPE);
	SPDR = 0;
#elif defined __AVR_ATtinyX4__ || defined __AVR_ATtinyX5__
	//~ USICR &= ~((1 << USIWM1) | (1 << USIWM0));
	USICR = 0;
	USISR = (1 << USIOIF);
#endif
}
