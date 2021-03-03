uniform float4x4 uView;
uniform float4x4 uProj;
uniform float3 uColorBottom;
uniform float3 uColorTop;

//--------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------
struct VS_INPUT
{
	float3 a_position : a_position;
};

struct VS_OUTPUT {
	float4 SV_Position : SV_Position;
	float yDir : YDIR;
};

VS_OUTPUT vsMain(VS_INPUT vsin)
{
	VS_OUTPUT res;
	float3 pointViewSpace = mul(uView, float4(vsin.a_position, 0.0)).xyz;
	float4 pointProj = mul(uProj, float4(pointViewSpace, 1.0));
	res.SV_Position = pointProj;
	
	res.yDir = normalize(vsin.a_position).y;

	return res;
}
//--------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------
struct PS_OUTPUT {
	float4 target0 : SV_Target0;
	float depth : SV_Depth;
};

PS_OUTPUT psMain(VS_OUTPUT IN)
{
	PS_OUTPUT psOut;
	
	const float3 color = lerp(uColorBottom, uColorTop, saturate((IN.yDir + 1.f) * 0.5f));
	psOut.target0 = float4(color, 1.f);
	psOut.depth = 1.0f;
	return psOut;
}