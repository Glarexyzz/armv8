initilialize:
    ldr w1, SET_BIT3
    ldr w2, GPFSEL2
    str w1, [w2]

    ldr w1, SET_BIT21
    ldr w2, GPSET0
    ldr w3, GPCLR0
    movz w0, #0

blink_loop:
high_signal_output:
    str w0, [w3]
    str w1, [w2]
    ldr w4, WAIT_TIME

wait_high:
    subs w4, w4, #1
    b.ne wait_high

low_signal_output:
    str w0, [w2]
    str w1, [w3]
    ldr w4, WAIT_TIME

wait_low:
    subs w4, w4, #1
    b.ne wait_low

b blink_loop
and x0, x0, x0

GPFSEL2:
    .int 0x3f200008
GPSET0:
    .int 0x3f20001c
GPCLR0:
    .int 0x3f200028
SET_BIT3:
    .int 0x08
SET_BIT21:
    .int 0X200000
WAIT_TIME:
    .int 0x000fffff