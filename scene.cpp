#include "scene.h"

Scene::Scene()
{
}

void Scene::addObject(LineObject newObject){
    mSceneL.append(newObject);
}

void Scene::addObject(PointLight newObject){
    mSceneP.append(newObject);
}

QImage Scene::getSceneImageLines(){
    //Probably a memory leak here !
    SceneImage = new QImage(mSceneL.size(),LineObject::getSize(),QImage::Format_ARGB32);

    //Set the Scene
    for(int j = 0; j < mSceneL.size(); j++){
        for(int i = 0; i < LineObject::getSize(); i++)
            SceneImage->setPixel(j,i,mSceneL[j].getAttributeAsPixel(i));
    }

    return *SceneImage;
}

QImage Scene::getSceneImageParticles(){
    //Probably a memory leak here !
    SceneImage = new QImage(mSceneP.size(),PointLight::getSize(),QImage::Format_ARGB32);

    //Set the Scene
    for(int j = 0; j < mSceneP.size(); j++){
        for(int i = 0; i < PointLight::getSize(); i++)
            SceneImage->setPixel(j,i,mSceneP[j].getAttributeAsPixel(i));
    }

    return *SceneImage;
}
