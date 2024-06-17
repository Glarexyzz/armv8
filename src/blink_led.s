#COMMENTS HAVE TO BE REMOVED BEFORE COMPILING

# Set bits 5-3 of word at 0x3f20 0008 to 001, this makes GIPO 21 an output

# Set bit 21 of GPSET0 at 0x3f20 001c to 1 to turn light on

# Set bit 21 of GPCLR0 at 0x3f20 0028 to 1 to clear signal / light off