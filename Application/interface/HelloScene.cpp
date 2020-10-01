#include "HelloScene.h"

#include <babylon/cameras/free_camera.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/meshes/mesh.h>

#include <babylon/engines/scene.h>
#include <babylon/maths/color4.h>

#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/mirror_texture.h>

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
    scene->clearColor = Color4(0.4f, 0.1f, 0.2f, 1.f);
    
    auto camera = ArcRotateCamera::New("camera1", 0.f, 0.f, 10.f, Vector3::Zero(), scene);
    camera->setPosition(Vector3(0.f, 5.f, -10.f));
    camera->attachControl(canvas, true);

    camera->upperBetaLimit   = Math::PI_2;
    camera->lowerRadiusLimit = 4.f;

    auto light       = HemisphericLight::New("light1", Vector3(0.f, 1.f, 0.f), scene);
    light->intensity = 0.7f;

    auto knot = Mesh::CreateTorusKnot("knot", 1, 0.4f, 128, 64, 2, 3, scene);

    // Mirror
    auto mirror                       = Mesh::CreateBox("Mirror", 1.0, scene);
    mirror->scaling                   = Vector3(100.f, 0.01f, 100.f);
    auto mirrorMaterial               = StandardMaterial::New("mirror", scene);
    auto reflectionTexture            = MirrorTexture::New("mirror", 512.f, scene, true);
    reflectionTexture->mirrorPlane    = Plane(0.f, -1.f, 0.f, -2.f);
    reflectionTexture->renderList     = {knot.get()};
    reflectionTexture->level          = 1.f;
    reflectionTexture->samples        = 8;
    mirrorMaterial->reflectionTexture = reflectionTexture;
    mirror->material                  = mirrorMaterial;
    mirror->position                  = Vector3(0.f, -2.f, 0.f);

    // Main material
    auto mainMaterial            = StandardMaterial::New("main", scene);
    knot->material               = mainMaterial;
//    mainMaterial->diffuseTexture = Texture::New("textures/amiga.jpg", scene);

    // Fog
    scene->fogMode  = Scene::FOGMODE_LINEAR;
    scene->fogColor = scene->clearColor;
    scene->fogStart = 20.f;
    scene->fogEnd   = 50.f;
    
//    // Create a FreeCamera, and set its position to (x:0, y:5, z:-10)
//    auto camera = FreeCamera::New("camera1", Vector3(0, 5, -10), scene);
//    // Target the camera to scene origin
//    camera->setTarget(Vector3::Zero());
//    // Attach the camera to the canvas
//    camera->attachControl(canvas, true);
//    // Create a basic light, aiming 0,1,0 - meaning, to the sky
//    auto light = HemisphericLight::New("light1", Vector3(0, 1, 0), scene);
//    // Default intensity is 1. Let's dim the light a small amount
//    light->intensity = 0.7f;
//    // Create a built-in "sphere" shape; its constructor takes 5 params: name,
//    // subdivs, size, scene
//    auto sphere = Mesh::CreateSphere("sphere1", 16, 2.f, scene);
//    // Move the sphere upward 1/2 of its height
//    sphere->position().y = 1.f;
//    // Create a built-in "ground" shape.
//    // Params: name, width, depth, subdivs, scene
//    Mesh::CreateGround("ground1", 6, 6, 2, scene);

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
