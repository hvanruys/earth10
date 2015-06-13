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

#include "geometryengine.h"

#include <QVector2D>
#include <QVector3D>
#include <QGenericMatrix>
#include <math.h>

struct VertexData
{
    QVector3D position;
    QVector2D texCoord;
};


GeometryEngine::GeometryEngine(QOpenGLShaderProgram *prog, QOpenGLShaderProgram *prognobump)
    : indiciesBuf(QOpenGLBuffer::IndexBuffer)
{
    initializeOpenGLFunctions();
    program = prog;
    programnobump = prognobump;
    vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&vao);

    initSphereGeometry(1.0f, 128, 64);
}

GeometryEngine::~GeometryEngine()
{

    vao.destroy();
    positionsBuf.destroy();
    indiciesBuf.destroy();
    textureBuf.destroy();
    normalsBuf.destroy();
}



void GeometryEngine::initSphereGeometry( float radius, int slices, int stacks )
{
    const float pi = 3.1415926535897932384626433832795f;
    const float _2pi = 2.0f * pi;

    QVector<GLfloat> positions(0);
    QVector<GLfloat> normals(0);
    QVector<GLfloat> textures(0);


    for( int i = 0; i <= stacks; ++i )
    {
        // V texture coordinate.
        float V = i / (float)stacks;
        float phi = V * pi;

        for ( int j = 0; j <= slices; ++j )
        {
            // U texture coordinate.
            float U = j / (float)slices;
            float theta = (U * _2pi) - (pi/2);

            float X = cos(theta) * sin(phi);
            float Y = cos(phi);
            float Z = sin(theta) * sin(phi);

            positions.append( X * radius );
            positions.append( Y * radius );
            positions.append( Z * radius );
            normals.append( X );
            normals.append( Y );
            normals.append( Z );
            textures.append( U );
            textures.append( V );
        }
    }

    // Now generate the index buffer
    QVector<GLushort> indicies;

    for( int i = 0; i < slices * stacks + slices; ++i )
    {
        indicies.append( i );
        indicies.append( i + slices + 1  );
        indicies.append( i + slices );

        indicies.append( i + slices + 1  );
        indicies.append( i );
        indicies.append( i + 1 );
    }

    nbrIndicies = indicies.size();

    positionsBuf.create();
    positionsBuf.setUsagePattern(QOpenGLBuffer::StaticDraw);

    textureBuf.create();
    textureBuf.setUsagePattern(QOpenGLBuffer::StaticDraw);

    normalsBuf.create();
    normalsBuf.setUsagePattern(QOpenGLBuffer::StaticDraw);

    indiciesBuf.create();
    indiciesBuf.setUsagePattern(QOpenGLBuffer::StaticDraw);


    positionsBuf.bind();
    positionsBuf.allocate(positions.data(), positions.size() * sizeof(GLfloat));

    normalsBuf.bind();
    normalsBuf.allocate(normals.data(), normals.size() * sizeof(GLfloat));


    textureBuf.bind();
    textureBuf.allocate(textures.data(), textures.size() * sizeof(GLfloat));


    indiciesBuf.bind();
    indiciesBuf.allocate(indicies.data(), indicies.size() * sizeof(GLushort));

    // Bind shader pipeline for use
    program->bind();

    vertexPosition = program->attributeLocation("VertexPosition");
    vertexTexCoord = program->attributeLocation("VertexTexCoord");
    vertexNormal = program->attributeLocation("VertexNormal");


    program->enableAttributeArray(vertexPosition);
    program->enableAttributeArray(vertexNormal);
    program->enableAttributeArray(vertexTexCoord);

    positionsBuf.bind();
    program->setAttributeBuffer(vertexPosition, GL_FLOAT, 0, 3);

    normalsBuf.bind();
    program->setAttributeBuffer(vertexNormal, GL_FLOAT, 0, 3);

    textureBuf.bind();
    program->setAttributeBuffer(vertexTexCoord, GL_FLOAT, 0, 2);

    program->setUniformValue("Light.Intensity", QVector3D(1.0f,1.0f,1.0f) );

    // Bind shader pipeline for use
    programnobump->bind();

    vertexPositionnobump = programnobump->attributeLocation("VertexPosition");
    vertexTexCoordnobump = programnobump->attributeLocation("VertexTexCoord");
    vertexNormalnobump = programnobump->attributeLocation("VertexNormal");


    programnobump->enableAttributeArray(vertexPositionnobump);
    programnobump->enableAttributeArray(vertexNormalnobump);
    programnobump->enableAttributeArray(vertexTexCoordnobump);

    positionsBuf.bind();
    programnobump->setAttributeBuffer(vertexPositionnobump, GL_FLOAT, 0, 3);

    normalsBuf.bind();
    programnobump->setAttributeBuffer(vertexNormalnobump, GL_FLOAT, 0, 3);

    textureBuf.bind();
    programnobump->setAttributeBuffer(vertexTexCoordnobump, GL_FLOAT, 0, 2);

    programnobump->setUniformValue("Light.Intensity", QVector3D(1.0f,1.0f,1.0f) );


}


void GeometryEngine::render(QMatrix4x4 projection, QMatrix4x4 modelview, bool bnobump)
{

    QOpenGLVertexArrayObject::Binder vaoBinder(&vao);

    QVector4D lightpos = modelview * QVector4D(-150.0f,0.0f,150.0f,1.0f);
//    QVector4D lightpos = QVector4D(-150.0f,0.0f,150.0f,1.0f);

    if(bnobump)
    {
        programnobump->setUniformValue("Light.Position", lightpos );
        programnobump->setUniformValue("Material.Kd", 1.0f, 1.0f, 1.0f); // diffuse
        programnobump->setUniformValue("Material.Ks", 0.40f, 0.40f, 0.40f); // specular
        programnobump->setUniformValue("Material.Ka", 0.060f, 0.060f, 0.060f); // ambient
        programnobump->setUniformValue("Material.Shininess", 100.0f);

        programnobump->setUniformValue("ModelViewMatrix", modelview);
        QMatrix3x3 normalmatrix1 = modelview.normalMatrix();
        QMatrix4x4 nm4 = modelview.transposed().inverted();
        float val[] = { nm4(0,0), nm4(0,1), nm4(0,2),
                            nm4(1,0), nm4(1,1), nm4(1,2),
                            nm4(2,0), nm4(2,1), nm4(2,2) };
        QMatrix3x3 normalmatrix(val);


        programnobump->setUniformValue("NormalMatrix", normalmatrix);
        programnobump->setUniformValue("MVP", projection * modelview);
    }
    else
    {

        program->setUniformValue("Light.Position", lightpos );
        program->setUniformValue("Material.Kd", 1.0f, 1.0f, 1.0f); // diffuse
        program->setUniformValue("Material.Ks", 0.4f, 0.4f, 0.4f); // specular
        program->setUniformValue("Material.Ka", 0.06f, 0.06f, 0.06f); // ambient
        program->setUniformValue("Material.Shininess", 100.0f);


        program->setUniformValue("ModelViewMatrix", modelview);
        QMatrix3x3 norm = modelview.normalMatrix();
        program->setUniformValue("NormalMatrix", norm);
        program->setUniformValue("MVP", projection * modelview);
    }

    glDrawElements(GL_TRIANGLES, nbrIndicies, GL_UNSIGNED_SHORT, 0);
}

