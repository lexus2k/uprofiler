ARCH ?= linux

include Makefile.$(ARCH)

.PHONY: help

help:
	@echo "Usage make [options] targets"
	@echo "    options:"
	@echo "        DESTDIR=path                  Specify install destination"
	@echo "        ARCH=<platform>               Specify platform: linux, mingw32, avr, esp32"
	@echo "    targets:"
	@echo "        library        Build library"
	@echo "        cppcheck       Run cppcheck tool for code static verification"
	@echo "        check          Build and run unit tests"
	@echo "        unittest       Only build unit tests"
	@echo "        all            Build uprofiler library"
	@echo "        docs           Build library documentation (requires doxygen)"
	@echo "        install        Install library"
	@echo "        clean          Remove all temporary generated files and binaries"
	@echo "        coverage       Dry run / generate unit tests coverage data"
