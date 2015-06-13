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

#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include "geometryengine.h"

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QVector2D>
#include <QBasicTimer>
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include "trackball.h"
#include "gshhsdata.h"
#include "skybox.h"

class GeometryEngine;

class MainWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = 0);
    ~MainWidget();

protected:
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent * event);
    void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
    //void timerEvent(QTimerEvent *e) Q_DECL_OVERRIDE;
    void mouseDownAction(int x, int y);
    //void paintEvent(QPaintEvent *e);


    void initializeGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;

    bool initShaders();
    void initTextures();

private:

    QOpenGLShaderProgram programearth;
    QOpenGLShaderProgram programearthnobump;
    QOpenGLShaderProgram programgshhs;
    QOpenGLShaderProgram programskybox;

    QOpenGLTexture *textureearth;
    QOpenGLTexture *texturenormal;
    GLuint uniformTexEarth;
    GLuint uniformTexLand;
    GLuint uniformTexEarthnobump;

    QPointF pixelPosToViewPos(const QPointF& p);
    //QPointF pixelPosToViewPos(QPointF &p);

    void toggleBorder();
    void toggleBumpMapping();
    void toggleDepth();
    void toggleWireFrame();
    void toggleTexture();
    void createAlphaTexture();

    //QBasicTimer timer;
    GeometryEngine *geometries;
    gshhsData *gshhs;
    SkyBox *skybox;

    QMatrix4x4 projection;
    float distance;
    QVector2D mousePressPosition;
    QVector3D rotationAxis;
    qreal angularSpeed;
    QQuaternion rotation;
    TrackBall trackball;
    //QTimer animationTimer;
    QTime m_time;
    int delay;
    bool bBorders;
    bool bNoBump;
    bool bDepthTest;
    bool bWireFrame;
    bool bTexture;
    int m_frames;

private slots:
    //void animate();

};

#endif // MAINWIDGET_H
