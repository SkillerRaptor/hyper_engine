/*
 * Copyright (c) 2024 SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef HYPER_ENGINE_GLOBALS_HLSLI
#define HYPER_ENGINE_GLOBALS_HLSLI

////////////////////////////////////////////////////////////////////////////////
// Bindless
////////////////////////////////////////////////////////////////////////////////

struct RenderResourceHandle {
    uint index;

    uint read_index() {
        return this.index;
    }

#ifdef HYPER_ENGINE_VULKAN
    uint write_index() {
        return this.read_index();
    }
#else
    uint write_index() {
        return this.read_index() + 1;
    }
#endif
};

#ifdef HYPER_ENGINE_VULKAN
    static const uint DESCRIPTOR_SET_BINDLESS_STORAGE_BUFFER = 0;
    static const uint DESCRIPTOR_SET_BINDLESS_SAMPLED_IMAGE = 1;
    static const uint DESCRIPTOR_SET_BINDLESS_STORAGE_IMAGE = 2;

    #define DEFINE_BUFFER_HEAP(type) \
    struct type##Handle { \
        uint internal_index; \
    }; \
    [[vk::binding(0, DESCRIPTOR_SET_BINDLESS_STORAGE_BUFFER)]] type g##_##type[]

    // TODO: Add samplers
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

    #define DEFINE_BUFFER_HEAP_OPERATOR(type) \
        type operator[](type##Handle identifier) { \
            return g_##type[NonUniformResourceIndex(identifier.internal_index)]; \
        }

    #define DEFINE_TEXTURE_HEAP_OPERATOR_VALUE(resource_type, register_name, value_type, handle_name) \
        resource_type<value_type> operator[](handle_name<value_type> identifier) { \
            return register_name##_##value_type[NonUniformResourceIndex(identifier.internal_index)]; \
        }

    #define DEFINE_TEXTURE_HEAP_OPERATOR(texture_type) \
    DEFINE_TEXTURE_HEAP_OPERATOR_VALUE(texture_type, g_##texture_type, float, texture_type##Handle) \
    DEFINE_TEXTURE_HEAP_OPERATOR_VALUE(texture_type, g_##texture_type, float2, texture_type##Handle) \
    DEFINE_TEXTURE_HEAP_OPERATOR_VALUE(texture_type, g_##texture_type, float3, texture_type##Handle) \
    DEFINE_TEXTURE_HEAP_OPERATOR_VALUE(texture_type, g_##texture_type, float4, texture_type##Handle)

    DEFINE_BUFFER_HEAP(ByteAddressBuffer);
    DEFINE_BUFFER_HEAP(RWByteAddressBuffer);

    DEFINE_TEXTURE_HEAP(Texture1D);
    DEFINE_TEXTURE_HEAP(Texture2D);
    DEFINE_TEXTURE_HEAP(Texture3D);
    DEFINE_TEXTURE_HEAP(TextureCube);

    DEFINE_RW_TEXTURE_HEAP(RWTexture1D);
    DEFINE_RW_TEXTURE_HEAP(RWTexture2D);
    DEFINE_RW_TEXTURE_HEAP(RWTexture3D);

    struct VulkanResourceDescriptorHeapInternal {
    DEFINE_BUFFER_HEAP_OPERATOR(ByteAddressBuffer)
    DEFINE_BUFFER_HEAP_OPERATOR(RWByteAddressBuffer)

    DEFINE_TEXTURE_HEAP_OPERATOR(Texture1D)
    DEFINE_TEXTURE_HEAP_OPERATOR(RWTexture1D)
    DEFINE_TEXTURE_HEAP_OPERATOR(Texture2D)
    DEFINE_TEXTURE_HEAP_OPERATOR(RWTexture2D)
    DEFINE_TEXTURE_HEAP_OPERATOR(Texture3D)
    DEFINE_TEXTURE_HEAP_OPERATOR(RWTexture3D)
    DEFINE_TEXTURE_HEAP_OPERATOR(TextureCube)
    };

    static VulkanResourceDescriptorHeapInternal VkResourceDescriptorHeap;

    #define DESCRIPTOR_HEAP(handle_type, handle) VkResourceDescriptorHeap[(handle_type) handle]
#else
    #define DESCRIPTOR_HEAP(handle_type, handle) ResourceDescriptorHeap[NonUniformResourceIndex(handle)]
#endif

struct SimpleBuffer {
  RenderResourceHandle handle;

  template <typename T>
  T load() {
    ByteAddressBuffer buffer = DESCRIPTOR_HEAP(ByteAddressBufferHandle, this.handle.read_index());
    T result = buffer.Load<T>(0);
    return result;
  }
};

struct RwSimpleBuffer {
  RenderResourceHandle handle;

  template <typename T>
  T load() {
    RWByteAddressBuffer buffer = DESCRIPTOR_HEAP(RWByteAddressBufferHandle, this.handle.read_index());
    T result = buffer.Load<T>(0);
    return result;
  }

  template <typename T>
  void store(T value) {
    RWByteAddressBuffer buffer = DESCRIPTOR_HEAP(RWByteAddressBufferHandle, this.handle.write_index());
    buffer.Store<T>(0, value);
  }
};

struct ArrayBuffer {
  RenderResourceHandle handle;

  template <typename T>
  T load(uint index) {
    ByteAddressBuffer buffer = DESCRIPTOR_HEAP(ByteAddressBufferHandle, this.handle.read_index());
    T result = buffer.Load<T>(sizeof(T) * index);
    return result;
  }
};

struct RwArrayBuffer {
  RenderResourceHandle handle;

  template <typename T>
  T load(uint index) {
    RWByteAddressBuffer buffer = DESCRIPTOR_HEAP(RWByteAddressBufferHandle, this.handle.read_index());
    T result = buffer.Load<T>(sizeof(T) * index);
    return result;
  }

  template <typename T>
  void store(uint index, T value) {
    RWByteAddressBuffer buffer = DESCRIPTOR_HEAP(RWByteAddressBufferHandle, this.handle.write_index());
    buffer.Store<T>(sizeof(T) * index, value);
  }
};

struct Texture {
  RenderResourceHandle handle;

  template <typename T>
  T load_1d(uint pos) {
    Texture1D<T> texture = DESCRIPTOR_HEAP(Texture1DHandle<T>, this.handle.read_index());
    return texture.load(uint2(pos, 0));
  }

  template <typename T>
  T load_2d(uint2 pos) {
    Texture2D<T> texture = DESCRIPTOR_HEAP(Texture2DHandle<T>, this.handle.read_index());
    return texture.load(uint3(pos, 0));
  }

  template <typename T>
  T load_3d(uint3 pos) {
    Texture3D<T> texture = DESCRIPTOR_HEAP(Texture3DHandle<T>, this.handle.read_index());
    return texture.load(uint4(pos, 0));
  }

  template<typename T>
  T sample_1d(SamplerState sampler, float u) {
      Texture1D<T> texture = DESCRIPTOR_HEAP(Texture1DHandle<T>, this.handle.read_index());
      return texture.Sample(sampler, u);
  }

  template<typename T>
  T sample_2d(SamplerState sampler, float2 uv) {
      Texture2D<T> texture = DESCRIPTOR_HEAP(Texture2DHandle<T>, this.handle.read_index());
      return texture.Sample(sampler, uv);
  }

  template<typename T>
  T sample_3d(SamplerState sampler, float3 uvw) {
      Texture3D<T> texture = DESCRIPTOR_HEAP(Texture3DHandle<T>, this.handle.read_index());
      return texture.Sample(sampler, uvw);
  }

  template<typename T>
  T sample_level_1d(SamplerState sampler, float u, float mip) {
      Texture1D<T> texture = DESCRIPTOR_HEAP(Texture1DHandle<T>, this.handle.read_index());
      return texture.SampleLevel(sampler, u, mip);
  }

  template<typename T>
  T sample_level_2d(SamplerState sampler, float2 uv, float mip) {
      Texture2D<T> texture = DESCRIPTOR_HEAP(Texture2DHandle<T>, this.handle.read_index());
      return texture.SampleLevel(sampler, uv, mip);
  }

  template<typename T>
  T sample_level_3d(SamplerState sampler, float3 uvw, float mip) {
      Texture3D<T> texture = DESCRIPTOR_HEAP(Texture3DHandle<T>, this.handle.read_index());
      return texture.SampleLevel(sampler, uvw, mip);
  }
};

struct RwTexture {
  RenderResourceHandle handle;

  template <typename T>
  T load_1d(uint pos) {
    Texture1D<T> texture = DESCRIPTOR_HEAP(Texture1DHandle<T>, this.handle.read_index());
    return texture.load(uint2(pos, 0));
  }

  template <typename T>
  T load_2d(uint2 pos) {
    Texture2D<T> texture = DESCRIPTOR_HEAP(Texture2DHandle<T>, this.handle.read_index());
    return texture.load(uint3(pos, 0));
  }

  template <typename T>
  T load_3d(uint3 pos) {
    Texture3D<T> texture = DESCRIPTOR_HEAP(Texture3DHandle<T>, this.handle.read_index());
    return texture.load(uint4(pos, 0));
  }

  template <typename T>
  void store_1d(uint pos, T value) {
    RWTexture1D<T> texture = DESCRIPTOR_HEAP(RWTexture1DHandle<T>, this.handle.write_index());
    texture[pos] = value;
  }

  template <typename T>
  void store_2d(uint2 pos, T value) {
    RWTexture2D<T> texture = DESCRIPTOR_HEAP(RWTexture2DHandle<T>, this.handle.write_index());
    texture[pos] = value;
  }

  template <typename T>
  void store_3d(uint3 pos, T value) {
    RWTexture3D<T> texture = DESCRIPTOR_HEAP(RWTexture3DHandle<T>, this.handle.write_index());
    texture[pos] = value;
  }

  template<typename T>
  T sample_1d(SamplerState sampler, float u) {
      Texture1D<T> texture = DESCRIPTOR_HEAP(Texture1DHandle<T>, this.handle.read_index());
      return texture.Sample(sampler, u);
  }

  template<typename T>
  T sample_2d(SamplerState sampler, float2 uv) {
      Texture2D<T> texture = DESCRIPTOR_HEAP(Texture2DHandle<T>, this.handle.read_index());
      return texture.Sample(sampler, uv);
  }

  template<typename T>
  T sample_3d(SamplerState sampler, float3 uvw) {
      Texture3D<T> texture = DESCRIPTOR_HEAP(Texture3DHandle<T>, this.handle.read_index());
      return texture.Sample(sampler, uvw);
  }

  template<typename T>
  T sample_level_1d(SamplerState sampler, float u, float mip) {
      Texture1D<T> texture = DESCRIPTOR_HEAP(Texture1DHandle<T>, this.handle.read_index());
      return texture.SampleLevel(sampler, u, mip);
  }

  template<typename T>
  T sample_level_2d(SamplerState sampler, float2 uv, float mip) {
      Texture2D<T> texture = DESCRIPTOR_HEAP(Texture2DHandle<T>, this.handle.read_index());
      return texture.SampleLevel(sampler, uv, mip);
  }

  template<typename T>
  T sample_level_3d(SamplerState sampler, float3 uvw, float mip) {
      Texture3D<T> texture = DESCRIPTOR_HEAP(Texture3DHandle<T>, this.handle.read_index());
      return texture.SampleLevel(sampler, uvw, mip);
  }
};


////////////////////////////////////////////////////////////////////////////////
// Push Constants
////////////////////////////////////////////////////////////////////////////////

struct BindingsOffset {
  RenderResourceHandle bindings_offset;
  uint unused_0;
  uint unused_1;
  uint unused_2;
};

#ifdef HYPER_ENGINE_VULKAN
    [[vk::push_constant]] ConstantBuffer<BindingsOffset> g_bindings_offset;
#else
    ConstantBuffer<BindingsOffset> g_bindings_offset : register(b0, space0);
#endif

template <typename T>
inline T load_bindings() {
  SimpleBuffer buffer = (SimpleBuffer) g_bindings_offset.bindings_offset.read_index();
  T result = buffer.load<T>();
  return result;
}

#endif
