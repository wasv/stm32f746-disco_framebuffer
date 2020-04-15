file build/firmware.elf
target extended-remote | openocd -c "gdb_port pipe" -f board/stm32f7discovery.cfg
