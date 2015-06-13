/****************************************************************************
**
** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mainwidget.h"

#include <QMouseEvent>
#include <QtOpenGL/QGLWidget>
#include <QFrame>

#include <math.h>

MainWidget::MainWidget(QWidget *parent) :
    QOpenGLWidget(parent),
    geometries(0),
    angularSpeed(0)
{
    setGeometry(0, 0, 1600, 900);
    bBorders = true;
    bNoBump = false;
    bDepthTest = true;
    bWireFrame = false;
    bTexture = true;
}

MainWidget::~MainWidget()
{
    // Make sure the context is current when deleting the texture
    // and the buffers.
    makeCurrent();
    delete geometries;
    doneCurrent();
}

void MainWidget::initializeGL()
{
    initializeOpenGLFunctions();

    connect(this, SIGNAL(frameSwapped()), this, SLOT(update()));

    //animationTimer.setSingleShot(true);
    //connect(&animationTimer, SIGNAL(timeout()), this, SLOT(animate()));

    distance = -4;
    trackball = TrackBall(0.0f, QVector3D(0, 1, 0), TrackBall::Sphere);

    glClearColor(0.05, 0.05, 0.05, 1);

    // Enable depth buffer
    glEnable(GL_DEPTH_TEST);

    // Enable back face culling
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    initShaders();
    initTextures();

    geometries = new GeometryEngine(&programearth, &programearthnobump);
    gshhs = new gshhsData(&programgshhs);
    skybox = new SkyBox(&programskybox);

 }

bool MainWidget::initShaders()
{
    if(!programearth.create())
        return false;

    if (!programearth.addShaderFromSourceFile(QOpenGLShader::Vertex, "../earth_10/shader/earthshader.vert"))
    {
        qDebug() << programearth.log();
        return false;
    }

    if (!programearth.addShaderFromSourceFile(QOpenGLShader::Fragment, "../earth_10/shader/earthshader.frag"))
    {
        qDebug() << programearth.log();
        return false;
    }


    if (!programearth.link())
    {
        qDebug() << programearth.log();
        return false;
    }

    uniformTexEarth = programearth.uniformLocation("TexEarth");
    uniformTexLand = programearth.uniformLocation("NormalMapTex");
    qDebug() << QString("uniformTexEarth = %1 uniformTexLand = %2").arg(uniformTexEarth).arg(uniformTexLand);


    if(!programearthnobump.create())
        return false;

    if (!programearthnobump.addShaderFromSourceFile(QOpenGLShader::Vertex, "../earth_10/shader/earthshadernobump.vert"))
    {
        qDebug() << programearthnobump.log();
        return false;
    }

    if (!programearthnobump.addShaderFromSourceFile(QOpenGLShader::Fragment, "../earth_10/shader/earthshadernobump.frag"))
    {
        qDebug() << programearthnobump.log();
        return false;
    }

    if (!programearthnobump.link())
    {
        qDebug() << programearthnobump.log();
        return false;
    }

    uniformTexEarthnobump = programearthnobump.uniformLocation("TexEarth");
    qDebug() << QString("uniformTexEarthnobump = %1").arg(uniformTexEarthnobump);




    if(!programgshhs.create())
        return false;

    if (!programgshhs.addShaderFromSourceFile(QOpenGLShader::Vertex, "../earth_10/shader/gshhs.vert"))
    {
        qDebug() << programgshhs.log();
        return false;
    }

    if (!programgshhs.addShaderFromSourceFile(QOpenGLShader::Fragment, "../earth_10/shader/gshhs.frag"))
    {
        qDebug() << programgshhs.log();
        return false;
    }

    if (!programgshhs.link())
    {
        qDebug() << programgshhs.log();
        return false;
    }


    if(!programskybox.create())
        return false;

    if (!programskybox.addShaderFromSourceFile(QOpenGLShader::Vertex, "../earth_10/shader/skybox.vert"))
    {
        qDebug() << programskybox.log();
        return false;
    }

    if (!programskybox.addShaderFromSourceFile(QOpenGLShader::Fragment, "../earth_10/shader/skybox.frag"))
    {
        qDebug() << programskybox.log();
        return false;
    }

    if (!programskybox.link())
    {
        qDebug() << programskybox.log();
        return false;
    }

}

void MainWidget::initTextures()
{

    QRgb *row;
    QImage earth;

    qDebug() << "loading earthalpha.png";
    bool ok = earth.load("../earth_10/earthalpha.png");

    if(ok)
    {
        textureearth = new QOpenGLTexture(QOpenGLTexture::Target::Target2D );
        textureearth->create();
        textureearth->setFormat(QOpenGLTexture::RGBA8_UNorm);
        textureearth->setData(earth.mirrored(true,false));
        textureearth->setMinificationFilter(QOpenGLTexture::Linear);
        textureearth->setMagnificationFilter(QOpenGLTexture::Linear);
        textureearth->setWrapMode(QOpenGLTexture::ClampToEdge);
        int texId = textureearth->textureId();
        qDebug() << QString("ID texture Earth = %1").arg(texId);
    }
    else
    {
        qDebug() << "error loading earthalpha";
        return;
    }

    qDebug() << "loading normal map";
    ok = earth.load("../earth_10/EarthNormal.png");

    if(ok)
    {
        texturenormal = new QOpenGLTexture(QOpenGLTexture::Target::Target2D );
        texturenormal->create();
        texturenormal->setFormat(QOpenGLTexture::RGBA8_UNorm);
        texturenormal->setData(earth.mirrored(true,false));
        texturenormal->setMinificationFilter(QOpenGLTexture::Linear);
        texturenormal->setMagnificationFilter(QOpenGLTexture::Linear);
        texturenormal->setWrapMode(QOpenGLTexture::ClampToEdge);
        int texId = texturenormal->textureId();
        qDebug() << QString("ID texture Normal map = %1").arg(texId);
    }
    else
    {
        qDebug() << "error loading normal map";
        return;
    }

}


void MainWidget::resizeGL(int w, int h)
{
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 7.0, field of view 45 degrees
    const qreal zNear = 0.1, zFar = 100.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}

void MainWidget::paintGL()
{

    makeCurrent();

    QPainter painter;
    painter.begin(this);
    painter.beginNativePainting();

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Enable depth buffer

    if(bDepthTest)
        glEnable(GL_DEPTH_TEST);

    QQuaternion quat = this->trackball.rotation();

    // Calculate model view transformation
    QMatrix4x4 modelviewearth;
    modelviewearth.translate(0.0, 0.0, distance);
    modelviewearth.rotate(quat);

    QMatrix4x4 rotmatrix;
    rotmatrix.rotate(quat);

    programskybox.bind();
    skybox->render(projection, modelviewearth, rotmatrix);
    programskybox.release();


    if(bWireFrame) // Draw as wireframe
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if(bTexture)
    {
        if(bNoBump)
        {
            programearthnobump.bind();
            textureearth->bind(0);
            geometries->render(projection, modelviewearth, bNoBump);
            textureearth->release();
            programearthnobump.release();
        }
        else
        {
            programearth.bind();
            textureearth->bind(0);
            texturenormal->bind(1);
            geometries->render(projection, modelviewearth, bNoBump);

            texturenormal->release();
            textureearth->release();
            programearth.release();
        }
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    if(bBorders)
    {
        programgshhs.bind();
        gshhs->render(projection, modelviewearth);
        gshhs->rendersoc(projection, modelviewearth);
        programgshhs.release();
    }

    glDisable(GL_DEPTH_TEST);

    painter.endNativePainting();

    QString framesPerSecond;
    if (const int elapsed = m_time.elapsed()) {
        framesPerSecond.setNum(m_frames /(elapsed / 1000.0), 'f', 2);
        painter.setPen(Qt::white);
        painter.drawText(20, 40, framesPerSecond + " paintGL calls / s");
    }

    //this->setWindowTitle(framesPerSecond + " paintGL calls / s" );

    painter.end();

    if (!(m_frames % 100)) {
        m_time.start();
        m_frames = 0;
    }
    ++m_frames;

}

void MainWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->buttons() & Qt::LeftButton)
    {
       this->trackball.push(pixelPosToViewPos(event->pos()), QQuaternion());
       event->accept();
    }

    if(event->buttons() & Qt::RightButton)
    {
       this->mouseDownAction(event->x(), event->y());
       event->accept();
    }

}

void MainWidget::mouseDownAction(int x, int y)
{

}

void MainWidget::mouseMoveEvent(QMouseEvent *event)
{


    if(event->buttons() & Qt::LeftButton)
    {
        this->trackball.move(pixelPosToViewPos(event->pos()), QQuaternion());
        event->accept();
    } else {
        this->trackball.release(pixelPosToViewPos(event->pos()), QQuaternion());
    }

}

void MainWidget::mouseReleaseEvent(QMouseEvent *event)
{

    if(event->button() == Qt::LeftButton)
    {
        this->trackball.release(pixelPosToViewPos(event->pos()),QQuaternion());
        event->accept();
    }

}

void MainWidget::wheelEvent(QWheelEvent * event)
{

    if (event->delta() > 0)
        distance += 0.1;
    else
        distance -= 0.1;
    //m_distExp += (float)(event->delta()/60 );
    if (distance < -500.0)
        distance = -500.0;
    if (distance > -1.2)
        distance = -1.2;
    event->accept();

}

void MainWidget::keyPressEvent(QKeyEvent *event)
{

    switch (event->key()) {
     case Qt::Key_B:
         toggleBorder();
         break;
    case Qt::Key_T:
        toggleTexture();
        break;
    case Qt::Key_W:
        toggleWireFrame();
        break;
    case Qt::Key_D:
        toggleDepth();
        break;
    case Qt::Key_V:
        toggleBumpMapping();
        break;
    case Qt::Key_Escape:
        close();
        break;
     default:
        break;
     }
}

void MainWidget::toggleBorder()
{
    if(bBorders)
        bBorders = false;
    else
        bBorders = true;
}

void MainWidget::toggleBumpMapping()
{
    if(bNoBump)
        bNoBump = false;
    else
        bNoBump = true;
}

void MainWidget::toggleDepth()
{
    if(bDepthTest)
        bDepthTest = false;
    else
        bDepthTest = true;
}

void MainWidget::toggleWireFrame()
{
    if(bWireFrame)
        bWireFrame = false;
    else
        bWireFrame = true;
}

void MainWidget::toggleTexture()
{
    if(bTexture)
        bTexture = false;
    else
        bTexture = true;
}

QPointF MainWidget::pixelPosToViewPos(const QPointF &p)
{
     return QPointF(2.0 * float(p.x()) / this->width() - 1.0,
                         1.0 - 2.0 * float(p.y()) / this->height());

}

//void Image::Convert32bitARGBtoRGBA()
//{
//    if(!isQtImage()) return;
//    QImage& q = *(m_data->image);
//    U32 count=0, max=(U32)(q.height()*q.width());
//    U32* p = (U32*)(q.bits());
//    U32 n;
//    while( count<max )
//    {
//        n = p[count];   //n = ARGB
//        p[count] =  0x00000000 |
//                ((n<<8)  & 0x0000ff00) |
//                ((n<<8)  & 0x00ff0000) |
//                ((n<<8)  & 0xff000000) |
//                ((n>>24) & 0x000000ff);
//                // p[count] = RGBA
//        count++;
//    }
//}

//void Image::Convert32bitRGBAtoARGB()
//{
//    if(!isQtImage()) return;
//    QImage& q = *(m_data->image);
//    U32 count=0, max=(U32)(q.height()*q.width());
//    U32* p = (U32*)(q.bits());
//    U32 n;
//    while( count<max )
//    {
//        n = p[count];   //n = RGBA
//        p[count] =  0x00000000 |
//                ((n>>8)  & 0x000000ff) |
//                ((n>>8)  & 0x0000ff00) |
//                ((n>>8)  & 0x00ff0000) |
//                ((n<<24) & 0xff000000);
//                // p[count] = ARGB
//        count++;
//    }
//}
//void MainWidget::animate()
//{
//    update();
//}


