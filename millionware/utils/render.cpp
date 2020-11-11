#include <algorithm>
#include <cstdint>

#include "render.hpp"

struct device_state_t
{
  uint32_t state_id;
  uint32_t new_value;
  uint32_t old_value;
};

static LPDIRECT3DDEVICE9 d3d9_device = nullptr;
static LPDIRECT3DVERTEXDECLARATION9 vertex_decl = nullptr;
static LPDIRECT3DVERTEXSHADER9 vertex_shader = nullptr;

static uint32_t device_fvf = 0;
static device_state_t device_states[] = {
  {D3DRS_LIGHTING, FALSE, 0},
  {D3DRS_FOGENABLE, FALSE, 0},
  {D3DRS_CULLMODE, D3DCULL_NONE, 0},
  {D3DRS_FILLMODE, D3DFILL_SOLID, 0},
  {D3DRS_ZENABLE, FALSE, 0},
  {D3DRS_SCISSORTESTENABLE, TRUE, 0},
  {D3DRS_ZWRITEENABLE, TRUE, 0},
  {D3DRS_STENCILENABLE, FALSE, 0},
  {D3DRS_MULTISAMPLEANTIALIAS, FALSE, 0},
  {D3DRS_ANTIALIASEDLINEENABLE, FALSE, 0},
  {D3DRS_ALPHABLENDENABLE, TRUE, 0},
  {D3DRS_ALPHATESTENABLE, FALSE, 0},
  {D3DRS_SEPARATEALPHABLENDENABLE, TRUE, 0},
  {D3DRS_SRCBLEND, D3DBLEND_SRCALPHA, 0},
  {D3DRS_SRCBLENDALPHA, D3DBLEND_INVDESTALPHA, 0},
  {D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA, 0},
  {D3DRS_DESTBLENDALPHA, D3DBLEND_ONE, 0},
  {D3DRS_SRGBWRITEENABLE, FALSE, 0},
  {D3DRS_COLORWRITEENABLE, D3DCOLORWRITEENABLE_RED | D3DCOLORWRITEENABLE_GREEN | D3DCOLORWRITEENABLE_BLUE | D3DCOLORWRITEENABLE_ALPHA, 0},
};

void render::initialize(const LPDIRECT3DDEVICE9 device) {
  d3d9_device = device;

  create_objects();
}

void render::shutdown() {
  destroy_objects();
}

void render::create_objects() {

}

void render::destroy_objects() {

}

void render::begin_frame() {
  d3d9_device->GetFVF(reinterpret_cast<DWORD*>(&device_fvf));
  d3d9_device->GetVertexDeclaration(&vertex_decl);
  d3d9_device->GetVertexShader(&vertex_shader);

  d3d9_device->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

  for (auto& state : device_states) {
    const auto render_state = static_cast<D3DRENDERSTATETYPE>(state.state_id);

    d3d9_device->GetRenderState(render_state, reinterpret_cast<DWORD*>(&state.old_value));
    d3d9_device->SetRenderState(render_state, state.new_value);
  }
}

void render::finish_frame() {
  d3d9_device->SetFVF(device_fvf);
  d3d9_device->SetVertexDeclaration(vertex_decl);
  d3d9_device->SetVertexShader(vertex_shader);

  for (auto& state : device_states) {
    d3d9_device->SetRenderState(static_cast<D3DRENDERSTATETYPE>(state.state_id), state.old_value);
  }
} 

void render::rect(const int test) {
  struct vertex_t
  {
    float x, y, z, r;

    uint32_t color;
  };

  const auto health_pt = std::clamp(static_cast<float>(test) / 100.0f, 0.0f, 1.0f);
  const auto redness = static_cast<int>(health_pt * 255.0f);

  vertex_t vertices[] = {
    {0.0f, 0.0f, 0.01f, 0.01f, D3DCOLOR_RGBA(redness, 0, 0, 255)},
    {48.0f, 0.0f, 0.01f, 0.01f, D3DCOLOR_RGBA(redness, 0, 0, 255)},
    {0.0f, 48.0f, 0.01f, 0.01f, D3DCOLOR_RGBA(redness, 0, 0, 255)},
    {48.0f, 48.0f, 0.01f, 0.01f, D3DCOLOR_RGBA(redness, 0, 0, 255)},
  };

  d3d9_device->SetTexture(0, nullptr);
  d3d9_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, &vertices, sizeof vertex_t);
}
