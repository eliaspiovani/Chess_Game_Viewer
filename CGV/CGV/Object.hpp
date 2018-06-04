//
//  Object.hpp
//  tutorial07_model_loading
//
//  Created by Elias Piovani on 21/04/2018.
//
#pragma once

#ifndef Object_hpp
#define Object_hpp

#include <stdio.h>
#include <vector>
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// Include GLEW
#include <GL/glew.h>

struct movements{
    char initPos[3];
    char finalPos[3];
    bool doubleMove;
    char initPos2[3];
    char finalPos2[3];
};

class Object
{
private:
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> n;

public:
    //    char path[10];
    const char *pieceType;
    bool firstMove = true;
    std::vector<glm::vec3> v;
    GLuint vb;
    GLuint uvb;
    GLuint nb;
    const GLuint *texture;
    const GLuint *textureID;
    glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
    
    Object(){}
    ~Object();

    void load(const char *path, const char *piecetype, const GLuint &Texture, const GLuint &TextureID);

//    void del();

    void setPos(float x, float y, float z);
};

class ProjMatrix
{
public:
    glm::mat4 ProjectionMatrix;
    glm::mat4 ViewMatrix;
    glm::mat4 ModelMatrix;
    glm::mat4 MVP;
    GLuint MatrixID;
    GLuint ViewMatrixID;
    GLuint ModelMatrixID;
    GLuint LightID;

    void uniform(GLuint &MID, GLuint &VMID, GLuint &MMID);

    void setLightID(GLuint &lightID);
};

class BoardMatrix
{
private:
    Object *Matrix[8][8];
    bool movingPiece = false;
    int nBPiecesDead = 0, nWPiecesDead = 0;
public:
    float nSteps = 8.0f;

    void init(Object WhitePieces[16], Object BlackPieces[16]);

    void print();

    bool move(const char pieceStart[3], const char pieceEnd[3]);
    
    void find_positions(std::vector<std::string> &turns, int &turn, movements &moves);

};

void setInitialPos(Object *WPieces, Object *BPieces);

#endif /* Object_hpp */
