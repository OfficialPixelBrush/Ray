#include <iostream>  
#include <math.h>
#include <SDL2/SDL.h>
#include <time.h>
#include <stdlib.h>

using namespace std;  

// Utility macros
#define CHECK_ERROR(test, message) \
    do { \
        if((test)) { \
            fprintf(stderr, "%s\n", (message)); \
            exit(1); \
        } \
    } while(0)

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define PI 3.1415926535897932384626433832795028841971693993751058209749445923078164062

SDL_Event event;
SDL_Renderer *renderer;
SDL_Window *window;

// Render Parameters
float RayStepSize = 1.0f;
int renderSteps = 5;
float maxDistance = RayStepSize * renderSteps;

// Maxmimum Object Numbers
int numberOfRays = 1;
int numberOfTriangles = 20;

// 2D float
struct Vec2 {
	float x,y;
};
typedef struct Vec2 Vec2;

// 3D float
struct Vec3 {
	float x,y,z;
};
typedef struct Vec3 Vec3;

// 3D vector
struct Point {
  float x, y, z;
};
typedef struct Point Point;

// RGB Color in range 0.0f - 1.0f
struct Color {
	float r;
	float g;
	float b;
};
typedef struct Color Color;

// Screen Pixel
struct ScreenPixel {
	Color color;
};
typedef struct ScreenPixel ScreenPixel;

// Runtime Variables
bool running = true;
bool topDown = false;

// Object Lists
struct Ray *RayArray;
struct Triangle *TriangleArray;

// ScreenBuffer
ScreenPixel screen[WINDOW_WIDTH][WINDOW_HEIGHT];
int currentPixelX,currentPixelY = 0;

// Camera Variables
Point origin;
float cameraRotation = 0.0f;
float cameraSpeedFrontBack = 0.0f;
float cameraSpeedLeftRight = 0.0f;
float cameraSpeedUpDown = 0.0f;
float cameraSpeedRotational = 0.0f;
float fieldOfView = 80.0f;

// Convert Radian to Degree
float radianToDegree(float radian) {
	return radian * 180.0f/PI;
}

// Convert Degree to Radian
float degreeToRadian(float degree) {
	return degree * PI/180.0f;
}

// Crappy clamp
float clamp(float in, float min, float max) {
	if (in > max) {
		in = max;
	}
	
	if (in < min) {
		in = min;
	}
	
	return in;
}

// Triangle Object
class Triangle {
	public:
		Point p1, p2, p3;
		// Maybe a color per vertex, then interpolate based on distance to vertexs
		Color color;
		
		Triangle(Point _vertex01, Point _vertex02, Point _vertex03, Color _color) {
			p1 = _vertex01;
			p2 = _vertex02;
			p3 = _vertex03;
			color = _color;
		}
};

// Provided by Google Bard
Point cross(const Point& a, const Point& b) {
	Point c;
	c.x = a.y * b.z - a.z * b.y;
	c.y = a.z * b.x - a.x * b.z;
	c.z = a.x * b.y - a.y * b.x;
	return c;
}

float dot(const Point& a, const Point& b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

Point PointSubtract(const Point& a, const Point& b) {
	Point c;
	c.x = a.x - b.x;
	c.y = a.y - b.y;
	c.z = a.z - b.z;
	return c;
}


bool lineIntersectsTriangle(const Point& start, const Point& direction, const Triangle& triangle, double& t, double& u, double& v) {
  // Compute the ray-plane intersection.
  Point normal = cross(PointSubtract(triangle.p2,triangle.p1), PointSubtract(triangle.p2,triangle.p1));
  double d = dot(normal, triangle.p1);
  double denominator = dot(normal, direction);
  if (denominator == 0) {
    // The ray is parallel to the triangle.
    return false;
  }
  t = (d - dot(normal, start)) / denominator;

  // Check if the intersection point is inside the triangle.
  Point intersection;
	intersection.x = start.x + t * direction.x;
	intersection.y = start.y + t * direction.x;
	intersection.z = start.z + t * direction.z;
  u = dot(cross(PointSubtract(triangle.p3, triangle.p2), PointSubtract(intersection, triangle.p2)), normal) / denominator;
  v = dot(cross(PointSubtract(triangle.p1, triangle.p3), PointSubtract(intersection, triangle.p3)), normal) / denominator;
  return u >= 0 && u <= 1 && v >= 0 && v <= 1 && u + v <= 1;
}

float getDistance(const Point& point1, const Point& point2) {
	return sqrt(
		pow((point2.x - point1.x),2) +
		pow((point2.y - point1.y),2) +
		pow((point2.z - point1.z),2)
	);
}

bool sphereNearRay(const Point& position) {
	Point sphereOrigin;
	sphereOrigin.x = 2.0f;
	sphereOrigin.y = 0.0f;
	sphereOrigin.z = 0.0f;
	float sphereRadius = 1.0f;
	if (getDistance(sphereOrigin, position) <= sphereRadius) {
		return 1;
	} else {
		return 0;
	}
}

// Ray object
class Ray {
	public:
		Point position;
		Point direction;
		
		Ray(float x, float y, float z, float _directionX, float _directionY, float _directionZ) {
			position.x = x;
			position.y = y;
			position.z = z;
			direction.x = _directionX;
			direction.y = _directionY;
			direction.z = _directionZ;
		}
		
		Ray() {
			position.x = origin.x;
			position.y = origin.y;
			position.z = origin.z;
			direction.x = 0.0;
			direction.y = 0.0;
			direction.z = 0.0;
		}
		
		int step() {
			// Save current position
			Point previousPosition = position;
			
			// Step ahead set amount
			float pitch = degreeToRadian(direction.x);
			float yawn 	= degreeToRadian(direction.y);
			float roll	= degreeToRadian(direction.z);
			position.x += (RayStepSize * sin(pitch));
			position.y += (RayStepSize * cos(yawn));
			position.z += (RayStepSize * tan(roll));
			
			//Triangle triangle = TriangleArray[1];

			double t, u, v;
			//bool intersects = lineIntersectsTriangle(position, direction, triangle, t, u, v);
			bool intersects = sphereNearRay(position);
			if (intersects) {
			  // The line intersects the triangle.
			  // The intersection point is at the barycentric coordinates (u, v, 1 - u - v).
				screen[currentPixelX][currentPixelY].color.r = 0.5f;
				screen[currentPixelX][currentPixelY].color.g = 0.0f;
				screen[currentPixelX][currentPixelY].color.b = 0.0f;
				return 1;
			} else {
				screen[currentPixelX][currentPixelY].color.r = 0.1f;
				screen[currentPixelX][currentPixelY].color.g = 0.3f;
				screen[currentPixelX][currentPixelY].color.b = 0.5f;
			}
			return 0;
		}
};

void PrintKeyInfo( SDL_KeyboardEvent *key ){
	/* Is it a release or a press? */
	if( key->type == SDL_KEYUP )
		printf( "Release:- " );
	else
		printf( "Press:- " );

	/* Print the hardware scancode first */
	printf( "Scancode: 0x%02X", key->keysym.scancode );
	/* Print the name of the key */
	printf( ", Name: %s", SDL_GetKeyName( key->keysym.sym ) );
	printf( "\n" );
}

int clearScreenBuffer() {
	for (int y = 0; y < WINDOW_HEIGHT; y++) {
		for (int x = 0; x < WINDOW_WIDTH; x++) {
			screen[x][y].color.r = 0;
			screen[x][y].color.g = 0;
			screen[x][y].color.b = 0;
		}
	}
	return 0;
}

int processControls() {
		const Uint8 *keys = SDL_GetKeyboardState(NULL);
		float playerSpeed = 0.1;
		cameraSpeedFrontBack /= 1.5;
		cameraSpeedLeftRight /= 1.5;
		cameraSpeedUpDown /= 1.5;
		cameraSpeedRotational /= 1.5;
		
		// Forward
        if (keys[SDL_SCANCODE_W]) {
			cameraSpeedFrontBack += playerSpeed;
		}
		
		// Backward
        if (keys[SDL_SCANCODE_S]) {
			cameraSpeedFrontBack -= playerSpeed;
		}
		
		// Left
        if (keys[SDL_SCANCODE_A]) {
			cameraSpeedLeftRight -= playerSpeed;
		}
		
		// Right
        if (keys[SDL_SCANCODE_D]) {
			cameraSpeedLeftRight += playerSpeed;
		}
		
		// UP Look-Left
        if (keys[SDL_SCANCODE_Q]) {
			cameraSpeedUpDown += playerSpeed;
		}
		
		// DOWN Look-Right
        if (keys[SDL_SCANCODE_E]) {
			cameraSpeedUpDown -= playerSpeed;
		}
		
		// Print Coordinates
		return 0;
}

int WinMain(int argc, char **argv) {
	printf("Hello, World!\n");
	srand(time(NULL)); 
	
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
	
	RayArray = (struct Ray *)calloc(numberOfRays+1, sizeof(struct Ray));
	TriangleArray = (struct Triangle *)calloc(numberOfTriangles+1, sizeof(struct Triangle));
	
	// Camera Origin
	origin.x = 0.0f;
	origin.y = 0.0f;
	origin.z = 0.0f;
	cameraRotation = 180.0f;
	RayArray[1] = *new Ray();
	
	// This is where we load the scene
	Point corner01;
	corner01.x = 5.0;
	corner01.y = 0.0;
	corner01.z = 0.0;
	Point corner02;
	corner02.x = 0.0;
	corner02.y = 5.0;
	corner02.z = 0.0;
	Point corner03;
	corner03.x = 0.0;
	corner03.y = 0.0;
	corner03.z = 5.0;
	Color triangleColor;
	triangleColor.r = 1.0;
	triangleColor.g = 1.0;
	triangleColor.b = 1.0;
	TriangleArray[1] = *new Triangle(corner01,corner02,corner03,triangleColor);
	
    while (running) {
		// Start new frame
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		clearScreenBuffer();
		
		// Update Origin
		origin.x += cameraSpeedLeftRight;
		origin.y += cameraSpeedFrontBack;
		origin.z += cameraSpeedUpDown;
		cameraRotation += cameraSpeedRotational;
		
		// Raytrace
		// Go over every pixel in the Field of View
		for (currentPixelY = 0; currentPixelY < WINDOW_HEIGHT; currentPixelY++) {
			for (currentPixelX = 0; currentPixelX < WINDOW_WIDTH; currentPixelX++) {
				for (int i = 0; i <= renderSteps; i++) {
					// Send out a Ray from the camera
					RayArray[1].position = origin;
					// This is where the FoV magically appears!
					RayArray[1].direction.x = (cameraRotation + ((fieldOfView/2)*-1)) + (fieldOfView/WINDOW_WIDTH*currentPixelX);
					RayArray[1].direction.y = (					((fieldOfView/2)*-1)) + (fieldOfView/WINDOW_HEIGHT*currentPixelY);
					
					Point previousPosition = RayArray[1].position;
					bool finishedLine = RayArray[1].step();
					if (finishedLine) {
						break;
					}
				}
			}
		}
		
		
		// Render result to screen
		for (currentPixelY = 0; currentPixelY < WINDOW_WIDTH ; currentPixelY++) {
			for (currentPixelX = 0; currentPixelX < WINDOW_WIDTH ; currentPixelX++) {
				Color pixelColor = screen[currentPixelX][currentPixelY].color;
				SDL_SetRenderDrawColor(
					renderer,
					(int)(clamp(pixelColor.r,0.0f,1.0f)*255),
					(int)(clamp(pixelColor.g,0.0f,1.0f)*255),
					(int)(clamp(pixelColor.b,0.0f,1.0f)*255),
					255
				);
				
				// Weirdly bent for some reason,
				// but I guess that's just down to how I shoot my rays
				//printf("%f\n",sliceSize);
				SDL_RenderDrawPoint (
					renderer,
					currentPixelX,
					currentPixelY
				);
			
				//SDL_RenderPresent(renderer);
			}
		}
	
        /*! updates the array of keystates */
        while ((SDL_PollEvent(&event)) != 0)
        {
            /*! request quit */
            if (event.type == SDL_QUIT) 
            { 
                running = false;
            }
        }
		
		processControls();
		
		SDL_RenderPresent(renderer);
		//running = false;
		printf("%f %f %f: %f\n", origin.x, origin.y, origin.z, cameraRotation);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
	return 0;
}