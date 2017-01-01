#include "light_component.h"

LightComponent::LightComponent()
{
}

LightComponent::LightComponent(const LightComponent& lightComponent)
{
	_light = lightComponent._light;
}

LightComponent::~LightComponent()
{

}