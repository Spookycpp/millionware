#pragma once

#include <d3dx9.h>

namespace render
{
  void initialize(LPDIRECT3DDEVICE9 device);
  void shutdown();

  void create_objects();
  void destroy_objects();

  void begin_frame();
  void finish_frame();

  void rect(int test);
}
