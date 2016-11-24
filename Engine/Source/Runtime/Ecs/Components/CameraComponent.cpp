#include "CameraComponent.h"
#include "../../Rendering/Camera.h"
#include "../../Rendering/RenderSurface.h"

CameraComponent::CameraComponent()
{
	mCamera = std::make_unique<Camera>();
	mGBuffer = std::make_shared<RenderSurface>();
	mOutputBuffer = std::make_shared<RenderSurface>();
	init({ 0, 0 });
}

CameraComponent::CameraComponent(const CameraComponent& cameraComponent)
{
	mCamera = std::make_unique<Camera>(*cameraComponent.getCamera());
	mHDR = cameraComponent.mHDR;
	mGBuffer = std::make_shared<RenderSurface>();
	mOutputBuffer = std::make_shared<RenderSurface>();
	init({ 0, 0 });
}

void CameraComponent::init(const uSize& size)
{
	std::uint32_t samplerFlags = 0
		| BGFX_TEXTURE_RT
		| BGFX_TEXTURE_MIN_POINT
		| BGFX_TEXTURE_MAG_POINT
		| BGFX_TEXTURE_MIP_POINT
		| BGFX_TEXTURE_U_CLAMP
		| BGFX_TEXTURE_V_CLAMP
		;

	auto surfaceFormat = gfx::TextureFormat::BGRA8;
	auto depthFormat = gfx::TextureFormat::D24;
	if (size.width == 0 && size.height == 0)
	{
		//create a depth buffer to share
		auto depthBuffer = std::make_shared<Texture>(gfx::BackbufferRatio::Equal, false, 1, depthFormat, samplerFlags);
		mGBuffer->populate
		(
			std::vector<std::shared_ptr<Texture>>
			{
				std::make_shared<Texture>(gfx::BackbufferRatio::Equal, false, 1, surfaceFormat, samplerFlags),
				std::make_shared<Texture>(gfx::BackbufferRatio::Equal, false, 1, surfaceFormat, samplerFlags),
				std::make_shared<Texture>(gfx::BackbufferRatio::Equal, false, 1, surfaceFormat, samplerFlags),
				depthBuffer
			}
		);

		mOutputBuffer->populate
		(
			std::vector<std::shared_ptr<Texture>>
			{
				std::make_shared<Texture>(gfx::BackbufferRatio::Equal, false, 1, surfaceFormat, samplerFlags),
				depthBuffer
			}
		);
	}
	else
	{
		//create a depth buffer to share
		auto depthBuffer = std::make_shared<Texture>(size.width, size.height, false, 1, depthFormat, samplerFlags);
		mGBuffer->populate
		(
			std::vector<std::shared_ptr<Texture>>
			{
				std::make_shared<Texture>(size.width, size.height, false, 1, surfaceFormat, samplerFlags),
				std::make_shared<Texture>(size.width, size.height, false, 1, surfaceFormat, samplerFlags),
				std::make_shared<Texture>(size.width, size.height, false, 1, surfaceFormat, samplerFlags),
				depthBuffer
			}
		);

		mOutputBuffer->populate
		(
			std::vector<std::shared_ptr<Texture>>
			{
				std::make_shared<Texture>(size.width, size.height, false, 1, surfaceFormat, samplerFlags),
				depthBuffer
			}
		);
	}

	setProjectionWindow();
}

CameraComponent::~CameraComponent()
{
}

void CameraComponent::updateInternal(const math::transform& t)
{
	// First update so the camera can cache the previous matrices
	mCamera->recordCurrentMatrices();

	// Set new transform
	mCamera->lookAt(t.getPosition(), t.getPosition() + t.zUnitAxis(), t.yUnitAxis());
	setProjectionWindow();
}

bool CameraComponent::getHDR() const
{
	return mHDR;
}

void CameraComponent::setHDR(bool hdr)
{
	mHDR = hdr;
}

void CameraComponent::setViewportSize(const uSize& size)
{
	auto oldSize = mOutputBuffer->getSize();
	if (size != oldSize)
	{
		init(size);
	}

	setProjectionWindow();
}

const uSize& CameraComponent::getViewportSize() const
{
	return mCamera->getViewportSize();
}

void CameraComponent::setProjectionWindow()
{
	auto size = mOutputBuffer->getSize();

	mCamera->setViewportSize(size);
}

float CameraComponent::getOrthographicSize() const
{
	return mCamera->getOrthographicSize();
}

void CameraComponent::setOrthographicSize(float size)
{
	mCamera->setOrthographicSize(size);
}

float CameraComponent::getPixelsPerUnit() const
{
	return mCamera->getPixelsPerUnit();
}

Camera* CameraComponent::getCamera() const
{
	return mCamera.get();
}

CameraComponent& CameraComponent::setFieldOfView(float fovDegrees)
{
	mCamera->setFOV(fovDegrees);

	return *this;
}

CameraComponent& CameraComponent::setNearClip(float distance)
{
	mCamera->setNearClip(distance);

	return *this;
}
CameraComponent& CameraComponent::setFarClip(float distance)
{
	mCamera->setFarClip(distance);

	return *this;
}

CameraComponent& CameraComponent::setProjectionMode(ProjectionMode mode)
{
	mCamera->setProjectionMode(mode);

	setProjectionWindow();

	return *this;
}

float CameraComponent::getFieldOfView() const
{
	return mCamera->getFOV();
}
float CameraComponent::getNearClip() const
{
	return mCamera->getNearClip();
}
float CameraComponent::getFarClip() const
{
	return mCamera->getFarClip();
}

ProjectionMode CameraComponent::getProjectionMode() const
{
	return mCamera->getProjectionMode();
}

std::shared_ptr<RenderSurface> CameraComponent::getOutputBuffer() const
{
	return mOutputBuffer;
}

std::shared_ptr<RenderSurface> CameraComponent::getGBuffer() const
{
	return mGBuffer;
}