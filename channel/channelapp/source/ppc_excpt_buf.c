/*
 * Compatibility storage required by libogc 3.2.x exception assembly.
 * The library references this context buffer but does not define it.
 */
#include <tuxedo/ppc/context.h>

PPCContext __ppc_excpt_buf __attribute__((aligned(8)));
