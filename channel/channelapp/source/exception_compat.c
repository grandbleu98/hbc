/*
 * Compatibility implementation for the legacy HBC chainloader.
 *
 * libogc 3.x replaced the old __exception_closeall() API with the
 * Tuxedo exception subsystem. Before handing control to another DOL,
 * restore the low-memory exception vectors to the legacy RFI stubs.
 */
#include <ogc/cache.h>
#include <ogc/machine/processor.h>

void __exception_closeall(void)
{
	u32 i;

	/* Match the legacy libogc shutdown sequence. */
	mtmsr(mfmsr() & ~MSR_EE);
	mtmsr(mfmsr() | (MSR_FP | MSR_RI));

	/*
	 * libogc 3.x initializes exception vectors at 0x80000100 through
	 * 0x80001300 and up to the breakpoint vector at 0x80001300,
	 * with one 0x100-byte slot per exception number.
	 *
	 * An RFI as the first instruction safely bypasses the installed
	 * handler when the chainloader transfers control to another title.
	 */
	for (i = 1; i <= 19; ++i) {
		volatile u32 *vector = (volatile u32 *)(0x80000000u | (i << 8));
		*vector = 0x4C000064u; /* rfi */
		DCFlushRangeNoSync((void *)vector, 0x100);
		ICInvalidateRange((void *)vector, 0x100);
	}

	_sync();
}
