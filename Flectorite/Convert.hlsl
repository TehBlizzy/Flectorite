
Texture2D<uint4> inputTexture : register(t0);
RWTexture2D<float4> outputTexture : register(u0);

[numthreads(8, 8, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    uint2 coord = dispatchThreadID.xy;
    uint4 packedColor = inputTexture[coord];

    // Decode R10G10B10A2_UNORM from uint4 packed bits
    // See DXGI docs or use bit shifts to unpack:

    uint r10 = packedColor.x & 0x3FF; // bits 0-9
    uint g10 = (packedColor.x >> 10) & 0x3FF; // bits 10-19
    uint b10 = (packedColor.x >> 20) & 0x3FF; // bits 20-29
    uint a2 = (packedColor.x >> 30) & 0x3; // bits 30-31

    // Convert 10-bit to normalized float [0,1]
    float4 color;
    color.r = r10 / 1023.0;
    color.g = g10 / 1023.0;
    color.b = b10 / 1023.0;
    color.a = a2 / 3.0;

    outputTexture[coord] = color;
}