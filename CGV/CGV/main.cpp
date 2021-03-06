
// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#ifdef _MSC_VER
#include "dirent.h"
#else
#include <dirent.h>
#endif

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;
int width = 1024, height = 768;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

// Include IMGUI

#include <imgui/imgui.h>
#include "imgui_impl_glfw_gl3.h"

#include "shader.hpp"
#include "texture.hpp"
#include "controls.hpp"
#include "objloader.hpp"
#include "Object.hpp"
#include "read_pgn.hpp"

void drawOBJ(Object &obj, ProjMatrix &PM)
{
    //    if (dynamic)
    //    {
    PM.MVP = PM.ProjectionMatrix * PM.ViewMatrix * glm::translate(glm::mat4(), obj.pos);
    glUniformMatrix4fv(PM.MatrixID, 1, GL_FALSE, &PM.MVP[0][0]);
    
    glm::vec3 lightPos = glm::vec3(7 - obj.pos.x,12 - obj.pos.y,7 - obj.pos.z);
    glUniform3f(PM.LightID, lightPos.x, lightPos.y, lightPos.z);
    //    }
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, *obj.texture);
    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(*obj.textureID, 0);
    
    // 1rst attribute buffer : vertices
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, obj.vb);
    glVertexAttribPointer(
                          0,                  // attribute
                          3,                  // size
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );
    
    // 2nd attribute buffer : UVs
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, obj.uvb);
    glVertexAttribPointer(
                          1,                                // attribute
                          2,                                // size
                          GL_FLOAT,                         // type
                          GL_FALSE,                         // normalized?
                          0,                                // stride
                          (void*)0                          // array buffer offset
                          );
    
    // 3rd attribute buffer : normals
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, obj.nb);
    glVertexAttribPointer(
                          2,                                // attribute
                          3,                                // size
                          GL_FLOAT,                         // type
                          GL_FALSE,                         // normalized?
                          0,                                // stride
                          (void*)0                          // array buffer offset
                          );
    
    // Draw the triangle !
    glDrawArrays(GL_TRIANGLES, 0, GLsizei(obj.v.size()) );
    //glDrawElements(GL_VERTEX_ARRAY, vertices.size(), GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    
}

int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }
    
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    // Open a window and create its OpenGL context
    window = glfwCreateWindow( width, height, "Chess Game Viewer", NULL, NULL); //1024, 768
    
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }
    
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    //    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    // Don't hide cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, width/2, height/2);
    
    // Dark blue background
    glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
    
    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);
    
    // Cull triangles which normal is not towards the camera
    glEnable(GL_CULL_FACE);
    
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "./shaders/TransformVertexShader.vertexshader", "./shaders/TextureFragmentShader.fragmentshader" );
    
    // Get a handle for our "MVP" uniform
    GLuint MatrixID = glGetUniformLocation(programID, "MVP");
    GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
    GLuint ModelMatrixID = glGetUniformLocation(programID, "M");
    
    // Load the texture
    GLuint Texture_Black = loadBMP_custom("resources/DW.bmp");
    GLuint Texture_White = loadBMP_custom("resources/LW.bmp");
    
    // Get a handle for our "myTextureSampler" uniform
    GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");
    
    Object Board[2];
    Board[0].load("resources/CB_BParts.obj", "BD", Texture_Black, TextureID);
    Board[1].load("resources/CB_WParts.obj", "BD", Texture_White, TextureID);
    
    //Create an array for all white pieces
    // 0 to 7 - Pawn / 8 and 9 - Rook / 10 and 11 - Knight / 12 and 13 - Bishop / 14 - Queen / 15 - King
    Object WPieces[16], BPieces[16];
    
    //Load the obj file and copy the data for all Pawns
    WPieces[0].load("resources/CB_Pawn.obj", "WP", Texture_White, TextureID);
    WPieces[1] = WPieces[2] = WPieces[3] = WPieces[4] = WPieces[5] = WPieces[6] = WPieces[7] = WPieces[0];
    
    WPieces[8].load("resources/CB_Rook.obj", "WR", Texture_White, TextureID);
    WPieces[9] = WPieces[8];
    WPieces[10].load("resources/CB_Knight.obj", "WN", Texture_White, TextureID);
    WPieces[11] = WPieces[10];
    WPieces[12].load("resources/CB_Bishop.obj", "WB", Texture_White, TextureID);
    WPieces[13] = WPieces[12];
    WPieces[14].load("resources/CB_Queen.obj", "WQ", Texture_White, TextureID);
    WPieces[15].load("resources/CB_King.obj", "WK", Texture_White, TextureID);
    
    //Load the obj file and copy the data for all Pawns
    BPieces[0].load("resources/CB_Pawn.obj", "BP", Texture_Black, TextureID);
    BPieces[1] = BPieces[2] = BPieces[3] = BPieces[4] = BPieces[5] = BPieces[6] = BPieces[7] = BPieces[0];
    
    BPieces[8].load("resources/CB_Rook.obj", "BR", Texture_Black, TextureID);
    BPieces[9] = BPieces[8];
    BPieces[10].load("resources/CB_KnightB.obj", "BN", Texture_Black, TextureID);
    BPieces[11] = BPieces[10];
    BPieces[12].load("resources/CB_Bishop.obj", "BB", Texture_Black, TextureID);
    BPieces[13] = BPieces[12];
    BPieces[14].load("resources/CB_Queen.obj", "BQ", Texture_Black, TextureID);
    BPieces[15].load("resources/CB_King.obj", "BK", Texture_Black, TextureID);
    
    setInitialPos(WPieces, BPieces);
    
    BoardMatrix boardMatrix;
    boardMatrix.init(WPieces, BPieces);
    boardMatrix.print();
    
    // Setup ImGui binding
    ImGui::CreateContext();
    ImGui_ImplGlfwGL3_Init(window, true);
    // Setup style
    ImGui::StyleColorsLight();
    
    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
    GLfloat LigthIntensity = glGetUniformLocation(programID, "LightIntensity");
    glUniform1f(LigthIntensity, 80.0f);
    
    //    glm::vec3 lightPos = glm::vec3(7,15,7);
    //    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
    
    static bool menuOpen = false, menuDebug = false;
    static bool mouseControl = false;
    static int mouseButton = GLFW_MOUSE_BUTTON_MIDDLE;
    static int oldState = GLFW_RELEASE;
    bool movingPiece = false, movingPieceBack = false, moveToEnd = false, playVideo = false;
    double startTime = 0;
    
    // String of all the headers contained in the file
    std::string header = "PGN not Loaded";
    //vector that contains all turns of the load game
    std::vector<std::string> turns;
    // integer to control the actual turn that is being played
    int turn = 0;
    // struct that contains initpos, finalpos, if is O-O or O-O-O and the initpos2, finalpos2 for the king and tower movement
    movements moves;
//    turns.push_back("ss");
//    printf("\n%c\n", turns[0][0]);
    do{
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // update window size
        glfwGetWindowSize(window, &width, &height);
        glViewport(0, 0, width, height);
        
        // prepare frame for ImGui
        ImGui_ImplGlfwGL3_NewFrame();
        
        // Use our shader
        glUseProgram(programID);
        
        glm::vec3 camPos;
        //        char buffer[10];
        // Compute the MVP matrix from keyboard and mouse input
        camPos = computeMatricesFromInputs(mouseButton);
        
        ProjMatrix PM;
        PM.setLightID(LightID);
        PM.uniform(MatrixID, ViewMatrixID, ModelMatrixID);
        
        //Fixed parts - Chess Board
        for (int i = 0; i < 2; i++) {
            drawOBJ(Board[i], PM);
        }
        
        // Draw all the pieces
        for (int i = 0; i < 16; i++) {
            drawOBJ(WPieces[i], PM);
            drawOBJ(BPieces[i], PM);
        }
        
        // when right arrow is pressed the software loads the next turn and set the flag movingPiece=True indicating a new movement
        // oldState enables the function only when the key is pressed, not while it is pressed
        int nextStep = glfwGetKey( window, GLFW_KEY_RIGHT );
        if (nextStep == GLFW_PRESS && oldState == GLFW_RELEASE && turns.size() > 0 && turn < turns.size() && !movingPiece && !movingPieceBack){
            boardMatrix.find_positions(turns, turn, moves);
            movingPiece = true;
            oldState = GLFW_PRESS;
        }
        
        // left arrow play previous move
        int previousStep = glfwGetKey( window, GLFW_KEY_LEFT );
        if (previousStep == GLFW_PRESS && oldState == GLFW_RELEASE && !movingPieceBack && !movingPiece && turn > 0){
            movingPieceBack = true;
            oldState = GLFW_PRESS;
        }
        
        // key add '+' increase the speed of the movement
        int speedUp = glfwGetKey( window, GLFW_KEY_KP_ADD);
        if (speedUp == GLFW_PRESS && oldState == GLFW_RELEASE && !movingPieceBack && !movingPiece){
            boardMatrix.nSteps--;
            if (boardMatrix.nSteps <= 0) boardMatrix.nSteps = 1;
            printf("SpeedUp %f", boardMatrix.nSteps);
            oldState = GLFW_PRESS;
        }
        // '-' decrease it, both by changing the value of nSteps
        int speedDown = glfwGetKey( window, GLFW_KEY_KP_SUBTRACT);
        if (speedDown == GLFW_PRESS && oldState == GLFW_RELEASE && !movingPieceBack && !movingPiece){
            
            boardMatrix.nSteps++;
            if (boardMatrix.nSteps > 20) boardMatrix.nSteps = 20;
            printf("SpeedDown %f", boardMatrix.nSteps);
            oldState = GLFW_PRESS;
        }
        
        // when space is pressed, the application starts to play all moves automatically
        int space = glfwGetKey( window, GLFW_KEY_SPACE);
        if (space == GLFW_PRESS && oldState == GLFW_RELEASE && turns.size() > 0 && turn < turns.size()){
            playVideo = !playVideo;
            startTime = glfwGetTime();
            oldState = GLFW_PRESS;
        }
        
        // ctrl + o , open the menu open PGN
        int leftCTRL = glfwGetKey( window, GLFW_KEY_LEFT_CONTROL);
        int key_O = glfwGetKey( window, GLFW_KEY_O);
        if (leftCTRL == GLFW_PRESS && key_O == GLFW_PRESS && oldState == GLFW_RELEASE){
            menuOpen = true;
            oldState = GLFW_PRESS;
        }
        
        // if no buttom is being pressed, reset olstate
        if (nextStep == GLFW_RELEASE &&
            previousStep == GLFW_RELEASE &&
            speedUp == GLFW_RELEASE &&
            speedDown == GLFW_RELEASE &&
            leftCTRL == GLFW_RELEASE &&
            key_O == GLFW_RELEASE &&
            space == GLFW_RELEASE &&
            oldState == GLFW_PRESS) oldState = GLFW_RELEASE;
        
        // if turn is bigger than the number of plies set bool of end of game
        if (turn >= turns.size() && !boardMatrix.eog && turns.size() > 0) {
            printf("End of the game");
            boardMatrix.eog = true;
            playVideo = false;
            // enable window showing the end of game
        }
        
        if (movingPiece && !boardMatrix.eog) {
            // if there is a movement (movingPiece=True) to apply this function is called
            // when the movement ends it update the variable (movingPiece=False)
            movingPiece = boardMatrix.move(moves.initPos, moves.finalPos, moves.doubleMove);
            if (moves.doubleMove) boardMatrix.move(moves.initPos2, moves.finalPos2, moves.doubleMove);
            if (!movingPiece)
            {
//                boardMatrix.print();
                turn++;
                printf("%d\n", turn);
            }
        }
        
        if (movingPieceBack) {
            movingPieceBack = boardMatrix.move_back();
            if (!movingPieceBack)
            {
//                boardMatrix.print();
                if (boardMatrix.eog) boardMatrix.eog = false;
                turn--;
                printf("%d\n", turn);
            }
        }
        
        if (moveToEnd) {
            while (turn < turns.size()) {
                boardMatrix.find_positions(turns, turn, moves);
                movingPiece = true;
                while (movingPiece) {
                    movingPiece = boardMatrix.move(moves.initPos, moves.finalPos, moves.doubleMove);
                    if (moves.doubleMove) boardMatrix.move(moves.initPos2, moves.finalPos2, moves.doubleMove);
                }
                turn++;
                printf("%d\n", turn);
            }
            moveToEnd = false;
        }
        
        if (playVideo) {
            if ((glfwGetTime() > startTime + 2.0) && turns.size() > 0 && turn < turns.size()&& !movingPiece && !movingPieceBack) {
//                printf("time");
                boardMatrix.find_positions(turns, turn, moves);
                movingPiece = true;
                startTime = glfwGetTime();
            }
        }
        
        // creates the menu bar
        {
            static bool b = false;
            
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("File"))
                {
                    if (ImGui::MenuItem("Open PGN", "Ctrl+O")) {
                        {
                            menuOpen = true;
                        }
                    }
                    if (ImGui::BeginMenu("Open Recent"))
                    {
                        ImGui::MenuItem("fish_hat.c");
                        ImGui::MenuItem("fish_hat.inl");
                        ImGui::MenuItem("fish_hat.h");
                        
                        ImGui::EndMenu();
                    }
                    if (ImGui::MenuItem("Quit", "Esc")) glfwSetWindowShouldClose(window, 1);
                    
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Options"))
                {
                    if (ImGui::BeginMenu("Texture"))
                    {
                        if (ImGui::MenuItem("Marble", NULL, b) && b == false){
                            Texture_Black = loadBMP_custom("resources/DM.bmp");
                            Texture_White = loadBMP_custom("resources/LM.bmp");
                            b = !b;
                        }
                        if (ImGui::MenuItem("Wood", NULL, !b) && b == true){
                            Texture_Black = loadBMP_custom("resources/DW.bmp");
                            Texture_White = loadBMP_custom("resources/LW.bmp");
                            b = !b;
                        }
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Lighting"))
                    {
                        static float f = 80.0f;
                        
                        //                        if (ImGui::MenuItem("Color", NULL, false)){
                        //
                        //                        }
                        if (ImGui::SliderFloat("Intensity", &f, 0.0f, 200.0f)){
                            glUniform1f(LigthIntensity, f);
                        }
                        ImGui::EndMenu();
                    }
                    if (ImGui::BeginMenu("Mouse"))
                    {
                        if (ImGui::MenuItem("Rotation Middle Button", NULL, !mouseControl) && mouseControl) {
                            mouseControl = !mouseControl;
                            mouseButton = GLFW_MOUSE_BUTTON_MIDDLE;
                        };
                        if (ImGui::MenuItem("Rotation Right Button", NULL, mouseControl) && !mouseControl) {
                            mouseControl = !mouseControl;
                            mouseButton = GLFW_MOUSE_BUTTON_RIGHT;
                        };
                        ImGui::EndMenu();
                    }
                    ImGui::EndMenu();
                    
                }
                if (ImGui::BeginMenu("Help"))
                {
                    if (ImGui::MenuItem("About")) {}
                    if (ImGui::MenuItem("Debug", NULL, menuDebug)) menuDebug = !menuDebug;
                    ImGui::EndMenu();
                }
                
                ImGui::EndMainMenuBar();
            }
        }
        
        // creates the bottom action buttons
        {
            ImGuiWindowFlags window_flags = 0;
            window_flags |= ImGuiWindowFlags_NoResize;
            window_flags |= ImGuiWindowFlags_NoScrollbar;
            window_flags |=ImGuiWindowFlags_NoTitleBar;
            window_flags |= ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoNav;
            ImGui::Begin("Buttons", 0, window_flags);
            ImGui::SetWindowPos(ImVec2 (width/2 - 330/2, height - 680/15));
            ImGui::SetWindowSize(ImVec2 (330, 35));
            if (ImGui::Button("   <<  "))
            {
                turn = 0;
                // reset board to the initial positions
                setInitialPos(WPieces, BPieces);
                boardMatrix.init(WPieces, BPieces);
            }
            ImGui::SameLine();
            if (ImGui::Button("   <   ") && !movingPieceBack && !movingPiece && turn > 0)
            {
                movingPieceBack = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("   >   ") && turns.size() > 0 && turn < turns.size() && !movingPiece && !movingPieceBack)
            {
                boardMatrix.find_positions(turns, turn, moves);
                movingPiece = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("   >>  ") && turns.size() > 0 && turn < turns.size())
            {
                moveToEnd = true;
            }
            ImGui::SameLine();
            if (ImGui::Button("   +   ") && turns.size() > 0 && turn < turns.size())
            {
                playVideo = !playVideo;
                startTime = glfwGetTime();
            }
            
            ImGui::End();
        }
        
        // debug window
        if (menuDebug) {
            ImGui::SetNextWindowBgAlpha(0.2);
            ImGui::Begin("Debug", NULL);
            ImGui::Text("Cam.X %.2f, Cam.Y %.2f, Cam.Z %.2f", camPos.x, camPos.y, camPos.z);
            ImGui::Text("Moving piece: %s", movingPiece ? "True":"False");
            ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }
        
        // End game window
        if (boardMatrix.eog) {
            ImGuiWindowFlags window_flags = 0;
            window_flags |= ImGuiWindowFlags_NoResize;
            window_flags |=ImGuiWindowFlags_NoTitleBar;
            ImGui::SetNextWindowBgAlpha(1.0);
            ImGui::SetNextWindowSize(ImVec2 (130,35));
            ImGui::SetNextWindowPos(ImVec2 (width/2 - 65, 30));
            ImGui::Begin("", NULL, window_flags);
            ImGui::Text("End of the game");
            ImGui::End();
        }
        
        // Game information
        if (true) {
            ImVec4 color = ImColor(255, 255, 255, 255);
            ImGui::SetNextWindowBgAlpha(0.0);
            ImGui::Begin("Game Information", NULL);
            ImGui::TextColored(color, "%s", header.c_str());
            ImGui::End();
        }
        
        // create a open file menu
        if (menuOpen) {
            ImGuiWindowFlags window_flags = 0;
            ImGui::SetNextWindowBgAlpha(1.0);
            window_flags |= ImGuiWindowFlags_NoResize;
            window_flags |=ImGuiWindowFlags_NoTitleBar;
            ImGui::Begin("Open File", NULL, window_flags);
            ImGui::SetWindowSize(ImVec2 (264, 264));
            
            ImGui::Text("Select a PGN file:");
            
            ImGui::BeginChild("child", ImVec2(0, 180), true);
            char file[1024];
            struct dirent *ent;
            
            // read folder and get all files .pgn inside that
            // read files in the directory PGN
            DIR *dir;
            if ((dir = opendir ("pgn")) != NULL) {
                // print all the files .pgn inside the menu item
                while ((ent = readdir (dir)) != NULL) {
                    
                    std::size_t is_pgn = std::string(ent->d_name).find(".pgn");
                    
                    if (is_pgn!=std::string::npos && ImGui::MenuItem(ent->d_name)) memcpy(file, ent->d_name, strlen(ent->d_name)+1);
                }
                closedir (dir);
            } else {
                // could not open directory
                perror ("Could not open directory pgn");
                return EXIT_FAILURE;
            }
            
            ImGui::EndChild();
            
            file[0] != '\0' ? ImGui::Text("Open: %s", file) : ImGui::Text("Open: ");
            
            // when cancel is pressed, just close the window
            if (ImGui::Button("Cancel"))
            {
                menuOpen = false;
            }
            ImGui::SameLine();
            // button open
            if (ImGui::Button("Open"))
            {
                char filePath[5] = "pgn/";
                strcat(filePath, file);
//                strcat(filePath, ".pgn");
                
                turns.clear();
                header = "\0";
                // open file defined by pgn/ file
                printf("%s\n", filePath);
                openPGN(filePath, header, turns);
                // reset board to the initial positions
                setInitialPos(WPieces, BPieces);
                boardMatrix.init(WPieces, BPieces);
                turn = 0;
                // insert it on the top of the recent list
                
                // verify if list is greatter than 5, if yes delete values
                
                // close menu
                menuOpen = false;
            }
            ImGui::End();
        }
        
        ImGui::Render();
        ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        
    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
          glfwWindowShouldClose(window) == 0 );
    
    // Cleanup shader
    glDeleteProgram(programID);
    glDeleteTextures(1, &Texture_Black);
    glDeleteTextures(1, &Texture_White);
    glDeleteVertexArrays(1, &VertexArrayID);
    
    // Close OpenGL window and terminate GLFW
    glfwTerminate();
    
    return 0;
}

