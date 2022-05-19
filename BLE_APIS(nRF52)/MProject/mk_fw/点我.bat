move   ..\_build\ibeacon.bin  .\
nrf.exe dfu genpkg ble_ibeacon_boot_firmware.zip --application ibeacon.bin --application-version 0xff --dev-revision 1 --dev-type 1 --sd-req 0x98