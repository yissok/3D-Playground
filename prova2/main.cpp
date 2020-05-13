//
//  main.cpp
//  prova2
//
//  Created by Andrea Lissak on 14/11/16.
//  Copyright © 2016 Andrea Lissak. All rights reserved.
//

#include <iostream>
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#ifdef _WIN32
#include <windows.h>
#endif
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdio.h>
#endif
#include <math.h>
#include <array>
#include <vector>

const int nFacesBun=9906;
const int nVerticesBun=5000;
const int nFacesScrew=10490;
const int nVerticesScrew=5247;

std::array<std::array<float, 3>,nVerticesScrew+1> normalsVerticesCube;

std::vector<std::array<float, 3>> vertices;
std::array<std::vector<float>,nVerticesScrew+1> facesOfVertices;
std::array<std::array<float, 3>,nFacesScrew+1> normalsFaces;
std::array<std::array<float, 3>,nVerticesScrew+1> normalsVertices;
std::vector<std::array<int, 3>> vertexIndices;

float scaleCoeff=0.4;
int bunnyOrScrew=0;
bool showc=false;
char rendermode;                            // Global variable for current rendering mode.
int i;                                      // GLOBAL COUNTER
int cubeRotator[3];
char activeButton[9];
float cameraPos[3]   = {5.0f, 5.0f,  5.0f};
float cameraFront[3]   = {-1.0f, -1.0f,  -1.0f};
float cameraUp[3]   = {0.0f, 1.0f,  0.0f};
float fakeCameraUp[3]   = {0.0f, 1.0f,  0.0f};
float ambient=1;
bool firstPos=true;
int lastX=0;
int lastY=0;
float yaw=0;
float pitch=0;
float maxx=0;
float maxy=0;
float maxz=0;
float minx=0;
float miny=0;
float minz=0;
int longestDimension=0;

int gameMode=0;
bool jumping=false;
float jumpingCounter=0;
float vinit=0;
const float gravity= 9.8;



// Very, VERY simple OBJ loader.
// Ignores everything except vertices and faces.
//
// Originally writen by Yongliang Yang using GLM,
// modified by Andrew Chinery to use Eigen, and
// modified by Christian Richardt to use plain C++11.
void transformVectorsInto3DObjects()
{
    FILE* file;
    if (bunnyOrScrew==0)
    {
        file=fopen("/Users/andrea/Documents/COSE/___UNI_ARCHIVE/projects/opengl/prova2/bunny.obj","r");
    }
    else if (bunnyOrScrew==1)
    {
        file=fopen("/Users/andrea/Documents/COSE/___UNI_ARCHIVE/projects/opengl/prova2/screwdriver.obj","r");
    }
    if (file == NULL)
    {
        printf("Could not open file. Is the path correct?\n");
        exit(0);
    }
    int i=0;
    int l=0;
    while (true)
    {
        char lineHeader[128];
        
        // Read the first word of the line.
        int res = fscanf(file, "%s", lineHeader);
        if (res == EOF) break;  // EOF = End Of File. Quit the loop.
        
        // Parse the line.
        if (strcmp(lineHeader, "v") == 0)
        {
            std::array<float, 3> vertex;
            fscanf(file, "%f %f %f\n", &vertex[0], &vertex[1], &vertex[2]);
            vertices.push_back(vertex);
            if (vertex[0]>maxx)
            {
                maxx=vertex[0];
                //std::cout << "\nmaxx"<<maxx;
            }
            if (vertex[1]>maxy)
            {
                maxy=vertex[1];
                //std::cout << "\nmaxx"<<maxy;
            }
            if (vertex[2]>maxz)
            {
                maxz=vertex[2];
                //std::cout << "\nmaxx"<<maxz;
            }
            if (vertex[0]<minx)
            {
                minx=vertex[0];
                //std::cout << "\nmaxx"<<minx;
            }
            if (vertex[1]<miny)
            {
                miny=vertex[1];
                //std::cout << "\nmaxx"<<miny;
            }
            if (vertex[2]<minz)
            {
                minz=vertex[2];
                //std::cout << "\nmaxx"<<minz;
            }
            
            if (10>l)
            {
                maxx=vertex[0];
                maxy=vertex[1];
                maxz=vertex[2];
                minx=vertex[0];
                miny=vertex[1];
                minz=vertex[2];
            }
            
            
        }
        else if (strcmp(lineHeader, "f") == 0)
        {
            std::array<int, 3> vertexIndex;
            fscanf(file, "%i %i %i\n", &vertexIndex[0], &vertexIndex[1], &vertexIndex[2]);
            vertexIndices.push_back(vertexIndex);
            
            //PUT FACES IN VERTEX
            facesOfVertices[vertexIndices[i][0]-1].push_back(i);
            facesOfVertices[vertexIndices[i][1]-1].push_back(i);
            facesOfVertices[vertexIndices[i][2]-1].push_back(i);
            
            
            i++;
        }
        else
        {
            // Probably a comment, eat up the rest of the line
            char ignored[1024];
            fgets(ignored, 1024, file);
        }
        l++;
    }
    
    for(int j=0;j<nVerticesScrew;j++)
    {
        //std::cout << "\n\ni) "<<j<<"\nlen: "<<(facesOfVertices[j].size());
        for(int i=0;i<facesOfVertices[j].size();i++)
        {
            //std::cout << "\ncontent: "<<(facesOfVertices[j][i]);
        }
    }

    
    
    if ((fabs(maxx-minx))>(fabs(maxy-miny)))
    {
        if ((fabs(maxx-minx))>(fabs(maxz-minz)))
        {
            longestDimension=1;
        }
        else
        {
            longestDimension=3;
        }
    }
    else
    {
        if ((fabs(maxy-miny))>(fabs(maxz-minz)))
        {
            longestDimension=2;
        }
        else
        {
            longestDimension=3;
        }
    }
    minx=maxx-minx;
    miny=maxy-miny;
    minz=maxy-minz;
    //printf("Done.\n");
    //std::cout << "\nI exist";
    std::cout << "\n\n\nmaxx"<<maxx;
    std::cout << "\nmaxy"<<maxy;
    std::cout << "\nmaxz"<<maxz;
    
    std::cout << "\n\nminx"<<minx;
    std::cout << "\nminy"<<miny;
    std::cout << "\nminz"<<minz;
}



// Scene initialisation.
void InitGL(GLvoid)
{
    glShadeModel(GL_SMOOTH);                // Enable smooth shading.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);   // Black background.
    glClearDepth(1.0f);                     // Depth buffer setup.
    glEnable(GL_DEPTH_TEST);                // Enables depth testing.
    glDepthFunc(GL_LEQUAL);                 // The type of depth testing to do.
    glEnable(GL_COLOR_MATERIAL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glDisable(GL_CULL_FACE);
    
    glEnable(GL_COLOR_MATERIAL);
    GLfloat mat_specular[] = { 1.0, 0.3, 0.0, 1.0 };
    GLfloat mat_shininess[] = { 70.0 };
    GLfloat light_position[] = { 0.0, 0.0, 2.0, 0.0 };
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel (GL_SMOOTH);
    
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    
    
    cubeRotator[0]=0;
    cubeRotator[1]=0;
    cubeRotator[2]=0;
    
    activeButton[0]='0';
    activeButton[1]='0';
    activeButton[2]='0';
    
    activeButton[3]='0';
    activeButton[4]='0';
    activeButton[5]='0';
    activeButton[6]='0';
    
    
    glClearColor (0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
 
}

void idle(void)
{
    glutPostRedisplay();  // Trigger display callback.
}

float* cross(float* a, float* b)
{
    //a2b3 - a3b2,     a2b0 - a0b2,     a0b1 - a1b0
    float result[3];
    
    result[0]=a[1]*b[2]-a[2]*b[1];
    result[1]=a[2]*b[0]-a[0]*b[2];
    result[2]=a[0]*b[1]-a[1]*b[0];
    
    return result;
}

float* norm(float* a)
{
    float result[3];
    float d=sqrt(a[1]*a[1]+a[0]*a[0]);
    d=sqrt(d*d+a[2]*a[2]);
    
    result[0]=a[0]/d;
    result[1]=a[1]/d;
    result[2]=a[2]/d;
    return result;
}

void cubeFace(float squareFace[8][3],int a,int b,int c,int d)
{
    glBegin(GL_QUADS);
    glColor3f(0.8f,1.0f,0.8f);
    glNormal3f(normalsVerticesCube[a][0], normalsVerticesCube[a][1], normalsVerticesCube[a][2]);
    glVertex3f(squareFace[a][0],    squareFace[a][1],   squareFace[a][2]);
    //glColor3f(1.0f,1.0f,0.0f);
    glNormal3f(normalsVerticesCube[b][0], normalsVerticesCube[b][1], normalsVerticesCube[b][2]);
    glVertex3f(squareFace[b][0],    squareFace[b][1],   squareFace[b][2]);
    //glColor3f(0.0f,1.0f,0.0f);
    glNormal3f(normalsVerticesCube[c][0], normalsVerticesCube[c][1], normalsVerticesCube[c][2]);
    glVertex3f(squareFace[c][0],    squareFace[c][1],   squareFace[c][2]);
    //glColor3f(0.0f,0.0f,1.0f);
    glNormal3f(normalsVerticesCube[d][0], normalsVerticesCube[d][1], normalsVerticesCube[d][2]);
    glVertex3f(squareFace[d][0],    squareFace[d][1],   squareFace[d][2]);
    glEnd();
}



void display(void)
{
    GLfloat light_position[] = { 0.0, 0.0, 2.0, 0.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    float coeff=0.05f;
    i++;
    gluLookAt(cameraPos[0],    cameraPos[1],   cameraPos[2],
              cameraPos[0]+cameraFront[0],    cameraPos[1]+cameraFront[1],   cameraPos[2]+cameraFront[2],
              cameraUp[0],    cameraUp[1],   cameraUp[2]);
    float a=1.0f;
    float squareFace[8][3]={{-a,a,a},{-a,-a,a},{-a,-a,-a},{-a,a,-a},{a,a,a},{a,-a,a},{a,-a,-a},{a,a,-a}};
    float squareFace2[4][3]={{a,a,a},{a,-a,a},{a,-a,-a},{a,a,-a}};//opposote to above
    glRotatef(cubeRotator[0],0.0f,1.0f,0.0f);
    glRotatef(cubeRotator[1],1.0f,0.0f,0.0f);
    glRotatef(cubeRotator[2],0.0f,0.0f,1.0f);
    
    
    int nface;
    int nvert;
    if (bunnyOrScrew==0)
    {
        nface=nFacesBun;
        nvert=nVerticesBun;
        scaleCoeff=0.4;
    }
    else
    {
        nface=nFacesScrew;
        nvert=nVerticesScrew;
        scaleCoeff=1.4;
    }
    switch(rendermode)
    {
        case 'f': // FACE
        {
            if (showc)
            {
                glDisable(GL_TEXTURE_2D);
                //glShadeModel(GL_SMOOTH);
                
                glShadeModel(GL_SMOOTH);
                
                float sumNormsA[3]={0,0,0};
                float d;
                
                
                //for (int i=0; i<3; i++)
                int i=0;
                {
                    glColor3f(squareFace[4][0]*ambient,    squareFace[4][1]*ambient,   squareFace[4][2]*ambient);
                    
                    glTexCoord2f(0.0, 0.0);
                    
                    for (int n=0; n<8; n++)
                    {
                        //std::cout << "\n";
                        for (int m=0; m<3; m++)
                        {
                            sumNormsA[m]=sumNormsA[m]+squareFace[n][m];
                            //std::cout << "\nsumNormsA["<<m<<"]: "<<sumNormsA[m];
                        }
                        d=sqrt(sumNormsA[1]*sumNormsA[1]+sumNormsA[0]*sumNormsA[0]);
                        d=sqrt(d*d+sumNormsA[2]*sumNormsA[2]);
                        
                        for (int k=0; k<3; k++)
                        {
                            sumNormsA[k]=sumNormsA[k]/d;
                            normalsVerticesCube[n][k]=sumNormsA[k];
                            
                            //std::cout << "\nnormalsVerticesCube["<<n<<"]["<<k<<"]: "<<normalsVerticesCube[n][k];
                        }
                        
                        sumNormsA[0]=0;
                        sumNormsA[1]=0;
                        sumNormsA[2]=0;
                        
                        
                        
                        //glColor3f(squareFace[4][1]*ambient,    squareFace[4][2]*ambient,   squareFace[4][0]*ambient);
                    }
                    
                    
                    
                    
                    //glNormal3f(normalsVerticesCube[n][0], normalsVerticesCube[n][1], normalsVerticesCube[n][2]);
                    
                    int a=0;
                    int b=1;
                    int c=2;
                    int d=3;
                    cubeFace(squareFace,a,b,c,d);
                    
                    a=0;
                    b=1;
                    c=5;
                    d=4;
                    cubeFace(squareFace,a,b,c,d);
                    
                    a=0;
                    b=4;
                    c=7;
                    d=3;
                    cubeFace(squareFace,a,b,c,d);
                    
                    a=6;
                    b=7;
                    c=4;
                    d=5;
                    cubeFace(squareFace,a,b,c,d);
                    
                    a=6;
                    b=7;
                    c=3;
                    d=2;
                    cubeFace(squareFace,a,b,c,d);
                    
                    
                    //                          0           1          2        3       4       5           6       7
                    //float squareFace[8][3]={{-a,a,a},{-a,-a,a},{-a,-a,-a},{-a,a,-a},{a,a,a},{a,-a,a},{a,-a,-a},{a,a,-a}};
                    
                    
                    a=6;
                    b=2;
                    c=1;
                    d=5;
                    cubeFace(squareFace,a,b,c,d);
                    
                }
                
                glDisable(GL_TEXTURE_2D);
                glShadeModel(GL_SMOOTH);
            }
            //glDisable(GL_TEXTURE_2D);showc
            if (bunnyOrScrew==2)
            {
                glEnable(GL_TEXTURE_2D);
                glShadeModel(GL_FLAT);
                
                
                for (int i=0; i<3; i++)
                {
                    glBegin(GL_QUADS);
                    glColor3f(squareFace[4][0]*ambient,    squareFace[4][1]*ambient,   squareFace[4][2]*ambient);
                    glTexCoord2f(0.0, 0.0);
                    glVertex3f(squareFace[0][i],    squareFace[0][(i+1)%3],   squareFace[0][(i+2)%3]);
                    glColor3f(squareFace[4][1]*ambient,    squareFace[4][2]*ambient,   squareFace[4][0]*ambient);
                    glTexCoord2f(1.0, 0.0);
                    glVertex3f(squareFace[1][i],    squareFace[1][(i+1)%3],   squareFace[1][(i+2)%3]);
                    glColor3f(1.0f*ambient, 1.0f*ambient, 0.0f*ambient);
                    glTexCoord2f(1.0, 1.0);
                    glVertex3f(squareFace[2][i],    squareFace[2][(i+1)%3],   squareFace[2][(i+2)%3]);
                    glColor3f(squareFace[4][2]*ambient,    squareFace[4][0]*ambient,   squareFace[4][1]*ambient);
                    glTexCoord2f(0.0, 1.0);
                    glVertex3f(squareFace[3][i],    squareFace[3][(i+1)%3],   squareFace[3][(i+2)%3]);
                    
                    glEnd();
                    
                    
                    
                    glBegin(GL_QUADS);
                    glColor3f(squareFace[4][0]*ambient,    squareFace[4][1]*ambient,   squareFace[4][2]*ambient);
                    glTexCoord2f(0.0, 0.0);
                    glVertex3f(squareFace2[0][i],    squareFace2[0][(i+1)%3],   squareFace2[0][(i+2)%3]);
                    glColor3f(squareFace[4][1]*ambient,    squareFace[4][2]*ambient,   squareFace[4][0]*ambient);
                    glTexCoord2f(1.0, 0.0);
                    glVertex3f(squareFace2[1][i],    squareFace2[1][(i+1)%3],   squareFace2[1][(i+2)%3]);
                    glColor3f(1.0f*ambient, 1.0f*ambient, 0.0f*ambient);
                    glTexCoord2f(1.0, 1.0);
                    glVertex3f(squareFace2[2][i],    squareFace2[2][(i+1)%3],   squareFace2[2][(i+2)%3]);
                    glColor3f(squareFace[4][2]*ambient,    squareFace[4][0]*ambient,   squareFace[4][1]*ambient);
                    glTexCoord2f(0.0, 1.0);
                    glVertex3f(squareFace2[3][i],    squareFace2[3][(i+1)%3],   squareFace2[3][(i+2)%3]);
                    glEnd();
                }
                
                glDisable(GL_TEXTURE_2D);
                glShadeModel(GL_SMOOTH);
            }
            
            if (bunnyOrScrew<2)
            {
                for(int j=0;j<nface;j++)
                {
                    
                    
                    //GENERATE 2 VECTORS FROM ONE OF 3 VERTICES: taking difference of 2 pairs of points (above)
                    
                    float V1[3]={vertices[vertexIndices[j][0]-1][0]-vertices[vertexIndices[j][1]-1][0],     vertices[vertexIndices[j][0]-1][1]-vertices[vertexIndices[j][1]-1][1],      vertices[vertexIndices[j][0]-1][2]-vertices[vertexIndices[j][1]-1][2]};
                    float V2[3]={vertices[vertexIndices[j][0]-1][0]-vertices[vertexIndices[j][2]-1][0],     vertices[vertexIndices[j][0]-1][1]-vertices[vertexIndices[j][2]-1][1],      vertices[vertexIndices[j][0]-1][2]-vertices[vertexIndices[j][2]-1][2]};
                    
                    float res[3];
                    
                    res[0]=V1[1]*V2[2]-V1[2]*V2[1];
                    res[1]=V1[2]*V2[0]-V1[0]*V2[2];
                    res[2]=V1[0]*V2[1]-V1[1]*V2[0];
                    
                    float normFace[3];
                    float d=sqrt(res[1]*res[1]+res[0]*res[0]);
                    d=sqrt(d*d+res[2]*res[2]);
                    
                    normFace[0]=res[0]/d;
                    normFace[1]=res[1]/d;
                    normFace[2]=res[2]/d;
                    normalsFaces[j][0]=normFace[0];
                    normalsFaces[j][1]=normFace[1];
                    normalsFaces[j][2]=normFace[2];
                }
                
                for(int i=0;i<nvert;i++)
                {
                    float sumNorms[3]={0,0,0};
                    for(int j=0;j<facesOfVertices[i].size();j++)
                    {
                        float angleBetween=0;
                        if((vertices[vertexIndices[facesOfVertices[i][j]][0]-1][0]==vertices[i][0])&(vertices[vertexIndices[facesOfVertices[i][j]][0]-1][1]==vertices[i][1])&(vertices[vertexIndices[facesOfVertices[i][j]][0]-1][2]==vertices[i][2]))
                        {
                            float v1x=vertices[vertexIndices[facesOfVertices[i][j]][1]-1][0]-vertices[vertexIndices[facesOfVertices[i][j]][0]-1][0];
                            float v1y=vertices[vertexIndices[facesOfVertices[i][j]][1]-1][1]-vertices[vertexIndices[facesOfVertices[i][j]][0]-1][1];
                            float v1z=vertices[vertexIndices[facesOfVertices[i][j]][1]-1][2]-vertices[vertexIndices[facesOfVertices[i][j]][0]-1][2];
                            
                            float v2x=vertices[vertexIndices[facesOfVertices[i][j]][2]-1][0]-vertices[vertexIndices[facesOfVertices[i][j]][0]-1][0];
                            float v2y=vertices[vertexIndices[facesOfVertices[i][j]][2]-1][1]-vertices[vertexIndices[facesOfVertices[i][j]][0]-1][1];
                            float v2z=vertices[vertexIndices[facesOfVertices[i][j]][2]-1][2]-vertices[vertexIndices[facesOfVertices[i][j]][0]-1][2];
                            
                            float d=sqrt(v1x*v1x+v1y*v1y);
                            d=sqrt(d*d+v1z*v1z);
                            v1x=v1x/d;
                            v1y=v1y/d;
                            v1z=v1z/d;
                            
                            float d2=sqrt(v2x*v2x+v2y*v2y);
                            d2=sqrt(d2*d2+v2z*v2z);
                            v2x=v2x/d2;
                            v2y=v2y/d2;
                            v2z=v2z/d2;
                            
                            angleBetween=acos(v1x*v2x+v1y*v2y+v1z*v2z);
                            
                            
                        }
                        
                        if((vertices[vertexIndices[facesOfVertices[i][j]][1]-1][0]==vertices[i][0])&(vertices[vertexIndices[facesOfVertices[i][j]][1]-1][1]==vertices[i][1])&(vertices[vertexIndices[facesOfVertices[i][j]][1]-1][2]==vertices[i][2]))
                        {
                            float v1x=vertices[vertexIndices[facesOfVertices[i][j]][0]-1][0]-vertices[vertexIndices[facesOfVertices[i][j]][1]-1][0];
                            float v1y=vertices[vertexIndices[facesOfVertices[i][j]][0]-1][1]-vertices[vertexIndices[facesOfVertices[i][j]][1]-1][1];
                            float v1z=vertices[vertexIndices[facesOfVertices[i][j]][0]-1][2]-vertices[vertexIndices[facesOfVertices[i][j]][1]-1][2];
                            
                            float v2x=vertices[vertexIndices[facesOfVertices[i][j]][2]-1][0]-vertices[vertexIndices[facesOfVertices[i][j]][1]-1][0];
                            float v2y=vertices[vertexIndices[facesOfVertices[i][j]][2]-1][1]-vertices[vertexIndices[facesOfVertices[i][j]][1]-1][1];
                            float v2z=vertices[vertexIndices[facesOfVertices[i][j]][2]-1][2]-vertices[vertexIndices[facesOfVertices[i][j]][1]-1][2];
                            
                            float d=sqrt(v1x*v1x+v1y*v1y);
                            d=sqrt(d*d+v1z*v1z);
                            v1x=v1x/d;
                            v1y=v1y/d;
                            v1z=v1z/d;
                            
                            float d2=sqrt(v2x*v2x+v2y*v2y);
                            d2=sqrt(d2*d2+v2z*v2z);
                            v2x=v2x/d2;
                            v2y=v2y/d2;
                            v2z=v2z/d2;
                            
                            angleBetween=acos(v1x*v2x+v1y*v2y+v1z*v2z);
                            
                        }
                        
                        if((vertices[vertexIndices[facesOfVertices[i][j]][2]-1][0]==vertices[i][0])&(vertices[vertexIndices[facesOfVertices[i][j]][2]-1][1]==vertices[i][1])&(vertices[vertexIndices[facesOfVertices[i][j]][2]-1][2]==vertices[i][2]))
                        {
                            float v1x=vertices[vertexIndices[facesOfVertices[i][j]][1]-1][0]-vertices[vertexIndices[facesOfVertices[i][j]][2]-1][0];
                            float v1y=vertices[vertexIndices[facesOfVertices[i][j]][1]-1][1]-vertices[vertexIndices[facesOfVertices[i][j]][2]-1][1];
                            float v1z=vertices[vertexIndices[facesOfVertices[i][j]][1]-1][2]-vertices[vertexIndices[facesOfVertices[i][j]][2]-1][2];
                            
                            float v2x=vertices[vertexIndices[facesOfVertices[i][j]][0]-1][0]-vertices[vertexIndices[facesOfVertices[i][j]][2]-1][0];
                            float v2y=vertices[vertexIndices[facesOfVertices[i][j]][0]-1][1]-vertices[vertexIndices[facesOfVertices[i][j]][2]-1][1];
                            float v2z=vertices[vertexIndices[facesOfVertices[i][j]][0]-1][2]-vertices[vertexIndices[facesOfVertices[i][j]][2]-1][2];
                            
                            float d=sqrt(v1x*v1x+v1y*v1y);
                            d=sqrt(d*d+v1z*v1z);
                            v1x=v1x/d;
                            v1y=v1y/d;
                            v1z=v1z/d;
                            
                            float d2=sqrt(v2x*v2x+v2y*v2y);
                            d2=sqrt(d2*d2+v2z*v2z);
                            v2x=v2x/d2;
                            v2y=v2y/d2;
                            v2z=v2z/d2;
                            
                            angleBetween=acos(v1x*v2x+v1y*v2y+v1z*v2z);
                            
                        }
                        
                        sumNorms[0]=sumNorms[0]+normalsFaces[facesOfVertices[i][j]][0]*angleBetween;
                        sumNorms[1]=sumNorms[1]+normalsFaces[facesOfVertices[i][j]][1]*angleBetween;
                        sumNorms[2]=sumNorms[2]+normalsFaces[facesOfVertices[i][j]][2]*angleBetween;
                    }
                    
                    float d=sqrt(sumNorms[1]*sumNorms[1]+sumNorms[0]*sumNorms[0]);
                    d=sqrt(d*d+sumNorms[2]*sumNorms[2]);
                    
                    sumNorms[0]=sumNorms[0]/d;
                    sumNorms[1]=sumNorms[1]/d;
                    sumNorms[2]=sumNorms[2]/d;
                    
                    normalsVertices[i][0]=sumNorms[0];
                    normalsVertices[i][1]=sumNorms[1];
                    normalsVertices[i][2]=sumNorms[2];
                    
                    float coeffNorm=10;
                    
                    //face norm
                    /*glBegin(GL_LINES);
                     glColor3f(1.0f, 1.0f, 0.0f);
                     glVertex3f(centersOfTriangles[i][0],centersOfTriangles[i][1],centersOfTriangles[i][2]);
                     glVertex3f(centersOfTriangles[i][0]+normalsFaces[i][0]/coeffNorm,centersOfTriangles[i][1]+normalsFaces[i][1]/coeffNorm,centersOfTriangles[i][2]+normalsFaces[i][2]/coeffNorm);
                     glEnd();
                     glLineWidth(5);
                     
                     glBegin(GL_POINTS);
                     glColor3f(1.0f, 0.0f, 0.0f);
                     glVertex3f(centersOfTriangles[i][0]+normalsFaces[i][0]/coeffNorm,centersOfTriangles[i][1]+normalsFaces[i][1]/coeffNorm,centersOfTriangles[i][2]+normalsFaces[i][2]/coeffNorm);
                     glEnd();
                     glPointSize(8);*/
                    
                    //vert norm
                    /*glBegin(GL_LINES);
                     glColor3f(1.0f, 1.0f, 0.0f);
                     glVertex3f(vertices[i][0],vertices[i][1],vertices[i][2]);
                     glVertex3f(vertices[i][0]+sumNorms[0]/coeffNorm,vertices[i][1]+sumNorms[1]/coeffNorm,vertices[i][2]+sumNorms[2]/coeffNorm);
                     glEnd();
                     glLineWidth(2);
                     
                     glBegin(GL_POINTS);
                     glColor3f(1.0f, 0.0f, 0.0f);
                     glVertex3f(vertices[i][0]+sumNorms[0]/coeffNorm,vertices[i][1]+sumNorms[1]/coeffNorm,vertices[i][2]+sumNorms[2]/coeffNorm);
                     glEnd();
                     glPointSize(3);*/
                }
                int k;
                std::array<std::array<float, 3>,nFacesScrew+1> centersOfTriangles;
                
                
                glBegin(GL_TRIANGLES);
                for(int j=0;j<nface;j++)
                {
                    /*int v1 = j % 7;
                     switch (v1) {
                     case 0:glColor3f(1.0f, 0.0f, 0.0f);
                     break;
                     case 1:glColor3f(0.0f, 1.0f, 0.0f);
                     break;za
                     case 2:glColor3f(0.0f, 0.0f, 1.0f);
                     break;
                     case 3:glColor3f(0.0f, 1.0f, 1.0f);
                     break;
                     case 4:glColor3f(1.0f, 0.0f, 1.0f);
                     break;
                     case 5:glColor3f(1.0f, 1.0f, 0.0f);
                     break;
                     case 6:glColor3f(1.0f, 1.0f, 1.0f);
                     break;
                     
                     default:
                     break;
                     }*/
                    glColor3f(139/256.0f,69/256.0f,19/256.0f);
                    //glColor3f(1.0f*ambient, 1.0f*ambient, 0.0f*ambient);
                    /*
                     
                     
                     
                     
                     glNormal3f(normalsVertices[vertexIndices[j][0]-1][0],normalsVertices[vertexIndices[j][0]-1][1],normalsVertices[vertexIndices[j][0]-1][2]);
                     glVertex3f((vertices[vertexIndices[j][0]-1][0]),(vertices[vertexIndices[j][0]-1][1]),(vertices[vertexIndices[j][0]-1][2]));
                     glNormal3f(normalsVertices[vertexIndices[j][1]-1][0],normalsVertices[vertexIndices[j][1]-1][1],normalsVertices[vertexIndices[j][1]-1][2]);
                     glVertex3f((vertices[vertexIndices[j][1]-1][0]),(vertices[vertexIndices[j][1]-1][1]),(vertices[vertexIndices[j][1]-1][2]));
                     glNormal3f(normalsVertices[vertexIndices[j][2]-1][0],normalsVertices[vertexIndices[j][2]-1][1],normalsVertices[vertexIndices[j][2]-1][2]);
                     glVertex3f((vertices[vertexIndices[j][2]-1][0]),(vertices[vertexIndices[j][2]-1][1]),(vertices[vertexIndices[j][2]-1][2]));*/
                    
                    
                    float reductionCoeff=0;
                    switch (longestDimension) {
                        case 1:
                            reductionCoeff=minx/2;
                            break;
                        case 2:
                            reductionCoeff=miny/2;
                            break;
                        case 3:
                            reductionCoeff=minz/2;
                            break;
                            
                        default:
                            break;
                    }
                    
                    float newMaxx=maxx;
                    float newMaxy=maxy;
                    float newMaxz=maxz;
                    
                    
                    
                    scaleCoeff=1/fabs(reductionCoeff);
                    //std::cout << "\nscaleCoeff"<<scaleCoeff;
                    
                    glNormal3f(normalsVertices[vertexIndices[j][0]-1][0],normalsVertices[vertexIndices[j][0]-1][1],normalsVertices[vertexIndices[j][0]-1][2]);
                    glVertex3f((vertices[vertexIndices[j][0]-1][0]-newMaxx)*scaleCoeff+1,(vertices[vertexIndices[j][0]-1][1]-newMaxy)*scaleCoeff+1,(vertices[vertexIndices[j][0]-1][2]-newMaxz)*scaleCoeff+1);
                    glNormal3f(normalsVertices[vertexIndices[j][1]-1][0],normalsVertices[vertexIndices[j][1]-1][1],normalsVertices[vertexIndices[j][1]-1][2]);
                    glVertex3f((vertices[vertexIndices[j][1]-1][0]-newMaxx)*scaleCoeff+1,(vertices[vertexIndices[j][1]-1][1]-newMaxy)*scaleCoeff+1,(vertices[vertexIndices[j][1]-1][2]-newMaxz)*scaleCoeff+1);
                    glNormal3f(normalsVertices[vertexIndices[j][2]-1][0],normalsVertices[vertexIndices[j][2]-1][1],normalsVertices[vertexIndices[j][2]-1][2]);
                    glVertex3f((vertices[vertexIndices[j][2]-1][0]-newMaxx)*scaleCoeff+1,(vertices[vertexIndices[j][2]-1][1]-newMaxy)*scaleCoeff+1,(vertices[vertexIndices[j][2]-1][2]-newMaxz)*scaleCoeff+1);
                    
                    
                    centersOfTriangles[j][0]=(vertices[vertexIndices[j][0]-1][0]+vertices[vertexIndices[j][1]-1][0]+vertices[vertexIndices[j][2]-1][0])/3;
                    centersOfTriangles[j][1]=(vertices[vertexIndices[j][0]-1][1]+vertices[vertexIndices[j][1]-1][1]+vertices[vertexIndices[j][2]-1][1])/3;
                    centersOfTriangles[j][2]=(vertices[vertexIndices[j][0]-1][2]+vertices[vertexIndices[j][1]-1][2]+vertices[vertexIndices[j][2]-1][2])/3;
                }
                glEnd();
            }
            
            
            break;
        }
            
        case 'v': //PT
        {
            
            
            glBegin(GL_POINTS);
            glColor3f(1.0f, 1.0f, 0.0f);
            for (int i=0; i<3; i++)
            {
                glVertex3f(squareFace[0][i],    squareFace[0][(i+1)%3],   squareFace[0][(i+2)%3]);
                glVertex3f(squareFace[1][i],    squareFace[1][(i+1)%3],   squareFace[1][(i+2)%3]);
                glVertex3f(squareFace[2][i],    squareFace[2][(i+1)%3],   squareFace[2][(i+2)%3]);
                glVertex3f(squareFace[3][i],    squareFace[3][(i+1)%3],   squareFace[3][(i+2)%3]);
                
                glVertex3f(squareFace2[0][i],    squareFace2[0][(i+1)%3],   squareFace2[0][(i+2)%3]);
                glVertex3f(squareFace2[1][i],    squareFace2[1][(i+1)%3],   squareFace2[1][(i+2)%3]);
                glVertex3f(squareFace2[2][i],    squareFace2[2][(i+1)%3],   squareFace2[2][(i+2)%3]);
                glVertex3f(squareFace2[3][i],    squareFace2[3][(i+1)%3],   squareFace2[3][(i+2)%3]);
            }
            glEnd();
            
            
            
            
            glBegin(GL_POINTS);
            glColor3f(1.0f,1.0f,1.0f);
            for(int j=0;j<nvert;j++)
            {
                glVertex3f((vertices[j][0]-maxx)*scaleCoeff,(vertices[j][1]-maxy)*scaleCoeff,(vertices[j][2]-maxz)*scaleCoeff);
            }
            glEnd();
            
            
            glPointSize(2);
            break;
        }
            
        case 'e': // LINES
        {
            glBegin(GL_LINES);
            glColor3f(1.0f, 1.0f, 1.0f);
            for (int i=0; i<3; i++)
            {
                glVertex3f(squareFace[0][i],    squareFace[0][(i+1)%3],   squareFace[0][(i+2)%3]);
                glVertex3f(squareFace[1][i],    squareFace[1][(i+1)%3],   squareFace[1][(i+2)%3]);
                glVertex3f(squareFace[2][i],    squareFace[2][(i+1)%3],   squareFace[2][(i+2)%3]);
                glVertex3f(squareFace[3][i],    squareFace[3][(i+1)%3],   squareFace[3][(i+2)%3]);
                
                glVertex3f(squareFace2[0][i],    squareFace2[0][(i+1)%3],   squareFace2[0][(i+2)%3]);
                glVertex3f(squareFace2[1][i],    squareFace2[1][(i+1)%3],   squareFace2[1][(i+2)%3]);
                glVertex3f(squareFace2[2][i],    squareFace2[2][(i+1)%3],   squareFace2[2][(i+2)%3]);
                glVertex3f(squareFace2[3][i],    squareFace2[3][(i+1)%3],   squareFace2[3][(i+2)%3]);
            }
            glEnd();
            
            glBegin(GL_LINES);
            glColor3f(1.0f, 1.0f, 1.0f);
            
            glBegin(GL_TRIANGLES);
            for(int j=0;j<nface;j++)
            {
                glVertex3f((vertices[vertexIndices[j][0]-1][0]-maxx)*scaleCoeff,(vertices[vertexIndices[j][0]-1][1]-maxy)*scaleCoeff,(vertices[vertexIndices[j][0]-1][2]-maxz)*scaleCoeff);
                glVertex3f((vertices[vertexIndices[j][1]-1][0]-maxx)*scaleCoeff,(vertices[vertexIndices[j][1]-1][1]-maxy)*scaleCoeff,(vertices[vertexIndices[j][1]-1][2]-maxz)*scaleCoeff);
                
                glVertex3f((vertices[vertexIndices[j][1]-1][0]-maxx)*scaleCoeff,(vertices[vertexIndices[j][1]-1][1]-maxy)*scaleCoeff,(vertices[vertexIndices[j][1]-1][2]-maxz)*scaleCoeff);
                glVertex3f((vertices[vertexIndices[j][2]-1][0]-maxx)*scaleCoeff,(vertices[vertexIndices[j][2]-1][1]-maxy)*scaleCoeff,(vertices[vertexIndices[j][2]-1][2]-maxz)*scaleCoeff);
                
                glVertex3f((vertices[vertexIndices[j][2]-1][0]-maxx)*scaleCoeff,(vertices[vertexIndices[j][2]-1][1]-maxy)*scaleCoeff,(vertices[vertexIndices[j][2]-1][2]-maxz)*scaleCoeff);
                glVertex3f((vertices[vertexIndices[j][0]-1][0]-maxx)*scaleCoeff,(vertices[vertexIndices[j][0]-1][1]-maxy)*scaleCoeff,(vertices[vertexIndices[j][0]-1][2]-maxz)*scaleCoeff);
                
                
            }
            glEnd();
            
            break;
        }
    }
    
    
    glRotatef(-cubeRotator[2],0.0f,0.0f,1.0f);
    glRotatef(-cubeRotator[1],1.0f,0.0f,0.0f);
    glRotatef(-cubeRotator[0],0.0f,1.0f,0.0f);
    
    {
        if(activeButton[0]=='m')
        {
            cubeRotator[0]+=2;
            if (cubeRotator[0]>=360)
            {
                cubeRotator[0]=0;
                cubeRotator[1]=0;
                cubeRotator[2]=0;
                activeButton[0]='0';
            }
        }
        if(activeButton[1]=='n')
        {
            cubeRotator[1]+=2;
            if (cubeRotator[1]>=360)
            {
                cubeRotator[0]=0;
                cubeRotator[1]=0;
                cubeRotator[2]=0;
                activeButton[1]='0';
            }
        }
        if(activeButton[2]=='b')
        {
            cubeRotator[2]+=2;
            if (cubeRotator[2]>=360)
            {
                cubeRotator[0]=0;
                cubeRotator[1]=0;
                cubeRotator[2]=0;
                activeButton[2]='0';
            }
        }
        
        
        
        if(activeButton[3]=='w')
        {
            if (gameMode==0)
            {
                cameraPos[0] += coeff * cameraFront[0];
                cameraPos[1] += coeff * cameraFront[1];
                cameraPos[2] += coeff * cameraFront[2];
            }
            else
            {
                cameraPos[0] += coeff * cameraFront[0];
                cameraPos[2] += coeff * cameraFront[2];
            }
        }
        if(activeButton[4]=='s')
        {
            if (gameMode==0)
            {
                cameraPos[0] -= coeff * cameraFront[0];
                cameraPos[1] -= coeff * cameraFront[1];
                cameraPos[2] -= coeff * cameraFront[2];
            }
            else
            {
                cameraPos[0] -= coeff * cameraFront[0];
                cameraPos[2] -= coeff * cameraFront[2];
            }
            
        }
        if(activeButton[5]=='a')
        {
            float* res=cross(cameraFront, cameraUp);
            cameraPos[0] -= res[0] * coeff;
            cameraPos[1] -= res[1] * coeff;
            cameraPos[2] -= res[2] * coeff;
        }
        if(activeButton[6]=='d')
        {
            float* res=cross(cameraFront, cameraUp);
            cameraPos[0] += res[0] * coeff;
            cameraPos[1] += res[1] * coeff;
            cameraPos[2] += res[2] * coeff;
        }
        if(activeButton[7]=='x')
        {
            float resultA[3];
            
            resultA[0]=cameraFront[1]*cameraUp[2]-cameraFront[2]*cameraUp[1];
            resultA[1]=cameraFront[2]*cameraUp[0]-cameraFront[0]*cameraUp[2];
            resultA[2]=cameraFront[0]*cameraUp[1]-cameraFront[1]*cameraUp[0];
            
            float resultB[3];
            
            resultB[0]=cameraFront[1]*resultA[2]-cameraFront[2]*resultA[1];
            resultB[1]=cameraFront[2]*resultA[0]-cameraFront[0]*resultA[2];
            resultB[2]=cameraFront[0]*resultA[1]-cameraFront[1]*resultA[0];
            
            float result[3];
            float d=sqrt(resultB[1]*resultB[1]+resultB[0]*resultB[0]);
            d=sqrt(d*d+resultB[2]*resultB[2]);
            
            resultB[0]=resultB[0]/d;
            resultB[1]=resultB[1]/d;
            resultB[2]=resultB[2]/d;
            
            cameraPos[0] -= coeff * resultB[0];//*(sin(pitch+56.5531))* cameraFront[0];
            cameraPos[1] -= coeff * resultB[1];//*cos(pitch+56.5531)* 0.5;
            cameraPos[2] -= coeff * resultB[2];//*(sin(pitch+56.5531))* cameraFront[2];
            
        }
        if(activeButton[8]=='z')
        {
            float resultA[3];
            
            resultA[0]=cameraFront[1]*cameraUp[2]-cameraFront[2]*cameraUp[1];
            resultA[1]=cameraFront[2]*cameraUp[0]-cameraFront[0]*cameraUp[2];
            resultA[2]=cameraFront[0]*cameraUp[1]-cameraFront[1]*cameraUp[0];
            
            float resultB[3];
            
            resultB[0]=cameraFront[1]*resultA[2]-cameraFront[2]*resultA[1];
            resultB[1]=cameraFront[2]*resultA[0]-cameraFront[0]*resultA[2];
            resultB[2]=cameraFront[0]*resultA[1]-cameraFront[1]*resultA[0];
            float result[3];
            float d=sqrt(resultB[1]*resultB[1]+resultB[0]*resultB[0]);
            d=sqrt(d*d+resultB[2]*resultB[2]);
            
            resultB[0]=resultB[0]/d;
            resultB[1]=resultB[1]/d;
            resultB[2]=resultB[2]/d;
            cameraPos[0] += coeff * resultB[0];//*(sin(pitch+56.5531))* cameraFront[0];
            cameraPos[1] += coeff * resultB[1];//*cos(pitch+56.5531)* 0.5;
            cameraPos[2] += coeff * resultB[2];
        }
        
        if (gameMode==1)
        {
            if (jumping)
            {
                jumpingCounter=jumpingCounter+0.03;
                cameraPos[1]=vinit*jumpingCounter-gravity/2*jumpingCounter*jumpingCounter-0.8;
                if (cameraPos[1]<-0.8)
                {
                    cameraPos[1]=-0.9;
                    jumping=false;
                }
            }
        }
        
        glBegin(GL_QUADS);
        /*glColor3f(0.0f, 0.0f, 1.0f);
         glVertex3f(-1.0f,2.5f,0.0f);
         glVertex3f(-1.0f,1.0f,0.0f);
         glVertex3f(-0.5f,1.0f,0.0f);
         glVertex3f(-0.5f,2.5f,0.0f);
         
         glColor3f(0.0f, 0.0f, 1.0f);
         glVertex3f(-0.5f,2.0f,0.0f);
         glVertex3f(-0.5f,2.5f,0.0f);
         glColor3f(0.0f, 1.0f, 0.0f);
         glVertex3f(0.5f,2.5f,0.0f);
         glVertex3f(0.5f,2.0f,0.0f);
         
         glColor3f(0.0f, 1.0f, 0.0f);
         glVertex3f(1.0f,2.5f,0.0f);
         glVertex3f(1.0f,1.0f,0.0f);
         glVertex3f(0.5f,1.0f,0.0f);
         glVertex3f(0.5f,2.5f,0.0f);*/
        
        
        
        
        
        glNormal3f(0,1,0);
        
        for (int i=0; i<20; i++)
        {
            for (int j=0; j<20; j++)
            {
                if ((i+j)%2==0)
                {
                    glColor3f(0.8f, 0.5f, 0.0f);
                }
                else
                {
                    glColor3f(0.4f, 0.2f, 0.0f);
                }
                glVertex3f(i+1-10,-1.0f,j+1-10);
                glVertex3f(i+1-10,-1.0f,j-10);
                glVertex3f(i-10,-1.0f,j-10);
                glVertex3f(i-10,-1.0f,j+1-10);
            }
        }
        
        glNormal3f(-1,0,0);
        glColor3f(1.0f, 0.0f, 0.0f);
        glVertex3f(10.0f,-1.0f,10.0f);
        glVertex3f(10.0f,-1.0f,-10.0f);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex3f(10.0f,1.0f,-10.0f);
        glVertex3f(10.0f,1.0f,10.0f);
        
        glNormal3f(0,0,-1);
        glColor3f(1.0f, 1.0f, 0.0f);
        glVertex3f(10.0f,-1.0f,10.0f);
        glVertex3f(-10.0f,-1.0f,10.0f);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex3f(-10.0f,1.0f,10.0f);
        glVertex3f(10.0f,1.0f,10.0f);
        
        glNormal3f(0,0,1);
        glColor3f(0.0f, 1.0f, 0.0f);
        glVertex3f(-10.0f,-1.0f,-10.0f);
        glVertex3f(10.0f,-1.0f,-10.0f);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex3f(10.0f,1.0f,-10.0f);
        glVertex3f(-10.0f,1.0f,-10.0f);
        
        glNormal3f(1,0,0);
        glColor3f(0.0f, 0.0f, 1.0f);
        glVertex3f(-10.0f,-1.0f,-10.0f);
        glVertex3f(-10.0f,-1.0f,10.0f);
        glColor3f(0.0f, 0.0f, 0.0f);
        glVertex3f(-10.0f,1.0f,10.0f);
        glVertex3f(-10.0f,1.0f,-10.0f);
        
        
        glEnd();
        
        glBegin(GL_LINES);
        glNormal3f(0,0,1);
        glColor3f(0.0f, 0.0f, 1.0f);
        
        glVertex3f(100.0f, 0.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        
        glColor3f(0.0f, 1.0f, 0.0f);
        
        glVertex3f(0.0f, 100.0f, 0.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        
        glColor3f(1.0f, 0.0f, 0.0f);
        
        glVertex3f(0.0f, 0.0f, 100.0f);
        glVertex3f(0.0f, 0.0f, 0.0f);
        
        
        glEnd();
        
        glutSwapBuffers();
    }
    
    
    
    
    
    
}

void reshape(int width, int height)
{
    if (height == 0){height = 1;}
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -3.6);
}

void keyboard(unsigned char key, int x, int y)// KEYBOARD
{
    float cameraSpeed = 0.35f;
    switch (key)
    {
        case 27:    exit(0);
            break;
            
        case 'v':   rendermode = 'v';
            break;
            
        case 'e':   rendermode = 'e';
            break;
            
        case 'f':   rendermode = 'f';
            break;
            
        case 'p':   bunnyOrScrew = 0;
            
            maxx=0;
            maxy=0;
            maxz=0;
            minx=0;
            miny=0;
            minz=0;
            vertices.clear();
            for (int i=0; i<nVerticesScrew+1; i++)
            {
                facesOfVertices[i].clear();
            }
            vertexIndices.clear();
            
            transformVectorsInto3DObjects();
            break;
            
        case 'o':   bunnyOrScrew = 1;
            maxx=0;
            maxy=0;
            maxz=0;
            minx=0;
            miny=0;
            minz=0;
            vertices.clear();
            for (int i=0; i<nVerticesScrew+1; i++)
            {
                facesOfVertices[i].clear();
            }
            vertexIndices.clear();
            transformVectorsInto3DObjects();
            break;
        case 'i':   //bunnyOrScrew = 2;
            if (showc)
            {
                showc=false;
            }
            else
            {
                showc=true;
            }
            break;
        case 'u':   bunnyOrScrew = 2;
            maxx=0;
            maxy=0;
            maxz=0;
            minx=0;
            miny=0;
            minz=0;
            vertices.clear();
            for (int i=0; i<nVerticesScrew+1; i++)
            {
                facesOfVertices[i].clear();
            }
            vertexIndices.clear();
            break;
            
            
        case 'w':
            activeButton[4]='0';
            activeButton[5]='0';
            activeButton[6]='0';
            activeButton[7]='0';
            activeButton[8]='0';
            if(activeButton[3]=='w')
            {
                activeButton[3]='0';
            }
            else
            {
                activeButton[3]='w';
            }
            
            
            
            break;
        case 's':   activeButton[3]='0';
            activeButton[5]='0';
            activeButton[6]='0';
            activeButton[7]='0';
            activeButton[8]='0';
            if(activeButton[4]=='s')
            {
                activeButton[4]='0';
            }
            else
            {
                activeButton[4]='s';
            }
            
            
            break;
        case 'a':   activeButton[3]='0';
            activeButton[4]='0';
            activeButton[6]='0';
            activeButton[7]='0';
            activeButton[8]='0';
            if(activeButton[5]=='a')
            {
                activeButton[5]='0';
            }
            else
            {
                activeButton[5]='a';
            }
            
            break;
        case 'd':   activeButton[3]='0';
            activeButton[4]='0';
            activeButton[5]='0';
            activeButton[7]='0';
            activeButton[8]='0';
            if(activeButton[6]=='d')
            {
                activeButton[6]='0';
            }
            else
            {
                activeButton[6]='d';
            }
            break;
        case 'x':   activeButton[3]='0';
            activeButton[4]='0';
            activeButton[5]='0';
            activeButton[6]='0';
            activeButton[8]='0';
            if(activeButton[7]=='x')
            {
                activeButton[7]='0';
            }
            else
            {
                activeButton[7]='x';
            }
            break;
        case 'z':   activeButton[3]='0';
            activeButton[4]='0';
            activeButton[5]='0';
            activeButton[6]='0';
            activeButton[7]='0';
            if(activeButton[8]=='z')
            {
                activeButton[8]='0';
            }
            else
            {
                activeButton[8]='z';
            }
            break;
            
            
            
            //CUBE ROTATION
        case 'm':
            cubeRotator[0]=0;
            cubeRotator[1]=0;
            cubeRotator[2]=0;
            activeButton[1]='0';
            activeButton[2]='0';
            if(activeButton[0]=='m')
            {
                activeButton[0]='0';
            }
            else
            {
                activeButton[0]='m';
            }
            break;
            
            
        case 'n':
            cubeRotator[0]=0;
            cubeRotator[1]=0;
            cubeRotator[2]=0;
            activeButton[0]='0';
            activeButton[2]='0';
            if(activeButton[1]=='n')
            {
                activeButton[1]='0';
            }
            else
            {
                activeButton[1]='n';
            }
            break;
            
            
        case 'b':
            cubeRotator[0]=0;
            cubeRotator[1]=0;
            cubeRotator[2]=0;
            activeButton[0]='0';
            activeButton[1]='0';
            if(activeButton[2]=='b')
            {
                activeButton[2]='0';
            }
            else
            {
                activeButton[2]='b';
            }
            break;
            
        case 'y':
            if (gameMode==0) {
                gameMode=1;
                cameraPos[1]=-0.9;
            }
            else
            {
                gameMode=0;
            }
            break;
        case ' ':
            if (gameMode==1)
            {
                if (!jumping)
                {
                    jumpingCounter=0;
                    vinit=10;
                    jumping=true;
                }
            }
            
            
            
            break;
            
        default:    break;
    }
    glutPostRedisplay();
}

void arrow_keys(int a_keys, int x, int y)// ARROWS
{
    switch (a_keys)
    {
        case GLUT_KEY_UP:   glutFullScreen();
            break;
            
        case GLUT_KEY_DOWN: glutReshapeWindow(500, 500);
            break;
            
        default:            break;
    }
}

void onMouse(int button, int state, int x, int y) {
    if (button == GLUT_RIGHT_BUTTON)// EXIT WHEN RIGHT
    {
        exit(0);
    }
    if (button == GLUT_LEFT_BUTTON)// EXIT WHEN RIGHT
    {
        if(firstPos)
        {
            lastX = x;
            lastY = y;
            firstPos = false;
            yaw=-90.32;
            pitch=-57.1751;//asin=0 is -56.5531 (1 rad)
        }
        lastX=x;
        lastY=y;
        
    }
}

void onMotion(int x, int y)
{
    float xoffset = -x + lastX;
    float yoffset = -lastY + y;
    lastX = x;
    lastY = y;
    
    float sensitivity = 0.002;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    yaw   += xoffset;
    pitch += yoffset;
    
    if(sin(pitch) < -0.95){pitch=-56.55-(M_PI/2)+0.3;}
    if(sin(pitch) > 0.95f){pitch=-56.55+(M_PI/2)-0.3;}
    
    cameraFront[0] = cos(yaw) * cos(pitch);
    cameraFront[1] = sin(pitch);
    cameraFront[2] = sin(yaw) * cos(pitch);
}

// Entry point to the application.
int main(int argc, char** argv)
{    glutInit(&argc, argv);
    //glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_MULTISAMPLE);
    glutInitWindowSize(500, 500);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutCreateWindow("OpenGL Playground");
    InitGL();
    rendermode = 'f';
    
    // CALLBACK
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(arrow_keys);
    glutMouseFunc(onMouse);
    glutMotionFunc(onMotion);
    glutIdleFunc(idle);
    transformVectorsInto3DObjects();
    
    glutMainLoop();
}
