struct VertexInput
{
    float3 position : POSITION;  // Vertex position, uses the POSITION semantic.
    float2 texcoord : TEXCOORD;  // Vertex position, uses the POSITION semantic.
    float4 color    : COLOR;     // Vertex color, uses the COLOR semantic.
};

struct VertexToPixel
{
    float4 position : SV_Position; // System Value Position, required for rasterization.
    float2 texcoord : TEXCOORD;
    float4 color    : COLOR;       // Interpolated color data.
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
    float4 pos = float4(vs.position,1.0f);
    pos = mul(pos, ModelMatrix);
    pos = mul(pos, ViewMatrix);
    pos = mul(pos, ProjectionMatrix);
    OUT.position = pos;
    OUT.color = vs.color;
    OUT.texcoord = vs.texcoord;
    return OUT;
}