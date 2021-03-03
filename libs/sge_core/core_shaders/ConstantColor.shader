
//--------------------------------------------------------------------
// Uniforms
//--------------------------------------------------------------------
uniform float4x4 uProjView;
uniform float4x4 uWorld;

uniform float4 uColor;

//--------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------
struct VS_INPUT
{
	float3 a_position : a_position;
};

struct VS_OUTPUT {
	float4 SV_Position : SV_Position;
};


VS_OUTPUT vsMain(VS_INPUT vsin)
{
	VS_OUTPUT res;

	const float4 worldPos = mul(uWorld, float4(vsin.a_position, 1.0));
	const float4 posProjSpace = mul(uProjView, worldPos);
	res.SV_Position = posProjSpace;
	
	return res;
}

//--------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------
float4 psMain(VS_OUTPUT IN) : SV_Target0
{
	return uColor;
}
