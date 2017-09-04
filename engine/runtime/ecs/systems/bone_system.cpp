#include "bone_system.h"

#include "../../rendering/mesh.h"
#include "../../system/engine.h"
#include "../components/model_component.h"
#include "../components/transform_component.h"
namespace runtime
{

void process_node(const std::unique_ptr<mesh::armature_node>& node, const skin_bind_data& bind_data,
				  runtime::entity& parent, std::vector<runtime::entity>& entity_nodes,
				  runtime::entity_component_system& ecs)
{
	if(!parent.valid())
		return;

	auto entity_node = ecs.create();
	entity_node.set_name(node->name);
	entity_node.assign<transform_component>().lock()->set_parent(parent).set_local_transform(
		node->local_transform);

	auto bone = bind_data.find_bone_by_id(node->name);
	if(bone)
	{
		entity_nodes.push_back(entity_node);
	}

	for(auto& child : node->children)
	{
		process_node(child, bind_data, entity_node, entity_nodes, ecs);
	}
}

static std::vector<math::transform>
get_transforms_for_bones(const std::vector<runtime::entity>& bone_entities)
{
	std::vector<math::transform> result;
	if(!bone_entities.empty())
	{
		result.reserve(bone_entities.size());
		for(const auto& e : bone_entities)
		{
			if(e.valid())
			{
				const auto bone_transform = e.get_component<transform_component>();
				if(!bone_transform.expired())
				{
					result.emplace_back(bone_transform.lock()->get_transform());
				}
				else
				{
					result.emplace_back();
				}
			}
			else
			{
				result.emplace_back();
			}
		}
	}

	return result;
}

void bone_system::frame_update(std::chrono::duration<float> dt)
{
	auto& ecs = core::get_subsystem<runtime::entity_component_system>();
	ecs.each<model_component>([this, &ecs](runtime::entity e, model_component& model_comp) {

		const auto& model = model_comp.get_model();
		auto mesh = model.get_lod(0);

		// If mesh isnt loaded yet skip it.
		if(!mesh)
			return;

		const auto& skin_data = mesh->get_skin_bind_data();

		// Has skinning data?
		if(skin_data.has_bones())
		{
			if(model_comp.get_bone_entities().size() <= 1)
			{
				const auto& armature = mesh->get_armature();
				std::vector<runtime::entity> be;
				process_node(armature, skin_data, e, be, ecs);
				model_comp.set_bone_entities(be);
				model_comp.set_static(false);
			}

			const auto& bone_entities = model_comp.get_bone_entities();
			auto transforms = get_transforms_for_bones(bone_entities);
			model_comp.set_bone_transforms(std::move(transforms));
		}

	});
}

bool bone_system::initialize()
{
	runtime::on_frame_update.connect(this, &bone_system::frame_update);

	return true;
}

void bone_system::dispose()
{
	runtime::on_frame_update.disconnect(this, &bone_system::frame_update);
}
}
