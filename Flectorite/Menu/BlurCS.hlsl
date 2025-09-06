cbuffer BlurParams : register(b0)
{
    float2 texelSize; // 1.0 / texture size (e.g., 1/width, 1/height)
    int horizontal; // 1 = horizontal pass, 0 = vertical pass
};

Texture2D<float4> inputTexture : register(t0);
RWTexture2D<float4> outputTexture : register(u0);

[numthreads(8, 8, 1)]
void main(uint3 dispatchThreadID : SV_DispatchThreadID)
{
    int2 texCoord = dispatchThreadID.xy;
    
    float weights[5] = { 0.227027f, 0.1945946f, 0.1216216f, 0.054054f, 0.016216f };
    
    float2 offset = horizontal == 1 ? float2(texelSize.x, 0) : float2(0, texelSize.y);
    float4 result = inputTexture[texCoord] * weights[0];

    for (int i = 1; i < 5; ++i)
    {
        result += inputTexture[texCoord + int2(offset * i)] * weights[i];
        result += inputTexture[texCoord - int2(offset * i)] * weights[i];
    }

    outputTexture[texCoord] = result;
}
