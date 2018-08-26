# vic20-keyboard-leonardo

In this project, I attempt to make a dead Commodore VIC-20 (somewhat) useful again. I have removed all the electronics from the case except for the keyboard PCB, and have attached this to an Arduino Leonardo clone. 
The keyboard PCB is very simple (A good circuit explanation can be found here: https://www.waitingforfriday.com/wp-content/uploads/2017/01/C64_Keyboard_Schematics_PNG.png), and can be read by just scanning rows/columns like you do with any other keyboard matrix.

Then, I use the Leonardo's "Keyboard.h" library to emulate a USB keyboard HID.

Some known issues thus far:
 * Some keys bounce (Should be fixable in SW)
 * Some keys intermittently don't press well (I have already disassembled the keyboard and cleaned it three times with Isopropyl alcohol, which has greatly improved performance, but some keys (notably the ">" key) still have issues).
 * The keyboard doesn't map well to the US layout. I think I can fix this with some jiggery-pokery (and I already have some) around the SHIFT key, but I'm not sure yet. Alternatively, I could research if there is modern support for the VIC-20 keyboard layout.
 * Actually, I just found this link which has an example of the jiggery-pokery I need: http://biosrhythm.com/?p=910
 
Otherwise, it's working very well! My next plan is to mount the Leonardo inside the case, along with a RPI.
