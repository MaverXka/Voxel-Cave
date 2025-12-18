struct PixelShaderInput
{
    float4 position : SV_POSITION;
    float3 color : COLOR; 
};


float4 Main(PixelShaderInput input) : SV_Target
{
     return float4(input.color,1.0f);
}