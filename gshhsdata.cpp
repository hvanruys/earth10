#include <QDebug>
#include "gshhsdata.h"

gshhsData::gshhsData(QOpenGLShaderProgram *prog)
{

    glf = QOpenGLContext::currentContext()->functions();

//    initializeOpenGLFunctions();

    program = prog;

    for(int i = 0; i < 3; i++)
    {
        vxp_data[i] = new Vxp;
        vxp_data[i]->nFeatures = 0;
    }

    Initialize("../earth_10/gshhs2_2_4/gshhs_i.b", "../earth_10/gshhs2_2_4/wdb_borders_i.b", "");

    QVector<GLfloat> positions1;
    int totalverts = 0;

    int k = 0;

    featurevertsindex[0].append(0);

    for( int i = 0; i < vxp_data[k]->nFeatures; i++)
    {
        for (int j = 0; j < vxp_data[k]->pFeatures[i].nVerts; j++)
        {
            positions1.append((float)vxp_data[k]->pFeatures[i].pVerts[j].x);
            positions1.append((float)vxp_data[k]->pFeatures[i].pVerts[j].y);
            positions1.append((float)vxp_data[k]->pFeatures[i].pVerts[j].z);

        }
        totalverts += vxp_data[k]->pFeatures[i].nVerts;
        featurevertsindex[0].append(totalverts);
    }


    k = 1;

    for( int i = 0; i < vxp_data[k]->nFeatures; i++)
    {
        for (int j = 0; j < vxp_data[k]->pFeatures[i].nVerts; j++)
        {
            positions1.append((float)vxp_data[k]->pFeatures[i].pVerts[j].x);
            positions1.append((float)vxp_data[k]->pFeatures[i].pVerts[j].y);
            positions1.append((float)vxp_data[k]->pFeatures[i].pVerts[j].z);

        }
        totalverts += vxp_data[k]->pFeatures[i].nVerts;
        featurevertsindex[0].append(totalverts);
    }


    vao.create();
    QOpenGLVertexArrayObject::Binder vaoBinder(&vao);

    positionsBuf1.create();
    positionsBuf1.setUsagePattern(QOpenGLBuffer::StaticDraw);
    positionsBuf1.bind();
    positionsBuf1.allocate(positions1.data(), positions1.size() * sizeof(GLfloat));

    qDebug() << QString("gshhsdata positions.size() = %1").arg(positions1.size());
    qDebug() << QString("gshhsdata totalverts = %1").arg(totalverts);

    nbrVerts1 = positions1.size()/3;


    delete [] vxp_data[0];
    delete [] vxp_data[1];

    // Bind shader pipeline for use
    program->bind();
    vertexPosition = program->attributeLocation("VertexPosition");
    program->enableAttributeArray(vertexPosition);
    program->setAttributeBuffer(vertexPosition, GL_FLOAT, 0, 3);

    vao2.create();
    QOpenGLVertexArrayObject::Binder vaoBinder2(&vao2);

    QVector<GLfloat> positions2;
    positions2.append(0.0f);
    positions2.append(0.0f);
    positions2.append(0.0f);

    positions2.append(0.0f);
    positions2.append(1.5f);
    positions2.append(0.0f);

    positions2.append(0.0f);
    positions2.append(0.0f);
    positions2.append(0.0f);

    positions2.append(1.5f);
    positions2.append(0.0f);
    positions2.append(0.0f);

    positions2.append(0.0f);
    positions2.append(0.0f);
    positions2.append(0.0f);

    positions2.append(0.0f);
    positions2.append(0.0f);
    positions2.append(1.5f);


    positionsBuf2.create();
    positionsBuf2.setUsagePattern(QOpenGLBuffer::StaticDraw);
    positionsBuf2.bind();
    positionsBuf2.allocate(positions2.data(), positions2.size() * sizeof(GLfloat));

    // Bind shader pipeline for use
    program->bind();
    vertexPosition = program->attributeLocation("VertexPosition");
    program->enableAttributeArray(vertexPosition);
    program->setAttributeBuffer(vertexPosition, GL_FLOAT, 0, 3);

    qDebug() << "size postionsBuf2 = " << positionsBuf2.size();


}

gshhsData::~gshhsData()
{
    vao.destroy();
    vao2.destroy();
    positionsBuf1.destroy();
    positionsBuf2.destroy();
 }

void gshhsData::render(QMatrix4x4 projection, QMatrix4x4 modelview)
{

    QOpenGLVertexArrayObject::Binder vaoBinder(&vao);

    program->setUniformValue("MVP", projection * modelview);
    QMatrix3x3 norm = modelview.normalMatrix();
    program->setUniformValue("NormalMatrix", norm);

    for(int i = 0; i < featurevertsindex[0].size()-1; i++)
        glf->glDrawArrays(GL_LINE_STRIP, featurevertsindex[0].at(i), featurevertsindex[0].at(i+1)-featurevertsindex[0].at(i));

}

void gshhsData::rendersoc(QMatrix4x4 projection, QMatrix4x4 modelview)
{

    QOpenGLVertexArrayObject::Binder vaoBinder(&vao2);

    program->setUniformValue("MVP", projection * modelview);
    QMatrix3x3 norm = modelview.normalMatrix();
    program->setUniformValue("NormalMatrix", norm);

    glf->glDrawArrays(GL_LINE_STRIP, 0, 6);

}


void gshhsData::Initialize(QString data1, QString data2, QString data3)
{
    QByteArray baglobe1 = data1.toUtf8();
    QByteArray baglobe2 = data2.toUtf8();
    QByteArray baglobe3 = data3.toUtf8();

    if (baglobe1.length() > 0)
    {
        int nFeatures = check_gshhs(baglobe1.data());
        load_gshhs(baglobe1.data(), nFeatures, vxp_data[0]);
        qDebug() << QString("gshhs 1 file loaded nFeatures = %1").arg(nFeatures);
    }

    if (baglobe2.length() > 0)
    {
        int nFeatures = check_gshhs(baglobe2.data());
        load_gshhs(baglobe2.data(), nFeatures, vxp_data[1]);
        qDebug() << QString("gshhs 2 file loaded nFeatures = %1").arg(nFeatures);
    }

    if (baglobe3.length() > 0)
    {
        int nFeatures = check_gshhs(baglobe3.data());
        load_gshhs(baglobe3.data(), nFeatures, vxp_data[2]);
        qDebug() << QString("gshhs 3 file loaded nFeatures = %1").arg(nFeatures);
    }


}

int gshhsData::check_gshhs(char *pFileName)
{

    //double w, e, s, n, area, f_area, lon, lat;
    //char source, kind[2] = {'P', 'L'}, c = '>', *file = NULL;
    //char *name[2] = {"polygon", "line"};
    FILE *fp = NULL;
//    int k, line, max_east = 270000000, info, single, error, ID, flip;
//    int  OK, level, version, greenwich, river, src, msformat = 0, first = 1;
    int max_east = 270000000;
    int flip;
    int version;

    size_t n_read;
    //struct POINT_GSHHS p;
    struct GSHHS h;

    int nFeatures = 0;


    if ((fp = fopen (pFileName, "rb")) == NULL ) {
            qDebug() << QString( "gshhs:  Could not find file %1.").arg(pFileName);
            return 0; //exit (EXIT_FAILURE);
    }

    n_read = fread ((void *)&h, (size_t)sizeof (struct GSHHS), (size_t)1, fp);
    version = (h.flag >> 8) & 255;
    flip = (version != GSHHS_DATA_RELEASE);	/* Take as sign that byte-swabbing is needed */


    while (n_read == 1)
    {
            if (flip)
            {
                    h.id = swabi4 ((unsigned int)h.id);
                    h.n  = swabi4 ((unsigned int)h.n);
                    h.west  = swabi4 ((unsigned int)h.west);
                    h.east  = swabi4 ((unsigned int)h.east);
                    h.south = swabi4 ((unsigned int)h.south);
                    h.north = swabi4 ((unsigned int)h.north);
                    h.area  = swabi4 ((unsigned int)h.area);
                    h.area_full  = swabi4 ((unsigned int)h.area_full);
                    h.flag  = swabi4 ((unsigned int)h.flag);
                    h.container  = swabi4 ((unsigned int)h.container);
                    h.ancestor  = swabi4 ((unsigned int)h.ancestor);
            }

            fseek (fp, (long)(h.n * sizeof(struct POINT_GSHHS)), SEEK_CUR);
            max_east = 180000000;	/* Only Eurasia needs 270 */
            n_read = fread((void *)&h, (size_t)sizeof (struct GSHHS), (size_t)1, fp);
            if (n_read > 0)
                nFeatures++;
    }

    fclose (fp);
    return (nFeatures + 1);
}

void gshhsData::load_gshhs(char *pFileName, int nTotFeatures, Vxp *vxp)
{

    double w, e, s, n, area, f_area, lon, lat;
    char source, kind[2] = {'P', 'L'}, c = '>';
    FILE *fp = NULL;
    int line, max_east = 270000000, info, single, error, ID, flip;
    int  level, version, greenwich, river, src, msformat = 0, first = 1;
    size_t n_read;
    struct POINT_GSHHS p;
    struct GSHHS h;
    //Vxp *vxp = new Vxp;

    int nFeatures = 0;

    info = single = error = ID = 0;


    if ((fp = fopen (pFileName, "rb")) == NULL ) {
            qDebug() << QString( "gshhs:  Could not find file %1.").arg(pFileName);
            return; // exit (EXIT_FAILURE);
    }

    n_read = fread ((void *)&h, (size_t)sizeof (struct GSHHS), (size_t)1, fp);
    version = (h.flag >> 8) & 255;
    flip = (version != GSHHS_DATA_RELEASE);	/* Take as sign that byte-swabbing is needed */

    vxp->nFeatures = nTotFeatures;
    vxp->pFeatures = new VxpFeature[nTotFeatures];

    while (n_read == 1)
    {
            if (flip)
            {
                    h.id = swabi4 ((unsigned int)h.id);
                    h.n  = swabi4 ((unsigned int)h.n);
                    h.west  = swabi4 ((unsigned int)h.west);
                    h.east  = swabi4 ((unsigned int)h.east);
                    h.south = swabi4 ((unsigned int)h.south);
                    h.north = swabi4 ((unsigned int)h.north);
                    h.area  = swabi4 ((unsigned int)h.area);
                    h.area_full  = swabi4 ((unsigned int)h.area_full);
                    h.flag  = swabi4 ((unsigned int)h.flag);
                    h.container  = swabi4 ((unsigned int)h.container);
                    h.ancestor  = swabi4 ((unsigned int)h.ancestor);
            }
            level = h.flag & 255;				/* Level is 1-4 */
            version = (h.flag >> 8) & 255;			/* Version is 1-7 */
            //if (first) fprintf (stderr, "gshhs %s - Found GSHHS version %d in file %s\n", GSHHS_PROG_VERSION, version, file);
            greenwich = (h.flag >> 16) & 1;			/* Greenwich is 0 or 1 */
            src = (h.flag >> 24) & 1;			/* Greenwich is 0 (WDBII) or 1 (WVS) */
            river = (h.flag >> 25) & 1;			/* River is 0 (not river) or 1 (is river) */
            w = h.west  * GSHHS_SCL;			/* Convert from microdegrees to degrees */
            e = h.east  * GSHHS_SCL;
            s = h.south * GSHHS_SCL;
            n = h.north * GSHHS_SCL;
            source = (src == 1) ? 'W' : 'C';		/* Either WVS or CIA (WDBII) pedigree */
            if (river) source = tolower ((int)source);	/* Lower case c means river-lake */
            line = (h.area) ? 0 : 1;			/* Either Polygon (0) or Line (1) (if no area) */
            area = 0.1 * h.area;				/* Now im km^2 */
            f_area = 0.1 * h.area_full;			/* Now im km^2 */

            //OK = (!single || h.id == ID);
            first = 0;

            if (!msformat) c = kind[line];

            vxp->pFeatures[nFeatures].nVerts = h.n;
            vxp->pFeatures[nFeatures].pVerts = new floatVector[ h.n ];
            vxp->pFeatures[nFeatures].pLonLat = new LonLatPair[ h.n ];


            for (int k = 0; k < h.n; k++)
            {
                if (fread ((void *)&p, (size_t)sizeof(struct POINT_GSHHS), (size_t)1, fp) != 1)
                {
                    //fprintf (stderr, "gshhs:  Error reading file %s for %s %d, point %d.\n", argv[1], name[line], h.id, k);
                    exit (EXIT_FAILURE);
                }
                if (flip)
                {
                    p.x = swabi4 ((unsigned int)p.x);
                    p.y = swabi4 ((unsigned int)p.y);
                }
                lon = p.x * GSHHS_SCL;
                if ((greenwich && p.x > max_east) || (h.west > 180000000)) lon -= 360.0;
                lat = p.y * GSHHS_SCL;
                LonLat2Point(lat, lon, &vxp->pFeatures[nFeatures].pVerts[k], 1.0f);
                vxp->pFeatures[nFeatures].pLonLat[k].lonmicro = p.x;
                vxp->pFeatures[nFeatures].pLonLat[k].latmicro = p.y;

            }

            max_east = 180000000;	/* Only Eurasia needs 270 */
            n_read = fread((void *)&h, (size_t)sizeof (struct GSHHS), (size_t)1, fp);
            nFeatures++;
    }

    fclose (fp);

}

void gshhsData::LonLat2Point(float lat, float lon, floatVector *pos, float radius)
{
        // lon -90..90
        // lat -180..180

        float	angX, angY;

        angX = lon * M_PI / 180.f;
        angY = lat * M_PI / 180.f;

        pos->x = cosf(angY) * sinf(angX) * radius;
        pos->y = sinf(angY) * radius;
        pos->z = cosf(angY) * cosf(angX) * radius;

}
