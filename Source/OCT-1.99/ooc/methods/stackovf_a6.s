| stackovf_a6.s -- return current A6 (frame pointer) for stackovf.c backtrace.
| Link when using SAS/C; add DEFINE STACKOVF_SASC_A6 when compiling stackovf.c.
| Syntax is GNU as (gas). For SAS/C assembler use: move.l a6,d0 / rts.

	.text
	.globl	_stackovf_get_a6
_stackovf_get_a6:
	movel	a6,d0
	rts
