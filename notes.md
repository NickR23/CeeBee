# Decoding opcodes
Suppose we have the following byte at 0x000000: 31

in binary -> 00110001

Where x = 00
      y = 110
      z = 001
      p = 3
      q = 0

z = 1, q = 0, p = 3
  This means load sp with the data at the next 16-bit operand (unsigned int)
  Which just so happens to be 0xffef (This address is little endian!)
  so load whats at 0xfeff into sp!
