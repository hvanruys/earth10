#ifndef GSHHSDATA_H
#define GSHHSDATA_H

#include "gshhs.h"
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>

// structures
struct floatVector {
        float x, y, z;
};

struct LonLatPair {
        int lonmicro;
        int latmicro;
};

// vector-file
struct VxpFeature {
        int	 nVerts;
        floatVector	*pVerts;
        LonLatPair *pLonLat;
};

struct Vxp {
        int         nFeatures;
        VxpFeature  *pFeatures;
};

class gshhsData//  : protected QOpenGLFunctions_4_3_Core
{
public:
    gshhsData(QOpenGLShaderProgram *prog);
    ~gshhsData();

    void Initialize(QString data1, QString data2, QString data3);
    void load_gshhs(char *pFileName, int nTotFeatures, Vxp *vxp);
    int check_gshhs(char *pFileName);
    void render(QMatrix4x4 projection, QMatrix4x4 modelview);
    void rendersoc(QMatrix4x4 projection, QMatrix4x4 modelview);

    Vxp	*vxp_data[3];
private:
    void LonLat2Point(float lat, float lon, floatVector *pos, float radius);
    QOpenGLShaderProgram *program;
    QOpenGLVertexArrayObject vao;
    QOpenGLVertexArrayObject vao2;

    QOpenGLBuffer positionsBuf1;
    QOpenGLBuffer positionsBuf2;
    GLuint vertexPosition;
    int nbrVerts1, nbrVerts2;
    QVector<GLuint> featurevertsindex[2];
    QOpenGLFunctions *glf;



};

#endif // GSHHSDATA_H
