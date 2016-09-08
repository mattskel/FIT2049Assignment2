cbuffer MatrixBuffer
{
	matrix world;
	matrix view;
	matrix projection;
};

struct VertexInput
{
	float4 position : POSITION;
	float4 colour : COLOR;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

struct PixelInput
{
	float4 position : SV_POSITION;
	float4 colour : COLOR;
	float3 normal : NORMAL;
	float2 uv : TEXCOORD;
};

PixelInput main( VertexInput input )
{
	PixelInput output;
	input.position.w = 1.0f;
	
	float4 position = mul(input.position, world);
	position = mul(position, view);
	position = mul(position, projection);
	output.position = position;

	output.colour = input.colour;

	output.normal = mul(input.normal, (float3x3)world);

	output.uv = input.uv;

	return output;
}