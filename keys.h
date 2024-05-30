#ifndef KEYS_H
#define KEYS_H

// перед закрытием игры
void BeforeCloseGame();


// одновременное нажатие клавиш
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sysexits.h>
#include <glob.h>
#include <linux/input.h>
#include <sys/stat.h>
#include <fcntl.h>

/* Return -1 if no key is being pressed, or else the lowest keycode 
   (c.f. linux/input-event-codes.h) of all the keys that are being pressed */
int keycode_of_key_being_pressed() { 
	FILE *kbd;
	glob_t kbddev;                                   // Glob structure for keyboard devices
	glob("/dev/input/by-path/*-kbd", 0, 0, &kbddev); // Glob select all keyboards
	int keycode = -1;                                // keycode of key being pressed
	for (int i = 0; i < kbddev.gl_pathc ; i++ ) {    // Loop through all the keyboard devices ...
		if (!(kbd = fopen(kbddev.gl_pathv[i], "r"))) { // ... and open them in turn (slow!)
			ErrorCloseGame("Run as root to read keyboard devices"); 
		}

		char key_map[KEY_MAX/8 + 1];          // Create a bit array the size of the number of keys
		memset(key_map, 0, sizeof(key_map));  // Fill keymap[] with zero's
		ioctl(fileno(kbd), EVIOCGKEY(sizeof(key_map)), key_map); // Read keyboard state into keymap[]
		for (int k = 0; k < KEY_MAX/8 + 1 && keycode < 0; k++) { // scan bytes in key_map[] from left to right
			for (int j = 0; j <8 ; j++) {       // scan each byte from lsb to msb
				if (key_map[k] & (1 << j)) {      // if this bit is set: key was being pressed
				  keycode = 8*k + j ;             // calculate corresponding keycode 
				  break;                          // don't scan for any other keys
				}
			}   
		}

		fclose(kbd);
		if (keycode)
			break;                              // don't scan for any other keyboards
	}
	return keycode;
}

// Получаем состояние клавиатуры
void RefreshKeyboardStatus() {
	setvbuf(stdout, NULL, _IONBF, 0); // Set stdout unbuffered
}

int GetKeyState(int Key) {
	return keycode_of_key_being_pressed() == Key;
}

#endif // KEYS_H
