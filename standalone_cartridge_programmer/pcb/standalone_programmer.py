#!/usr/bin/python

# Copyright 2017 Google Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# ----------------------------------------------
# USB Acorn Electron cartridge reader/programmer
# ----------------------------------------------

# by Phillip Pearson

# Hardware: Cartridge port, 3v3 regulator, CPLD, Pro Micro

# Function: The CPLD is configured to allow SPI access to the cartridge. The
# Pro Micro is configured with CMSIS-DAP + serial firmware, to allow it to
# program the CPLD (via xc3sprog) and access the cartridge via the programmed
# CPLD.

# ----------------------------------------------

# CPLD firmware: SPI to cartridge interface (VHDL), built using Xilinx ISE and
# programmed using xc3sprog.

# Pro Micro firmware: CMSIS-DAP (C++), built and programmed via the Arduino
# IDE.

# PC software: Python code to talk to the Pro Micro via a USB serial port, and
# read and write cartridges.

# ----------------------- circuit notes -----------------------

# 5V power comes in from the USB port, is regulated down to 3v3 for the CPLD,
# and passed straight to the cartridge.

# The AVR speaks SPI to the CPLD over its hardware SPI port, configured at
# 1MHz.  The CPLD inverts and copies this clock out as 16MHZ and PHI0 for the
# cartridge. (So 16MHz and PHI0 both clock at 1MHz).

# All SPI transactions take place with SS=GND.  The CPLD is in reset when
# SS=VCC.  SS must not transition at the same time as SCK.

# To read a byte from memory: the AVR sends 0x81 A15-8 A7-0 0x00.  The memory
# byte is returned in the final byte.

# To write a byte to memory: the AVR sends 0x81 A15-8 A7-0 D.

# ----------------------- old stuff from revision 1 -----------------------

# Bug in rev1 PCB: CPLD pin numbers were from the PC44 version, not VQ44, so
# they're all offset by 6.  (PC44 pin 7 is VQ44 pin 1).  Fixed in rev2.

# Change from rev1-rev2: In rev1, the AVR uses GPIO for its SPI connection to
# the CPLD, saving the hardware SPI port for a programmer connection.  I
# realized I'll almost never use the programmer connection (because it has a
# USB bootloader), so in rev2 the hardware SPI port is connected to the CPLD.

# 16MHz clock is generated by the Pro Micro and passed to the CPLD and
# cartridge.

# Pro Micro talks async serial?  spi?  to cpld.  spi would probably work best,
# need to share the programming port though.  make cpld tristate MISO when
# /SS=1 I guess?

# TODO: figure out how to get 16MHz output from pro micro
#       or, failing that, generate a 2MHz PHI0

# http://extremeelectronics.co.in/avr-tutorials/pwm-signal-generation-by-using-avr-timers-part-ii/

# timer0 is 8-bit
# timer1 and timer3 are 16-bit
    # min pwm res is 2-bit (ICRn or OCRnA = 0x0003)
# timer4 is 10-bit and driven by a 64MHz input
#   PC7 is CLK0, divided clock output, although that isn't brought out to a pin
#   it looks like the PLL produces a 32-96MHz output, which is divided down to make
#   a 48MHz USB clock and a configurable (/1, /1.5, /2 ratio) CLKtmr, so maybe
#   I can run that into a high speed timer and produce 16MHz output?
#   https://harizanov.com/2013/04/crazy-high-frequency-pwm-with-atmega32u4/
#   Apparently timer4 counts at 64MHz (I guess 96/1.5).

import sys, os
sys.path.insert(0, os.path.join(os.path.dirname(sys.argv[0]), "../../third_party/myelin-kicad.pretty"))
from myelin_kicad_pcb import dump_bom, dump_netlist, Component, Pin, C0805, R0805

# Socket to match the 44-pin edge connector in an Electron cartridge
# http://www.mouser.com/ProductDetail/TE-Connectivity-AMP/5530843-4/
cartridge_port = Component(
	footprint="myelin-kicad:acorn_electron_cartridge_socket",
	identifier="CON1",
	value="cartridge port",
	desc="Socket to match the 44-pin edge connector in an Electron cartridge, e.g. TE 5530843-4 / Digikey A31717-ND",
	pins=[
        Pin("A1", "5V", ["5V"]),
        Pin("A2", "nOE", ["cart_nOE"]), # essential
        Pin("A3", "nRST", ["cart_nRST"]),
        Pin("A4", "CSRW", ["cart_RnW"]), # essential
        Pin("A5", "A8", ["cart_A8"]), # essential
        Pin("A6", "A13", ["cart_A13"]), # essential
        Pin("A7", "A12", ["cart_A12"]), # essential
        Pin("A8", "PHI0", ["cart_PHI0"]), # essential
        Pin("A9", "-5V"),
        Pin("A10", "NC"),
        Pin("A11", "nRDY", ["cart_nRDY"]),
        Pin("A12", "nNMI", ["cart_nNMI"]),
        Pin("A13", "nIRQ", ["cart_nIRQ"]),
        Pin("A14", "nINFC", ["cart_nINFC"]), # nice to have, essential for dumping MGC
        Pin("A15", "nINFD", ["cart_nINFD"]), # nice to have
        Pin("A16", "ROMQA", ["cart_ROMQA"]), # essential (A14)
        Pin("A17", "16MHZ", ["cart_16MHZ"]), # nice to have
        Pin("A18", "nROMSTB"),
        Pin("A19", "ADOUT"),
        Pin("A20", "AGND"),
        Pin("A21", "ADIN"),
        Pin("A22", "GND", ["GND"]),
        Pin("B1", "5V", ["5V"]),
        Pin("B2", "A10", ["cart_A10"]), # essential
        Pin("B3", "D3", ["cart_D3"]), # essential
        Pin("B4", "A11", ["cart_A11"]), # essential
        Pin("B5", "A9", ["cart_A9"]), # essential
        Pin("B6", "D7", ["cart_D7"]), # essential
        Pin("B7", "D6", ["cart_D6"]), # essential
        Pin("B8", "D5", ["cart_D5"]), # essential
        Pin("B9", "D4", ["cart_D4"]), # essential
        Pin("B10", "nOE2", ["cart_nOE2"]), # essential for carts responding to rom 13
        Pin("B11", "BA7", ["cart_A7"]), # essential
        Pin("B12", "BA6", ["cart_A6"]), # essential
        Pin("B13", "BA5", ["cart_A5"]), # essential
        Pin("B14", "BA4", ["cart_A4"]), # essential
        Pin("B15", "BA3", ["cart_A3"]),
        Pin("B16", "BA2", ["cart_A2"]), # essential
        Pin("B17", "BA1", ["cart_A1"]), # essential
        Pin("B18", "BA0", ["cart_A0"]), # essential
        Pin("B19", "D0", ["cart_D0"]), # essential
        Pin("B20", "D2", ["cart_D2"]), # essential
        Pin("B21", "D1", ["cart_D1"]), # essential
        Pin("B22", "GND", ["GND"]), # essential
	],
)

# Sparkfun Pro Micro
pro_micro = Component(
	footprint="myelin-kicad:pro_micro",
	identifier="U1",
	value="Pro Micro 5V",
	desc="Sparkfun Pro Micro or eBay/Aliexpress clone, with ATMEGA32U4 MCU and micro USB socket (NOT ATMEGA328 or mini USB)",
	pins=[
		# two rows of 12 pins, counter clockwise with pin 1 at top left
		Pin(1, "D1_TX_PD3"),
		Pin(2, "D0_RX_PD2"),
		Pin(3, "GND", ["GND"]),
		Pin(4, "GND", ["GND"]),
		Pin(5, "D2_SDA_PD1"),
		Pin(6, "D3_SCL_OC0B_PD0"),
		Pin(7, "D4_A6_PD4"),
		Pin(8, "D5_nOC4A_PC6"),
		Pin(9, "D6_A7_OC4D_PD7", ["cart_nIRQ"]),
		Pin(10, "D7_PE6", ["cart_nNMI"]),
		Pin(11, "D8_A8_PB4", ["cart_nRDY"]),
		Pin(12, "D9_A9_nOC4B_PB5", ["cart_nRST"]), # pull this one up
		Pin(13, "D10_A10_OC4B_PB6", ["cpld_SS"]),
		Pin(14, "D16_MOSI_PB2", ["avr_MOSI"]),
		Pin(15, "D14_MISO_PB3", ["avr_MISO"]),
		Pin(16, "D15_SCK_PB1", ["avr_SCK"]),
		Pin(17, "D18_A0_PF7", ["cpld_TDO"]),
		Pin(18, "D19_A1_PF6", ["cpld_TMS"]),
		Pin(19, "D20_A2_PF5", ["cpld_TCK"]),
		Pin(20, "D21_A3_PF4", ["cpld_TDI"]),
		Pin(21, "VCC"), # NC - we want the raw 5V from USB (although this is probably the same)
		Pin(22, "RST", ["avr_RST"]),
		Pin(23, "GND", ["GND"]),
		Pin(24, "VRAW", ["5V"]),
	],
)

avr_isp = Component(
    footprint="Pin_Headers:Pin_Header_Straight_2x03_Pitch2.54mm",
    identifier="CON2",
    value="AVR ISP",
	desc="2x3 0.1 inch header for AVR programming (OPTIONAL)",
    pins=[
        Pin(1, "MISO", ["avr_MISO"]),
        Pin(2, "VCC", ["5V"]),
        Pin(3, "SCK", ["avr_SCK"]),
        Pin(4, "MOSI", ["avr_MOSI"]),
        Pin(5, "RST", ["avr_RST"]),
        Pin(6, "GND", ["GND"]),
    ],
)

# CPLD glue: Xilinx XC9572XL-10VQG44
# https://www.xilinx.com/support/documentation/data_sheets/ds057.pdf
# VQ44 package: https://www.xilinx.com/support/documentation/package_specs/vq44.pdf
cpld = Component(
    footprint="myelin-kicad:xilinx_vqg44",
    identifier="U2",
    value="XC9572XL-10VQG44C",
    desc="IC CPLD Xilinx 72MC 44-pin; https://www.digikey.com/products/en?keywords=122-1448-ND",
    pins=[
        # change ../cpld/constraints.ucf if any pinouts change
        Pin(39, "P1.2", ["cart_A7"]),
        Pin(40, "P1.5", ["cart_A6"]),
        Pin(41, "P1.6", ["cart_A5"]),
        Pin(42, "P1.8", ["cart_A4"]),
        Pin(43, "P1.9-GCK1", ["cart_PHI0"]),
        Pin(44, "P1.11-GCK2", ["cart_A3"]),
        Pin(1, "P1.14-GCK3", ["cart_16MHZ"]),
        Pin(2, "P1.15", ["cart_nINFC"]),
        Pin(3, "P1.17", ["cart_nINFD"]),
        Pin(4, "GND", ["GND"]),
        Pin(5, "P3.2", ["cart_A2"]),
        Pin(6, "P3.5", ["cart_ROMQA"]),
        Pin(7, "P3.8", ["cart_A1"]),
        Pin(8, "P3.9", ["cart_A0"]),
        Pin(9, "TDI", ["cpld_TDI"]),
        Pin(10, "TMS", ["cpld_TMS"]),
        Pin(11, "TCK", ["cpld_TCK"]),
        Pin(12, "P3.11", ["cart_D0"]),
        Pin(13, "P3.14", ["cart_D2"]),
        Pin(14, "P3.15", ["cart_D1"]),
        Pin(15, "VCCINT_3V3", ["3V3"]),
        Pin(16, "P3.17", ["avr_MOSI"]),
        Pin(17, "GND", ["GND"]),
        Pin(18, "P3.16", ["avr_MISO"]),
        Pin(19, "P4.2", ["avr_SCK"]),
        Pin(20, "P4.5", ["cpld_SS"]),
        Pin(21, "P4.8", ["cart_nOE"]),
        Pin(22, "P4.11", ["cart_A10"]),
        Pin(23, "P4.14", ["cart_D3"]),
        Pin(24, "TDO", ["cpld_TDO"]),
        Pin(25, "GND", ["GND"]),
        Pin(26, "VCCIO_2V5_3V3", ["3V3"]),
        Pin(27, "P4.15", ["cart_RnW"]),
        Pin(28, "P4.17", ["cart_A11"]),
        Pin(29, "P2.2", ["cart_A8"]),
        Pin(30, "P2.5", ["cart_A9"]),
        Pin(31, "P2.6", ["cart_A13"]),
        Pin(32, "P2.8", ["cart_D7"]),
        Pin(33, "P2.9-GSR", ["cart_A12"]),
        Pin(34, "P2.11-GTS2", ["cart_D6"]),
        Pin(35, "VCCINT_3V3", ["3V3"]),
        Pin(36, "P2.14-GTS1", ["cart_D5"]),
        Pin(37, "P2.15", ["cart_D4"]),
        Pin(38, "P2.17", ["cart_nOE2"]),
    ],
)
cpld_cap1 = C0805("100n", "3V3", "GND", ref="C1")
cpld_cap2 = C0805("100n", "3V3", "GND", ref="C2")
cpld_cap3 = C0805("100n", "3V3", "GND", ref="C5")

# In case the Pro Micro turns out to be a pain in the ass to use for CPLD programming...
# altera jtag header, like in the lc-electronics xc9572xl board
# left column: tck tdo tms nc tdi
# right column: gnd vcc nc nc gnd
cpld_jtag = Component(
    footprint="Pin_Headers:Pin_Header_Straight_2x05_Pitch2.54mm",
    identifier="CON3",
    value="jtag",
	desc="2x5 header for JTAG programming.  Use generic 0.1 inch header strip or Digikey ED1543-ND.",
    pins=[
        Pin(1, "TCK", ["cpld_TCK"]), # top left
        Pin(2, "GND", ["GND"]), # top right
        Pin(3, "TDO", ["cpld_TDO"]),
        Pin(4, "3V3", ["3V3"]),
        Pin(5, "TMS", ["cpld_TMS"]),
        Pin(6, "NC"),
        Pin(7, "NC"),
        Pin(8, "NC"),
        Pin(9, "TDI", ["cpld_TDI"]),
        Pin(10, "GND", ["GND"]),
    ],
)

regulator = Component(
    footprint="TO_SOT_Packages_SMD:SOT-89-3",
    identifier="U3",
    value="MCP1700T-3302E/MB",
	desc="3.3V LDO regulator.  Search for the exact part number because there are many variants.",
    pins=[
        Pin(2, "VIN", ["5V"]),
        Pin(3, "VOUT", ["3V3"]),
        Pin(1, "GND", ["GND"]),
    ],
)
reg_in_cap = C0805("1u", "5V", "GND", ref="C3")
reg_out_cap = C0805("1u", "3V3", "GND", ref="C4")

dump_netlist("standalone_programmer.net")
dump_bom("bill_of_materials.txt", "readable_bill_of_materials.txt")
