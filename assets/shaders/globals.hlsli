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

struct ResourceHandle {
    uint index;

    uint read_index() {
        return index;
    }

#ifdef HYPER_ENGINE_VULKAN
    uint write_index() {
        return read_index();
    }
#else
    uint write_index() {
        return read_index() + 1;
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

    #undef DEFINE_BUFFER_HEAP
    #undef DEFINE_TEXTURE_HEAP
    #undef DEFINE_RW_TEXTURE_HEAP
    #undef DEFINE_BUFFER_HEAP_OPERATOR
    #undef DEFINE_TEXTURE_HEAP_OPERATOR_VALUE
    #undef DEFINE_TEXTURE_HEAP_OPERATOR

    static VulkanResourceDescriptorHeapInternal VkResourceDescriptorHeap;

    #define DESCRIPTOR_HEAP(handle_type, handle) VkResourceDescriptorHeap[(handle_type) handle]
#else
    #define DESCRIPTOR_HEAP(handle_type, handle) ResourceDescriptorHeap[NonUniformResourceIndex(handle)]
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
    ResourceHandle handle;

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

#undef DESCRIPTOR_HEAP

////////////////////////////////////////////////////////////////////////////////
// Shader Interop
////////////////////////////////////////////////////////////////////////////////

struct Mesh {
    ArrayBuffer positions;
    ArrayBuffer normals;
    uint padding_0;
    uint padding_1;

    inline float4 get_position(uint index) {
        return positions.load<float4>(index);
    }

    inline float4 get_normal(uint index) {
        return normals.load<float4>(index);
    }
};

struct Material {
    float4 base_color;

    // TODO: Add textures
};

////////////////////////////////////////////////////////////////////////////////
// Push Constants
////////////////////////////////////////////////////////////////////////////////

#ifdef HYPER_ENGINE_VULKAN
    #define DEFINE_PUSH_CONSTANT(value_type, name) [[vk::push_constant]] value_type name
#else
    #define DEFINE_PUSH_CONSTANT(value_type, name) ConstantBuffer<value_type> name : register(b0, space0)
#endif

struct ObjectPushConstants {
    uint mesh;
    uint material;
    uint padding_0;
    uint padding_1;

    inline Mesh get_mesh() {
        ResourceHandle mesh_handle = (ResourceHandle) mesh;
        SimpleBuffer buffer = (SimpleBuffer) mesh_handle.read_index();
        return buffer.load<Mesh>();
    }

    inline Material get_material() {
        ResourceHandle material_handle = (ResourceHandle) material;
        SimpleBuffer buffer = (SimpleBuffer) material_handle.read_index();
        return buffer.load<Material>();
    }
};

////////////////////////////////////////////////////////////////////////////////
// Globals
////////////////////////////////////////////////////////////////////////////////

// DESCRIPTOR_SET_SLOT_FRAME
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

inline Frame get_frame() {
    SimpleBuffer buffer = (SimpleBuffer) DESCRIPTOR_SET_SLOT_FRAME;
    return buffer.load<Frame>();
}

// DESCRIPTOR_SET_SLOT_CAMERA
struct Camera {
    float4x4 view_projection;
};

inline Camera get_camera() {
    SimpleBuffer buffer = (SimpleBuffer) DESCRIPTOR_SET_SLOT_CAMERA;
    return buffer.load<Camera>();
}

#endif
