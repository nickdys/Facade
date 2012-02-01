#include "SceneLoader.h"

#include "Graphics/CameraComponent.hpp"
#include "Graphics/MeshComponent.hpp"
#include "Graphics/LightComponent.hpp"

Scene* SceneLoader::LoadScene(string path)
{
    Scene* scene = nullptr;
    xml_document doc;
    xml_parse_result result = doc.load_file(path.c_str());
    cout << "Loading result: " << result.description() << endl;
    if(result)
    {
        scene = new Scene(path.c_str());
        xml_node nodes = doc.child(SL_SCENE).child(SL_NODES);

        if(!nodes)
            return scene;

        for(xml_node node = nodes.first_child(); node; node = node.next_sibling())
        {
            _LoadNode(scene, node);
        }
    }

    return scene;
}

void SceneLoader::_LoadNode(Scene* scene, const xml_node& node)
{
    if(node)
    {
        Node* dt_node = scene->AddChildNode(new Node(node.attribute(SL_NAME).value()));
        xml_node pos = node.child(SL_POS);
        xml_node rot = node.child(SL_ROT);
        xml_node scale = node.child(SL_SCALE);

        //Set the node's position, rotation and scale.
        dt_node->SetPosition(pos.attribute(SL_X).as_float(), pos.attribute(SL_Y).as_float(),
            pos.attribute(SL_Z).as_float());
        dt_node->SetRotation(Ogre::Quaternion(rot.attribute(SL_QW).as_float(), 
            rot.attribute(SL_QX).as_float(), rot.attribute(SL_QY).as_float(), rot.attribute(SL_QZ).as_float()));
        dt_node->SetScale(Ogre::Vector3(scale.attribute(SL_X).as_float(), scale.attribute(SL_Y).as_float(),
            scale.attribute(SL_Z).as_float()));

        //Add components to the node.
        for(xml_node component = scale.next_sibling(); component; component = component.next_sibling())
        {
            if(strcmp(component.name(), SL_CAMERA) == 0)
            {
                CameraComponent* cam = dt_node->AddComponent<CameraComponent>(new CameraComponent(
                    component.attribute(SL_NAME).value()));
            }
            else if(strcmp(component.name(), SL_MESH) == 0)
            {
                MeshComponent* mesh = dt_node->AddComponent<MeshComponent>(new MeshComponent(
                    component.attribute(SL_MESH_HANDLE).value(), component.first_child().attribute(SL_MATERIAL).value(),
                    component.attribute(SL_NAME).value()));

                mesh->SetCastShadows(component.attribute(SL_CAST_SHADOWS).as_bool());
            }
            else if(strcmp(component.name(), SL_LIGHT) == 0)
            {
                LightComponent* light = dt_node->AddComponent<LightComponent>(new LightComponent(
                    component.attribute(SL_NAME).value()));

                light->SetCastShadows(component.attribute(SL_CAST_SHADOWS).as_bool());
            }
        }
    }
}