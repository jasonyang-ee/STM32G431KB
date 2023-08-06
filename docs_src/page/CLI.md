# CLI

I am using lwshell. Credit to <https://github.com/MaJerle/lwshell>


## Machnism

- Direct usage without FreeRTOS:
	- Inside `HAL_UARTEx_RxEventCallback()`:
		```c++
		cli.setSize(Size);
		cli.parse();
		```

- Start of the Instance:
	- Before `main()`:
		```c++
		CLI cli{};
		```
	
	- Inside `main()`:
		```c++
		cli.init();
		```

## C++ Wrapper

Additionally, I made a c++ wrapper class to use lwshell.

This is how you do all the functions and use this hardware as playground. All playground feature should be accessed via this Command Line Interface.

CLI input and output using UART (SerialCOM) as described above.

To expend commands:
1. Add a static function that returns its run result
	```c++
	static int32_t led(int32_t, char**);
	```
2. Register this function to be called when command is parsed in the RX.
	```c++
	lwshell_register_cmd("led", &CLI::led, NULL);
	```
3. Complete your command function.
   
   **REMEMBER TO `return int32_t;` ELSE SYSTEM WILL GET STUCK.**


## Customization

Finally, we can populate our `static` custom command functions.

You will need to define the command and register it in `init()`.


**Example Command with Sub Commands:**

- Inside init():
	```c++
	lwshell_register_cmd("led", &CLI::led, NULL);
	```

- Function Define:
	```c++
	static int32_t CLI::led(int32_t argc, char** argv) {
		const char* led_help =
			"\nLED Functions:\n"
			"  on\tTurns ON LED\n"
			"  off\tTurns OFF LED\n"
			"  breath\tLED in breath effect mode\n"
			"  blink\tLED in slow blink mode\n"
			"  rapid\tLED in fast blink mode\n\n";

		if (!strcmp(argv[1], "help")) serialCOM.sendString(led_help);
		if (!strcmp(argv[1], "on")) led_user.on();
		if (!strcmp(argv[1], "off")) led_user.off();
		if (!strcmp(argv[1], "breath")) led_user.breath();
		if (!strcmp(argv[1], "blink")) led_user.blink();
		if (!strcmp(argv[1], "rapid")) led_user.rapid();
		return 0;
	}
	```