#include "glann.h"


GLANN::GLANN(unsigned int renderPasses, Scene *renderScene,
             QWidget *parent, QGLWidget *shareWidget)
      : QGLWidget(parent, shareWidget)
{
    QScreen *screen = QApplication::screens().at(0);
    int width = 512;//screen->size().width();
    int height = 512;//screen->size().height();

    //qDebug() << width << height << "------------------ WIDTH , HEIGHT";

    setFixedWidth(width);
    setFixedHeight(height);
    this->width = width;
    this->height = height;

    qsrand((uint)QTime::currentTime().msec());

    Scene *tmpScene = (new SceneLoader("demoScene"))->getScene();
    SceneImageParticles = tmpScene->getSceneImageParticles();
    SceneImageLines = tmpScene->getSceneImageLines();

    this->TexWidth = SceneImageParticles.width();
    this->TexHeight = SceneImageParticles.height();

    randTex = new Playground(width,height);

    emptyTex = new QImage(width,height,QImage::Format_ARGB32);
}

void GLANN::initializeGL(){
    //setAutoBufferSwap(true);

    makeCurrent();
    initializeGLFunctions();

    //glEnable(GL_CULL_FACE);

    glClearColor(0.0, 0.0, 0.0, 0.0);

    initShader();
    initTextures();

    // Generate 2 VBOs
    glGenBuffers(1, &vboId0);
    glGenBuffers(1, &vboId1);

    VertexData vertices[] = {
        // Vertex data for face 0
                {QVector3D(-1.0, -1.0,  1.0), QVector2D(0.0, 0.0)},  // v0
                {QVector3D( 1.0, -1.0,  1.0), QVector2D(1.0, 0.0)},  // v1
                {QVector3D(-1.0,  1.0,  1.0), QVector2D(0.0, 1.0)},  // v2
                {QVector3D( 1.0,  1.0,  1.0), QVector2D(1.0, 1.0)},  // v3
    };
    // Transfer vertex data to VBO 0
    glBindBuffer(GL_ARRAY_BUFFER, vboId0);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(VertexData), vertices, GL_STATIC_DRAW);

     GLushort indices[] = {
                  0,  1,  2,  3,  3,
    };
    // Transfer index data to VBO 1
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboId1);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 5 * sizeof(GLushort), indices, GL_STATIC_DRAW);

    //Init the Framebuffer
    initFbo();

    //mRenderPasses = 1;
    // Use QBasicTimer because its faster than QTimer
    timer.start(0, this);
}

bool GLANN::initFbo(){
    fbo = new QOpenGLFramebufferObject(width, height);
    return true;
}

void GLANN::resizeGL(int w, int h){
    glViewport(0,0,w,h);
}

void GLANN::mouseMoveEvent(QMouseEvent* event){

}

void GLANN::mousePressEvent(QMouseEvent* event){

    Scene *tmpScene = (new SceneLoader("demoScene"))->getScene();
    SceneImageParticles = tmpScene->getSceneImageParticles();
    SceneImageLines = tmpScene->getSceneImageLines();

    glDeleteTextures(1,&pixelsSceneLines);
    glDeleteTextures(1,&pixelsSceneParticels);

    //Bind Scene
    pixelsSceneParticels = QGLWidget::bindTexture(SceneImageParticles);
    pixelsSceneLines = QGLWidget::bindTexture(SceneImageParticles);

}

void GLANN::paintGL(){


    render();

    //increment number of rendered frames
    mNumFrames++;
}

void GLANN::render(){

    for(int i = 0; i < samples; i++){

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
        // Render to our framebuffer
        fbo->bind();
        //glViewport(0,0,width,height);


        //Set program to fbo render mode
        program.setUniformValue("shaderMode",1);

        // Tell OpenGL which VBOs to use
        glBindBuffer(GL_ARRAY_BUFFER, vboId0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboId1);

        // Offset for position
        int offset = 0;

        // Tell OpenGL programmable pipeline how to locate vertex position data
        int vertexLocation = program.attributeLocation("a_position");
        program.enableAttributeArray(vertexLocation);
        glVertexAttribPointer(vertexLocation, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

        // Offset for texture coordinate
        offset += sizeof(QVector3D);

        // Tell OpenGL programmable pipeline how to locate vertex texture coordinate data
        int texcoordLocation = program.attributeLocation("a_texcoord");
        program.enableAttributeArray(texcoordLocation);
        glVertexAttribPointer(texcoordLocation, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (const void *)offset);

            // Set random seed
            program.setUniformValue("seedX", ((float)qrand()/RAND_MAX));
            program.setUniformValue("seedY", ((float)qrand()/RAND_MAX));

             glActiveTexture(GL_TEXTURE0);
             glBindTexture(GL_TEXTURE_2D, pixelsSceneParticels);

             glActiveTexture(GL_TEXTURE1);
             glBindTexture(GL_TEXTURE_2D, randTexPixels);

             glActiveTexture(GL_TEXTURE2);
             glBindTexture(GL_TEXTURE_2D, pixelsSceneLines);

             glActiveTexture(GL_TEXTURE3);
             glBindTexture(GL_TEXTURE_2D, pixelsRenderedImage);

             // Draw cube geometry using indices from VBO 1
             glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_SHORT, 0);

             //qDebug() << glGetError() << "Line 183";

         fbo->release();

         //glDeleteTextures(1,&pixelsRenderedImage);
         pixelsRenderedImage = fbo->texture();

    }

     //Set Program to screen frendering
     program.setUniformValue("shaderMode",0);
     //Set Viewport back to default
     glViewport(0,0,width,height);
     //Render To Screen
     glActiveTexture(GL_TEXTURE0);
     glBindTexture(GL_TEXTURE_2D, pixelsSceneParticels);

     glActiveTexture(GL_TEXTURE1);
     glBindTexture(GL_TEXTURE_2D, randTexPixels);

     glActiveTexture(GL_TEXTURE2);
     glBindTexture(GL_TEXTURE_2D, pixelsSceneLines);

     glActiveTexture(GL_TEXTURE3);
     glBindTexture(GL_TEXTURE_2D, pixelsRenderedImage);

     // Draw quad geometry using indices from VBO 1
     glDrawElements(GL_TRIANGLE_STRIP, 5, GL_UNSIGNED_SHORT, 0);


}

void GLANN::timerEvent(QTimerEvent *)
{
    // Update scene
    update();
}

void GLANN::initTextures(){

    //glEnable(GL_TEXTURE_2D);

    //FEEDBACK Texture
    renderedImageUCHAR = new unsigned char [TexWidth*TexHeight*4];

    //Bind random Order Array
    randTexPixels = QGLWidget::bindTexture(*randTex);

    //Bind Scene
    pixelsSceneParticels = QGLWidget::bindTexture(SceneImageParticles);
    pixelsSceneLines = QGLWidget::bindTexture(SceneImageParticles);

    emptyTex->fill(qRgba(255,255,255,255));
    pixelsRenderedImage = bindTexture(*emptyTex);

    // Poor filtering.
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void GLANN::initShader(){

    // Compile vertex shader
    if (!program.addShaderFromSourceFile(QGLShader::Vertex, ":/vshader.glsl"))
        close();

    // Compile fragment shader
    if (!program.addShaderFromSourceFile(QGLShader::Fragment, ":/fshader.glsl"))
        close();

    // Link shader pipeline
    if (!program.link())
        close();

    // Bind shader pipeline for use
    if (!program.bind())
        close();

    // Use texture unit 0
    program.setUniformValue("Particles",0);

    // Use texture unit 1
    program.setUniformValue("randTex",1);

    // Use texture unit 1
    program.setUniformValue("Lines",2);

    // Use texture unit 1
    program.setUniformValue("renderedImage",3);

    //width
    program.setUniformValue("width", width);

    //height
    program.setUniformValue("height", height);

    //height
    program.setUniformValue("samples", samples);

    //width
    program.setUniformValue("numParticles", SceneImageParticles.width());

    //height
    program.setUniformValue("numParametersP", PointLight::getSize());

    //width
    program.setUniformValue("numLines", SceneImageLines.width());

    //height
    program.setUniformValue("numParametersL", LineObject::getSize());

}

void GLANN::getFeedbackTexture(){

    //Playground TexImage(thisSize.width(), thisSize.height());
    glReadPixels(0,0,width,height,GL_RGBA,GL_UNSIGNED_BYTE,renderedImageUCHAR);

    //qDebug("%i , %i" ,TexImage->size().width(),TexImage->size().height());

    for(int i = 0; i < width*height; i++){
             randTex->setPixel(i%width,(int)(width*height-i-1)/height,
                               qRgba(renderedImageUCHAR[i*4],
                                     renderedImageUCHAR[i*4+1],
                                     renderedImageUCHAR[i*4+2],
                                     renderedImageUCHAR[i*4+3]));
    }
}
