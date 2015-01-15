#ifndef GLANN_H
#define GLANN_H

#pragma once

#include <QApplication>
#include <QScreen>

#include <QGLShaderProgram>
#include <QGLFunctions>
#include <QOpenGLFramebufferObject>
#include <QGLPixelBuffer>

#include <QApplication>

#include <QBasicTimer>
#include <QTime>
#include <QImage>
#include <QVector>

#include <QtGui/QOpenGLFramebufferObject>
#include <QtGui/QMouseEvent>

#include "sceneloader.h"
#include "playground.h"
#include "Floatbits.h"
#include "bitonicindexmask.h"

class GLANN : public QGLWidget,protected QGLFunctions
{
    Q_OBJECT

public:
    GLANN(unsigned int renderPasses, Scene *renderScene = 0,
          QWidget *parent = 0, QGLWidget *shareWidget = 0);

    QImage getRenderedImage();

protected:

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    void timerEvent(QTimerEvent *);

    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);

private:
    void render();

    void getFeedbackTexture();

    void initShader();
    void initTextures();
    bool initFbo();

    float xTemp = 0;
    float yTemp = 0;

    unsigned int width;
    unsigned int height;
    unsigned int TexWidth;
    unsigned int TexHeight;

    QBasicTimer timer;


    Scene *mScene;
    QImage SceneImageParticles;
    QImage SceneImageLines;
    QImage *randTex;

    QGLShaderProgram program;
    QOpenGLFramebufferObject *fbo;

    GLuint randTexPixels;
    GLuint pixelsSceneLines;
    GLuint pixelsSceneParticels;
    unsigned char *renderedImageUCHAR;

    //unsigned int texSize = 512;

    unsigned int numObjects;
    unsigned int numParam;

    unsigned int mNumFrames = 0;

    GLuint vboId0;
    GLuint vboId1;
    struct VertexData
    {
        QVector3D position;
        QVector2D texCoord;
    };

};

#endif // GLANN_H
