/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef HE_GLOBALS_HLSLI
#define HE_GLOBALS_HLSLI

////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////
static const float PI = 3.14159265359;

////////////////////////////////////////////////////////////////////////////////
// Bindless
////////////////////////////////////////////////////////////////////////////////

struct ResourceHandle {
    uint index;

    bool is_valid() {
        return index != 0xffffffff;
    }

    uint read_index() {
        return index;
    }

#ifdef HE_VULKAN
    uint write_index() {
        return read_index();
    }
#else
    uint write_index() {
        return read_index() + 1;
    }
#endif
};

#ifdef HE_VULKAN
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

    #define DEFINE_TEXTURE_HEAP_OPERATOR(texture_type) \
        DEFINE_TEXTURE_HEAP_OPERATOR_VALUE(texture_type, g_##texture_type, float, texture_type##Handle) \
        DEFINE_TEXTURE_HEAP_OPERATOR_VALUE(texture_type, g_##texture_type, float2, texture_type##Handle) \
        DEFINE_TEXTURE_HEAP_OPERATOR_VALUE(texture_type, g_##texture_type, float3, texture_type##Handle) \
        DEFINE_TEXTURE_HEAP_OPERATOR_VALUE(texture_type, g_##texture_type, float4, texture_type##Handle)

    #define DEFINE_SAMPLER_HEAP_OPERATOR(type) \
        type operator[](type##Handle identifier) { \
            return g_##type[NonUniformResourceIndex(identifier.internal_index)]; \
        }

    DEFINE_BUFFER_HEAP(ByteAddressBuffer);
    DEFINE_BUFFER_HEAP(RWByteAddressBuffer);

    DEFINE_TEXTURE_HEAP(Texture1D);
    DEFINE_TEXTURE_HEAP(Texture2D);
    DEFINE_TEXTURE_HEAP(Texture3D);
    DEFINE_TEXTURE_HEAP(TextureCube);

    DEFINE_RW_TEXTURE_HEAP(RWTexture1D);
    DEFINE_RW_TEXTURE_HEAP(RWTexture2D);
    DEFINE_RW_TEXTURE_HEAP(RWTexture3D);

    DEFINE_SAMPLER_HEAP(SamplerState);

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

        DEFINE_SAMPLER_HEAP_OPERATOR(SamplerState)
    };

    #undef DEFINE_BUFFER_HEAP
    #undef DEFINE_TEXTURE_HEAP
    #undef DEFINE_RW_TEXTURE_HEAP
    #undef DEFINE_SAMPPLER_HEAP
    #undef DEFINE_BUFFER_HEAP_OPERATOR
    #undef DEFINE_TEXTURE_HEAP_OPERATOR_VALUE
    #undef DEFINE_TEXTURE_HEAP_OPERATOR
    #undef DEFINE_SAMPLER_HEAP_OPERATOR

    static VulkanResourceDescriptorHeapInternal VkResourceDescriptorHeap;

    #define DESCRIPTOR_HEAP(handle_type, handle) VkResourceDescriptorHeap[(handle_type) handle]
    #define SAMPLER_HEAP(handle_type, handle) VkResourceDescriptorHeap[(handle_type) handle]
#else
    #define DESCRIPTOR_HEAP(handle_type, handle) ResourceDescriptorHeap[NonUniformResourceIndex(handle)]
    #define SAMPLER_HEAP(handle_type, handle) SamplerDescriptorHeap[NonUniformResourceIndex(handle)]
#endif

struct SimpleBuffer {
    ResourceHandle handle;

    template <typename T>
    T load() {
        ByteAddressBuffer buffer = DESCRIPTOR_HEAP(ByteAddressBufferHandle, this.handle.read_index());
        T result = buffer.Load<T>(0);
        return result;
    }
};

struct RwSimpleBuffer {
    ResourceHandle handle;

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
    ResourceHandle handle;

    template <typename T>
    T load(uint index) {
        ByteAddressBuffer buffer = DESCRIPTOR_HEAP(ByteAddressBufferHandle, this.handle.read_index());
        T result = buffer.Load<T>(sizeof(T) * index);
        return result;
    }
};

struct RwArrayBuffer {
    ResourceHandle handle;

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
    ResourceHandle handle;

    template <typename T>
    T load_1d(uint pos) {
        Texture1D<T> texture = DESCRIPTOR_HEAP(Texture1DHandle<T>, this.handle.read_index());
        return texture.Load(uint2(pos, 0));
    }

    template <typename T>
    T load_2d(uint2 pos) {
        Texture2D<T> texture = DESCRIPTOR_HEAP(Texture2DHandle<T>, this.handle.read_index());
        return texture.Load(uint3(pos, 0));
    }

    template <typename T>
    T load_3d(uint3 pos) {
        Texture3D<T> texture = DESCRIPTOR_HEAP(Texture3DHandle<T>, this.handle.read_index());
        return texture.Load(uint4(pos, 0));
    }

    template<typename T>
    T sample_1d(SamplerState s, float u) {
        Texture1D<T> texture = DESCRIPTOR_HEAP(Texture1DHandle<T>, this.handle.read_index());
        return texture.Sample(s, u);
    }

    template<typename T>
    T sample_2d(SamplerState s, float2 uv) {
        Texture2D<T> texture = DESCRIPTOR_HEAP(Texture2DHandle<T>, this.handle.read_index());
        return texture.Sample(s, uv);
    }

    template<typename T>
    T sample_3d(SamplerState s, float3 uvw) {
        Texture3D<T> texture = DESCRIPTOR_HEAP(Texture3DHandle<T>, this.handle.read_index());
        return texture.Sample(s, uvw);
    }

    template<typename T>
    T sample_cube(SamplerState s, float3 uvw) {
        TextureCube<T> texture = DESCRIPTOR_HEAP(TextureCubeHandle<T>, this.handle.read_index());
        return texture.Sample(s, uvw);
    }

    template<typename T>
    T sample_level_1d(SamplerState s, float u, float mip) {
        Texture1D<T> texture = DESCRIPTOR_HEAP(Texture1DHandle<T>, this.handle.read_index());
        return texture.SampleLevel(s, u, mip);
    }

    template<typename T>
    T sample_level_2d(SamplerState s, float2 uv, float mip) {
        Texture2D<T> texture = DESCRIPTOR_HEAP(Texture2DHandle<T>, this.handle.read_index());
        return texture.SampleLevel(s, uv, mip);
    }

    template<typename T>
    T sample_level_3d(SamplerState s, float3 uvw, float mip) {
        Texture3D<T> texture = DESCRIPTOR_HEAP(Texture3DHandle<T>, this.handle.read_index());
        return texture.SampleLevel(s, uvw, mip);
    }

    template<typename T>
    T sample_level_cube(SamplerState s, float3 uvw, float mip) {
        TextureCube<T> texture = DESCRIPTOR_HEAP(TextureCubeHandle<T>, this.handle.read_index());
        return texture.SampleLevel(s, uvw, mip);
    }
};

struct RwTexture {
    ResourceHandle handle;

    template <typename T>
    T load_1d(uint pos) {
        Texture1D<T> texture = DESCRIPTOR_HEAP(Texture1DHandle<T>, this.handle.read_index());
        return texture.Load(uint2(pos, 0));
    }

    template <typename T>
    T load_2d(uint2 pos) {
        Texture2D<T> texture = DESCRIPTOR_HEAP(Texture2DHandle<T>, this.handle.read_index());
        return texture.Load(uint3(pos, 0));
    }

    template <typename T>
    T load_3d(uint3 pos) {
        Texture3D<T> texture = DESCRIPTOR_HEAP(Texture3DHandle<T>, this.handle.read_index());
        return texture.Load(uint4(pos, 0));
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
    T sample_cube(SamplerState s, float3 uvw) {
        TextureCube<T> texture = DESCRIPTOR_HEAP(TextureCubeHandle<T>, this.handle.read_index());
        return texture.Sample(s, uvw);
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

    template<typename T>
    T sample_level_cube(SamplerState s, float3 uvw, float mip) {
        TextureCube<T> texture = DESCRIPTOR_HEAP(TextureCubeHandle<T>, this.handle.read_index());
        return texture.SampleLevel(s, uvw, mip);
    }
};

struct Sampler {
    ResourceHandle handle;

    SamplerState load() {
        SamplerState sampler_state_value = SAMPLER_HEAP(SamplerStateHandle, this.handle.read_index());
        return sampler_state_value;
    }
};

#undef DESCRIPTOR_HEAP

#endif // HE_GLOBALS_HLSLI
