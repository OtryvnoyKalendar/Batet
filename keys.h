#ifndef KEYS_H
#define KEYS_H

#include <sysexits.h>
#include <glob.h>
#include <linux/input.h>

int GetKeyState(int key) {
	FILE *kbd;
	glob_t kbddev; // Glob structure for keyboard devices
	glob("/dev/input/by-path/*-kbd", 0, 0, &kbddev); // Glob select all keyboards

	for (int i = 0; i < kbddev.gl_pathc ; i++ ) // Loop through all the keyboard devices
	{
		kbd = fopen(kbddev.gl_pathv[i], "r");

		char key_map[KEY_MAX/8 + 1]; // Create a byte array the size of the number of keys

		memset(key_map, 0, sizeof(key_map)); // Initate the array to zero's
		ioctl(fileno(kbd), EVIOCGKEY(sizeof(key_map)), key_map); // Fill the keymap with the current keyboard state

		int keyb = key_map[key/8]; // The key we want (and the seven others arround it)
		int mask = 1 << (key % 8); // Put a one in the same column as the key state will be in

		if (keyb & mask) // If they key is pressed
		{
			return 1;
		}

		fclose(kbd);
	}
	
	return 0;
}

#endif // KEYS_H
