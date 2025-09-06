#pragma once

#include <d3d12.h>

struct CD3DX12_HEAP_PROPERTIES : public D3D12_HEAP_PROPERTIES {
    CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE type) {
        Type = type;
        CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        CreationNodeMask = 1;
        VisibleNodeMask = 1;
    }
};

struct CD3DX12_RESOURCE_DESC : public D3D12_RESOURCE_DESC {
    static CD3DX12_RESOURCE_DESC Tex2D(
        DXGI_FORMAT format,
        UINT64 width,
        UINT height,
        UINT16 arraySize = 1,
        UINT16 mipLevels = 1,
        UINT sampleCount = 1,
        UINT sampleQuality = 0,
        D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
        D3D12_TEXTURE_LAYOUT layout = D3D12_TEXTURE_LAYOUT_UNKNOWN
    ) {
        CD3DX12_RESOURCE_DESC desc = {};
        desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        desc.Alignment = 0;
        desc.Width = width;
        desc.Height = height;
        desc.DepthOrArraySize = arraySize;
        desc.MipLevels = mipLevels;
        desc.Format = format;
        desc.SampleDesc.Count = sampleCount;
        desc.SampleDesc.Quality = sampleQuality;
        desc.Layout = layout;
        desc.Flags = flags;
        return desc;
    }
};

struct CD3DX12_RESOURCE_BARRIER : public D3D12_RESOURCE_BARRIER {
    static CD3DX12_RESOURCE_BARRIER TransitionFn(
        ID3D12Resource* resource,
        D3D12_RESOURCE_STATES stateBefore,
        D3D12_RESOURCE_STATES stateAfter,
        UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
        D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE
    ) {
        CD3DX12_RESOURCE_BARRIER result = {};
        result.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        result.Flags = flags;
        result.Transition.pResource = resource;
        result.Transition.StateBefore = stateBefore;
        result.Transition.StateAfter = stateAfter;
        result.Transition.Subresource = subresource;
        return result;
    }
};

struct CD3DX12_DESCRIPTOR_RANGE1 : public D3D12_DESCRIPTOR_RANGE1 {
    CD3DX12_DESCRIPTOR_RANGE1(
        D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
        UINT numDescriptors,
        UINT baseShaderRegister,
        UINT registerSpace = 0,
        D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
        UINT offsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    ) {
        Init(rangeType, numDescriptors, baseShaderRegister, registerSpace, flags, offsetInDescriptorsFromTableStart);
    }

    void Init(
        D3D12_DESCRIPTOR_RANGE_TYPE rangeType,
        UINT numDescriptors,
        UINT baseShaderRegister,
        UINT registerSpace = 0,
        D3D12_DESCRIPTOR_RANGE_FLAGS flags = D3D12_DESCRIPTOR_RANGE_FLAG_NONE,
        UINT offsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND
    ) {
        RangeType = rangeType;
        NumDescriptors = numDescriptors;
        BaseShaderRegister = baseShaderRegister;
        RegisterSpace = registerSpace;
        Flags = flags;
        OffsetInDescriptorsFromTableStart = offsetInDescriptorsFromTableStart;
    }
};

struct CD3DX12_ROOT_PARAMETER1 : public D3D12_ROOT_PARAMETER1 {
    void InitAsConstants(
        UINT num32BitValues,
        UINT shaderRegister,
        UINT registerSpace = 0,
        D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL
    ) {
        ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
        ShaderVisibility = visibility;
        Constants.Num32BitValues = num32BitValues;
        Constants.ShaderRegister = shaderRegister;
        Constants.RegisterSpace = registerSpace;
    }

    void InitAsDescriptorTable(
        UINT numRanges,
        const CD3DX12_DESCRIPTOR_RANGE1* pDescriptorRanges,
        D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL
    ) {
        ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
        ShaderVisibility = visibility;
        DescriptorTable.NumDescriptorRanges = numRanges;
        DescriptorTable.pDescriptorRanges = pDescriptorRanges;
    }
};

struct CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC : public D3D12_VERSIONED_ROOT_SIGNATURE_DESC {
    void Init_1_1(
        UINT numParameters,
        const D3D12_ROOT_PARAMETER1* parameters,
        UINT numStaticSamplers = 0,
        const D3D12_STATIC_SAMPLER_DESC* staticSamplers = nullptr,
        D3D12_ROOT_SIGNATURE_FLAGS flags = D3D12_ROOT_SIGNATURE_FLAG_NONE
    ) {
        Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
        Desc_1_1.NumParameters = numParameters;
        Desc_1_1.pParameters = parameters;
        Desc_1_1.NumStaticSamplers = numStaticSamplers;
        Desc_1_1.pStaticSamplers = staticSamplers;
        Desc_1_1.Flags = flags;
    }
};

inline HRESULT D3DX12SerializeVersionedRootSignature(
    const D3D12_VERSIONED_ROOT_SIGNATURE_DESC* pRootSignature,
    D3D_ROOT_SIGNATURE_VERSION rootSignatureVersion,
    ID3DBlob** ppBlob,
    ID3DBlob** ppErrorBlob)
{
    return D3D12SerializeVersionedRootSignature(pRootSignature, ppBlob, ppErrorBlob);
}
