/* hello.c -- Simple Hello World for 68HC11 bootstrap mode
   Copyright 2000, 2001, 2002, 2003 Free Software Foundation, Inc.
   Written by Stephane Carrez (stcarrez@nerim.fr)

This file is part of GEL.

GEL is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GEL is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GEL; see the file COPYING.  If not, write to
the Free Software Foundation, 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.  */

/*! @page hello Hello World Tutorial for 68HC11 bootstrap mode

    This example is a simple hello program that prints "Hello World!"
    on the 68HC11 serial line.  It is self contained by the fact that
    it does not use any include, Makefile or configuration file of GEL.

    To compile this program, you can use the following command:

        m6811-elf-gcc -g -Os -mshort -Wl,-m,m68hc11elfb -o hello.elf hello.c

    This will produce the `hello.elf' file.  This file in a binary file
    in the ELF/DWARF-2 standard format.  It contains the 68HC11 binary
    code, the symbols and the debugging information suitable for gdb.
    To obtain the S19 file corresponding to the program, you can use
    the following command:

        m6811-elf-objcopy --only-section=.text --only-section=.rodata
                          --only-section=.vectors --only-section=.data
                          --output-target=srec hello.elf hello.s19

    and this will produce `hello.s19' file in Motorola S19 format.

    To run or test this program, you can use the simulator or gdb:

    m6811-elf-run hello.elf

    or

    m6811-elf-gdb hello.elf
    (gdb) target sim
    (gdb) load
    (gdb) run

    From the debugger you can execute step by step the program if you
    do the following:

    (gdb) target sim
    (gdb) load
    (gdb) b main
    Breakpoint 1 at 0x6e: file hello.c, line 166.
    (gdb) run
    ...
    (gdb) next
    ...

  @htmlonly
  Source file: <a href="hello_2hello_8c-source.html">hello.c</a>
  @endhtmlonly

*/

/* Part 1:  68HC11 Definitions.

   This part defines several flags and values to access the
   68HC11 SIO registers.  The SIO registers are mapped in memory
   and are therefore accessed as a global variable.

   The complete list of flags are available when you include
   the following file:

   #include <sys/ports.h>

   They are defined in <asm-m68hc11/ports_def_F.h>,
   <asm-m68hc11/ports_def_E.h> or <asm-m68hc12/ports_def.h>
   depending on the microcontroller.

*/

/*
#define M6811_BAUD	0x2B	// SCI Baud register
#define M6811_SCCR1	0x2C	// SCI Control register 1
#define M6811_SCCR2	0x2D	// SCI Control register 2
#define M6811_SCSR	0x2E	// SCI Status register
#define M6811_SCDR	0x2F	// SCI Data (Read => RDR, Write => TDR)

// Flags of the SCCR2 register.
#define M6811_TE	0x08	// Transmit Enable
#define M6811_RE	0x04	// Receive Enable

// Flags of the SCSR register.
#define M6811_TDRE	0x80	// Transmit Data Register Empty

// Flags of the BAUD register.
#define M6811_SCP1	0x20	// SCI Baud rate prescaler select
#define M6811_SCP0	0x10
#define M6811_SCR2	0x04	// SCI Baud rate select
#define M6811_SCR1	0x02
#define M6811_SCR0	0x01

#define M6811_BAUD_DIV_1	(0)
#define M6811_BAUD_DIV_3	(M6811_SCP0)
#define M6811_BAUD_DIV_4	(M6811_SCP1)
#define M6811_BAUD_DIV_13	(M6811_SCP1|M6811_SCP0)

#define M6811_DEF_BAUD M6811_BAUD_DIV_4 // 1200 baud
*/

#define M6812_SC0BDH    0xC0    // SCI Baud rate control register (upper 8-bits)
#define M6812_SC0BDL    0xC1    // SCI Baud rate control register (lower 8-bits)
#define M6812_SC0CR1    0xC2    // SCI Control Register 1
#define M6812_SC0CR2    0xC3    // SCI Control Register 2
#define M6812_SC0SR1    0xC4    // SCI Status Register 1
#define M6812_SC0SR2    0xC5    // SCI Status Register 2
#define M6812_SC0DRH    0xC6    // SCI Data Register high
#define M6812_SC0DRL    0xC7    // SCI Data Register low

// Flags of the SC0CR1 register.
#define M6812_LOOPS     0x80    // SCI LOOP Mode/Single-Wire Mode Enable Bit
#define M6812_WOMS      0x40    // Wired-OR Mode for Serial Pins
#define M6812_RSRC      0x20    // Receiver Source Bit
#define M6812_M         0x10    // Mode Bit (select character format)
#define M6812_WAKE      0x08    // Wakeup by Address Mark/Idle Bit
#define M6812_ILT       0x04    // Idle Line Type Bit
#define M6812_PE        0x02    // Parity Enable Bit
#define M6812_PT        0x01    // Parity Type Bit

// Flags of the SC0CR2 register.
#define M6812_TIE       0x80    // Transmit Interrupt Enable Bit
#define M6812_TCIE      0x40    // Transmit Complete Interrupt Enable Bit
#define M6812_RIE       0x20    // Receiver Interrupt Enable Bit
#define M6812_ILIE      0x10    // Idle Line Interrupt Enable Bit
#define M6812_TE        0x08    // Transmitter Enable Bit
#define M6812_RE        0x04    // Receiver Enable Bit
#define M6812_RWU       0x02    // Receiver Wakeup Control Bit
#define M6812_SBK       0x01    // Send Break Bit

// Flags of the SC0SR1 register.
#define M6812_TDRE      0x80    // Transmit Data Register Empty Flag
#define M6812_TC        0x40    // Transmit Complete Flag
#define M6812_RDRF      0x20    // Receive Data Register Full Flag
#define M6812_IDLE      0x10    // Idle Line Detected Flag
#define M6812_OR        0x08    // Overrun Error Flag
#define M6812_NF        0x04    // Noise Error Flag
#define M6812_FE        0x02    // Framing Error Flag
#define M6812_PF        0x01    // Parity Error Flag

// Flags of the SC0SR2 register.
#define M6812_RAF       0x01    // Receiver Active Flag

// Baud rate information
#define M6812_MCLK      8000000 // 8Mhz "master" bus clock frequency
#define M6812_BAUD_RATE 9600    // 9600 BAUD is the desired baud rate
#define M6812_BAUD_DATA (unsigned int)M6812_MCLK/(16*M6812_BAUD_RATE)

/* The I/O registers are represented by a volatile array.
   Address if fixed at link time.  For this particular example,
   the _io_ports address is defined in the `memory.x' file.  */
extern volatile unsigned char _io_ports[];

/* Part 2:  68HC11 SIO Operations.

   This part defines some functions to write characters
   on the SIO.  They access the IO register through the
   `io_ports' global variable.

   These operations can be used in a program by using
   the following include:

   #include <sys/sio.h>

*/

/*! Send the character on the serial line.

    This function sends the character \a c on the serial line.
    Before sending, it waits for the transmitter to be ready.
    Once the function returns, the character is in the SCI queue
    and it may not be sent completely over the serial line.

    @param c character to send on the serial line.

    @see serial_init, serial_flush
*/
static inline void serial_send (char c)
{
  // Wait until the SIO has finished to send the character.
  while (!(_io_ports[M6812_SC0SR1] & M6812_TDRE))
    continue;

  _io_ports[M6812_SC0DRL] = c;
  _io_ports[M6812_SC0CR2] |= M6812_TE;
}

void serial_print (const char *msg)
{
  while (*msg != 0)
    serial_send (*msg++);
}

/* Part 3:  Hello World

   When the function 'main' is entered, the stack pointer is
   initialized, the global variables are initialized but the
   SIO as well as other 68HC11 devices are not yet initialized.
   In this example, we first configure the SIO by setting
   the baud rate and the character format.  The SIO must then
   be started by enabling the transmitter and receiver.

   The SIO initialization is inlined here for clarity but it
   is implemented by the 'serial_init' operation available when
   <sys/sio.h> is included.

   Once the SIO is initialized, we can write messages using
   serial_print().

   Note: In this example, the main() function returns and this will give
   back control to the startup code which will in turn call exit().  The
   exit() will loop forever arround a 'wai' instruction.  Indeed, a real
   68HC11 hardware has no way to exit!
*/

int main (void)
{
  // Configure the SCI to send at M6812_BAUD_RATE baud.
  _io_ports[M6812_SC0BDH] = (M6812_BAUD_DATA & 0xFF00) >> 8;
  _io_ports[M6812_SC0BDL] = M6812_BAUD_DATA & 0xFF;

  // Configure SCI for normal operation, character format 1 start, 8-bits, 1 stop.
  _io_ports[M6812_SC0CR1] = 0;

  // Enable receiver and transmitter.
  _io_ports[M6812_SC0CR2] = M6812_TE | M6812_RE;

  serial_print ("Hello world!\n");
  return 0;
}
