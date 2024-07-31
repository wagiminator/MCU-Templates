// ===================================================================================
// Arduino IDE Wrapper File
// ===================================================================================
// Project:  CH559 Example
// Author:   Stefan Wagner
// Year:     2022
// URL:      https://github.com/wagiminator
// ===================================================================================
// This sketch intentionally contains no code and serves only as an anchor for the
// Arduino IDE. Ensure that this .ino file has the same name as the folder in which
// it is located. Additionally, the subfolder "src" must be present, as this is 
// where the code and libraries are located.
// ===================================================================================
// Compilation Instructions for the Arduino IDE:
// ---------------------------------------------
// - Make sure you have installed ch55xduino: https://github.com/DeqingSun/ch55xduino
// - Copy the .ino and .c files as well as the /src folder together into one folder
//   and name it like the .ino file. Open the .ino file in the Arduino IDE. Go to 
//   "Tools -> Board -> CH55x Boards -> CH559 Board". Under "Tools" select the 
//   following board options:
//   - Clock Source:   16 MHz (internal)
//   - USB Settings:   USER CODE /w 148B USB RAM
//   - Bootloader pin: P5.1 (D+) pull-up
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Click on "Upload" immediatly afterwards.
// ===================================================================================

#ifndef USER_USB_RAM
#error "This firmware needs to be compiled with a USER USB setting"
#endif

unsigned char _sdcc_external_startup (void) __nonbanked {
  return 0;
}
