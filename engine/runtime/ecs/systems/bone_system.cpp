#include "bone_system.h"

#include "../../rendering/mesh.h"
#include "../../system/engine.h"
#include "../components/model_component.h"
#include "../components/transform_component.h"
namespace runtime
{

void process_node(const mesh::armature_node* const node, const skin_bind_data& bind_data,
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
		process_node(child.get(), bind_data, entity_node, entity_nodes, ecs);
	}
}

void bone_system::frame_update(std::chrono::duration<float> dt)
{
	auto& ecs = core::get_subsystem<runtime::entity_component_system>();
	ecs.each<model_component>(
		[this, &ecs](runtime::entity e, model_component& model_comp) {

			const auto& model = model_comp.get_model();
			auto mesh = model.get_lod(0);

			// If mesh isnt loaded yet skip it.
			if(!mesh)
				return;

			const auto& skin_data = mesh->get_skin_bind_data();

			// Has skinning data?
//			if(skin_data.has_bones())
//			{
//                const auto& bone_transforms = model_comp.get_bone_transforms();
//                auto it = _bones.find(e);
//                if(it == _bones.end() && bone_transforms.size() <= 1)
//                {
//                    const auto& armature = mesh->get_armature();
//					if(armature->children.empty())
//						return;

//					std::vector<runtime::entity> be;
//					process_node(mesh->get_armature()->children[0].get(), skin_data, e, be, ecs);
//					// model_comp.set_bone_entities(be);
//					model_comp.set_static(false);
//                    _bones[e] = be;
					
//                }
//				const auto& be = _bones[e];
//                std::vector<math::transform> result;
//                if(!be.empty())
//                {
//                    result.reserve(be.size());
//                    for(const auto& e : be)
//                    {
//                        if(e.valid())
//                        {
//                            const auto bone_transform = e.get_component<transform_component>();
//                            if(!bone_transform.expired())
//                            {
//                                result.emplace_back(bone_transform.lock()->get_transform());
//                            }
//                            else
//                            {
//                                result.emplace_back();
//                            }
//                        }
//                        else
//                        {
//                            result.emplace_back();
//                        }
//                    }
//                }

//                model_comp.set_bone_transforms(result);
//			}

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
