struct PixelShaderInput
{
    float3 position : SV_POSITION;
    float4 color : COLOR;
};


float4 Main(PixelShaderInput input) : SV_Target
{
 return input.color;
}