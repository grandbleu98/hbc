/*
 * Compatibility implementation for the legacy HBC chainloader.
 *
 * libogc 3.x replaced the old __exception_closeall() API with the
 * Tuxedo exception subsystem. Before handing control to another DOL,
 * restore only the PowerPC exception vectors used by the Wii.
 */
#include <ogc/cache.h>
#include <ogc/machine/processor.h>

void __exception_closeall(void)
{
	static const u16 exception_vectors[] = {
		0x0100, /* System reset */
		0x0200, /* Machine check */
		0x0300, /* Data storage */
		0x0400, /* Instruction storage */
		0x0500, /* External interrupt */
		0x0600, /* Alignment */
		0x0700, /* Program */
		0x0800, /* Floating point unavailable */
		0x0900, /* Decrementer */
		0x0C00, /* System call */
		0x0D00, /* Trace */
		0x0F00, /* Performance monitor */
		0x1300, /* Instruction address breakpoint */
		0x1400, /* Reserved */
		0x1700  /* Thermal */
	};
	u32 i;

	/* Match the legacy libogc shutdown sequence. */
	mtmsr(mfmsr() & ~MSR_EE);
	mtmsr(mfmsr() | (MSR_FP | MSR_RI));

	/*
	 * Do not write every 0x100-byte slot. Several offsets between
	 * these vectors are reserved low-memory locations, not exception
	 * vectors, and must remain untouched.
	 */
	for (i = 0; i < sizeof(exception_vectors) / sizeof(exception_vectors[0]); ++i) {
		volatile u32 *vector = (volatile u32 *)(0x80000000u | exception_vectors[i]);
		*vector = 0x4C000064u; /* rfi */
		DCFlushRangeNoSync((void *)vector, 0x100);
		ICInvalidateRange((void *)vector, 0x100);
	}

	_sync();
}
