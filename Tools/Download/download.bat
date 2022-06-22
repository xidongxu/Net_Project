.\bin\STM32_Programmer_CLI.exe -c port=SWD mode=NORMAL -w ..\..\H7_NetxDuo_FileX\EWARM\H7_NetxDuo_FileX\Exe\H7_NetxDuo_FileX.bin 0x90000000 -v -el .\bin\ExternalLoader\ART-Pi_W25Q64.stldr -g 0x08000000
timeout 3
#pause
exit