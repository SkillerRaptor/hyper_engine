/*
 * Copyright (c) 2023 SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

////////////////////////////////////////////////////////////////////////////////
// Bindless
////////////////////////////////////////////////////////////////////////////////

struct RenderResourceHandle {
  uint index;
};

struct BindingsOffset {
  RenderResourceHandle bindings_offset;
  uint unused_0;
  uint unused_1;
  uint unused_2;
};

static const uint DESCRIPTOR_SET_BINDLESS_STORAGE_BUFFER = 0;
static const uint DESCRIPTOR_SET_BINDLESS_SAMPLED_IMAGE = 1;
static const uint DESCRIPTOR_SET_BINDLESS_STORAGE_IMAGE = 2;
static const uint DESCRIPTOR_SET_BINDLESS_SAMPLER = 3;

#define DEFINE_BUFFER_HEAP(type) \
  struct type##Handle { \
    uint internal_index; \
  }; \
  [[vk::binding(0, DESCRIPTOR_SET_BINDLESS_STORAGE_BUFFER)]] type g##_##type[]

#define DEFINE_TEXTURE_HEAP(texture_type) \
  template <typename T> \
  struct texture_type##Handle { \
    uint internal_index; \
  }; \
  [[vk::binding(0, DESCRIPTOR_SET_BINDLESS_SAMPLED_IMAGE)]] texture_type<float> g##_##texture_type##_##float[]; \
  [[vk::binding(0, DESCRIPTOR_SET_BINDLESS_SAMPLED_IMAGE)]] texture_type<float2> g##_##texture_type##_##float2[]; \
  [[vk::binding(0, DESCRIPTOR_SET_BINDLESS_SAMPLED_IMAGE)]] texture_type<float3> g##_##texture_type##_##float3[]; \
  [[vk::binding(0, DESCRIPTOR_SET_BINDLESS_SAMPLED_IMAGE)]] texture_type<float4> g##_##texture_type##_##float4[]

#define DEFINE_RW_TEXTURE_HEAP(texture_type) \
  template <typename T> \
  struct texture_type##Handle { \
    uint internal_index; \
  }; \
  [[vk::binding(0, DESCRIPTOR_SET_BINDLESS_STORAGE_IMAGE)]] texture_type<float> g##_##texture_type##_##float[]; \
  [[vk::binding(0, DESCRIPTOR_SET_BINDLESS_STORAGE_IMAGE)]] texture_type<float2> g##_##texture_type##_##float2[]; \
  [[vk::binding(0, DESCRIPTOR_SET_BINDLESS_STORAGE_IMAGE)]] texture_type<float3> g##_##texture_type##_##float3[]; \
  [[vk::binding(0, DESCRIPTOR_SET_BINDLESS_STORAGE_IMAGE)]] texture_type<float4> g##_##texture_type##_##float4[]

#define DEFINE_SAMPLER_HEAP(type) \
  struct type##Handle { \
    uint internal_index; \
  }; \
  [[vk::binding(0, DESCRIPTOR_SET_BINDLESS_SAMPLER)]] type g##_##type[]

#define DEFINE_BUFFER_HEAP_OPERATOR(type) \
    type operator[](type##Handle identifier) { \
        return g_##type[NonUniformResourceIndex(identifier.internal_index)]; \
    }

#define DEFINE_TEXTURE_HEAP_OPERATOR_VALUE(resource_type, register_name, value_type, handle_name) \
    resource_type<value_type> operator[](handle_name<value_type> identifier) { \
        return register_name##_##value_type[NonUniformResourceIndex(identifier.internal_index)]; \
    }

#define DEFINE_SAMPLER_HEAP_OPERATOR(type) \
    type operator[](type##Handle identifier) { \
        return g_##type[NonUniformResourceIndex(identifier.internal_index)]; \
    }

#define DEFINE_TEXTURE_HEAP_OPERATOR(texture_type) \
  DEFINE_TEXTURE_HEAP_OPERATOR_VALUE(texture_type, g_##texture_type, float, texture_type##Handle) \
  DEFINE_TEXTURE_HEAP_OPERATOR_VALUE(texture_type, g_##texture_type, float2, texture_type##Handle) \
  DEFINE_TEXTURE_HEAP_OPERATOR_VALUE(texture_type, g_##texture_type, float3, texture_type##Handle) \
  DEFINE_TEXTURE_HEAP_OPERATOR_VALUE(texture_type, g_##texture_type, float4, texture_type##Handle)

[[vk::push_constant]] ConstantBuffer<BindingsOffset> g_bindings_offset;

DEFINE_BUFFER_HEAP(ByteAddressBuffer);
DEFINE_BUFFER_HEAP(RWByteAddressBuffer);

DEFINE_TEXTURE_HEAP(Texture1D);
DEFINE_TEXTURE_HEAP(Texture1DArray);
DEFINE_TEXTURE_HEAP(Texture2D);
DEFINE_TEXTURE_HEAP(Texture2DArray);
DEFINE_TEXTURE_HEAP(Texture3D);
DEFINE_TEXTURE_HEAP(TextureCube);

DEFINE_RW_TEXTURE_HEAP(RWTexture1D);
DEFINE_RW_TEXTURE_HEAP(RWTexture1DArray);
DEFINE_RW_TEXTURE_HEAP(RWTexture2D);
DEFINE_RW_TEXTURE_HEAP(RWTexture2DArray);
DEFINE_RW_TEXTURE_HEAP(RWTexture3D);

DEFINE_SAMPLER_HEAP(SamplerState);

struct VulkanResourceDescriptorHeapInternal {
  DEFINE_BUFFER_HEAP_OPERATOR(ByteAddressBuffer)
  DEFINE_BUFFER_HEAP_OPERATOR(RWByteAddressBuffer)

  DEFINE_TEXTURE_HEAP_OPERATOR(Texture1D)
  DEFINE_TEXTURE_HEAP_OPERATOR(RWTexture1D)
  DEFINE_TEXTURE_HEAP_OPERATOR(Texture1DArray)
  DEFINE_TEXTURE_HEAP_OPERATOR(RWTexture1DArray)
  DEFINE_TEXTURE_HEAP_OPERATOR(Texture2D)
  DEFINE_TEXTURE_HEAP_OPERATOR(RWTexture2D)
  DEFINE_TEXTURE_HEAP_OPERATOR(Texture2DArray)
  DEFINE_TEXTURE_HEAP_OPERATOR(RWTexture2DArray)
  DEFINE_TEXTURE_HEAP_OPERATOR(Texture3D)
  DEFINE_TEXTURE_HEAP_OPERATOR(RWTexture3D)
  DEFINE_TEXTURE_HEAP_OPERATOR(TextureCube)

  DEFINE_SAMPLER_HEAP_OPERATOR(SamplerState)
};

static VulkanResourceDescriptorHeapInternal VkResourceDescriptorHeap;

#define DESCRIPTOR_HEAP(handle_type, handle) VkResourceDescriptorHeap[(handle_type) handle]

template <typename T>
T load_bindings() {
  T result = DESCRIPTOR_HEAP(ByteAddressBufferHandle, g_bindings_offset.bindings_offset.index).Load<T>(0);
  return result;
}

struct SimpleBuffer {
  RenderResourceHandle handle;

  template <typename T>
  T load() {
    ByteAddressBuffer buffer = DESCRIPTOR_HEAP(ByteAddressBufferHandle, this.handle.index);
    T result = buffer.Load<T>(0);
    return result;
  }
};

struct RwSimpleBuffer {
  RenderResourceHandle handle;

  template <typename T>
  T load() {
    RWByteAddressBuffer buffer = DESCRIPTOR_HEAP(RWByteAddressBufferHandle, this.handle.index);
    T result = buffer.Load<T>(0);
    return result;
  }

  template <typename T>
  void store(T value) {
    RWByteAddressBuffer buffer = DESCRIPTOR_HEAP(RWByteAddressBufferHandle, this.handle.index);
    buffer.Store<T>(0, value);
  }
};

struct ArrayBuffer {
  RenderResourceHandle handle;

  template <typename T>
  T load(uint index) {
    ByteAddressBuffer buffer = DESCRIPTOR_HEAP(ByteAddressBufferHandle, this.handle.index);
    T result = buffer.Load<T>(sizeof(T) * index);
    return result;
  }
};

struct RwArrayBuffer {
  RenderResourceHandle handle;

  template <typename T>
  T load(uint index) {
    RWByteAddressBuffer buffer = DESCRIPTOR_HEAP(RWByteAddressBufferHandle, this.handle.index);
    T result = buffer.Load<T>(sizeof(T) * index);
    return result;
  }

  template <typename T>
  void store(uint index, T value) {
    RWByteAddressBuffer buffer = DESCRIPTOR_HEAP(RWByteAddressBufferHandle, this.handle.index);
    buffer.Store<T>(sizeof(T) * index, value);
  }
};

struct Texture {
  RenderResourceHandle handle;

  template <typename T>
  T load_1d(uint pos) {
    Texture1D<T> texture = DESCRIPTOR_HEAP(Texture1DHandle<T>, this.handle.index);
    return texture.Load(uint2(pos, 0));
  }

  template <typename T>
  T load_1d_array(uint2 pos) {
    Texture1DArray<T> texture = DESCRIPTOR_HEAP(Texture1DArrayHandle<T>, this.handle.index);
    return texture.Load(uint3(pos.x, 0, pos.y));
  }

  template <typename T>
  T load_2d(uint2 pos) {
    Texture2D<T> texture = DESCRIPTOR_HEAP(Texture2DHandle<T>, this.handle.index);
    return texture.Load(uint3(pos, 0));
  }

  template <typename T>
  T load_2d_array(uint3 pos) {
    Texture2DArray<T> texture = DESCRIPTOR_HEAP(Texture2DArrayHandle<T>, this.handle.index);
    return texture.Load(uint4(pos.xy, 0, pos.z));
  }

  template <typename T>
  T load_3d(uint3 pos) {
    Texture3D<T> texture = DESCRIPTOR_HEAP(Texture3DHandle<T>, this.handle.index);
    return texture.Load(uint4(pos, 0));
  }

  template<typename T>
  T sample_1d(float u) {
      Texture1D<T> texture = DESCRIPTOR_HEAP(Texture1DHandle<T>, this.handle.index);
      SamplerState sampler = DESCRIPTOR_HEAP(SamplerStateHandle, this.handle.index);
      return texture.Sample(sampler, u);
  }

  template<typename T>
  T sample_2d(float2 uv) {
      Texture2D<T> texture = DESCRIPTOR_HEAP(Texture2DHandle<T>, this.handle.index);
      SamplerState sampler = DESCRIPTOR_HEAP(SamplerStateHandle, this.handle.index);
      return texture.Sample(sampler, uv);
  }

  template<typename T>
  T sample_3d(float3 uvw) {
      Texture3D<T> texture = DESCRIPTOR_HEAP(Texture3DHandle<T>, this.handle.index);
      SamplerState sampler = DESCRIPTOR_HEAP(SamplerStateHandle, this.handle.index);
      return texture.Sample(sampler, uvw);
  }

  template<typename T>
  T sample_level_1d(float u, float mip) {
      Texture2D<T> texture = DESCRIPTOR_HEAP(Texture1DHandle<T>, this.handle.index);
      SamplerState sampler = DESCRIPTOR_HEAP(SamplerStateHandle, this.handle.index);
      return texture.SampleLevel(sampler, u, mip);
  }

  template<typename T>
  T sample_level_2d(float2 uv, float mip) {
      Texture2D<T> texture = DESCRIPTOR_HEAP(Texture2DHandle<T>, this.handle.index);
      SamplerState sampler = DESCRIPTOR_HEAP(SamplerStateHandle, this.handle.index);
      return texture.SampleLevel(sampler, uv, mip);
  }

  template<typename T>
  T sample_level_3d(float3 uvw, float mip) {
      Texture3D<T> texture = DESCRIPTOR_HEAP(Texture3DHandle<T>, this.handle.index);
      SamplerState sampler = DESCRIPTOR_HEAP(SamplerStateHandle, this.handle.index);
      return texture.SampleLevel(sampler, uvw, mip);
  }
};

struct RwTexture {
  RenderResourceHandle handle;

  template <typename T>
  T load_1d(uint pos) {
    Texture1D<T> texture = DESCRIPTOR_HEAP(Texture1DHandle<T>, this.handle.index);
    return texture.Load(uint2(pos, 0));
  }

  template <typename T>
  T load_1d_array(uint2 pos) {
    Texture1DArray<T> texture = DESCRIPTOR_HEAP(Texture1DArrayHandle<T>, this.handle.index);
    return texture.Load(uint3(pos.x, 0, pos.y));
  }

  template <typename T>
  T load_2d(uint2 pos) {
    Texture2D<T> texture = DESCRIPTOR_HEAP(Texture2DHandle<T>, this.handle.index);
    return texture.Load(uint3(pos, 0));
  }

  template <typename T>
  T load_2d_array(uint3 pos) {
    Texture2DArray<T> texture = DESCRIPTOR_HEAP(Texture2DArrayHandle<T>, this.handle.index);
    return texture.Load(uint4(pos.xy, 0, pos.z));
  }

  template <typename T>
  T load_3d(uint3 pos) {
    Texture3D<T> texture = DESCRIPTOR_HEAP(Texture3DHandle<T>, this.handle.index);
    return texture.Load(uint4(pos, 0));
  }

  template <typename T>
  void store_1d(uint pos, T value) {
    RWTexture1D<T> texture = DESCRIPTOR_HEAP(RWTexture1DHandle<T>, this.handle.index);
    texture[pos] = value;
  }

  template <typename T>
  void store_1d_array(uint2 pos, T value) {
    RWTexture1DArray<T> texture = DESCRIPTOR_HEAP(RWTexture1DArrayHandle<T>, this.handle.index);
    texture[pos] = value;
  }

  template <typename T>
  void store_2d(uint2 pos, T value) {
    RWTexture2D<T> texture = DESCRIPTOR_HEAP(RWTexture2DHandle<T>, this.handle.index);
    texture[pos] = value;
  }

  template <typename T>
  void store_2d_array(uint3 pos, T value) {
    RWTexture2DArray<T> texture = DESCRIPTOR_HEAP(RWTexture2DArrayHandle<T>, this.handle.index);
    texture[pos] = value;
  }

  template <typename T>
  void store_3d(uint3 pos, T value) {
    RWTexture3D<T> texture = DESCRIPTOR_HEAP(RWTexture3DHandle<T>, this.handle.index);
    texture[pos] = value;
  }

  template<typename T>
  T sample_1d(float u) {
      Texture1D<T> texture = DESCRIPTOR_HEAP(Texture1DHandle<T>, this.handle.index);
      SamplerState sampler = DESCRIPTOR_HEAP(SamplerStateHandle, this.handle.index);
      return texture.Sample(sampler, u);
  }

  template<typename T>
  T sample_2d(float2 uv) {
      Texture2D<T> texture = DESCRIPTOR_HEAP(Texture2DHandle<T>, this.handle.index);
      SamplerState sampler = DESCRIPTOR_HEAP(SamplerStateHandle, this.handle.index);
      return texture.Sample(sampler, uv);
  }

  template<typename T>
  T sample_3d(float3 uvw) {
      Texture3D<T> texture = DESCRIPTOR_HEAP(Texture3DHandle<T>, this.handle.index);
      SamplerState sampler = DESCRIPTOR_HEAP(SamplerStateHandle, this.handle.index);
      return texture.Sample(sampler, uvw);
  }

  template<typename T>
  T sample_level_1d(float u, float mip) {
      Texture2D<T> texture = DESCRIPTOR_HEAP(Texture1DHandle<T>, this.handle.index);
      SamplerState sampler = DESCRIPTOR_HEAP(SamplerStateHandle, this.handle.index);
      return texture.SampleLevel(sampler, u, mip);
  }

  template<typename T>
  T sample_level_2d(float2 uv, float mip) {
      Texture2D<T> texture = DESCRIPTOR_HEAP(Texture2DHandle<T>, this.handle.index);
      SamplerState sampler = DESCRIPTOR_HEAP(SamplerStateHandle, this.handle.index);
      return texture.SampleLevel(sampler, uv, mip);
  }

  template<typename T>
  T sample_level_3d(float3 uvw, float mip) {
      Texture3D<T> texture = DESCRIPTOR_HEAP(Texture3DHandle<T>, this.handle.index);
      SamplerState sampler = DESCRIPTOR_HEAP(SamplerStateHandle, this.handle.index);
      return texture.SampleLevel(sampler, uvw, mip);
  }
};

////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////

static const uint DESCRIPTOR_SET_SLOT_FRAME = 0;
static const uint DESCRIPTOR_SET_SLOT_CAMERA = 1;

struct Frame {
  float time;
  float delta_time;
  float unused_0;
  float unused_1;

  uint frame_count;
  uint unused_2;
  uint unused_3;
  uint unused_4;

  float2 screen_size;
  float2 unused_5;
};

struct Camera {
  float4x4 view_projection;
};

inline Frame get_frame() {
  ByteAddressBuffer buffer = DESCRIPTOR_HEAP(ByteAddressBufferHandle, DESCRIPTOR_SET_SLOT_FRAME);
  return buffer.Load<Frame>(0);
}

inline Camera get_camera() {
  ByteAddressBuffer buffer = DESCRIPTOR_HEAP(ByteAddressBufferHandle, DESCRIPTOR_SET_SLOT_CAMERA);
  return buffer.Load<Camera>(0);
}

////////////////////////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////////////////////////

float3 linear_to_srgb(float3 x)
{
  if (all(x < float3(0.0031308, 0.0031308, 0.0031308))) {
    return 12.92 * x;
  } else {
    return 1.055 * pow(x, 1.0 / 2.4) - 0.055;
  }
}

float3 srgb_to_linear(float3 x)
{
  if (all(x < float3(0.04045, 0.04045, 0.04045))) {
    return x / 12.92;
  } else {
    return pow((x + 0.055) / 1.055, 2.4);
  }
}