//
//  Object.cpp
//  tutorial07_model_loading
//
//  Created by Elias Piovani on 21/04/2018.
//

#include <iostream>
// Include GLEW
#include <GL/glew.h>
#include <vector>
// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;
#include "Object.hpp"
#include "objloader.hpp"
#include "controls.hpp"

#include <math.h>

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
    
    void load(const char *path, const char *piecetype, const GLuint &Texture, const GLuint &TextureID)
    {
        pieceType = piecetype;
        
        // implement if obj was loaded well
        bool wParts = loadOBJ(path, v, uvs, n);
        glGenBuffers(1, &vb);
        glBindBuffer(GL_ARRAY_BUFFER, vb);
        glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(glm::vec3), &v[0], GL_STATIC_DRAW);
        
        glGenBuffers(1, &uvb);
        glBindBuffer(GL_ARRAY_BUFFER, uvb);
        glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);
        
        glGenBuffers(1, &nb);
        glBindBuffer(GL_ARRAY_BUFFER, nb);
        glBufferData(GL_ARRAY_BUFFER, n.size() * sizeof(glm::vec3), &n[0], GL_STATIC_DRAW);
        
        texture = &Texture;
        textureID = &TextureID;
    }
    
    void del()
    {
        glDeleteBuffers(1, &uvb);
        glDeleteBuffers(1, &vb);
        glDeleteBuffers(1, &nb);
    }
    
//    bool fstMove()
//    {
//        return firstMove;
//    }
    
    void setPos(float x, float y, float z)
    {
        pos = glm::vec3(x, y, z);
        firstMove = false;
    }
    
//    glm::vec3 getPos()
//    {
//        return pos;
//    }
    
//    const char* getType()
//    {
//        return pieceType;
//    }
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
    
    void uniform(GLuint &MID, GLuint &VMID, GLuint &MMID)
    {
        ProjectionMatrix = getProjectionMatrix();
        ViewMatrix = getViewMatrix();
        ModelMatrix = glm::mat4(1.0);
        MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;
        
        MatrixID = MID;
        ViewMatrixID = VMID;
        ModelMatrixID = MMID;
        
        glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
        glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
        glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);
    }
    
    void setLightID(GLuint &lightID)
    {
        LightID = lightID;
    }
    
};

class BoardMatrix
{
private:
    Object *Matrix[8][8];
    bool movingPiece = false;
    int nBPiecesDead = 0, nWPiecesDead = 0;
public:
    
    
    void init(Object WhitePieces[16], Object BlackPieces[16])
    {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                Matrix[i][j] = NULL;
            }
        }
        // 0 to 7 - Pawn / 8 and 9 - Rook / 10 and 11 - Knight / 12 and 13 - Bishop / 14 - Queen / 15 - King
        for (int j = 0; j < 8; j++) {
            Matrix[1][j] = &WhitePieces[j];
            Matrix[6][j] = &BlackPieces[j];
        }
        Matrix[0][0] = &WhitePieces[8];
        Matrix[0][1] = &WhitePieces[10];
        Matrix[0][2] = &WhitePieces[12];
        Matrix[0][3] = &WhitePieces[14];
        Matrix[0][4] = &WhitePieces[15];
        Matrix[0][5] = &WhitePieces[13];
        Matrix[0][6] = &WhitePieces[11];
        Matrix[0][7] = &WhitePieces[9];
        
        Matrix[7][0] = &BlackPieces[8];
        Matrix[7][1] = &BlackPieces[10];
        Matrix[7][2] = &BlackPieces[12];
        Matrix[7][3] = &BlackPieces[14];
        Matrix[7][4] = &BlackPieces[15];
        Matrix[7][5] = &BlackPieces[13];
        Matrix[7][6] = &BlackPieces[11];
        Matrix[7][7] = &BlackPieces[9];
    }
    
    void print()
    {
        for (int i = 0; i < 8; i++) {
            for (int j = 0; j < 8; j++) {
                if (Matrix[i][j] != NULL) printf("%s\t", Matrix[i][j]->pieceType);
                else printf("%p\t", Matrix[i][j]);
            }
            printf("\n");
        }
    }
    
    bool move(const char pieceStart[3], const char pieceEnd[3])
    {
        int initPosX = 0, initPosZ = 0, finalPosX = 0, finalPosZ = 0;
        
        movingPiece = true;
        
        char startColumn = pieceStart[0];
        char startRow = pieceStart[1];
        char endColumn = pieceEnd[0];
        char endRow = pieceEnd[1];
        
        switch (startColumn) {
            case 'a':
                initPosZ = 0;
                break;
            case 'b':
                initPosZ = 1;
                break;
            case 'c':
                initPosZ = 2;
                break;
            case 'd':
                initPosZ = 3;
                break;
            case 'e':
                initPosZ = 4;
                break;
            case 'f':
                initPosZ = 5;
                break;
            case 'g':
                initPosZ = 6;
                break;
            case 'h':
                initPosZ = 7;
                break;
        }
        
        switch (startRow) {
            case '1':
                initPosX = 0;
                break;
            case '2':
                initPosX = 1;
                break;
            case '3':
                initPosX = 2;
                break;
            case '4':
                initPosX = 3;
                break;
            case '5':
                initPosX = 4;
                break;
            case '6':
                initPosX = 5;
                break;
            case '7':
                initPosX = 6;
                break;
            case '8':
                initPosX = 7;
                break;
                
        }
        
        switch (endColumn) {
            case 'a':
                finalPosZ = 0;
                break;
            case 'b':
                finalPosZ = 1;
                break;
            case 'c':
                finalPosZ = 2;
                break;
            case 'd':
                finalPosZ = 3;
                break;
            case 'e':
                finalPosZ = 4;
                break;
            case 'f':
                finalPosZ = 5;
                break;
            case 'g':
                finalPosZ = 6;
                break;
            case 'h':
                finalPosZ = 7;
                break;
        }
        
        switch (endRow) {
            case '1':
                finalPosX = 0;
                break;
            case '2':
                finalPosX = 1;
                break;
            case '3':
                finalPosX = 2;
                break;
            case '4':
                finalPosX = 3;
                break;
            case '5':
                finalPosX = 4;
                break;
            case '6':
                finalPosX = 5;
                break;
            case '7':
                finalPosX = 6;
                break;
            case '8':
                finalPosX = 7;
                break;
        }
        
//        float step = 0.15f;
        float speed = 20.0f;
        float stepX = float(finalPosX - initPosX)/speed;
        float stepZ = float(finalPosZ - initPosZ)/speed;
        
        //implementation - verify if contains s piece on pieceStart!
        if (Matrix[initPosX][initPosZ] == NULL) {
            return false;
        }
        
//        Matrix[initPosX][initPosZ]->pos.x = float(finalPosX * 2.0f);
//        Matrix[initPosX][initPosZ]->pos.z = float(finalPosZ * 2.0f);
        
//        if (Matrix[int(initPosX)][int(initPosZ)]->pos.x != finalPosX * 2.0f) {
//            if (Matrix[int(initPosX)][int(initPosZ)]->pos.x < finalPosX * 2.0f)
//            {
//                Matrix[int(initPosX)][int(initPosZ)]->pos.x += stepX;
//            }
//            else if (Matrix[int(initPosX)][int(initPosZ)]->pos.x > finalPosX * 2.0f) //+ stepZ)
//            {
//                Matrix[int(initPosX)][int(initPosZ)]->pos.x += stepX;
//            }
//        }

//        if (Matrix[int(initPosX)][int(initPosZ)]->pos.z != finalPosZ * 2.0f) {
 //           if (Matrix[int(initPosX)][int(initPosZ)]->pos.z < finalPosZ * 2.0f)
 //           {
//                Matrix[int(initPosX)][int(initPosZ)]->pos.z += stepZ;
//            }
//            else if (Matrix[int(initPosX)][int(initPosZ)]->pos.z > finalPosZ * 2.0f) // + stepZ)
//            {
//                Matrix[int(initPosX)][int(initPosZ)]->pos.z += stepZ;
//            }
//        }
    
//        float distX = (finalPosX * 2.0f) - Matrix[initPosX][initPosZ]->pos.x;
//        float distZ = (finalPosZ * 2.0f) - Matrix[initPosX][initPosZ]->pos.z;
        
        float dist = sqrt(pow((finalPosX * 2.0f) - (Matrix[initPosX][initPosZ]->pos.x), 2.0) +
                          pow((finalPosZ * 2.0f) - (Matrix[initPosX][initPosZ]->pos.z), 2.0));
        
        float initDist = sqrt(pow((finalPosX * 2.0f) - (initPosX * 2.0f), 2.0) +
                    pow((finalPosZ * 2.0f) - (initPosZ * 2.0f), 2.0));
        
        float y = sin(((3.14159)/initDist) * (initDist - dist));
        printf("%f\n", y);
        
        if (dist > 0.0002) {
            Matrix[int(initPosX)][int(initPosZ)]->pos.y = y * 4.0;
            Matrix[int(initPosX)][int(initPosZ)]->pos.x += stepX;
            Matrix[int(initPosX)][int(initPosZ)]->pos.z += stepZ;
        }
        else //Matrix[int(initPosX)][int(initPosZ)]->pos.y = 0.0;
        
        //if ((distX*distX < 0.0002f) && (distZ*distZ < 0.0002f))
        {
            Matrix[int(initPosX)][int(initPosZ)]->pos.y = 0.0;
            //update boardMatrix with new position of the piece
            // if piece killed another pice put it aside of the table
            if (Matrix[finalPosX][finalPosZ] != NULL) {
                // if piece is black piece goes to the right of the board, if is white goes to the left
                if (Matrix[finalPosX][finalPosZ]->pieceType[0] == 'B') {
                    Matrix[finalPosX][finalPosZ]->pos.x = -2.0f + nBPiecesDead; // -2.0f 18.0f
                    Matrix[finalPosX][finalPosZ]->pos.z = 18.0f;
                    nBPiecesDead++;
                }
                else
                {
                    Matrix[finalPosX][finalPosZ]->pos.x = 16.0f + nWPiecesDead;
                    Matrix[finalPosX][finalPosZ]->pos.z = -4.0f;
                    nBPiecesDead++;
                }
                
            }
            Matrix[finalPosX][finalPosZ] = Matrix[initPosX][initPosZ];
            Matrix[initPosX][initPosZ] = NULL;
            
            movingPiece = false;
        }
        
        return movingPiece;
    }
};












