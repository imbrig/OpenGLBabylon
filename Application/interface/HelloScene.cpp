#include "HelloScene.h"

#include <babylon/cameras/free_camera.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/render_target_texture.h>
#include <babylon/meshes/mesh.h>

#include <babylon/meshes/mesh_builder.h>
#include <babylon/meshes/builders/mesh_builder_options.h>

#include <iostream>
#include <stdexcept>

#define LOG_OBJECT_ADRESS(variable_name, message) \
std::cout << (message) << ": " << #variable_name << " addr=" << (void *)(&variable_name) << "\n";
#define LOG_POINTER_ADRESS(variable_name, message) \
std::cout << message << ": " << #variable_name << " addr=" << (void *)(variable_name) << "\n";

// This files demonstrates how to create a very simple renderable scene
struct HelloScene : public BABYLON::IRenderableScene
{
  HelloScene(){}
  
  const char* getName() override { return "Hello Scene"; }

  void initializeScene(BABYLON::ICanvas* canvas, BABYLON::Scene* scene) override
  {
    using namespace BABYLON;
    // Create a FreeCamera, and set its position to (x:0, y:5, z:-10)
    auto camera = FreeCamera::New("camera1", Vector3(0, 0, -9), scene);

    // Target the camera to scene origin
    camera->setTarget(Vector3::Zero());

    // Attach the camera to the canvas
    camera->attachControl(canvas, true);

    // Create a basic light, aiming 0,1,0 - meaning, to the sky
    auto light = HemisphericLight::New("light1", Vector3(0, 1, 0), scene);

    // Default intensity is 1. Let's dim the light a small amount
    light->intensity = 0.7f;

    // Create a built-in "sphere" shape; its constructor takes 5 params: name,
    // subdivs, size, scene
    auto sphere = Mesh::CreateSphere("sphere1", 16, 1.f, scene);

    // Move the sphere upward 1/2 of its height
    sphere->position().y = 0.5f;

//    PlaneOptions options;
//    options.width = 10.24f;
//    options.height = 5.12f;
//    auto plane         = MeshBuilder::CreatePlane("BackgroundPlane", options, scene);
//    auto planeMaterial = StandardMaterial::New("ground1Material", scene);
//    plane->material    = planeMaterial;
//    plane->setAbsolutePosition(Vector3(0, 0, -2));
//    
//    // Create the render target texture
//    auto renderTexture = RenderTargetTexture::New("render", RenderTargetSize{1024, 512}, scene);
//    renderTexture->renderList().emplace_back(sphere.get());
//    planeMaterial->diffuseTexture = renderTexture;
//    
//    // Unlit material for acurate rendering of texture on plane.
//    planeMaterial->disableLighting = true;
//    planeMaterial->emissiveColor = Color3(1, 1, 1);
//    
//    scene->customRenderTargets.emplace_back(renderTexture.get());
  }
};

std::shared_ptr<BABYLON::IRenderableScene> MakeHelloScene()
{
  return std::make_shared<HelloScene>();
}
