#include "Application.h"

int main()
{
  const uint32_t width{ 800 };
  const uint32_t height{ 800 };
  Application app{ "Vulkan", width, height };
  app.run();
}