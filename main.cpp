#include <iostream>
#include <gccore.h>
#include <wiiuse/wpad.h>
#include <unistd.h>
#include <stdexcept>

extern "C" {
#include <libpatcher/libpatcher.h>
}

#include "EVCSaveFile.h"

static void* xfb = nullptr;
static GXRModeObj* rmode = nullptr;

int main() {
  VIDEO_Init();

  rmode = VIDEO_GetPreferredMode(nullptr);
  xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
  console_init(xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * VI_DISPLAY_PIX_SZ);
  VIDEO_Configure(rmode);
  VIDEO_SetNextFramebuffer(xfb);
  VIDEO_SetBlack(FALSE);
  VIDEO_Flush();
  VIDEO_WaitVSync();
  if (rmode->viTVMode & VI_NON_INTERLACE) VIDEO_WaitVSync();

  WPAD_Init();
  ISFS_Initialize();
  CONF_Init();

  std::cout << std::endl << std::endl << std::endl;
  std::cout << "WiiLink EVC Transfer Tool - (c) 2023 WiiLink" << std::endl;
  std::cout << "v0.1" << std::endl << std::endl;

  bool success = apply_patches();
  if (!success) {
    std::cout << "Failed to apply patches!" << std::endl;
    sleep(5);
    WII_ReturnToMenu();
  }

  EVCSaveFile save_file{};

  if (save_file.Error().empty())
  {
    std::cout << "Removing current RiiConnect24 polls..." << std::endl << std::endl;
    save_file.RemoveLarsen();

    if (!save_file.FlushData())
    {
      std::cout << "Failed to save EVC save file" << std::endl;
    }
  }
  else
  {
    std::cout << save_file.Error() << std::endl << std::endl;
  }

  std::cout << "If you have any questions, join the WiiLink Discord at: " << std::endl << "https://discord.gg/reqUMqxu8D" << std::endl << std::endl;
  std::cout << "Press HOME to return to the Wii Menu." << std::endl;

  while (true) {
    WPAD_ScanPads();
    u32 pressed = WPAD_ButtonsDown(0);

    if (pressed & WPAD_BUTTON_HOME) WII_ReturnToMenu();

    VIDEO_WaitVSync();
  }

  return 0;
}