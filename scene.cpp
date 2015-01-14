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

QImage* Scene::getSceneImage(){
    //Probably a memory leak here !
    SceneImage = new QImage(mSceneP.size(),PointLight::getSize(),QImage::Format_ARGB32);
/*
    //Set the Border
    LineObject b0(0,0,1.0,0.0001,qRgba(0,0,0,255),0.0,0,0,0);
    for(int i = 0; i < LineObject::getSize(); i++)
        SceneImage->setPixel(0,i,b0.getAttributeAsPixel(i));

    LineObject b1(1.0,0,0.999,1.0,qRgba(0,0,0,255),0.0,0,0,0);
    for(int i = 0; i < LineObject::getSize(); i++)
        SceneImage->setPixel(1,i,b1.getAttributeAsPixel(i));

    LineObject b2(1.0,1.0,0,0.999,qRgba(0,0,0,255),0.0,0,0,0);
    for(int i = 0; i < LineObject::getSize(); i++)
        SceneImage->setPixel(2,i,b2.getAttributeAsPixel(i));

    LineObject b3(0,1.0,0.0001,0,qRgba(0,0,0,255),0.0,0,0,0);
    for(int i = 0; i < LineObject::getSize(); i++)
        SceneImage->setPixel(3,i,b3.getAttributeAsPixel(i));

    //Set the Scene
    for(int j = 0; j < mScene.size(); j++){
        for(int i = 0; i < LineObject::getSize(); i++)
            SceneImage->setPixel(j+4,i,mScene[j].getAttributeAsPixel(i));
    }
*/

    //Set the Scene
    for(int j = 0; j < mSceneP.size(); j++){
        for(int i = 0; i < PointLight::getSize(); i++)
            SceneImage->setPixel(j,i,mSceneP[j].getAttributeAsPixel(i));
    }

    return SceneImage;
}
