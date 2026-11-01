#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <wiiuse/wpad.h>

static void *xfb;
static GXRModeObj *rmode;

int main(int argc, char **argv)
{
    int wpad_result;

    VIDEO_Init();
    WPAD_Init();

    rmode = VIDEO_GetPreferredMode(NULL);
    xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    if (!xfb) {
        while (1) VIDEO_WaitVSync();
    }

    console_init(xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight,
                 rmode->fbWidth * VI_DISPLAY_PIX_SZ);
    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if (rmode->viTVMode & VI_NON_INTERLACE)
        VIDEO_WaitVSync();

    wpad_result = WPAD_Init();

    printf("\x1b[2J\x1b[2;0H");
    printf("HBC LOADER PROBE\n");
    printf("------------------------------\n");
    printf("Video initialization: OK\n");
    printf("WPAD_Init result: %d\n", wpad_result);
    printf("\n");
    printf("If this screen is visible, the Wii\n");
    printf("and HBC DOL loading path are working.\n");
    printf("\n");
    printf("Press HOME on a Wii Remote to exit.\n");

    while (SYS_MainLoop()) {
        WPAD_ScanPads();
        if (WPAD_ButtonsDown(0) & WPAD_BUTTON_HOME)
            break;
        VIDEO_WaitVSync();
    }

    return 0;
}
