#include "FWDDefault_buildShadowMaps.h"

uniform float4x4 projView;
uniform float4x4 world;

#if OPT_LightType == FWDDBSM_OPT_LightType_Point
// Point light shadow map building specifics.
uniform float3 uPointLightPositionWs;
uniform float uPointLightFarPlaneDistance;
#endif

//--------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------
struct VS_INPUT
{
	float3 a_position : a_position;
};

struct VS_OUTPUT {
	float4 SV_Position : SV_Position;
#if OPT_LightType == FWDDBSM_OPT_LightType_Point
	float3 vertexPosWs : vertexPosWs;
#endif
};

VS_OUTPUT vsMain(VS_INPUT vsin)
{
	VS_OUTPUT res;
	const float4 worldPos = mul(world, float4(vsin.a_position, 1.0));
	const float4 posProjSpace = mul(projView, worldPos);
	
	res.SV_Position = posProjSpace;

#if OPT_LightType == FWDDBSM_OPT_LightType_Point
	res.vertexPosWs = worldPos.xyz;
#endif

	return res;
}
//--------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------
struct PS_OUTPUT {
	float4 target0 : SV_Target0;
#if OPT_LightType == FWDDBSM_OPT_LightType_Point
	float zDepth : SV_Depth;
#endif
};

PS_OUTPUT psMain(VS_OUTPUT IN)
{
	PS_OUTPUT psOut;
	psOut.target0 = float4(1.f, 1.f, 1.f, 1.f);
	
#if OPT_LightType == FWDDBSM_OPT_LightType_Point
	// [FWDDEF_POINTLIGHT_LINEAR_ZDEPTH]
	// Make the point light shadow map z-depth linear as
	// it is going to be easier to sample it in the FWDDefault_shading.
	// Caution for OpenGL:
	// While the Z/W outputed from Vertex Shader needs to be in [-1;1] range,
	// gl_FragDepth does not relate to the depth range, it is always a value between [0;1] and get remapped later.
	const float zDepth01 = length(IN.vertexPosWs - uPointLightPositionWs) / uPointLightFarPlaneDistance;

	psOut.zDepth = zDepth01;

#endif

	return psOut;
}