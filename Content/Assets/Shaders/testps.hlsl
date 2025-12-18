
Texture2D testTexture : register(t0);
SamplerState texSampler : register(s0);

struct PixelShaderInput
{
    float4 position : SV_POSITION; // The clip-space position of the pixel
    float2 texcoord : TEXCOORD;
    float4 color : COLOR;  // The texture coordinates
};


float4 Main(PixelShaderInput input) : SV_Target
{
   // if (true)
    //{
     //   return input.color;
    //}

    return testTexture.Sample(texSampler, input.texcoord);
}