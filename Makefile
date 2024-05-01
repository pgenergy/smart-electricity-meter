TARGET = tasmota

.PHONY: flush
flush:
	pio run -t erase -e ${TARGET}

.PHONY: build
build:
	pio run -e ${TARGET}

.PHONY: upload
upload:
	pio run -t upload -e ${TARGET}

.PHONY: monitor
monitor:
	pio device monitor

