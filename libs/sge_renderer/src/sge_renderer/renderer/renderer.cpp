#include "renderer.h"

namespace sge {

#if 0
template<typename T>
T* SGEDevice::requestResource()
{
	sgeAssert(false);
	return nullptr;
}
#endif

#define SGE_Impl_request_resource(T, RT)             \
	template <>                                      \
	T* SGEDevice::requestResource<T>() {             \
		return static_cast<T*>(requestResource(RT)); \
	}

SGE_Impl_request_resource(Buffer, ResourceType::Buffer);
SGE_Impl_request_resource(Texture, ResourceType::Texture);
SGE_Impl_request_resource(FrameTarget, ResourceType::FrameTarget);
SGE_Impl_request_resource(Shader, ResourceType::Shader);
SGE_Impl_request_resource(ShadingProgram, ResourceType::ShadingProgram);
SGE_Impl_request_resource(RasterizerState, ResourceType::RasterizerState);
SGE_Impl_request_resource(DepthStencilState, ResourceType::DepthStencilState);
SGE_Impl_request_resource(BlendState, ResourceType::BlendState);
SGE_Impl_request_resource(SamplerState, ResourceType::Sampler);
SGE_Impl_request_resource(Query, ResourceType::Query);

} // namespace sge
