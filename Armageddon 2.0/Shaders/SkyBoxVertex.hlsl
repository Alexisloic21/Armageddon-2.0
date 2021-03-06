
cbuffer TransFormBuffer : register(b0)
{
    row_major float4x4 WorldMat;
    row_major float4x4 ProjectionMat;
    row_major float4x4 ViewMat;
    row_major float4x4 MVP;
};

struct VSinput
{
    float3 position : POSITION;

};

struct PSinput
{
    float4 position : SV_Position;
    float3 textCoord : TEXCOORD0;

};
PSinput main(VSinput input)
{
    PSinput output = (PSinput) 0;
    float4x4 WMVP = mul(ViewMat, ProjectionMat);
    float4 pos = mul(float4(input.position, 1.0f), WMVP);
    output.position = pos;
    output.textCoord = input.position;
    return output;
}