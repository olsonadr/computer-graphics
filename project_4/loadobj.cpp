#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <GL/gl.h>

#include <vector>

// delimiters for parsing the obj file:

#define OBJDELIMS " \t"

struct OBJVertex
{
    float x, y, z;
};

struct OBJNormal
{
    float nx, ny, nz;
};

struct OBJTextureCoord
{
    float s, t, p;
};

struct OBJface
{
    int v, n, t;
};

void OBJCross(float[3], float[3], float[3]);
char *ReadRestOfLine(FILE *);
void ReadObjVTN(char *, int *, int *, int *);
float OBJUnit(float[3]);
float OBJUnit(float[3], float[3]);

int LoadObjFile(char *name)
{
    char *cmd; // the command string
    char *str; // argument string

    std::vector<struct OBJVertex> Vertices(10000);
    std::vector<struct OBJNormal> OBJNormals(10000);
    std::vector<struct OBJTextureCoord> OBJTextureCoords(10000);

    Vertices.clear();
    OBJNormals.clear();
    OBJTextureCoords.clear();

    struct OBJVertex sv;
    struct OBJNormal sn;
    struct OBJTextureCoord st;

    // open the input file:

    FILE *fp = fopen(name, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Cannot open .obj file '%s'\n", name);
        return 1;
    }

    float xmin = 1.e+37f;
    float ymin = 1.e+37f;
    float zmin = 1.e+37f;
    float xmax = -xmin;
    float ymax = -ymin;
    float zmax = -zmin;

    glBegin(GL_TRIANGLES);

    char *line;
    for (;;)
    {
        line = ReadRestOfLine(fp);
        if (line == NULL)
            break;

        // skip this line if it is a comment:

        if (line[0] == '#')
            continue;

        // skip this line if it is something we don't feel like handling today:

        if (line[0] == 'g')
            continue;

        if (line[0] == 'm')
            continue;

        if (line[0] == 's')
            continue;

        if (line[0] == 'u')
            continue;

        // get the command string:

        cmd = strtok(line, OBJDELIMS);

        // skip this line if it is empty:

        if (cmd == NULL)
            continue;

        if (strcmp(cmd, "v") == 0)
        {
            str = strtok(NULL, OBJDELIMS);
            sv.x = atof(str);

            str = strtok(NULL, OBJDELIMS);
            sv.y = atof(str);

            str = strtok(NULL, OBJDELIMS);
            sv.z = atof(str);

            Vertices.push_back(sv);

            if (sv.x < xmin)
                xmin = sv.x;
            if (sv.x > xmax)
                xmax = sv.x;
            if (sv.y < ymin)
                ymin = sv.y;
            if (sv.y > ymax)
                ymax = sv.y;
            if (sv.z < zmin)
                zmin = sv.z;
            if (sv.z > zmax)
                zmax = sv.z;

            continue;
        }

        if (strcmp(cmd, "vn") == 0)
        {
            str = strtok(NULL, OBJDELIMS);
            sn.nx = atof(str);

            str = strtok(NULL, OBJDELIMS);
            sn.ny = atof(str);

            str = strtok(NULL, OBJDELIMS);
            sn.nz = atof(str);

            OBJNormals.push_back(sn);

            continue;
        }

        if (strcmp(cmd, "vt") == 0)
        {
            st.s = st.t = st.p = 0.;

            str = strtok(NULL, OBJDELIMS);
            st.s = atof(str);

            str = strtok(NULL, OBJDELIMS);
            if (str != NULL)
                st.t = atof(str);

            str = strtok(NULL, OBJDELIMS);
            if (str != NULL)
                st.p = atof(str);

            OBJTextureCoords.push_back(st);

            continue;
        }

        if (strcmp(cmd, "f") == 0)
        {
            struct OBJface vertices[10];
            for (int i = 0; i < 10; i++)
            {
                vertices[i].v = 0;
                vertices[i].n = 0;
                vertices[i].t = 0;
            }

            int sizev = (int)Vertices.size();
            int sizen = (int)OBJNormals.size();
            int sizet = (int)OBJTextureCoords.size();

            int numVertices = 0;
            bool valid = true;
            int vtx = 0;
            char *str;
            while ((str = strtok(NULL, OBJDELIMS)) != NULL)
            {
                int v, n, t;
                ReadObjVTN(str, &v, &t, &n);

                // if v, n, or t are negative, they are wrt the end of their respective list:

                if (v < 0)
                    v += (sizev + 1);

                if (n < 0)
                    n += (sizen + 1);

                if (t < 0)
                    t += (sizet + 1);

                // be sure we are not out-of-bounds (<vector> will abort):

                if (t > sizet)
                {
                    if (t != 0)
                        fprintf(stderr, "Read texture coord %d, but only have %d so far\n", t, sizet);
                    t = 0;
                }

                if (n > sizen)
                {
                    if (n != 0)
                        fprintf(stderr, "Read OBJnormal %d, but only have %d so far\n", n, sizen);
                    n = 0;
                }

                if (v > sizev)
                {
                    if (v != 0)
                        fprintf(stderr, "Read OBJvertex coord %d, but only have %d so far\n", v, sizev);
                    v = 0;
                    valid = false;
                }

                vertices[vtx].v = v;
                vertices[vtx].n = n;
                vertices[vtx].t = t;
                vtx++;

                if (vtx >= 10)
                    break;

                numVertices++;
            }

            // if vertices are invalid, don't draw anything this time:

            if (!valid)
                continue;

            if (numVertices < 3)
                continue;

            // list the vertices:

            int numTriangles = numVertices - 2;

            for (int it = 0; it < numTriangles; it++)
            {
                int vv[3];
                vv[0] = 0;
                vv[1] = it + 1;
                vv[2] = it + 2;

                // get the planar OBJnormal, in case OBJvertex OBJnormals are not defined:

                struct OBJVertex *v0 = &Vertices[vertices[vv[0]].v - 1];
                struct OBJVertex *v1 = &Vertices[vertices[vv[1]].v - 1];
                struct OBJVertex *v2 = &Vertices[vertices[vv[2]].v - 1];

                float v01[3], v02[3], norm[3];
                v01[0] = v1->x - v0->x;
                v01[1] = v1->y - v0->y;
                v01[2] = v1->z - v0->z;
                v02[0] = v2->x - v0->x;
                v02[1] = v2->y - v0->y;
                v02[2] = v2->z - v0->z;
                OBJCross(v01, v02, norm);
                OBJUnit(norm, norm);
                glNormal3fv(norm);

                for (int vtx = 0; vtx < 3; vtx++)
                {
                    if (vertices[vv[vtx]].t != 0)
                    {
                        struct OBJTextureCoord *tp = &OBJTextureCoords[vertices[vv[vtx]].t - 1];
                        glTexCoord2f(tp->s, tp->t);
                    }

                    if (vertices[vv[vtx]].n != 0)
                    {
                        struct OBJNormal *np = &OBJNormals[vertices[vv[vtx]].n - 1];
                        glNormal3f(np->nx, np->ny, np->nz);
                    }

                    struct OBJVertex *vp = &Vertices[vertices[vv[vtx]].v - 1];
                    glVertex3f(vp->x, vp->y, vp->z);
                }
            }
            continue;
        }

        if (strcmp(cmd, "s") == 0)
        {
            continue;
        }
    }

    delete[] line;

    glEnd();
    fclose(fp);

    // fprintf(stderr, "Obj file range: [%8.3f,%8.3f,%8.3f] -> [%8.3f,%8.3f,%8.3f]\n",
    //         xmin, ymin, zmin, xmax, ymax, zmax);
    // fprintf(stderr, "Obj file center = (%8.3f,%8.3f,%8.3f)\n",
    //         (xmin + xmax) / 2., (ymin + ymax) / 2., (zmin + zmax) / 2.);
    // fprintf(stderr, "Obj file  span = (%8.3f,%8.3f,%8.3f)\n",
    //         xmax - xmin, ymax - ymin, zmax - zmin);

    return 0;
}

int LoadObjFileFromBlender(char *name)
{
    char *cmd; // the command string
    char *str; // argument string

    std::vector<struct OBJVertex> Vertices(10000);
    std::vector<struct OBJNormal> OBJNormals(10000);
    std::vector<struct OBJTextureCoord> OBJTextureCoords(10000);

    Vertices.clear();
    OBJNormals.clear();
    OBJTextureCoords.clear();

    struct OBJVertex sv;
    struct OBJNormal sn;
    struct OBJTextureCoord st;

    // open the input file:

    FILE *fp = fopen(name, "r");
    if (fp == NULL)
    {
        fprintf(stderr, "Cannot open .obj file '%s'\n", name);
        return 1;
    }

    float xmin = 1.e+37f;
    float ymin = 1.e+37f;
    float zmin = 1.e+37f;
    float xmax = -xmin;
    float ymax = -ymin;
    float zmax = -zmin;

    float smin=2, tmin=smin;
    float smax=-1, tmax=smax;

    glBegin(GL_TRIANGLES);

    for (;;)
    {
        char *line = ReadRestOfLine(fp);
        if (line == NULL)
            break;

        // skip this line if it is a comment:

        if (line[0] == '#')
            continue;

        // skip this line if it is something we don't feel like handling today:

        if (line[0] == 'g')
            continue;

        if (line[0] == 'm')
            continue;

        if (line[0] == 's')
            continue;

        if (line[0] == 'u')
            continue;

        // get the command string:

        cmd = strtok(line, OBJDELIMS);

        // skip this line if it is empty:

        if (cmd == NULL)
            continue;

        if (strcmp(cmd, "v") == 0)
        {
            str = strtok(NULL, OBJDELIMS);
            sv.x = atof(str);

            str = strtok(NULL, OBJDELIMS);
            sv.y = atof(str);

            str = strtok(NULL, OBJDELIMS);
            sv.z = atof(str);

            Vertices.push_back(sv);

            if (sv.x < xmin)
                xmin = sv.x;
            if (sv.x > xmax)
                xmax = sv.x;
            if (sv.y < ymin)
                ymin = sv.y;
            if (sv.y > ymax)
                ymax = sv.y;
            if (sv.z < zmin)
                zmin = sv.z;
            if (sv.z > zmax)
                zmax = sv.z;

            continue;
        }

        if (strcmp(cmd, "vn") == 0)
        {
            str = strtok(NULL, OBJDELIMS);
            sn.nx = atof(str);

            str = strtok(NULL, OBJDELIMS);
            sn.ny = atof(str);

            str = strtok(NULL, OBJDELIMS);
            sn.nz = atof(str);

            OBJNormals.push_back(sn);

            continue;
        }

        if (strcmp(cmd, "vt") == 0)
        {
            st.s = st.t = st.p = 0.;

            str = strtok(NULL, OBJDELIMS);
            st.s = atof(str);

            str = strtok(NULL, OBJDELIMS);
            if (str != NULL)
                st.t = atof(str);
                // st.t = 1 - atof(str);

            str = strtok(NULL, OBJDELIMS);
            if (str != NULL)
                st.p = atof(str);

            if (st.s < smin)
                smin = st.s;
            if (st.s > smax)
                smax = st.s;
            if (st.t < tmin)
                tmin = st.t;
            if (st.t > tmax)
                tmax = st.t;

            OBJTextureCoords.push_back(st);

            continue;
        }

        if (strcmp(cmd, "f") == 0)
        {
            struct OBJface vertices[10];
            for (int i = 0; i < 10; i++)
            {
                vertices[i].v = 0;
                vertices[i].n = 0;
                vertices[i].t = 0;
            }

            int sizev = (int)Vertices.size();
            int sizen = (int)OBJNormals.size();
            int sizet = (int)OBJTextureCoords.size();

            int numVertices = 0;
            bool valid = true;
            int vtx = 0;
            char *str;
            while ((str = strtok(NULL, OBJDELIMS)) != NULL)
            {
                int v, n, t;
                ReadObjVTN(str, &v, &t, &n);

                // if v, n, or t are negative, they are wrt the end of their respective list:

                if (v < 0)
                    v += (sizev + 1);

                if (n < 0)
                    n += (sizen + 1);

                if (t < 0)
                    t += (sizet + 1);

                // be sure we are not out-of-bounds (<vector> will abort):

                if (t > sizet)
                {
                    if (t != 0)
                        fprintf(stderr, "Read texture coord %d, but only have %d so far\n", t, sizet);
                    t = 0;
                }

                if (n > sizen)
                {
                    if (n != 0)
                        fprintf(stderr, "Read OBJnormal %d, but only have %d so far\n", n, sizen);
                    n = 0;
                }

                if (v > sizev)
                {
                    if (v != 0)
                        fprintf(stderr, "Read OBJvertex coord %d, but only have %d so far\n", v, sizev);
                    v = 0;
                    valid = false;
                }

                vertices[vtx].v = v;
                vertices[vtx].n = n;
                vertices[vtx].t = t;
                vtx++;

                if (vtx >= 10)
                    break;

                numVertices++;
            }

            // if vertices are invalid, don't draw anything this time:

            if (!valid)
                continue;

            if (numVertices < 3)
                continue;

            // list the vertices:

            int numTriangles = numVertices - 2;

            for (int it = 0; it < numTriangles; it++)
            {
                int vv[3];
                vv[0] = 0;
                vv[1] = it + 1;
                vv[2] = it + 2;

                // get the planar OBJnormal, in case OBJvertex OBJnormals are not defined:

                struct OBJVertex *v0 = &Vertices[vertices[vv[0]].v - 1];
                struct OBJVertex *v1 = &Vertices[vertices[vv[1]].v - 1];
                struct OBJVertex *v2 = &Vertices[vertices[vv[2]].v - 1];

                float v01[3], v02[3], norm[3];
                v01[0] = v1->x - v0->x;
                v01[1] = v1->y - v0->y;
                v01[2] = v1->z - v0->z;
                v02[0] = v2->x - v0->x;
                v02[1] = v2->y - v0->y;
                v02[2] = v2->z - v0->z;
                OBJCross(v01, v02, norm);
                OBJUnit(norm, norm);
                glNormal3fv(norm);

                for (int vtx = 0; vtx < 3; vtx++)
                {
                    if (vertices[vv[vtx]].t != 0)
                    {
                        struct OBJTextureCoord *tp = &OBJTextureCoords[vertices[vv[vtx]].t - 1];
                        glTexCoord2f(tp->s, tp->t);
                    }

                    if (vertices[vv[vtx]].n != 0)
                    {
                        struct OBJNormal *np = &OBJNormals[vertices[vv[vtx]].n - 1];
                        glNormal3f(np->nx, np->ny, np->nz);
                    }

                    struct OBJVertex *vp = &Vertices[vertices[vv[vtx]].v - 1];
                    glVertex3f(vp->x, vp->y, vp->z);
                }
            }
            continue;
        }

        if (strcmp(cmd, "s") == 0)
        {
            continue;
        }
    }

    glEnd();
    fclose(fp);

    // fprintf(stderr, "Obj file range: [%8.3f,%8.3f,%8.3f] -> [%8.3f,%8.3f,%8.3f]\n",
    //         xmin, ymin, zmin, xmax, ymax, zmax);
    // fprintf(stderr, "Obj file center = (%8.3f,%8.3f,%8.3f)\n",
    //         (xmin + xmax) / 2., (ymin + ymax) / 2., (zmin + zmax) / 2.);
    // fprintf(stderr, "Obj file  span = (%8.3f,%8.3f,%8.3f)\n",
    //         xmax - xmin, ymax - ymin, zmax - zmin);
    // fprintf(stderr, "Obj file st range = (%8.3f,%8.3f) (%8.3f, %8.3f)\n",
    //         smin, smax, tmin, tmax);

    return 0;
}

void OBJCross(float v1[3], float v2[3], float vout[3])
{
    float tmp[3];

    tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
    tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
    tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];

    vout[0] = tmp[0];
    vout[1] = tmp[1];
    vout[2] = tmp[2];
}

float OBJUnit(float v[3])
{
    float dist;

    dist = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];

    if (dist > 0.0)
    {
        dist = sqrt(dist);
        v[0] /= dist;
        v[1] /= dist;
        v[2] /= dist;
    }

    return dist;
}

float OBJUnit(float vin[3], float vout[3])
{
    float dist;

    dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];

    if (dist > 0.0)
    {
        dist = sqrt(dist);
        vout[0] = vin[0] / dist;
        vout[1] = vin[1] / dist;
        vout[2] = vin[2] / dist;
    }
    else
    {
        vout[0] = vin[0];
        vout[1] = vin[1];
        vout[2] = vin[2];
    }

    return dist;
}

char *
ReadRestOfLine(FILE *fp)
{
    static char *line;
    std::vector<char> tmp(1000);
    tmp.clear();

    for (;;)
    {
        int c = getc(fp);

        if (c == EOF && tmp.size() == 0)
        {
            return NULL;
        }

        if (c == EOF || c == '\n')
        {
            delete[] line;
            line = new char[tmp.size() + 1];
            for (int i = 0; i < (int)tmp.size(); i++)
            {
                line[i] = tmp[i];
            }
            line[tmp.size()] = '\0'; // terminating null
            return line;
        }
        else
        {
            tmp.push_back(c);
        }
    }

    return (char *)"";
}

void ReadObjVTN(char *str, int *v, int *t, int *n)
{
    // can be one of v, v//n, v/t, v/t/n:

    if (strstr(str, "//")) // v//n
    {
        *t = 0;
        sscanf(str, "%d//%d", v, n);
        return;
    }
    else if (sscanf(str, "%d/%d/%d", v, t, n) == 3) // v/t/n
    {
        return;
    }
    else
    {
        *n = 0;
        if (sscanf(str, "%d/%d", v, t) == 2) // v/t
        {
            return;
        }
        else // v
        {
            *n = *t = 0;
            sscanf(str, "%d", v);
        }
    }
}
