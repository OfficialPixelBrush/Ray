#include <iostream>  
#include <math.h>
#include <SDL2/SDL.h>
#include <time.h>
#include <stdlib.h>
#include <thread>
#include <vector>

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
#define TICKS_FOR_NEXT_FRAME (1000 / 60)

// 2D Position
struct Point {
	float x;
	float y;
};
typedef struct Point Point;
	
struct Vec3 {
	float x;
	float y;
	float z;
};
typedef struct Vec3 Vec3;

// RGB Color Struct going from 0.0f - 1.0f
struct Color {
	float r;
	float g;
	float b;
};
typedef struct Color Color;

// Screen Column Struct, containing an RGB Pixel color, and the distance it has
struct ScreenColumn {
	Color color;
	float distance;
};
typedef struct ScreenColumn ScreenColumn;

/* ---- Pointers ---- */
// Contains the Array Objects used for calculating the screen
struct Ray *RayArray;
// Contains the Lines making up the level
struct Line *LineArray;
// Contains the Point Lights of the Level
struct PointLight *LightArray;
// Contains the calculated Distances and Colors of the Floorlight
struct Color *FloorLightArray;

/* -- SDL Pointers --*/
SDL_Event event;
SDL_Renderer *renderer;
SDL_Window *window;
SDL_Texture *texture;

ScreenColumn screen[WINDOW_WIDTH];

// Camera Variables

/* ---- Runtime Variables ---- */
bool frameDone = false;
bool running = true;
int lastFrameTime = 0;

/* ---- Object Limits ---- */
int numberOfLines = 20;
int numberOfLights = 6;
int numberOfBounces = 0;
int currentRenderStep = 0;
int numberOfRenderSectors = 1;
int numberOfRays = numberOfRenderSectors;

/* ---- Camera Variables ---- */
Point cameraPosition;
float cameraRotation = 0.0f;
float fieldOfView = 180.0f;
float maxRenderDistance = 1000.0f;


/* ---- Player Variables ---- */
float maxSpeed = 6.0f;
float cameraSpeedHorziontal = 0.0f;
float cameraSpeedVertical = 0.0f;
float cameraSpeedRotational = 0.0f;

/* ---- Debug Variables */
bool topDown = false;
bool renderMode = false;
Color skyLight;
Color floorColor;;

/* ---- CLASSES ---- */

// A primitive PointLight
class PointLight {
	public:
		Point position;
		Color color;
		float brightness;
		float distance;
		
	PointLight(Point _p, Color _color, float _brightness, float _distance) {
		position = _p;
		color = _color;
		brightness = _brightness;
		distance = _distance;
	}
	
	PointLight(float x, float y, int r, int g, int b, float _brightness, float _distance) {
		position.x = x;
		position.y = y;
		color.r = r;
		color.g = g;
		color.b = b;
		brightness = _brightness;
		distance = _distance;
	}
};


// A simple line or wall
class Line {
	public:
		Point p1;
		Point p2;
		Color color;
		float roughness;
		Line(float x1, float y1, float x2, float y2) {
			p1.x = x1;
			p1.y = y1;
			p2.x = x2;
			p2.y = y2;
			color.r = 1.0;
			color.g = 1.0;
			color.b = 1.0;
			roughness = 1.0f;
		}
		
		Line(Point _p1, Point _p2) {
			p1 = _p1;
			p2 = _p2;
			color.r = 1.0;
			color.g = 1.0;
			color.b = 1.0;
			roughness = 1.0f;
		}
		
		Line(float x1, float y1, float x2, float y2, int r, int g, int b) {
			p1.x = x1;
			p1.y = y1;
			p2.x = x2;
			p2.y = y2;
			color.r = r;
			color.g = g;
			color.b = b;
			roughness = 1.0f;
		}
		
		Line(Point _p1, Point _p2, Color _color) {
			p1 = _p1;
			p2 = _p2;
			color = _color;
			roughness = 1.0f;
		}
};

/* ---- HELPER FUNCTIONS ---- */
// If returns 0, no intersection has been found
// If returns > 0, index of Line
bool ccw(const Point& p1, const Point& p2, const Point& p3) {
  return (p3.y - p1.y) * (p2.x - p1.x) > (p2.y - p1.y) * (p3.x - p1.x);
}

bool intersect(const Point& p1, const Point& p2, const Point& p3,
               const Point& p4) {
  return ccw(p1, p3, p4) != ccw(p2, p3, p4) &&
         ccw(p1, p2, p3) != ccw(p1, p2, p4);
}

int checkIfAnyLinesIntersect(const Point& p1, const Point& p2) {
  for (int i = 1; i <= numberOfLines; i++) {
    if (intersect(p1, p2, LineArray[i].p1, LineArray[i].p2)) {
      return i;
    }
  }
  return 0;
}

/* Input two lines, returns their Intersection Point */
Point getIntersectionPoint(Line Line1, Line Line2) {
	Point result;
    float x1 = Line1.p1.x;
    float y1 = Line1.p1.y;
    float x2 = Line1.p2.x;
    float y2 = Line1.p2.y;
    float x3 = Line2.p1.x;
    float y3 = Line2.p1.y;
    float x4 = Line2.p2.x;
    float y4 = Line2.p2.y;

    float x, y = 0;
	result.x = x;
	result.y = y;

    // Check if Line1 is vertical
    if (x1 == x2) {
        if (x3 == x4) {
            // Both Lines are vertical, so they are either coincident or parallel.
            if (x1 == x3 && ((min(y1, y2) <= max(y3, y4) && min(y3, y4) <= max(y1, y2)) || 
                (min(y3, y4) <= max(y1, y2) && min(y1, y2) <= max(y3, y4)))) {
                return result; // Return a non-zero value to indicate an intersection
            }
        } else {
            // Calculate the equation of the Line representing Line2
            float m2 = (y4 - y3) / (x4 - x3);
            float b2 = y3 - m2 * x3;

            // Calculate the x-coordinate of the intersection point
            x = x1;
            y = m2 * x + b2;
        }
    } else {
        float m1 = (y2 - y1) / (x2 - x1);
        float m2 = (y4 - y3) / (x4 - x3);

        if (m1 == m2) {
			// Lines are parallel, no intersection 
            return result;
        }

        x = (m1 * x1 - y1 - m2 * x3 + y3) / (m1 - m2);
        y = m1 * (x - x1) + y1;
    }

    // Check if the intersection point is within the Line segments
    if ((min(x1, x2) <= x <= max(x1, x2)) && (min(x3, x4) <= x <= max(x3, x4))) {
		result.x = x;
		result.y = y; // Return a non-zero value to indicate an intersection
    }
	return result;
}

// Convert Radian to Degree
float radianToDegree(float radian) {
	return radian * 180.0f/PI;
}

// Convert Degree to Radian
float degreeToRadian(float degree) {
	return degree * PI/180.0f;
}

// Get the Distance between two Points
float getDistance(Point p1, Point p2) {
	return sqrt((p2.x-p1.x)*(p2.x-p1.x) + (p2.y - p1.y)*(p2.y - p1.y));
}

// Calculate the normal angle of a Line
float calculateNormalAngle(Line& line) {
  // Calculate the direction vector of the line.
  float dx = line.p2.x - line.p1.x;
  float dy = line.p2.y - line.p1.y;

  // Calculate the magnitude of the direction vector.
  double magnitude = sqrt(dx * dx + dy * dy);

  // Calculate the normal vector of the line.
  double nx = -dy / magnitude;
  double ny = dx / magnitude;

  // Calculate the angle between the normal vector and the positive x-axis.
  double angle = atan2(ny, nx);

  // Return the angle in radians.
  return angle;
}

// Clamp a float between min and max
float clamp(float in, float min, float max) {
	if (in > max) {
		in = max;
	}
	
	if (in < min) {
		in = min;
	}
	
	return in;
}

/* ---- INPUT HANDLING ---- */
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

int processControls() {
		const Uint8 *keys = SDL_GetKeyboardState(NULL);
		cameraSpeedHorziontal /= 2.0f;
		cameraSpeedVertical /= 2.0f;
		cameraSpeedRotational /= 2.0f;
		
		// Forward
        if (keys[SDL_SCANCODE_W]) {
			cameraSpeedHorziontal += 1.0f*sin(degreeToRadian(cameraRotation));
			cameraSpeedVertical += 1.0f*cos(degreeToRadian(cameraRotation));
		}
		
		// Backward
        if (keys[SDL_SCANCODE_S]) {
			cameraSpeedHorziontal -= 1.0f*sin(degreeToRadian(cameraRotation));
			cameraSpeedVertical -= 1.0f*cos(degreeToRadian(cameraRotation));
		}
		
		// Left
        if (keys[SDL_SCANCODE_A]) {
			cameraSpeedHorziontal += 1.0f*sin(degreeToRadian(cameraRotation+90));
			cameraSpeedVertical += 1.0f*cos(degreeToRadian(cameraRotation+90));
		}
		
		// Right
        if (keys[SDL_SCANCODE_D]) {
			cameraSpeedHorziontal += 1.0f*sin(degreeToRadian(cameraRotation-90));
			cameraSpeedVertical += 1.0f*cos(degreeToRadian(cameraRotation-90));
		}
		
		// Look-Left
        if (keys[SDL_SCANCODE_Q]) {
			cameraSpeedRotational += 1.0f;
		}
		
		// Look-Right
        if (keys[SDL_SCANCODE_E]) {
			cameraSpeedRotational -= 1.0f;
		}
		
		// Quit
        if (keys[SDL_SCANCODE_ESCAPE]) {
			running = false;
		}
		return 0;
}

void updateInputs() {
	float lightMotion = -2.0;
	while(running) {		
		processControls();
		
		// Update Origin
		cameraPosition.x += cameraSpeedHorziontal;
		cameraPosition.y += cameraSpeedVertical;
		cameraRotation += cameraSpeedRotational;
		
		if (cameraSpeedHorziontal >= maxSpeed) {
			cameraSpeedHorziontal = maxSpeed;
		}
		if (cameraSpeedVertical >= maxSpeed) {
			cameraSpeedVertical = maxSpeed;
		}
		if (cameraSpeedRotational >= maxSpeed) {
			cameraSpeedRotational = maxSpeed;
		}
		SDL_Delay(16);
		
		// Object movement
		
		// LightAnimation
		if (LightArray[2].position.x < (640*2)/32) {
			lightMotion*=-1;
		}
		
		if (LightArray[2].position.x > (640*2)/32*31) {
			lightMotion*=-1;
		}
	
		LightArray[2].position.x+=lightMotion;
	}
}

// Get the bounce angle based on the Angle of incident and the normal angle
float getBounceReflection(float incidentAngle, float normalAngle) {
	// Convert angles from degrees to radians
	float incidentDirectionRad = degreeToRadian(incidentAngle);
	float normalAngleRad = degreeToRadian(normalAngle);
	
	// Calculate the incident direction vector
	float incidentX = cos(incidentDirectionRad);
	float incidentY = sin(incidentDirectionRad);

	// Calculate the normal vector
	float normalX = cos(normalAngleRad);
	float normalY = sin(normalAngleRad);

	// Calculate the dot product of incident and normal vectors
	float dotProduct = incidentX * normalX + incidentY * normalY;

	// Calculate the reflection vector
	float reflectionX = incidentX - 2 * dotProduct * normalX;
	float reflectionY = incidentY - 2 * dotProduct * normalY;

	// Calculate the angle of the reflection vector in degrees
	return radianToDegree(atan2(reflectionY, reflectionX));
}

/* ---- RAYTRACING ---- */
// The Ray class, used for raytracing
class Ray {
	public:
		Point position;
		float direction;
		int currentPixel;
		float RayStepSize = 0.1f;
		
		Ray(float x, float y, float _direction) {
			position.x = x;
			position.y = y;
			direction = _direction;
		}
		
		Ray() {
			position.x = cameraPosition.x;
			position.y = cameraPosition.y;
			direction = cameraRotation;
		}
		
		// Used to step through the Ray until a wall is hit
		int step() {
			//printf("%f\n",RayStepSize);
			//RayStepSize = getDistance(position,cameraPosition)/maxRenderDistance*(WINDOW_HEIGHT/2);
			RayStepSize = 5.0f;
			Point previousPosition = position;
			float rad = degreeToRadian(direction);
			position.x += (RayStepSize*sin(rad));
			position.y += (RayStepSize*cos(rad));
			
			// Draws those cool light columns
			// Iterate through all the lights in the scene
			for (int currentLightIndex = 1; currentLightIndex <= numberOfLights; currentLightIndex++) {
				PointLight currentLightObject = LightArray[currentLightIndex];
				// Draw a Cylinder with a 5 Unit radius
				if (getDistance(position,currentLightObject.position) <= 5.0f){
					float distance = getDistance(position,cameraPosition);
					// Set the Color of that column to the color of the lamp
					screen[currentPixel].color.r = (currentLightObject.color.r);
					screen[currentPixel].color.g = (currentLightObject.color.g);
					screen[currentPixel].color.b = (currentLightObject.color.b);
					screen[currentPixel].distance = distance;
					return 1;
				}
			}
			
			/* ---- Check if a Line has been hit ---- */
			int intersectedLineIndex = checkIfAnyLinesIntersect(previousPosition,position);
			Line* intersectedLineObject = &LineArray[intersectedLineIndex];
			// If a Line has been hit, draw it
			if (intersectedLineIndex) {
				// Get Intersection Point of Ray and Line
				Line RayLine(cameraPosition,position);
				Point intersectionPoint = getIntersectionPoint(RayLine,*intersectedLineObject);
				
				// Reset Position of the Ray to it's previous position
				// (to avoid intersecting with the same line twice when calculating the lighting)
				position = previousPosition;
				
				// Get distance to camera
				screen[currentPixel].distance = getDistance(position, cameraPosition);
				
				// Light Distance Shading
				// Check if we hit anything on our way to a lightsource
				for (int currentLightIndex = 1; currentLightIndex <= numberOfLights; currentLightIndex++) {
					// Get current Light
					PointLight currentLightObject = LightArray[currentLightIndex];
					int lightLineID = checkIfAnyLinesIntersect(position,currentLightObject.position);
					// If no line obstructs the lightsource, add it's value to the screen
					if (lightLineID) {
						screen[currentPixel].color.r += 0;
						screen[currentPixel].color.g += 0;
						screen[currentPixel].color.b += 0;
					} else {
						// Scale brightness acording to distance
						if (getDistance(position,currentLightObject.position) <= currentLightObject.distance) {
							float distanceToLight = getDistance(position,currentLightObject.position);
							float normalizedLight = (1.0-(distanceToLight/currentLightObject.distance)) * currentLightObject.brightness;
							normalizedLight = pow(normalizedLight,2);
							screen[currentPixel].color.r += (currentLightObject.color.r * intersectedLineObject->color.r * normalizedLight);
							screen[currentPixel].color.g += (currentLightObject.color.g * intersectedLineObject->color.g * normalizedLight);
							screen[currentPixel].color.b += (currentLightObject.color.b * intersectedLineObject->color.b * normalizedLight);
						}
					}
				}
				
				// Distance Based Shade	
				float normalizedShade = 1.0f-(getDistance(cameraPosition,position)/maxRenderDistance);
				screen[currentPixel].color.r *= pow(normalizedShade,2);
				screen[currentPixel].color.g *= pow(normalizedShade,2);
				screen[currentPixel].color.b *= pow(normalizedShade,2);	
				return 1;
			} else {
				// Calculate the Floorlight
				Color pointColor;
				pointColor.r = 0.0f;
				pointColor.g = 0.0f;
				pointColor.b = 0.0f;
				// Iterate over every light in the scene
				for (int currentLightIndex = 1; currentLightIndex <= numberOfLights; currentLightIndex++) {
					// Get the current Light
					PointLight currentLight = LightArray[currentLightIndex];
					// Check if any lines cover the path to the lightsource
					int lineCoveringLight = checkIfAnyLinesIntersect(position,currentLight.position);
					// If a line covers up the Lightsource, skip this section
					if (!lineCoveringLight) {
						// If no line is found, calculate the relevant Pixel
						if (getDistance(position,currentLight.position) <= currentLight.distance) {
							float distanceToLight = getDistance(position,currentLight.position);
							float normalizedLight = (1.0f-(distanceToLight/currentLight.distance)) * currentLight.brightness;
							normalizedLight = pow(normalizedLight,2);
							pointColor.r += (currentLight.color.r * floorColor.r * normalizedLight);
							pointColor.g += (currentLight.color.g * floorColor.g * normalizedLight);
							pointColor.b += (currentLight.color.b * floorColor.b * normalizedLight);
						}
					}
				}
				
				// Calculate Distance fall-off
				float normalizedShade = 1.0f-(getDistance(cameraPosition,position)/maxRenderDistance);
				pointColor.r *= pow(normalizedShade,2);
				pointColor.g *= pow(normalizedShade,2);
				pointColor.b *= pow(normalizedShade,2);
				
				// Clamp the result
				pointColor.r = clamp(pointColor.r,0.0f,1.0f);
				pointColor.g = clamp(pointColor.g,0.0f,1.0f);
				pointColor.b = clamp(pointColor.b,0.0f,1.0f);
				// Add it to the FloorLightArray
				FloorLightArray[currentRenderStep + currentPixel*(WINDOW_HEIGHT/2)] = pointColor;
			}
			
			return 0;
		}
};

// Raytrace Render Column
void traceColumn(Ray& currentRay) {	
	// Send out a Ray from the camera
	currentRay.position = cameraPosition;
	// This is where the FoV magically appears!
	currentRay.direction = (cameraRotation + ((fieldOfView/2)*-1)) + (fieldOfView/WINDOW_WIDTH*currentRay.currentPixel);
	
	int maxRenderSteps = WINDOW_HEIGHT/2+1;
	for (currentRenderStep = 0; currentRenderStep <= maxRenderSteps; currentRenderStep++) {
		//while(RayArray[1].bounces < numberOfBounces) {
		Point previousPosition = currentRay.position;
		bool finishedLine = currentRay.step();
		
		// Stop rendering if we hit a wall
		if (finishedLine) {
			break;
		} else {
			// If we never hit a wall, infinite distance
			if (currentRenderStep == maxRenderSteps) {
				screen[currentRay.currentPixel].distance = maxRenderDistance;
				break;
			}
		}
	}
}

// Trace the assigned render sector
int traceRenderSector(Ray& currentRay) {
	int start = currentRay.currentPixel;
	int finish = currentRay.currentPixel + (WINDOW_WIDTH/numberOfRenderSectors);
	while (currentRay.currentPixel < finish) {
		screen[currentRay.currentPixel].color.r = 0;
		screen[currentRay.currentPixel].color.g = 0;
		screen[currentRay.currentPixel].color.b = 0;
		traceColumn(currentRay);
		currentRay.currentPixel++;
	}
	return 0;
}

/* ---- DISPLAYING ---- */
// Load Color as SDL Render Color
int loadColor(const Color& color) {
	SDL_SetRenderDrawColor(
		renderer,
		(int)(clamp(color.r,0.0f,1.0f)*255),
		(int)(clamp(color.g,0.0f,1.0f)*255),
		(int)(clamp(color.b,0.0f,1.0f)*255),
		255
	);	
	return 0;
}

// Update Display
void updateScreen() {
	void *texture_pixels;
	int texture_pitch;
	int lastPos;
	float sliceSize;
	while(running) {
		// Start new frame
		//SDL_SetRenderDrawColor(renderer, 255, 0, 255, 0);
		//SDL_RenderClear(renderer);
		if (frameDone && (lastFrameTime+TICKS_FOR_NEXT_FRAME <= SDL_GetTicks())) {
			SDL_LockTexture(texture, NULL, &texture_pixels, &texture_pitch);
			// Render current Column
			for (int currentColumn = 0; currentColumn < WINDOW_WIDTH ; currentColumn++) {
				ScreenColumn* currentScreenColumn = &screen[currentColumn];
				Color renderColor = currentScreenColumn->color;
				
				// Ground shadows first
				sliceSize = (WINDOW_HEIGHT/6) * (log((currentScreenColumn->distance / maxRenderDistance)));
				
				for (int currentPixel = 0; currentPixel <= WINDOW_HEIGHT; currentPixel++) {
					uint8_t *pixel = (uint8_t *)texture_pixels + currentPixel * texture_pitch + (WINDOW_WIDTH-currentColumn) * 4;
					pixel[0] = (uint8_t)(renderColor.r*255);
					pixel[1] = (uint8_t)(renderColor.g*255);
					pixel[2] = (uint8_t)(renderColor.b*255);
					pixel[3] = 0xFF;
				}
				
				lastPos = WINDOW_HEIGHT;
				for (int lightRow = 1; lightRow <= (WINDOW_HEIGHT/2); lightRow++) {
					float projectedPixel = (WINDOW_HEIGHT/6) * (log(((float)lightRow)/((float)(WINDOW_HEIGHT/2))));
					Color pointColor = FloorLightArray[lightRow + currentColumn*(WINDOW_HEIGHT/2)];
					float resultPixel = (WINDOW_HEIGHT/2-(projectedPixel));
					
					if (projectedPixel < sliceSize) {
						while (lastPos >= resultPixel) {
							uint8_t *pixel = (uint8_t *)texture_pixels + lastPos * texture_pitch + (WINDOW_WIDTH-currentColumn) * 4;
							pixel[0] = (uint8_t)(pointColor.r*255);
							pixel[1] = (uint8_t)(pointColor.g*255);
							pixel[2] = (uint8_t)(pointColor.b*255);
							pixel[3] = 0xFF;
							
							pixel = (uint8_t *)texture_pixels + (WINDOW_HEIGHT-lastPos) * texture_pitch + (WINDOW_WIDTH-currentColumn) * 4;
							pixel[0] = (uint8_t)(pointColor.r*255);
							pixel[1] = (uint8_t)(pointColor.g*255);
							pixel[2] = (uint8_t)(pointColor.b*255);
							pixel[3] = 0xFF;
							lastPos--;
						}
					} else {
						break;
					}
				}
			}
			SDL_UnlockTexture(texture);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			
			// Weirdly bent for some reason,
			// but I guess that's just down to how I shoot my rays
			/*
			for (int currentColumn = 0; currentColumn < WINDOW_WIDTH ; currentColumn++) {
				sliceSize = (WINDOW_HEIGHT/6) * (log((screen[currentColumn].distance / maxRenderDistance)));
				Color renderColor = screen[currentColumn].color;
				loadColor(renderColor);
				SDL_RenderDrawLine(
					renderer,
					WINDOW_WIDTH-currentColumn,
					WINDOW_HEIGHT/2-sliceSize,
					WINDOW_WIDTH-currentColumn,
					WINDOW_HEIGHT/2+sliceSize
				);
			}
			*/
			SDL_RenderPresent(renderer);
			lastFrameTime = SDL_GetTicks();
			//SDL_RenderPresent(renderer);
			//clearScreenBuffer();
			//SDL_RenderPresent(renderer);
			frameDone = false;
		} else {
			SDL_Delay(1);
		}
	}
}

/* --- MAIN ---- */
// Ye olden Main function
int WinMain(int argc, char **argv) {
	printf("Hello, World!\n");
	//srand(time(NULL)); 
	
    //SDL_Init(SDL_INIT_VIDEO);
	SDL_Window *window = SDL_CreateWindow(
		"Ray",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		WINDOW_WIDTH,
		WINDOW_HEIGHT,
		SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE// | SDL_WINDOW_FULLSCREEN_DESKTOP
	);
    CHECK_ERROR(window == NULL, SDL_GetError());
	
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED ); 
	CHECK_ERROR(renderer == NULL, SDL_GetError());
	
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);
			
	skyLight.r = 0.1f;
	skyLight.g = 0.3f;
	skyLight.b = 0.5f;
	
	floorColor.r = 1.0f;
	floorColor.g = 1.0f;
	floorColor.b = 1.0f;
	
	RayArray = (struct Ray *)calloc(numberOfRays+1, sizeof(struct Ray));
	LineArray = (struct Line *)calloc(numberOfLines+1, sizeof(struct Line));
	LightArray = (struct PointLight *)calloc(numberOfLights+1, sizeof(struct PointLight));
	FloorLightArray = (struct Color *)calloc(WINDOW_WIDTH*(WINDOW_HEIGHT/2)+1, sizeof(struct Color));
	
	// Camera Origin
	cameraPosition.x = 640/2;
	cameraPosition.y = 480/3*2-5;
	cameraRotation = 180.0f;
	for (int i = 1; i <= numberOfRays; i++) {
		RayArray[i] = *new Ray();
		RayArray[i].currentPixel = i-1;
	}

	
	// Scene is loaded here
	LineArray[1] = *new Line(0	,0	,640*2,0	,1.0, 0.0, 0.0);
	LineArray[2] = *new Line(640*2,480,640*2,0	,0.0, 1.0, 0.0);
	LineArray[3] = *new Line(0	,480,640*2,480,0.0, 0.0, 1.0);
	LineArray[4] = *new Line(0 	,480,0	,0	,1.0, 1.0, 1.0);
	
	// House
	LineArray[5] = *new Line(100,180,400,180,1.0, 1.0, 1.0);
	LineArray[6] = *new Line(200,180,200,280,1.0, 1.0, 1.0);
	LineArray[7] = *new Line(200,280,300,280,1.0, 1.0, 1.0);
	LineArray[8] = *new Line(400,180,400,480,1.0, 1.0, 1.0);
	
	// House
	LineArray[9] =  *new Line(660,200,680,210,1.0, 1.0, 1.0);
	LineArray[10] = *new Line(680,210,690,230,1.0, 1.0, 1.0);
	LineArray[11] = *new Line(690,230,680,250,1.0, 1.0, 1.0);
	LineArray[12] = *new Line(680,250,660,260,1.0, 1.0, 1.0);
	LineArray[13] = *new Line(660,260,640,250,1.0, 1.0, 1.0);
	LineArray[14] = *new Line(640,250,630,230,1.0, 1.0, 1.0);
	LineArray[15] = *new Line(630,230,640,210,1.0, 1.0, 1.0);
	LineArray[16] = *new Line(640,210,660,200,1.0, 1.0, 1.0);
	
	// Lines
	/*LineArray[1] = *new Line(0					,0					,WINDOW_WIDTH		,0					,0.5, 0.5, 0.5	);
	LineArray[2] = *new Line(WINDOW_WIDTH		,WINDOW_HEIGHT		,WINDOW_WIDTH		,0					,0.5, 0.5, 0.5	);
	LineArray[3] = *new Line(WINDOW_WIDTH		,WINDOW_HEIGHT		,0					,WINDOW_HEIGHT		,0.5, 0.5, 0.5	);
	LineArray[4] = *new Line(0 					,WINDOW_HEIGHT		,0					,0					,0.5, 0.5, 0.5	);
	
	// Smaller Room
	LineArray[5] = *new Line(WINDOW_WIDTH/3		,WINDOW_HEIGHT/3	,WINDOW_WIDTH/9*4	,WINDOW_HEIGHT/3	,1.0, 0.0, 0.0	);
	LineArray[6] = *new Line(WINDOW_WIDTH/9*5	,WINDOW_HEIGHT/3	,WINDOW_WIDTH/3*2	,WINDOW_HEIGHT/3	,1.0, 1.0, 0.0	);
	
	LineArray[7] = *new Line(WINDOW_WIDTH/3*2	,WINDOW_HEIGHT/3*2	,WINDOW_WIDTH/3*2	,WINDOW_HEIGHT/3	,0.0, 1.0, 0.0	);
	LineArray[8] = *new Line(WINDOW_WIDTH/3*2	,WINDOW_HEIGHT/3*2	,WINDOW_WIDTH/3		,WINDOW_HEIGHT/3*2	,0.0, 0.0, 1.0	);
	LineArray[9] = *new Line(WINDOW_WIDTH/3		,WINDOW_HEIGHT/3*2	,WINDOW_WIDTH/3		,WINDOW_HEIGHT/3	,1.0, 1.0, 1.0	);
	*/
	// Lights
	//LightArray[1] = *new PointLight(WINDOW_WIDTH-5	, 300,1.0	,1.0	,1.0	,1.0f	,128.0f	);
	
	LightArray[1] = *new PointLight(640/2		, 480/2	,1.0	,1.0	,1.0	,1.0f	,512.0f	);
	LightArray[2] = *new PointLight(640/5		, 480/5	,1.0	,1.0	,1.0	,1.0f	,512.0f	);
	LightArray[3] = *new PointLight(770	, 110	,1.0	,0.0	,0.0	,1.0f	,512.0f	);
	LightArray[4] = *new PointLight(500	, 260	,0.0	,1.0	,0.0	,1.0f	,512.0f	);
	LightArray[5] = *new PointLight(760	, 350	,0.0	,0.0	,1.0	,1.0f	,512.0f	);
	//LightArray[3] = *new PointLight(WINDOW_WIDTH/5*4	, WINDOW_HEIGHT/3*2	,1.0	,1.0	,1.0	,1.0f	,512.0f	);
	/*LightArray[2] = *new PointLight(WINDOW_WIDTH/4*4	, WINDOW_HEIGHT/4,1.0	,1.0	,1.0	,1.0f	,512.0f	);
	LightArray[3] = *new PointLight(WINDOW_WIDTH/4	, WINDOW_HEIGHT/4*4,1.0	,1.0	,1.0	,1.0f	,512.0f	);
	LightArray[4] = *new PointLight(WINDOW_WIDTH/4*4	, WINDOW_HEIGHT/4*4,1.0	,1.0	,1.0	,1.0f	,512.0f	);
	*/
	/*
	// Light Position
	LightArray[1] = *new PointLight(WINDOW_WIDTH/3	,WINDOW_HEIGHT/2,1.0	,1.0	,1.0	,1.0f	,128.0f	);
	LightArray[2] = *new PointLight(WINDOW_WIDTH/3*2,20.0f			,1.0	,1.0	,1.0	,2.0f	,512.0f	);
	
	//RayArray[1] = *new Ray(WINDOW_WIDTH/10,0,45);
	LineArray[1] = *new Line(0				,WINDOW_HEIGHT-200	,80				, WINDOW_HEIGHT/2	,1.0, 0, 0	);
	LineArray[2] = *new Line(80				,WINDOW_HEIGHT/2	,150			, WINDOW_HEIGHT-30	,1.0,1.0,0	);
	LineArray[3] = *new Line(150			,WINDOW_HEIGHT-30	,WINDOW_WIDTH/2	, WINDOW_HEIGHT-100	,0	,1.0,0	);
	LineArray[4] = *new Line(WINDOW_WIDTH/2	,WINDOW_HEIGHT-100	,300			, WINDOW_HEIGHT-200	,0	,1.0,1.0);
	LineArray[5] = *new Line(300			,WINDOW_HEIGHT-200	,WINDOW_WIDTH	, WINDOW_HEIGHT		,0	,0	,1.0);
	
	Point corner1;
	corner1.x = WINDOW_WIDTH/3;
	corner1.y = 160;
	Point corner2;
	corner2.x = WINDOW_WIDTH/3+80;
	corner2.y = 180;
	Point corner3;
	corner3.x = WINDOW_WIDTH/4;
	corner3.y = 300;
	Point corner4;
	corner4.x = WINDOW_WIDTH/3+70;
	corner4.y = 100;
	
	Color segment1;
	segment1.r = 1.0;
	segment1.g = 0;
	segment1.b = 1.0;
	
	LineArray[6] = *new Line(corner1,corner3,segment1);
	LineArray[7] = *new Line(corner2,corner3,segment1);
	LineArray[8] = *new Line(corner2,corner4,segment1);
	*/
	
	// Threadpools
	std::vector<std::thread> raytraceThreadPool;
	std::vector<std::thread> renderThreadPool;
	std::thread gameplayThread(updateInputs);
	
	renderThreadPool.emplace_back(updateScreen);
	
    while (running) {		
		if (topDown) {
			// Render Lights
			for (int i = 1; i <= numberOfLights; i++) {
				SDL_SetRenderDrawColor(renderer, LightArray[i].color.r*255, LightArray[i].color.g*255, LightArray[i].color.b*255, 255);
				SDL_RenderDrawPoint(renderer, (int)LightArray[i].position.x, (int)LightArray[i].position.y);
			}
			
			
			// Render Lines
			for (int i = 1; i <= numberOfLines; i++) {
				SDL_SetRenderDrawColor(renderer, LineArray[i].color.r*255, LineArray[i].color.g*255, LineArray[i].color.b*255, 255);
				SDL_RenderDrawLine(renderer, (int)LineArray[i].p1.x, (int)LineArray[i].p1.y, (int)LineArray[i].p2.x, (int)LineArray[i].p2.y);
			}
		}
		
		// Raytrace
		// Only trace new frames if last frame is done drawing
		if (!frameDone) {
			for (int currentSector = 0; currentSector < numberOfRenderSectors; currentSector++) {
				//printf("%d,", currentColumn);
				Ray& currentRay = RayArray[currentSector];
				currentRay.currentPixel = (WINDOW_WIDTH/numberOfRenderSectors)*currentSector;
				raytraceThreadPool.emplace_back([&currentRay]() { traceRenderSector(currentRay); });
				//traceColumn(currentRay);
			}
			
			// The magic of threading
			for (auto& t : raytraceThreadPool) {
				if (t.joinable()) {
					t.join();
				}
			}
			frameDone = true;
		}
		
		// Update Screen handled by Render Thread
		//updateScreen();
	
        /*! updates the array of keystates */
		//running = false;
		while ((SDL_PollEvent(&event)) != 0)
		{
			/*! request quit */
			if (event.type == SDL_QUIT) 
			{ 
				running = false;
			}
		}
		
		// Keystrokes are processed in keystroke thread
		//processControls();
    }
	end:
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
	return 0;
}