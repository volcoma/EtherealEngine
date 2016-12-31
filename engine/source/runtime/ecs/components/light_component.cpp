#include "light_component.h"

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