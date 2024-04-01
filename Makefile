#
#   Copyright 2024 (C) Alexey Dynda
#    This file is part of uProfiler.
#
#   GNU General Public License Usage

#   Protocol Library is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.

#   Protocol Library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.

#   You should have received a copy of the GNU General Public License
#   along with Protocol Library.  If not, see <http://www.gnu.org/licenses/>.

#   Commercial License Usage

#   Licensees holding valid commercial uProfiler licenses may use this file in
#   accordance with the commercial license agreement provided in accordance with
#   the terms contained in a written agreement between you and Alexey Dynda.
#   For further information contact via email on github account.
#

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
