struct VertexInput
{
    float3 position : POSITION;
    float3 color : COLOR;     
};

struct VertexToPixel
{
    float4 position : SV_Position;
    float3 color : COLOR;      
};

cbuffer TransformBuffer : register(b0)
{
    matrix ProjectionMatrix;
    matrix ViewMatrix;
    matrix ModelMatrix;
}


VertexToPixel Main(VertexInput vs)
{
    VertexToPixel OUT;
    float4 pos = float4(vs.position, 1.0f);
    pos = mul(pos, ModelMatrix);
    pos = mul(pos, ViewMatrix);
    pos = mul(pos, ProjectionMatrix);
    OUT.position = pos;
    OUT.color = vs.color;
    return OUT;
}