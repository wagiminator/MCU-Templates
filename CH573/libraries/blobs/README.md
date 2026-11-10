# WCH Bluetooth LE Library for CH573/CH571
This folder contains the precompiled Bluetooth Low Energy library of WCH for the CH573/CH571 (`LIBCH57xBLE.a`, version V2.20, 2024/01/02) and its header file (`CH57xBLE_LIB.h`). The library contains the BLE controller (link layer) and host stack (L2CAP, ATT, GATT, GAP, SM) as well as the TMOS task scheduler. It is taken unchanged from the CH573 EVT package of WCH.

The library is only required by the Bluetooth LE handler (*ble_handler.c*). All other parts of the template and its libraries are open source and do not depend on it. The makefile of the template automatically links the library if *ble_handler.c* is present in the *src* folder; it searches for the *blobs* folder in the project folder, in the *src* folder and in *../libraries/blobs*.

## License
According to WCH, this software (modified or not) and binary may only be used for microcontrollers manufactured by Nanjing Qinheng Microelectronics Co., Ltd. (WCH). The library is not covered by the CC BY-SA license of this repository.

## Links
- [WCH: CH573 EVT package](https://www.wch-ic.com/downloads/CH573EVT_ZIP.html)
- [WCH: BLE Software Development Reference Manual](../../datasheet/WCH_BLE_Software_Development_Reference_Manual.pdf)
