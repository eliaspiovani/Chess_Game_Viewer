// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "shader.hpp"
#include "texture.hpp"
#include "controls.hpp"
#include "objloader.hpp"
#include "text2D.hpp"

#include "Object.cpp"

//void drawOBJ(std::vector<glm::vec3> &vertices, GLuint &Texture, GLuint &TextureID, GLuint &vertexbuffer, GLuint &uvbuffer, GLuint &normalbuffer)
void drawOBJ(Object &obj, ProjMatrix &PM, bool dynamic)
{
    if (dynamic)
    {
        PM.MVP = PM.ProjectionMatrix * PM.ViewMatrix * glm::translate(glm::mat4(), obj.pos);
        glUniformMatrix4fv(PM.MatrixID, 1, GL_FALSE, &PM.MVP[0][0]);
    }
    
    // Bind our texture in Texture Unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, obj.texture);
    // Set our "myTextureSampler" sampler to use Texture Unit 0
    glUniform1i(obj.textureID, 0);
    
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
    glDrawArrays(GL_TRIANGLES, 0, obj.v.size() );
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
	window = glfwCreateWindow( 1024, 768, "Chess Game Viewer", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

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
	GLuint programID = LoadShaders( "shaders/TransformVertexShader.vertexshader", "shaders/TextureFragmentShader.fragmentshader" );

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
    Board[0].load("resources/CB_BParts.obj", Texture_Black, TextureID);
    Board[1].load("resources/CB_WParts.obj", Texture_White, TextureID);
    
    //Create an array for all white pieces
    // 0 to 7 - Pawn / 8 and 9 - Rook / 10 and 11 - Knight / 12 and 13 - Bishop / 14 - Queen / 15 - King
    Object WPieces[16], BPieces[16];
    BoardMatrix boardMatrix;
    boardMatrix.init(WPieces, BPieces);
    boardMatrix.print();
    
    //Load the obj file and copy the data for all Pawns
    WPieces[0].load("resources/CB_Pawn.obj", Texture_White, TextureID);
    WPieces[1] = WPieces[2] = WPieces[3] = WPieces[4] = WPieces[5] = WPieces[6] = WPieces[7] = WPieces[0];
    
    // Set positions for all Pawns
    // X - move forward - rows / Y - move up / Z - move to the side - columns
    for (int i = 0; i < 8; i++) {
        WPieces[i].setPos(2.0f, 0.0f, 2.0f*i);
    }
    
    // Set positions for other pieces
    WPieces[8].load("resources/CB_Rook.obj", Texture_White, TextureID);
    WPieces[9] = WPieces[8];
    WPieces[8].setPos(0.0f, 0.0f, 0.0f);
    WPieces[9].setPos(0.0f, 0.0f, 14.0f);
    
    WPieces[10].load("resources/CB_Knight.obj", Texture_White, TextureID);
    WPieces[11] = WPieces[10];
    WPieces[10].setPos(0.0f, 0.0f, 2.0f);
    WPieces[11].setPos(0.0f, 0.0f, 12.0f);
    
    WPieces[12].load("resources/CB_Bishop.obj", Texture_White, TextureID);
    WPieces[13] = WPieces[12];
    WPieces[12].setPos(0.0f, 0.0f, 4.0f);
    WPieces[13].setPos(0.0f, 0.0f, 10.0f);
    
    WPieces[14].load("resources/CB_Queen.obj", Texture_White, TextureID);
    WPieces[15].load("resources/CB_King.obj", Texture_White, TextureID);
    WPieces[14].setPos(0.0f, 0.0f, 6.0f);
    WPieces[15].setPos(0.0f, 0.0f, 8.0f);
    
    //Load the obj file and copy the data for all Pawns
    BPieces[0].load("resources/CB_Pawn.obj", Texture_Black, TextureID);
    BPieces[1] = BPieces[2] = BPieces[3] = BPieces[4] = BPieces[5] = BPieces[6] = BPieces[7] = BPieces[0];
    
    // Set positions for all Pawns
    for (int i = 0; i < 8; i++) {
        BPieces[i].setPos(12.0f, 0.0f, 2.0f*i);
    }
    
    // Set positions for other pieces
    BPieces[8].load("resources/CB_Rook.obj", Texture_Black, TextureID);
    BPieces[9] = BPieces[8];
    BPieces[8].setPos(14.0f, 0.0f, 0.0f);
    BPieces[9].setPos(14.0f, 0.0f, 14.0f);
    
    BPieces[10].load("resources/CB_KnightB.obj", Texture_Black, TextureID);
    BPieces[11] = BPieces[10];
    BPieces[10].setPos(14.0f, 0.0f, 2.0f);
    BPieces[11].setPos(14.0f, 0.0f, 12.0f);
    
    BPieces[12].load("resources/CB_Bishop.obj", Texture_Black, TextureID);
    BPieces[13] = BPieces[12];
    BPieces[12].setPos(14.0f, 0.0f, 4.0f);
    BPieces[13].setPos(14.0f, 0.0f, 10.0f);
    
    BPieces[14].load("resources/CB_Queen.obj", Texture_Black, TextureID);
    BPieces[15].load("resources/CB_King.obj", Texture_Black, TextureID);
    BPieces[14].setPos(14.0f, 0.0f, 6.0f);
    BPieces[15].setPos(14.0f, 0.0f, 8.0f);
    
    // Get a handle for our "LightPosition" uniform
    glUseProgram(programID);
    GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");
    
    glm::vec3 lightPos = glm::vec3(0,15,0);
    glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);
    
    // Initialize our little text library with the Holstein font
    initText2D( "resources/Holstein.DDS" );
    
    static int oldState = GLFW_RELEASE;
    bool movingPiece = false;
    
	do{
		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // Use our shader
		glUseProgram(programID);
        
        glm::vec3 camPos;
        char buffer[10];
		// Compute the MVP matrix from keyboard and mouse input
        camPos = computeMatricesFromInputs();
        
        ProjMatrix PM;
        PM.uniform(MatrixID, ViewMatrixID, ModelMatrixID);
        
        //Fixed parts - Chess Board
        for (int i = 0; i < 2; i++) {
            drawOBJ(Board[i], PM, 0);
        }
        
        // Draw all the pieces
        for (int i = 0; i < 16; i++) {
            drawOBJ(WPieces[i], PM, 1);
            drawOBJ(BPieces[i], PM, 1);
        }
        
        // Move one piece to y=14
//        if (WPieces[2].pos.x < 10) {
//            WPieces[2].pos.x += 0.20f;
//        }
        
        // when Space is pressed the software loads the next turn and set the flag movingPiece=True indicating a new movement
        // oldState enables the function only when the key is pressed, not while it is pressed
        int nextStep = glfwGetKey( window, GLFW_KEY_SPACE );
        if (nextStep == GLFW_PRESS && oldState == GLFW_RELEASE && !movingPiece){
            movingPiece = true;
            //call function to read file and return which piece to move and where to move it
//            pieceToMove, whereToMove = getNextTurn(boardMatrix);
            printf("KeyPressed");
            oldState = GLFW_PRESS;
        }
        else if (nextStep == GLFW_RELEASE && oldState == GLFW_PRESS) oldState = GLFW_RELEASE;
        
        if (movingPiece) {
            // if there is a movement (movingPiece=True) to apply this function is called
            // when the movement ends it update the variable (movingPiece=False)
//            movingPiece = boardMatrix.move(pieceToMove, whereToMove);
            movingPiece = boardMatrix.move("a1", "h8");
        }
        
        //write text on screen
        printText2D("Chess Board v0.001", 5, 585, 12);
        sprintf(buffer, "%.2f", camPos.x);
        printText2D(buffer, 5, 5, 12);
        sprintf(buffer, "%.2f", camPos.y);
        printText2D(buffer, 100, 5, 12);
        sprintf(buffer, "%.2f", camPos.z);
        printText2D(buffer, 200, 5, 12);
        printText2D("Moving Piece:", 570, 5, 12);
        printText2D(movingPiece ? "True":"False", 730, 5, 12);
        
        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        
	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
    for (int i = 0; i < 2; i++) {
        Board[i].del();
    }
    for (int i = 0; i < 16; i++) {
        WPieces[i].del();
    }
    
	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture_Black);
    glDeleteTextures(1, &Texture_White);
	glDeleteVertexArrays(1, &VertexArrayID);
    
    // Delete the text's VBO, the shader and the texture
    cleanupText2D();

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

