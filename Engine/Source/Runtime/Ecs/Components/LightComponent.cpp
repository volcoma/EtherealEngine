#include "LightComponent.h"
#include "../../Rendering/Light.h"

LightComponent::LightComponent()
{
}

LightComponent::LightComponent(const LightComponent& lightComponent)
{
	mLight = lightComponent.mLight;
}

LightComponent::~LightComponent()
{

}