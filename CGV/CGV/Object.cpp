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

void Object::load(const char *path, const char *piecetype)
{
    pieceType = piecetype;
    
//    glDeleteBuffers(1, &uvb);
//    glDeleteBuffers(1, &vb);
//    glDeleteBuffers(1, &nb);
    
    v.clear();
    uvs.clear();
    n.clear();
    
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
    // reset values to start new game
    nBPiecesDead = nWPiecesDead = 0;
    // end of game is false
    eog = false;
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

bool BoardMatrix::move(const char pieceStart[3], const char pieceEnd[3], const bool doubleMove)
{
    int initPosX = 0, initPosZ = 0, finalPosX = 0, finalPosZ = 0;
    moves_history moves_h {};
    
    movingPiece = true;
    
    Object *capt_piece = nullptr;
    
    initPosZ = pieceStart[0] - 97;
    initPosX = pieceStart[1] - 49;
    finalPosZ = pieceEnd[0] - 97;
    finalPosX = pieceEnd[1] - 49;
    
    //        float step = 0.15f;
    float stepX = float(finalPosX - initPosX)/nSteps;
    float stepZ = float(finalPosZ - initPosZ)/nSteps;
    
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
                capt_piece = Matrix[finalPosX][finalPosZ];
            }
            else
            {
                Matrix[finalPosX][finalPosZ]->pos.x = 16.0f - nWPiecesDead;
                Matrix[finalPosX][finalPosZ]->pos.z = -4.0f;
                nWPiecesDead++;
                capt_piece = Matrix[finalPosX][finalPosZ];
            }
            
        }

        
        Matrix[finalPosX][finalPosZ] = Matrix[initPosX][initPosZ];
        Matrix[initPosX][initPosZ] = NULL;
        
        if (promote && Matrix[finalPosX][finalPosZ]->pieceType[1] == 'P') {
            printf("Promote pawn\n");
            if (Matrix[finalPosX][finalPosZ]->pieceType[0] == 'B') Matrix[finalPosX][finalPosZ]->load("resources/CB_Queen.obj", "BQ");
            else Matrix[finalPosX][finalPosZ]->load("resources/CB_Queen.obj", "WQ");
            Matrix[finalPosX][finalPosZ]->promoted = true;
            moves_h.promoted = true;
            promote = false;
        }
        
        moves_h.initPos[0] = pieceStart[0];
        moves_h.initPos[1] = pieceStart[1];
        
        moves_h.finalPos[0] = pieceEnd[0];
        moves_h.finalPos[1] = pieceEnd[1];
        
        moves_h.piece = capt_piece;
        
        moves_h.doubleMove = doubleMove;
        
        m_his.push_back(moves_h);
        
        movingPiece = false;
    }
    
    return movingPiece;
}

bool BoardMatrix::move_back() {
    moves_history last = m_his.back(), doubleMove;
    
    if (m_his.size() > 2) {
        doubleMove = m_his.at(m_his.size() - 2);
    }
    
    static Object *temp;
    static bool captured = false;
    
    if (last.piece != nullptr) {
        last.piece->setPos((last.finalPos[1] - 49)*2, 0, (last.finalPos[0] - 97)*2);
        temp = last.piece;
        last.piece = nullptr;
        captured = true;
    }
    
    Object *ptr = Matrix[last.finalPos[1] - 49][last.finalPos[0] - 97];
    if (last.promoted && ptr->pieceType[1] != 'P') {
        printf("Demote pawn\n");
        if (ptr->pieceType[0] == 'B')
            ptr->load("resources/CB_Pawn.obj", "BP");
        else
            ptr->load("resources/CB_Pawn.obj", "WP");
        ptr->promoted = false;
        last.promoted = false;
    }
    
    bool end_move = move(last.finalPos, last.initPos, false);
    if (last.doubleMove) move(doubleMove.finalPos, doubleMove.initPos, false);
    
    // delete past movements, when moving back, the code saves it too, so we need to delete two instances
    if (!end_move) {
        if (captured) {
            if (temp->pieceType[0] == 'W') nWPiecesDead--;
            else nBPiecesDead--;
            Matrix[last.finalPos[1] - 49][last.finalPos[0] - 97] = temp;
        }
        
        captured = false;
//        print();
        printf("%s %s\n", last.finalPos, last.initPos);
        if (last.doubleMove){
            m_his.pop_back();
            m_his.pop_back();
        }
        m_his.pop_back();
        m_his.pop_back();
    }
    
    return end_move;
}

void BoardMatrix::find_positions(std::vector<std::string> &plies, int &ply, movements &moves)
{
    float PI = 3.1415;
    moves.doubleMove = false;
    char initPosColumn = '0';
    char initPosRow = '0';
    // 0 - white, 1 - black
    char color = (ply % 2) ? 'B' : 'W';
    // get type of piece
    char piece = plies[ply][0];
    // search for x in the movement
    //    if (turns[turn].find('x')) turns[turn].erase(1, 1);
    bool isXturn = false;
    std::size_t found = plies[ply].find("x");
    if (found!=std::string::npos)
    {
        isXturn = true;
        if (plies[ply][0] < 'a' || plies[ply][0] > 'h') plies[ply].erase(found, 1);
    }
    //search for + and delete it
    found = plies[ply].find("+");
    if (found!=std::string::npos) plies[ply].erase(found, 1);
    found = plies[ply].find("#");
    if (found!=std::string::npos) plies[ply].erase(found, 1);
    found = plies[ply].find("\r");
    if (found!=std::string::npos) plies[ply].erase(found, 1);
    
    // search for = in the movement
    found = plies[ply].find("=");
    if (found!=std::string::npos) {
        promote = true;
        promoteTo = plies[ply][3];
//        plies[ply].erase(found, 2);
    }
    
    //    initPosZ = pieceStart[0] - 97;
    //    initPosX = pieceStart[1] - 49;
    
    printf("%s %c\n", plies[ply].c_str(), color);
    
    bool first_found = false, double_column = false, double_row = false;
    int column, row;

    switch (piece)
    {
        case 'R':
            if (plies[ply].size() >= 4) {
                column = plies[ply][2];
                row = plies[ply][3];
                if (plies[ply][1] >= 'a' && plies[ply][1] <= 'h') double_column = true;
                else double_row = true;
            } else
            {
                column = plies[ply][1];
                row = plies[ply][2];
            }
            // Calculate all Rook movements
            
            for (int theta = 0; theta < 360 && !first_found; theta += 90) {
                for (int R = 1; R < 8; R++) {
                    signed int x = (sin(theta * PI / 180.0)) >= 0 ? (int)(sin(theta * PI / 180.0) + 0.5) : (int)(sin(theta * PI / 180.0) - 0.5);
                    signed int y = (cos(theta * PI / 180.0)) >= 0 ? (int)(cos(theta * PI / 180.0) + 0.5) : (int)(cos(theta * PI / 180.0) - 0.5);
                    int a = double_column ? plies[ply][1] : column + R * x;
                    int b = double_row ? plies[ply][1] : row + R * y;
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
                            break;
                        }else if (Matrix[b - 49][a - 97] != NULL && Matrix[b - 49][a - 97]->pieceType[1] != piece){
                            break;
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
                int a = plies[ply][1] + x;
                int b = plies[ply][2] + y;
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
            if (plies[ply].size() > 4) {
                column = plies[ply][2];
                row = plies[ply][3];
                if (plies[ply][1] >= 'a' && plies[ply][1] <= 'h') double_column = true;
                else double_row = true;
            } else
            {
                column = plies[ply][1];
                row = plies[ply][2];
            }
            // Calculate all Queen movements
            for (int theta = 0; theta < 360 && !first_found; theta += 45) {
                for (int R = 1; R < 8; R++) {
                    signed int x = (sin(theta * PI / 180.0)) >= 0 ? (int)(sin(theta * PI / 180.0) + 0.5) : (int)(sin(theta * PI / 180.0) - 0.5);
                    signed int y = (cos(theta * PI / 180.0)) >= 0 ? (int)(cos(theta * PI / 180.0) + 0.5) : (int)(cos(theta * PI / 180.0) - 0.5);
                    int a = double_column ? plies[ply][1] : column + R * x;
                    int b = double_row ? plies[ply][1] : row + R * y;
                    if (a >= 'a' && a <= 'h' && b >= '1' && b <= '8') {
                        if (Matrix[b - 49][a - 97] != NULL &&
                            Matrix[b - 49][a - 97]->pieceType[0] == color &&
                            Matrix[b - 49][a - 97]->pieceType[1] == piece)
                        {
                            //                            printf("%s", Matrix[b - 49][a - 97]->pieceType);
                            initPosColumn = a;
                            initPosRow = b;
                            first_found = true;
                            break;
                        }else if (Matrix[b - 49][a - 97] != NULL){
                            break;
                        }
                    }
                }
            }
            break;
        case 'B':
            if (plies[ply].size() > 4) {
                column = plies[ply][2];
                row = plies[ply][3];
                if (plies[ply][1] >= 'a' && plies[ply][1] <= 'h') double_column = true;
                else double_row = true;
            } else
            {
                column = plies[ply][1];
                row = plies[ply][2];
            }
            // Calculate all Bishop movements
            for (int theta = 45; theta < 360 && !first_found; theta += 90) {
                for (int R = 1; R < 8; R++) { // R = 1.414
                    signed int x = (sin(theta * PI / 180.0)) >= 0 ? (int)(sin(theta * PI / 180.0) + 0.5) : (int)(sin(theta * PI / 180.0) - 0.5);
                    signed int y = (cos(theta * PI / 180.0)) >= 0 ? (int)(cos(theta * PI / 180.0) + 0.5) : (int)(cos(theta * PI / 180.0) - 0.5);
                    int a = double_column ? plies[ply][1] : column + R * x;
                    int b = double_row ? plies[ply][1] : row + R * y;
                    if (a >= 'a' && a <= 'h' && b >= '1' && b <= '8') {
                        if (Matrix[b - 49][a - 97] != NULL &&
                            Matrix[b - 49][a - 97]->pieceType[0] == color &&
                            Matrix[b - 49][a - 97]->pieceType[1] == piece)
                        {
                            //                            printf("%s", Matrix[b - 49][a - 97]->pieceType);
                            initPosColumn = a;
                            initPosRow = b;
                            first_found = true;
                            break;
                        }else if (Matrix[b - 49][a - 97] != NULL){
                            break;
                        }
                    }
                }
            }
            break;
        case 'N':
        {
            std::vector<int> a, b;
            if (plies[ply].size() > 4) {
                if (plies[ply][1] >= 'a' && plies[ply][1] <= 'h') {
                    a = {plies[ply][1], plies[ply][1], plies[ply][1], plies[ply][1],
                        plies[ply][1], plies[ply][1], plies[ply][1], plies[ply][1]};
                    b = {plies[ply][3] +2, plies[ply][3] -2, plies[ply][3] +1, plies[ply][3] -1,
                        plies[ply][3] +2, plies[ply][3] -2, plies[ply][3] +1, plies[ply][3] -1};
                    double_column = true;
                } else {
                    b = {plies[ply][1], plies[ply][1], plies[ply][1], plies[ply][1],
                        plies[ply][1], plies[ply][1], plies[ply][1], plies[ply][1]};
                    a = {plies[ply][2] +2, plies[ply][2] -2, plies[ply][2] +1, plies[ply][2] -1,
                        plies[ply][2] +2, plies[ply][2] -2, plies[ply][2] +1, plies[ply][2] -1};
                    double_row = true;
                }
                
            } else
            {
                a = {plies[ply][1] +1, plies[ply][1] +1, plies[ply][1] +2, plies[ply][1] +2,
                    plies[ply][1] -1, plies[ply][1] -1, plies[ply][1] -2, plies[ply][1] -2};
                b = {plies[ply][2] +2, plies[ply][2] -2, plies[ply][2] +1, plies[ply][2] -1,
                    plies[ply][2] +2, plies[ply][2] -2, plies[ply][2] +1, plies[ply][2] -1};
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
                        break;
                    }
                }
                    
            }
            
        }

            break;
        case 'O':
            
            moves.doubleMove = true;
            if (plies[ply].size() < 4) {
                if (color == 'W') {
                    initPosColumn = 'e';
                    initPosRow = '1';
                    strcpy(moves.finalPos, "g1");
                    strcpy(moves.initPos2, "h1");
                    strcpy(moves.finalPos2, "f1");
                } else {
                    initPosColumn = 'e';
                    initPosRow = '8';
                    strcpy(moves.finalPos, "g8");
                    strcpy(moves.initPos2, "h8");
                    strcpy(moves.finalPos2, "f8");
                }
            } else {
                if (color == 'W') {
                    initPosColumn = 'e';
                    initPosRow = '1';
                    strcpy(moves.finalPos, "c1");
                    strcpy(moves.initPos2, "a1");
                    strcpy(moves.finalPos2, "d1");
                } else {
                    initPosColumn = 'e';
                    initPosRow = '8';
                    strcpy(moves.finalPos, "c8");
                    strcpy(moves.initPos2, "a8");
                    strcpy(moves.finalPos2, "d8");
                }
            }
                
            break;
            
//        case '1':
//            printf("End of game");
//            eog = true;
//            break;
//
//        case '0':
//            printf("End of game");
//            eog = true;
//            break;
            
        default:
            std::vector <int> a, b;
            char initialColumn = ' ';
            char finalpos[2];
            bool twoPosib = false;
            
            if (plies[ply].size() > 2 && !promote) {
                initialColumn = plies[ply][0];
                finalpos[0] = plies[ply][2];
                finalpos[1] = plies[ply][3];
                twoPosib = true;
                double_column = true;
            } else{
                finalpos[0] = plies[ply][0];
                finalpos[1] = plies[ply][1];
            }
            
            
            if (!isXturn)
            {
                a = {finalpos[0], finalpos[0]};
                if (color == 'B') b = {finalpos[1] +1, finalpos[1] +2};
                else b = {finalpos[1] -1, finalpos[1] -2};
                
            } else
            {
//                a = {finalpos[0] +1, finalpos[0] -1};
                if (!twoPosib) a = {finalpos[0] +1, finalpos[0] -1};
                else a = {initialColumn, initialColumn};
                if (color == 'B') b = {finalpos[1] +1, finalpos[1] +1};
                else b = {finalpos[1] -1, finalpos[1] -1};
            }
            
            for (int i = 0; i < 2; i++) {
                if (a[i] >= 'a' && a[i] <= 'h' && b[i] >= '1' && b[i] <= '8') {
                    if (Matrix[b[i] - 49][a[i] - 97] != NULL &&
                        Matrix[b[i] - 49][a[i] - 97]->pieceType[0] == color &&
                        Matrix[b[i] - 49][a[i] - 97]->pieceType[1] == 'P')
                    {
                        initPosColumn = a[i];
                        initPosRow = b[i];
                        break;
                    }
                }
            }
            
            if ((initPosColumn < 'a' || initPosColumn > 'h') || (initPosRow < '1' || initPosRow > '8')) {
                printf("Error in the calculations");
                throw "Error";
            }
            
            break;
    }
    
    
    moves.initPos[0] = initPosColumn; // save a
    moves.initPos[1] = initPosRow; // save b
    if (!moves.doubleMove && !double_column && !double_row && !promote) {
        moves.finalPos[0] = (plies[ply].size() > 2) ? plies[ply][1] : plies[ply][0];    //save turns[turn][1]
        moves.finalPos[1] = (plies[ply].size() > 2) ? plies[ply][2] : plies[ply][1];    //save turns[turn][2]
    } else if ((double_column || double_row) && !promote)
    {
        moves.finalPos[0] = plies[ply][2];    //save turns[turn][1]
        moves.finalPos[1] = plies[ply][3];    //save turns[turn][2]
    } else if (promote)
    {
        moves.finalPos[0] = plies[ply][0];    //save turns[turn][1]
        moves.finalPos[1] = plies[ply][1];    //save turns[turn][2]
    }
    
    printf("init %c%c final %c%c\n", moves.initPos[0], moves.initPos[1], moves.finalPos[0], moves.finalPos[1]);
    
    
//    if (ply >= plys.size()) {
//        printf("End of game");
//        eog = true;
//    }
    
}

void setInitialPos(Object *WPieces, Object *BPieces)
{
    // Set positions for all Pawns
    // X - move forward - rows / Y - move up / Z - move to the side - columns
    for (int i = 0; i < 8; i++) {
        if (WPieces[i].promoted) WPieces[i].load("resources/CB_Pawn.obj", "WP");
        WPieces[i].setPos(2.0f, 0.0f, 2.0f*i);
        WPieces[i].promoted = false;
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
        if (BPieces[i].promoted) BPieces[i].load("resources/CB_Pawn.obj", "BP");
        BPieces[i].setPos(12.0f, 0.0f, 2.0f*i);
        BPieces[i].promoted = false;
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

