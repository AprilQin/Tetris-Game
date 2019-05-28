// Two-Dimensional Tetris Game created by April Qin   

#include "Angel.h"

//---------------------------------------------------------------------------
struct indexIn2Darray
{
	int i;
	int j;
};

//facilitate drawing
//
GLuint vao, vao1; //for binding context of drawing 
const int gamePoints = 12000; // I can have maximum 10*20*6 = 1200 points before game over
const int gridPoints = 64; //11 verticle lines are defined by 22 vertices + 21 horizontal lines defined by 42 vertices
const int shapePoints = 24; //each shape is made of 8 triangles defined by 24 vertices 
const int boxPoints = 6;
int startIndex = 0; //record where to start saving the vertex data of a new selected shape in the array game
vec2 grid[gridPoints]; //to store background grid points
vec3 gridcolors[gridPoints]; //the store the color of the background grid
vec2 game[gamePoints]; // to store all points of the game
vec3 colors[gamePoints]; //to store colors of each vertex of the game -- the shapes
vec2 current[shapePoints]; // to store the current piece
vec3 currentColor[shapePoints];
vec2 temp1[shapePoints]; //this temp reserved for moving left, right, down checking boundary
// vec2 temp2[shapePoints]; //this temp reserved for rotation transformation checking boundary 
//define the boundary of the game for checking if all vertices in temp pass the test
GLfloat xLefBound = -0.6;
GLfloat xRigBound = 0.6;
GLfloat yTopBound = 1.1;
GLfloat yLowBound = -1.1;
const float bias = 0.01;//for checking boundary (this is the accepted rounding error after doing transformation and moving left, down, right)


//facilitate rotation, and other movements 
//
GLfloat angle = M_PI / 2; //90 degree
static mat2 rotationMatrix = mat2 ( cos(angle), sin(angle), -sin(angle), cos(angle) ); //rotation matrix for rotating 90 degree left about the origin
static vec2 downStep = vec2( 0.0, -0.1 );
static vec2 leftStep = vec2(-0.1, 0.0);
static vec2 rightStep = vec2(0.1, 0.0);


//facilitate collision checking by storing the position of the past shapes to a 2d array
//
int state[20][10]; /*because we are taking the top right corner of a box to calculate which index that point belongs to. since it's top right point, so the left most line and the bottom most line in the background grid is excluded.
					remember that 10*20 array should be 20 rows and 10 column*/
float xCoordinate = -0.5; //used to store the top right corner x coordinate of a box
float yCoordinate = -1.1; //used to store the top right corner y coordinate of a box
int xbox = 0; //used to store the corresponding x index in the 2d array
int ybox = 0; //used to store the corresponding y index in the 2d array


//define shapes and colors
//
static vec3  base_colors[] = {
		vec3( 0.878, 0.207, 0.101 ),
		vec3( 0.878, 0.870, 0.101 ),
		vec3( 0.933, 0.650, 0.266 ),
		vec3( 0.933, 0.266, 0.741 ),
		vec3( 0.666, 0.266, 0.933 ),
		vec3( 0.266, 0.333, 0.933 ),
		vec3( 0.266, 0.933, 0.368 ),
};

vec2 vertices[15] = {vec2(-0.2, 1.0), vec2(-0.1, 1.0), vec2(0.0, 1.0), vec2(0.1, 1.0), vec2(0.2, 1.0),
					vec2(-0.2, 0.9), vec2(-0.1, 0.9), vec2(0.0, 0.9), vec2(0.1, 0.9), vec2(0.2, 0.9),
					vec2(-0.2, 0.8), vec2(-0.1, 0.8), vec2(0.0, 0.8), vec2(0.1, 0.8), vec2(0.2, 0.8)}; //possible starting points of any piece

vec2 O[shapePoints] = {
	 			vertices[1], vertices[2], vertices[7], vertices[1], vertices[6], vertices[7],  
				vertices[2], vertices[3], vertices[8], vertices[2], vertices[7], vertices[8], 
				vertices[6], vertices[7], vertices[12],vertices[6], vertices[11],vertices[12], 
				vertices[7], vertices[8], vertices[13],vertices[7], vertices[12],vertices[13]
			}; //special case when calculating center of rotation 
vec3 Ocolor[shapePoints] = {
				base_colors[0], base_colors[0], base_colors[0], base_colors[0], base_colors[0], base_colors[0], 
				base_colors[0], base_colors[0], base_colors[0], base_colors[0], base_colors[0], base_colors[0],
				base_colors[0], base_colors[0], base_colors[0], base_colors[0], base_colors[0], base_colors[0],
				base_colors[0], base_colors[0], base_colors[0], base_colors[0], base_colors[0], base_colors[0]
};


vec2 I[shapePoints] = {
				vertices[2], vertices[3], vertices[8], vertices[2], vertices[7], vertices[8], //center of this two triangles is pivot of rotation (0-5)
	 			vertices[0], vertices[1], vertices[6],vertices[0], vertices[6], vertices[5],
				vertices[1], vertices[2], vertices[7],vertices[1], vertices[6], vertices[7], 
				vertices[3], vertices[4], vertices[9], vertices[3], vertices[8], vertices[9]
			};
vec3 Icolor[shapePoints] = {
				base_colors[1], base_colors[1], base_colors[1], base_colors[1], base_colors[1], base_colors[1], 
				base_colors[1], base_colors[1], base_colors[1], base_colors[1], base_colors[1], base_colors[1],
				base_colors[1], base_colors[1], base_colors[1], base_colors[1], base_colors[1], base_colors[1],
				base_colors[1], base_colors[1], base_colors[1], base_colors[1], base_colors[1], base_colors[1]
};



vec2 S[shapePoints] = {
				vertices[1], vertices[2], vertices[7],vertices[1], vertices[7], vertices[6], //center of this two triangles is pivot of rotation (0-5)
	 			vertices[3], vertices[2], vertices[8],vertices[2], vertices[8], vertices[7], 
				vertices[6], vertices[7], vertices[12], vertices[6], vertices[12], vertices[11], 
				vertices[5], vertices[6], vertices[11], vertices[5], vertices[11], vertices[10]
			};

vec3 Scolor[shapePoints] = {
				base_colors[2], base_colors[2], base_colors[2], base_colors[2], base_colors[2], base_colors[2], 
				base_colors[2], base_colors[2], base_colors[2], base_colors[2], base_colors[2], base_colors[2], 
				base_colors[2], base_colors[2], base_colors[2], base_colors[2], base_colors[2], base_colors[2], 
				base_colors[2], base_colors[2], base_colors[2], base_colors[2], base_colors[2], base_colors[2] 
};




vec2 Z[shapePoints] = {
				vertices[2], vertices[3], vertices[8],vertices[2], vertices[7], vertices[8], //center of this two triangles is pivot of rotation (0-5)
	 			vertices[1], vertices[2], vertices[7],vertices[1], vertices[6], vertices[7],  				 
				vertices[7], vertices[8], vertices[13], vertices[7], vertices[12], vertices[13], 
				vertices[8], vertices[9], vertices[14], vertices[8], vertices[13], vertices[14]
			};
		
vec3 Zcolor[shapePoints] = {
				base_colors[3], base_colors[3], base_colors[3], base_colors[3], base_colors[3], base_colors[3], 
				base_colors[3], base_colors[3], base_colors[3], base_colors[3], base_colors[3], base_colors[3],
				base_colors[3], base_colors[3], base_colors[3], base_colors[3], base_colors[3], base_colors[3],
				base_colors[3], base_colors[3], base_colors[3], base_colors[3], base_colors[3], base_colors[3]
};





vec2 L[shapePoints] = {
				vertices[2], vertices[3], vertices[8],vertices[2], vertices[7], vertices[8], //center of this two triangles is pivot of rotation (0-5)
	 			vertices[3], vertices[4], vertices[9],vertices[3], vertices[8], vertices[9],   
				vertices[1], vertices[2], vertices[7], vertices[1], vertices[6], vertices[7], 
				vertices[6], vertices[7], vertices[12], vertices[6], vertices[11], vertices[12]
			};



vec3 Lcolor[shapePoints] = {
				base_colors[4], base_colors[4], base_colors[4], base_colors[4], base_colors[4], base_colors[4], 
				base_colors[4], base_colors[4], base_colors[4], base_colors[4], base_colors[4], base_colors[4], 
				base_colors[4], base_colors[4], base_colors[4], base_colors[4], base_colors[4], base_colors[4], 
				base_colors[4], base_colors[4], base_colors[4], base_colors[4], base_colors[4], base_colors[4]
};



vec2 J[shapePoints] = {
				vertices[2], vertices[3], vertices[8],vertices[2], vertices[7], vertices[8], //center of this two triangles is pivot of rotation (0-5)
	 			vertices[3], vertices[4], vertices[9],vertices[3], vertices[8], vertices[9],  
				vertices[1], vertices[2], vertices[7], vertices[1], vertices[6], vertices[7], 
				vertices[8], vertices[9], vertices[14], vertices[8], vertices[13], vertices[14]
			};
vec3 Jcolor[shapePoints] = {
				base_colors[5], base_colors[5], base_colors[5], base_colors[5], base_colors[5], base_colors[5], 
				base_colors[5], base_colors[5], base_colors[5], base_colors[5], base_colors[5], base_colors[5], 
				base_colors[5], base_colors[5], base_colors[5], base_colors[5], base_colors[5], base_colors[5], 
				base_colors[5], base_colors[5], base_colors[5], base_colors[5], base_colors[5], base_colors[5]
};



vec2 T[shapePoints] = {
				vertices[2], vertices[3], vertices[8],vertices[2], vertices[7], vertices[8],  //center of this two triangles is pivot of rotation (0-5)
	 			vertices[3], vertices[4], vertices[9],vertices[3], vertices[8], vertices[9],  
				vertices[1], vertices[2], vertices[7], vertices[1], vertices[6], vertices[7], 
				vertices[7], vertices[8], vertices[13], vertices[7], vertices[12], vertices[13]
			};
vec3 Tcolor[shapePoints] = {
				base_colors[6], base_colors[6], base_colors[6], base_colors[6], base_colors[6], base_colors[6], 
				base_colors[6], base_colors[6], base_colors[6], base_colors[6], base_colors[6], base_colors[6], 
				base_colors[6], base_colors[6], base_colors[6], base_colors[6], base_colors[6], base_colors[6], 
				base_colors[6], base_colors[6], base_colors[6], base_colors[6], base_colors[6], base_colors[6]
};


//----------------------------------------------------------------------------

void print_vec2array(vec2* array, int size) //for debugging
{
	printf("size is %i\n", size);
	for (int i =0; i<size; i++)
	{
		printf("vec %i = %f, %f\n", i , array[i].x, array[i].y);
	}
}
void cleanupStateBuffer()
{
	for (int i = 0; i< 20; i++)
	{
		for (int j=0; j<10; j++)
		{
			state[i][j] = 0;
		}
	}
}

void cleanupGameBuffer()
{
	vec2 cleanVec = vec2(0.0, 0.0);
	for (int i = 0; i < startIndex+24; i++)
	{
		game[i] = cleanVec;
	}
}

//for copying colors of vertices to their corresponding buffers
void copyarrayVec3 (vec3* old_arr, int old_start, vec3* new_arr, int new_start, int size)
{
	for (int i = old_start; i < size + old_start; i++)
	{
		new_arr[new_start] = old_arr[i];

		new_start++;
	}
}

/* this function can be changed  to get rid of the fromStart argument*/
void copyarrayVec2(vec2* old_arr, int old_start, vec2* new_arr, int new_start, int size)
{
	/*if I copy to temp, it should go to temp starting from index 0
	  if I copy to game, it should go to game starting from the global variable startIndex
	  if I copy from current to box, it should start copy every 6 index in current, and all 6 points goes to box to index 0 to 5 each time*/	
	// int index = new_start;
	for (int i = old_start; i < size + old_start; i++)
	{

		new_arr[new_start] = old_arr[i];

		new_start++;
	}
}

bool topRowCheck()
{
	bool topRowOccupied = false;
	for (int i = 0; i < 10; i++)
	{
		if (state[0][i] == 1)
			topRowOccupied = true;
	}
	return topRowOccupied;
}


void selectShape()
{
	bool topRowOccupied = topRowCheck(); //check if there's anything on the top row, if there is, GG
	if (topRowOccupied)
	{
		// cleanupGameBuffer();
		// cleanupStateBuffer();
		// selectShape();
		exit( EXIT_SUCCESS );
	}

	//randomly select a shape to fill the current array
	srand( time(NULL));
	int shape = rand()%7;
	switch (shape)
	{
		case 0:	
			copyarrayVec2(O, 0, current, 0, 24);
			copyarrayVec3(Ocolor, 0, currentColor, 0, 24);
			break;
		case 1:	
			copyarrayVec2(I, 0, current, 0, 24);
			copyarrayVec3(Icolor, 0, currentColor, 0, 24);
			break;
		case 2:	
			copyarrayVec2(S, 0, current, 0, 24);
			copyarrayVec3(Scolor, 0, currentColor, 0, 24);
			break;
		case 3:	
			copyarrayVec2(Z, 0, current, 0, 24);
			copyarrayVec3(Zcolor, 0, currentColor, 0, 24);
			break;
		case 4:	
			copyarrayVec2(L, 0, current, 0, 24);
			copyarrayVec3(Lcolor, 0, currentColor, 0, 24);
			break;
		case 5:	
			copyarrayVec2(J, 0, current, 0, 24);
			copyarrayVec3(Jcolor, 0, currentColor, 0, 24);
			break;
		case 6:	
			copyarrayVec2(T, 0, current, 0, 24);
			copyarrayVec3(Tcolor, 0, currentColor, 0, 24);
			break;
	}
}


void findIndexOfBox(vec2* box)
{
	xCoordinate = box[0].x;
	yCoordinate = box[0].y;

	for (int i = 1; i<6; i++)
	{
		if(xCoordinate < box[i].x)
			xCoordinate = box[i].x;

		if(yCoordinate < box[i].y)
			yCoordinate = box[i].y;	
	}

	//to calculate index to use for collision check in state array at state[xbox][ybox], need to check how far it is to (-0.4, 1)
	//(-0.4,1 ) is mapped to state[0][0], changing value of x axis is applied to ybox, changing of y axis is applied to xbox 
	ybox = round( (xCoordinate + 0.4)/0.1 );	/*since all other points must have a value > -0.4, they add 0.4 must result in a positive number
												do a round() to the result because there can be rounding error which result in taking the floor that the result eg 
												the exact result = 3 but the calculated result is 2.9, if I don't do round, it will be 2 when we assign it to ybox
												because ybox is an int and there will be a float to int conversion*/

	xbox = round( (1 - yCoordinate)/0.1 ); //since all other points are smaller than 1, use 1 - y will result in a positive number
}

bool collisionCheck()
{
	bool notCollide = true; // true means no collision

	if (state[xbox][ybox] == 1)
		notCollide = false;

	return notCollide;
}



bool boundaryCheck(vec2 step, vec2* piece, bool rotation)
{
	// need to store into boxtemp first, find index of each box, check collision first, then check boundary of all vertices if collision does not fail
	bool eligibility = true;
	vec2 boxTemp[boxPoints];
	int start = 0; 

	for (int i = 0; i < 4; i++ ) 
    {   
    	for (int j = 0; j < 6; j++)
    	{
    		int index = i*6+j; //this is the index of the current vertex that we should look up in array piece and index that we should save to temp1 if the box passes the collision test and every vertex passes the boundary test

    		vec2 newPosition; 
 
    		if (rotation)
    			newPosition = rotationMatrix * (piece[index] - step) + step;
    		else
    			newPosition =  piece[index]+ step;

	        //check this newPosition is out of boundary, eligibility = false
        	if ( fabs(xLefBound - newPosition.x) < bias ||  fabs(newPosition.x - xRigBound) < bias || fabs(yLowBound - newPosition.y) < bias  )
        	{ 
            	eligibility = false; //out of boundary
	        	break;
	    	}

	    	boxTemp[j] = newPosition;

    	}

    	if (!eligibility)
    	{
    		break;
    	}
    	else
    	{
    		findIndexOfBox(boxTemp); //update the global variable xCoordinate and yCoordinate with the coordinate of top right corner of the current box
    		eligibility = collisionCheck();  

    		if (!eligibility)
    		{
    			break;
    		} 

    		copyarrayVec2(boxTemp, 0, temp1, start, 6);
    		start = start + 6;
    	}	
    }

	return eligibility;		
}

bool shapeOCheck(vec2* piece)
{
	bool shapeO = false;
	indexIn2Darray index[4];
	for (int i = 0; i<4; i++) //there are 4 boxes stored in current
	{
		vec2 box[6];
		int s = i*6;
		copyarrayVec2(piece, s, box, 0, 6); //copy a box from current to box array
		findIndexOfBox(box);
		index[i].i = xbox;
		index[i].j = ybox;
	}
	if ( index[0].i == index[1].i && index[0].j == index[2].j && index[2].i == index[3].i && index[1].j == index[3].j)
		shapeO = true;

	return shapeO;
}

bool movement(int direction, vec2* piece){
	/*call check boundary to calculate the new vertices of a piece when it moves to left, right, down or rotation specified by step
	and save the vertices to temp1. If all vertices passed the test, return true upon coming back to this function. This function
	copies the data from temp1 to both current and game buffer*/

		bool update = true;
		switch (direction)
		{
			case 1:
				update = boundaryCheck(leftStep, piece, false);
				break;

			case 2:
				update = boundaryCheck(downStep, piece, false);
				break;

			case 3:
				update = boundaryCheck(rightStep, piece, false);
				break;

			case 4:
				bool shapeO = shapeOCheck(piece);
				if (shapeO)
					update = false;
				else
				{
					vec2 center = vec2(0.0,0.0);
					for ( int i = 0; i < 6; ++i ) { //calculate pivot of rotation
						center += piece[i];
				    }  
					center = center/ 6; //for loop + this divide by 6 step finds the center of rotation. Assuming the first 6 vertices is the box that contains the center of rotation
					update = boundaryCheck(center, piece, true);
				}
			
		}
		
		if (update == true){
			copyarrayVec2(temp1, 0, current, 0, 24);//need to update current because I passed current to movement when I called it in stepDownDelay function 
			copyarrayVec2(temp1, 0, game, startIndex, 24); //need to update game because I use game to draw not current			
		}

		glutPostRedisplay();

		return update;
}

float findYCoordinateOfIndexI(int i) //find in state buffer the ith rows's corresponding y coordinate in game
{
	return (1 - i*0.1);
}

void reduceRow(int index, float y)
{
	//update state buffer --- copy every row with values above it, then fill the top row with 0s
	for (int i = index; i >= 0; i--)
	{
		for (int j = 0; j < 10; j++)
		{
			if(i == 0)
				state[i][j] = 0;
			else
				state[i][j] = state[i-1][j];
		}
	}

	//update game vertices that's close enough to y or > y  by adding -0.1. There are startIndex number of points in the game buffer
	for (int i = 0; i < startIndex; i++)
	{
		if ( game[i].y  >= y || (y - game[i].y) <= bias )
			game[i] = game[i]+ downStep;
	}
}

void rowfullCheck()
{
	for (int i = 19; i >= 0; i--)
	{
		bool rowfull = true;
		for (int j = 0; j < 10; j++)
		{
			if (state[i][j] == 0) //if any value in the row is 0, then the row is not full
				rowfull = false; 
		}
		if (rowfull)
		{
			float y = findYCoordinateOfIndexI(i);
			reduceRow(i, y);
			i++; //this row is full, and we move next row down one row we should recheck this row again
		} 	
	}
}

//----------------------------------------------------------------------------

//timed function.
void stepdownDelay(int)
{
	bool eligibility = movement(2, current);
	
	if ( eligibility == false){ //not able to move down any more

		for (int i = 0; i<4; i++) //there are 4 boxes stored in current
		{

			vec2 box[6];
			int s = i*6;

			copyarrayVec2(current, s, box, 0, 6); //copy a box from current to box array
			findIndexOfBox(box);

			state[xbox][ybox] = 1; 


		}
		startIndex = startIndex + 24; //update the new start index in game buffer to append new shape
		rowfullCheck();

		printf("selecting a new shape\n");
		selectShape(); //update current to new shape
		
		copyarrayVec2(current, 0, game, startIndex, 24); //copy current to game buffer
		copyarrayVec3(currentColor, 0, colors, startIndex, 24); //copy current color to the current moving piece
	}
    //update canvas
    glutPostRedisplay();
    //keep going down
    glutTimerFunc(500.0, stepdownDelay, 0);	
}

void
keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
    	case 113:
	    	// printf("Q pressed! exit\n");
	        exit( EXIT_SUCCESS );
	        break;
	    case 114:
	    	// printf("r pressed! clean up game and state buffer\n");
	    	cleanupGameBuffer();
	    	cleanupStateBuffer();
	    	selectShape(); //need to select new shape to update current, otherwise it'll use current with old indices
	    	break;
    }  
}

void keyboardSpecial( int key, int x, int y )
{
    switch ( key ) {

    case GLUT_KEY_LEFT:
		movement(1, current);
		break;

    case GLUT_KEY_DOWN:      
    	movement(2, current);
    	break;

    case GLUT_KEY_RIGHT:
    	movement(3, current);
    	break;

    case  GLUT_KEY_UP:
        movement(4, current);       
        break;
    }  
}

//----------------------------------------------------------------------------

void specifyBackground (int w, int h){
	//define the grid left to right, bottom to top by specifing vertices of each line
	int vIndex = 0;
	//width w pixels need to draw w+1 lines
	for (int i =0; i<=w; i++){
		float xCoordinate = 0.1*i - 0.5; //calculate x coordinate of verticle lines
		grid[vIndex] = vec2(xCoordinate, -1.0); //save coordinates of a vertex 
		vIndex++;
		grid[vIndex] = vec2(xCoordinate, 1.0);
		vIndex++;	
	}

	for (int i =0; i<=h; i++){
		float yCoordinate = 0.1*i - 1; //calculate y coordinate of horizontal lines
		grid[vIndex] = vec2(-0.5, yCoordinate); //save coordinates of a vertex 
		vIndex++;
		grid[vIndex] = vec2(0.5, yCoordinate);
		vIndex++;	
	}

	for (int i = 1; i <= 2*(h+w+2); i++) /*we assign all vertices same color, which is grey, and there are in total h+1 + w+1 lines
										which mean 2 * (h+w+2) points*/
	{
		gridcolors[i-1] = vec3( 0.349, 0.345, 0.372 );
	}
}


void
init( void )
{
	specifyBackground(10,20); //put vertices of the background lines in array grid
	selectShape();
	// startIndex = startIndex + 24; //please do not update start index here because I need to use it to update the shape in game when it's moving 
	copyarrayVec2(current, 0, game, 0, 24);
	copyarrayVec3(currentColor, 0, colors, 0, 24);
	cleanupStateBuffer(); //initialize all indices in state buffer with 0

    // create and bind vao for background grid
    glGenVertexArrays( 1, &vao);
    glBindVertexArray( vao );

    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );

    glBufferData( GL_ARRAY_BUFFER, sizeof(grid)+sizeof(gridcolors), NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0 , sizeof(grid), grid);
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(grid), sizeof(gridcolors), gridcolors);

    GLuint program = InitShader( "vshader.glsl", "fshader.glsl" );
    glUseProgram( program );

    GLuint background = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( background );
    glVertexAttribPointer( background, 2, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );

    GLuint backgroundcolor = glGetAttribLocation( program, "vColor" );
    glEnableVertexAttribArray( backgroundcolor );
    glVertexAttribPointer ( backgroundcolor, 3, GL_FLOAT, GL_FALSE, 0 , BUFFER_OFFSET(sizeof(grid)) );

    glClearColor( 0.0, 0.0, 0.0, 1.0 ); // black background
}



void
display( void )
{
    //send game data
    glGenVertexArrays(1, &vao1);
    glBindVertexArray(vao1);
    GLuint buffer1;
    glGenBuffers( 1, &buffer1 );
    glBindBuffer( GL_ARRAY_BUFFER, buffer1 );

    glBufferData( GL_ARRAY_BUFFER, sizeof(game)+ sizeof(colors), NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, sizeof(game), game);
    glBufferSubData( GL_ARRAY_BUFFER, sizeof(game), sizeof(colors), colors );


    GLuint program1 = InitShader( "vshader.glsl", "fshader.glsl" );
    glUseProgram( program1);

    GLuint loc = glGetAttribLocation( program1, "vPosition" );
    glEnableVertexAttribArray( loc );
    glVertexAttribPointer( loc, 2, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );

    GLuint col = glGetAttribLocation( program1, "vColor" );
    glEnableVertexAttribArray( col );
    glVertexAttribPointer( col, 3, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(sizeof(game)) );

    //drawing
    glClear( GL_COLOR_BUFFER_BIT );     // clear the window

    glBindVertexArray(vao1);
    glDrawArrays(GL_TRIANGLES, 0, startIndex+24); // draw the shapes

    glBindVertexArray(vao); //make vao active
    glDrawArrays( GL_LINES, 0, gridPoints );    // draw the grid
	// glLineWidth(2.0);
    glFlush();
}


//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA );
    glutInitWindowSize(512,512);
    glutInitContextVersion( 3, 2 );
    glutInitContextProfile( GLUT_CORE_PROFILE );
    glutCreateWindow( "Tetris Game" );

    // Iff you get a segmentation error at line 34, please uncomment the line below
    glewExperimental = GL_TRUE; 
    glewInit();

    init();

    glutDisplayFunc( display );   
    glutTimerFunc(500.0, stepdownDelay, 0);    
    glutKeyboardFunc( keyboard );
    glutSpecialFunc( keyboardSpecial );

    glutMainLoop();
    return 0;
}
