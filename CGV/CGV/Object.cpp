//
//  Object.cpp
//  tutorial07_model_loading
//
//  Created by Elias Piovani on 21/04/2018.
//

#include <iostream>
// Include GLEW
//#include <GL/glew.h>
//#include <vector>
// Include GLM
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//using namespace glm;
#include "objloader.hpp"
#include "controls.hpp"

#include "Object.hpp"

//#include <math.h>

Object::~Object(){
    // Cleanup VBO
    glDeleteBuffers(1, &uvb);
    glDeleteBuffers(1, &vb);
    glDeleteBuffers(1, &nb);
}

void Object::load(const char *path, const char *piecetype, const GLuint &Texture, const GLuint &TextureID)
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

//void Object::del()
//{
//    glDeleteBuffers(1, &uvb);
//    glDeleteBuffers(1, &vb);
//    glDeleteBuffers(1, &nb);
//}

void Object::setPos(float x, float y, float z)
{
    pos = glm::vec3(x, y, z);
    firstMove = false;
}

void ProjMatrix::uniform(GLuint &MID, GLuint &VMID, GLuint &MMID)
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

void ProjMatrix::setLightID(GLuint &lightID)
{
    LightID = lightID;
}

void BoardMatrix::init(Object WhitePieces[16], Object BlackPieces[16])
{
    // Init matrix with Null values
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

void BoardMatrix::print()
{
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if (Matrix[i][j] != NULL) printf("%s\t", Matrix[i][j]->pieceType);
            else printf("%p\t", Matrix[i][j]);
        }
        printf("\n");
    }
}

bool BoardMatrix::move(const char pieceStart[2], const char pieceEnd[2])
{
    int initPosX = 0, initPosZ = 0, finalPosX = 0, finalPosZ = 0;
    
    movingPiece = true;
    
    //    char startColumn = pieceStart[0];
    //    char startRow = pieceStart[1];
    //    char endColumn = pieceEnd[0];
    //    char endRow = pieceEnd[1];
    
    initPosZ = pieceStart[0] - 97;
    initPosX = pieceStart[1] - 49;
    finalPosZ = pieceEnd[0] - 97;
    finalPosX = pieceEnd[1] - 49;
    
    //    switch (startColumn) {
    //        case 'a':
    //            initPosZ = 0;
    //            break;
    //        case 'b':
    //            initPosZ = 1;
    //            break;
    //        case 'c':
    //            initPosZ = 2;
    //            break;
    //        case 'd':
    //            initPosZ = 3;
    //            break;
    //        case 'e':
    //            initPosZ = 4;
    //            break;
    //        case 'f':
    //            initPosZ = 5;
    //            break;
    //        case 'g':
    //            initPosZ = 6;
    //            break;
    //        case 'h':
    //            initPosZ = 7;
    //            break;
    //    }
    
    //    switch (startRow) {
    //        case '1':
    //            initPosX = 0;
    //            break;
    //        case '2':
    //            initPosX = 1;
    //            break;
    //        case '3':
    //            initPosX = 2;
    //            break;
    //        case '4':
    //            initPosX = 3;
    //            break;
    //        case '5':
    //            initPosX = 4;
    //            break;
    //        case '6':
    //            initPosX = 5;
    //            break;
    //        case '7':
    //            initPosX = 6;
    //            break;
    //        case '8':
    //            initPosX = 7;
    //            break;
    //
    //    }
    
    //    switch (endColumn) {
    //        case 'a':
    //            finalPosZ = 0;
    //            break;
    //        case 'b':
    //            finalPosZ = 1;
    //            break;
    //        case 'c':
    //            finalPosZ = 2;
    //            break;
    //        case 'd':
    //            finalPosZ = 3;
    //            break;
    //        case 'e':
    //            finalPosZ = 4;
    //            break;
    //        case 'f':
    //            finalPosZ = 5;
    //            break;
    //        case 'g':
    //            finalPosZ = 6;
    //            break;
    //        case 'h':
    //            finalPosZ = 7;
    //            break;
    //    }
    
    //    switch (endRow) {
    //        case '1':
    //            finalPosX = 0;
    //            break;
    //        case '2':
    //            finalPosX = 1;
    //            break;
    //        case '3':
    //            finalPosX = 2;
    //            break;
    //        case '4':
    //            finalPosX = 3;
    //            break;
    //        case '5':
    //            finalPosX = 4;
    //            break;
    //        case '6':
    //            finalPosX = 5;
    //            break;
    //        case '7':
    //            finalPosX = 6;
    //            break;
    //        case '8':
    //            finalPosX = 7;
    //            break;
    //    }
    
    //        float step = 0.15f;
    float speed = 20.0f;
    float stepX = float(finalPosX - initPosX)/speed;
    float stepZ = float(finalPosZ - initPosZ)/speed;
    
    //implementation - verify if contains s piece on pieceStart!
    if (Matrix[initPosX][initPosZ] == NULL) {
        return false;
    }
    
    float dist = sqrt(pow((finalPosX * 2.0f) - (Matrix[initPosX][initPosZ]->pos.x), 2.0) +
                      pow((finalPosZ * 2.0f) - (Matrix[initPosX][initPosZ]->pos.z), 2.0));
    
    float initDist = sqrt(pow((finalPosX * 2.0f) - (initPosX * 2.0f), 2.0) +
                          pow((finalPosZ * 2.0f) - (initPosZ * 2.0f), 2.0));
    
    if (dist > 0.0002) {
        Matrix[int(initPosX)][int(initPosZ)]->pos.x += stepX;
        // Only Knights can jump over other piece, so applies an arc movement to the Knights only
        if (Matrix[initPosX][initPosZ]->pieceType[1] == 'N') {
            float y = sin(((3.14159)/initDist) * (initDist - dist));
            Matrix[int(initPosX)][int(initPosZ)]->pos.y = y * 4.0;
        }
        Matrix[int(initPosX)][int(initPosZ)]->pos.z += stepZ;
    }
    else
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

void BoardMatrix::find_positions(std::vector<std::string> &plys, int &ply, movements &moves)
{
    float PI = 3.1415;
    
    char initPosColumn = '0';
    char initPosRow = '0';
    // 0 - white, 1 - black
    char color = (ply % 2) ? 'B' : 'W';
    // get type of piece
    char piece = plys[ply][0];
    // search for x in the movement
    //    if (turns[turn].find('x')) turns[turn].erase(1, 1);
    bool isXturn = false;
    std::size_t found = plys[ply].find("x");
    if (found!=std::string::npos)
    {
        isXturn = true;
        plys[ply].erase(found, 1);
    }
    //search for + and delete it
    found = plys[ply].find("+");
    if (found!=std::string::npos) plys[ply].erase(found, 1);
    // search for = in the movement
    //TODO
    
    //    initPosZ = pieceStart[0] - 97;
    //    initPosX = pieceStart[1] - 49;
    
    printf("%s %c\n", plys[ply].c_str(), color);
    
    bool first_found = false;
    switch (piece)
    {
        case 'R':
            // Calculate all Rook movements
            for (int R = 1; R < 8 && !first_found; R++) {
                for (int theta = 0; theta < 360; theta += 90) {
                    signed int x = (sin(theta * PI / 180.0)) >= 0 ? (int)(sin(theta * PI / 180.0) + 0.5) : (int)(sin(theta * PI / 180.0) - 0.5);
                    signed int y = (cos(theta * PI / 180.0)) >= 0 ? (int)(cos(theta * PI / 180.0) + 0.5) : (int)(cos(theta * PI / 180.0) - 0.5);
                    int a = plys[ply][1] + R * x;
                    int b = plys[ply][2] + R * y;
                    if (a >= 'a' && a <= 'h' && b >= '1' && b <= '8') {
                        //                        printf("%c %c\n", a, b);
                        if (Matrix[b - 49][a - 97] != NULL &&
                            Matrix[b - 49][a - 97]->pieceType[0] == color &&
                            Matrix[b - 49][a - 97]->pieceType[1] == piece)
                        {
                            //                            printf("%s", Matrix[b - 49][a - 97]->pieceType);
                            initPosColumn = a;
                            initPosRow = b;
                            first_found = true;
                        }
                    }
                }
            }
            break;
        case 'K':
            // Calculate all King movements
            for (int theta = 0; theta < 360; theta += 45) {
                signed int x = (sin(theta * PI / 180.0)) >= 0 ? (int)(sin(theta * PI / 180.0) + 0.5) : (int)(sin(theta * PI / 180.0) - 0.5);
                signed int y = (cos(theta * PI / 180.0)) >= 0 ? (int)(cos(theta * PI / 180.0) + 0.5) : (int)(cos(theta * PI / 180.0) - 0.5);
                int a = plys[ply][1] + x;
                int b = plys[ply][2] + y;
                if (a >= 'a' && a <= 'h' && b >= '1' && b <= '8') {
                    if (Matrix[b - 49][a - 97] != NULL &&
                        Matrix[b - 49][a - 97]->pieceType[0] == color &&
                        Matrix[b - 49][a - 97]->pieceType[1] == piece)
                    {
                        //                            printf("%s", Matrix[b - 49][a - 97]->pieceType);
                        initPosColumn = a;
                        initPosRow = b;
                    }
                }
            }
            break;
        case 'Q':
            // Calculate all Queen movements
            for (int R = 1; R < 8 && !first_found; R++) {
                for (int theta = 0; theta < 360; theta += 45) {
                    signed int x = (sin(theta * PI / 180.0)) >= 0 ? (int)(sin(theta * PI / 180.0) + 0.5) : (int)(sin(theta * PI / 180.0) - 0.5);
                    signed int y = (cos(theta * PI / 180.0)) >= 0 ? (int)(cos(theta * PI / 180.0) + 0.5) : (int)(cos(theta * PI / 180.0) - 0.5);
                    int a = plys[ply][1] + R * x;
                    int b = plys[ply][2] + R * y;
                    if (a >= 'a' && a <= 'h' && b >= '1' && b <= '8') {
                        if (Matrix[b - 49][a - 97] != NULL &&
                            Matrix[b - 49][a - 97]->pieceType[0] == color &&
                            Matrix[b - 49][a - 97]->pieceType[1] == piece)
                        {
                            //                            printf("%s", Matrix[b - 49][a - 97]->pieceType);
                            initPosColumn = a;
                            initPosRow = b;
                            first_found = true;
                        }
                    }
                }
            }
            break;
        case 'B':
            // Calculate all Bishop movements
            for (int R = 1; R < 8 && !first_found; R++) { // R = 1.414
                for (int theta = 45; theta < 360; theta += 90) {
                    signed int x = (sin(theta * PI / 180.0)) >= 0 ? (int)(sin(theta * PI / 180.0) + 0.5) : (int)(sin(theta * PI / 180.0) - 0.5);
                    signed int y = (cos(theta * PI / 180.0)) >= 0 ? (int)(cos(theta * PI / 180.0) + 0.5) : (int)(cos(theta * PI / 180.0) - 0.5);
                    int a = plys[ply][1] + R * x;
                    int b = plys[ply][2] + R * y;
                    if (a >= 'a' && a <= 'h' && b >= '1' && b <= '8') {
                        if (Matrix[b - 49][a - 97] != NULL &&
                            Matrix[b - 49][a - 97]->pieceType[0] == color &&
                            Matrix[b - 49][a - 97]->pieceType[1] == piece)
                        {
                            //                            printf("%s", Matrix[b - 49][a - 97]->pieceType);
                            initPosColumn = a;
                            initPosRow = b;
                            first_found = true;
                        }
                    }
                }
            }
            break;
        case 'N':
        {
            std::vector<int> a, b;
            if (plys[ply].size() > 3) {
                a = {plys[ply][1], plys[ply][1], plys[ply][1], plys[ply][1],
                    plys[ply][1], plys[ply][1], plys[ply][1], plys[ply][1]};
                b = {plys[ply][3] +2, plys[ply][3] -2, plys[ply][3] +1, plys[ply][3] -1,
                    plys[ply][3] +2, plys[ply][3] -2, plys[ply][3] +1, plys[ply][3] -1};
            } else
            {
                a = {plys[ply][1] +1, plys[ply][1] +1, plys[ply][1] +2, plys[ply][1] +2,
                    plys[ply][1] -1, plys[ply][1] -1, plys[ply][1] -2, plys[ply][1] -2};
                b = {plys[ply][2] +2, plys[ply][2] -2, plys[ply][2] +1, plys[ply][2] -1,
                    plys[ply][2] +2, plys[ply][2] -2, plys[ply][2] +1, plys[ply][2] -1};
            }
            
            
            for (int i = 0; i < 8 && !first_found; i++) {
                if (a[i] >= 'a' && a[i] <= 'h' && b[i] >= '1' && b[i] <= '8') {
                    if (Matrix[b[i] - 49][a[i] - 97] != NULL &&
                        Matrix[b[i] - 49][a[i] - 97]->pieceType[0] == color &&
                        Matrix[b[i] - 49][a[i] - 97]->pieceType[1] == piece)
                    {
                        initPosColumn = a[i];
                        initPosRow = b[i];
                        first_found = true;
                    }
                }
                    
            }
            
        }

            break;
        case 'O':
            
            break;
        default:
            std::vector <int> a, b;
            char initialColumn = ' ';
            char finalpos[3];
            bool twoPosib = false;
            
            if (plys[ply].size() > 2) {
                initialColumn = plys[ply][0];
                finalpos[0] = plys[ply][1];
                finalpos[1] = plys[ply][2];
                twoPosib = true;
            } else{
                finalpos[0] = plys[ply][0];
                finalpos[1] = plys[ply][1];
            }
            
            
            if (!isXturn)
            {
                a = {finalpos[0], finalpos[0]};
                if (color == 'B') {
                    b = {finalpos[1] +1, finalpos[1] +2};
                } else
                {
                    b = {finalpos[1] -1, finalpos[1] -2};
                }
                
            } else
            {
//                a = {finalpos[0] +1, finalpos[0] -1};
                if (!twoPosib) a = {finalpos[0] +1, finalpos[0] -1};
                else a = {initialColumn, initialColumn};
                if (color == 'B') {
                    b = {finalpos[1] +1, finalpos[1] +1};
                } else
                {
                    b = {finalpos[1] -1, finalpos[1] -1};
                }
            }
            
            for (int i = 0; i < 2; i++) {
                if (a[i] >= 'a' && a[i] <= 'h' && b[i] >= '1' && b[i] <= '8') {
                    if (Matrix[b[i] - 49][a[i] - 97] != NULL &&
                        Matrix[b[i] - 49][a[i] - 97]->pieceType[0] == color &&
                        Matrix[b[i] - 49][a[i] - 97]->pieceType[1] == 'P')
                    {
                        initPosColumn = a[i];
                        initPosRow = b[i];
                    }
                }
            }
            
            break;
    }
    
    printf("%c%c %c%c\n", initPosColumn, initPosRow,
           (plys[ply].size() > 2) ? plys[ply][1] : plys[ply][0], (plys[ply].size() > 2) ? plys[ply][2] : plys[ply][1]);
    
    moves.initPos[0] = initPosColumn; // save a
    moves.initPos[1] = initPosRow; // save b
    moves.finalPos[0] = (plys[ply].size() > 2) ? plys[ply][1] : plys[ply][0];    //save turns[turn][1]
    moves.finalPos[1] = (plys[ply].size() > 2) ? plys[ply][2] : plys[ply][1];    //save turns[turn][2]
    moves.doubleMove = false;
}


void setInitialPos(Object *WPieces, Object *BPieces)
{
    // Set positions for all Pawns
    // X - move forward - rows / Y - move up / Z - move to the side - columns
    for (int i = 0; i < 8; i++) {
        WPieces[i].setPos(2.0f, 0.0f, 2.0f*i);
    }
    // Set inital positions for all pieces
    WPieces[8].setPos(0.0f, 0.0f, 0.0f);
    WPieces[9].setPos(0.0f, 0.0f, 14.0f);
    WPieces[10].setPos(0.0f, 0.0f, 2.0f);
    WPieces[11].setPos(0.0f, 0.0f, 12.0f);
    WPieces[12].setPos(0.0f, 0.0f, 4.0f);
    WPieces[13].setPos(0.0f, 0.0f, 10.0f);
    WPieces[14].setPos(0.0f, 0.0f, 6.0f);
    WPieces[15].setPos(0.0f, 0.0f, 8.0f);
    
    // Set positions for all Pawns
    for (int i = 0; i < 8; i++) {
        BPieces[i].setPos(12.0f, 0.0f, 2.0f*i);
    }
    // Set positions for black pieces
    BPieces[8].setPos(14.0f, 0.0f, 0.0f);
    BPieces[9].setPos(14.0f, 0.0f, 14.0f);
    BPieces[10].setPos(14.0f, 0.0f, 2.0f);
    BPieces[11].setPos(14.0f, 0.0f, 12.0f);
    BPieces[12].setPos(14.0f, 0.0f, 4.0f);
    BPieces[13].setPos(14.0f, 0.0f, 10.0f);
    BPieces[14].setPos(14.0f, 0.0f, 6.0f);
    BPieces[15].setPos(14.0f, 0.0f, 8.0f);
}

