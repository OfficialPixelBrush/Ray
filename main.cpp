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

float RayStepSize = 1.0f;
int renderSteps = 1000;
float maxDistance = RayStepSize * renderSteps;
float fieldOfView = 90.0f;

int numberOfRays = 1;
int numberOfLines = 20;
int numberOfLights = 5;
int numberOfBounces = 0;

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

struct Color {
	float r;
	float g;
	float b;
};
typedef struct Color Color;

struct ScreenPixel {
	Color c;
	float distance;
};
typedef struct ScreenPixel ScreenPixel;

struct Ray *RayArray;
struct Line *LineArray;
struct PointLight *LightArray;
struct Color *BounceArray;
ScreenPixel screen[WINDOW_WIDTH];
int currentPixel = 0;
Point origin;
bool running = true;
float fovAngle = 0.0f;
float cameraSpeedHorziontal = 0.0f;
float cameraSpeedVertical = 0.0f;
float cameraSpeedRotational = 0.0f;
bool topDown = false;
Color skyLight;

// A primitive PointLight
class PointLight {
	public:
		Point position;
		Color c;
		float brightness;
		float distance;
		
	PointLight(Point _p, Color _c, float _brightness, float _distance) {
		position = _p;
		c = _c;
		brightness = _brightness;
		distance = _distance;
	}
	
	PointLight(float x, float y, int r, int g, int b, float _brightness, float _distance) {
		position.x = x;
		position.y = y;
		c.r = r;
		c.g = g;
		c.b = b;
		brightness = _brightness;
		distance = _distance;
	}
};


// A simple line, with a front and back
class Line {
	public:
		Point p1;
		Point p2;
		Color c;
		float roughness;
		Line(float x1, float y1, float x2, float y2) {
			p1.x = x1;
			p1.y = y1;
			p2.x = x2;
			p2.y = y2;
			c.r = 1.0;
			c.g = 1.0;
			c.b = 1.0;
			roughness = 1.0f;
		}
		
		Line(Point _p1, Point _p2) {
			p1 = _p1;
			p2 = _p2;
			c.r = 1.0;
			c.g = 1.0;
			c.b = 1.0;
			roughness = 1.0f;
		}
		
		Line(float x1, float y1, float x2, float y2, int r, int g, int b) {
			p1.x = x1;
			p1.y = y1;
			p2.x = x2;
			p2.y = y2;
			c.r = r;
			c.g = g;
			c.b = b;
			roughness = 1.0f;
		}
		
		Line(Point _p1, Point _p2, Color _c) {
			p1 = _p1;
			p2 = _p2;
			c = _c;
			roughness = 1.0f;
		}
};

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

/*
getNormal(Line& line) {
	float LineSlope = (LineArray[intersectedLineID].p2.y - LineArray[intersectedLineID].p1.y) / (LineArray[intersectedLineID].p2.x - LineArray[intersectedLineID].p1.x);
	LineSlope -= 90.0f * (3.14159265359f/180.0f);
	
}*/

float radianToDegree(float radian) {
	return radian * 180.0f/PI;
}

float degreeToRadian(float degree) {
	return degree * PI/180.0f;
}

float getDistance(Point p1, Point p2) {
	return sqrt((p2.x-p1.x)*(p2.x-p1.x) + (p2.y - p1.y)*(p2.y - p1.y));
}

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

class Ray {
	public:
		Point position;
		float direction;
		int bounces;
		
		Ray(float x, float y, float _direction) {
			position.x = x;
			position.y = y;
			direction = _direction;
		}
		
		Ray() {
			position.x = origin.x;
			position.y = origin.y;
			direction = 0.0f;
		}
		
		int step() {
			Point previousPosition = position;
			float rad = degreeToRadian(direction);
			position.x += (RayStepSize*sin(rad));
			position.y += (RayStepSize*cos(rad));
			
			if (bounces == 0) {
				for (int l = 1; l < numberOfLights; l++) {
					PointLight currentL = LightArray[l];
					if (getDistance(position,currentL.position) < 5.0f){
						float distance = getDistance(position,origin);
						screen[currentPixel].c.r = (currentL.c.r);
						screen[currentPixel].c.g = (currentL.c.g);
						screen[currentPixel].c.b = (currentL.c.b);
						screen[currentPixel].distance = distance;
						return 1;
					}
				}
			}
			//printf("%d %d -> %d %d\n", (int)previousPosition.x, (int)previousPosition.y, (int)position.x, (int)position.y);
			
			// Check for intersection
			int intersectedLineID = checkIfAnyLinesIntersect(previousPosition,position);
			if (intersectedLineID != 0) {
				// Get Intersection Point 
				Line RayLine(origin,position);
				Point intersectionPoint = getIntersectionPoint(RayLine,LineArray[intersectedLineID]);
				
				// Reset Position
				position = previousPosition;
				
				// Get distance to camera
				if (bounces == 0) {
					screen[currentPixel].distance = getDistance(position, origin);
				}/* else {
					bounceDistances[currentPixel][bounces] = getDistance(position, lastBounce);
				}*/
				
				// Light Distance Shading
				// Check if we hit anything on our way to a lightsource
				for (int currentLightID = 1; currentLightID <= numberOfLights; currentLightID++) {
					// Get current Light
					PointLight currentLight = LightArray[currentLightID];
					int lightLineID = checkIfAnyLinesIntersect(position,currentLight.position);
					if (lightLineID) {
						//printf("%d: Line #%d Intersects\n",currentPixel,lightLineID);
						//float normalizedShade = getDistance(light,position);
						screen[currentPixel].c.r += 0;
						screen[currentPixel].c.g += 0;
						screen[currentPixel].c.b += 0;
					} else {
						if (getDistance(position,currentLight.position) < currentLight.distance) {
							float distanceToLight = getDistance(position,currentLight.position);
							float normalizedLight = (1.0-(distanceToLight/currentLight.distance)) * currentLight.brightness;
							normalizedLight = pow(normalizedLight,2);
							//printf("NL: %f\n", normalizedLight);
							screen[currentPixel].c.r += (LineArray[intersectedLineID].c.r * normalizedLight) / pow(bounces+1,2);
							screen[currentPixel].c.g += (LineArray[intersectedLineID].c.g * normalizedLight) / pow(bounces+1,2);
							screen[currentPixel].c.b += (LineArray[intersectedLineID].c.b * normalizedLight) / pow(bounces+1,2);
							//printf("%f, %f, %f\n", screen[currentPixel].r, screen[currentPixel].g, screen[currentPixel].b);
							//running = 0;
						}
					}
				}
				
				// Distance Based Shade				
				
				float normalizedShade = 1-(getDistance(origin,position)/maxDistance);
				screen[currentPixel].c.r *= pow(normalizedShade,2);
				screen[currentPixel].c.g *= pow(normalizedShade,2);
				screen[currentPixel].c.b *= pow(normalizedShade,2);
				
				
				// Line Normals
					/*
				if (topDown) {
					float normalAngle = calculateNormalAngle(LineArray[intersectedLineID]);
					SDL_RenderDrawLine(
						renderer,
						position.x,
						position.y,
						position.x + (20*cos(normalAngle)),
						position.y + (20*sin(normalAngle))
					);
				}*/
				
				// WIP Bouncelight
				// Reflect
				
				//printf("Ray intersects with Line %d\n", intersectedLineID);
				
				float normalAngle = calculateNormalAngle(LineArray[intersectedLineID]);
				direction = getBounceReflection(direction,normalAngle);
				//direction = getBounceReflection((180-(rand()%180)),normalAngle); //(((rand()%1000)/1000)*LineArray[intersectedLineID].roughness)*180.0;
				
				if (normalAngle == 0.0f) {
					direction = direction+180;
					//printf("Direction of Line #%d: %f°\n",intersectedLineID,direction);	
				}
				//direction = getBounceReflection(direction,normalAngle);
				
				
				if (bounces >= numberOfBounces) {
					return 1;
				} else {
					//lastBounce = position;
					bounces++;
				}
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
	for (int i = 0; i < WINDOW_WIDTH; i++) {
		screen[i].c.r = 0;
		screen[i].c.g = 0;
		screen[i].c.b = 0;
	}
	return 0;
}

void drawSky() {
	Color localSkyLight = skyLight;
	localSkyLight.r*=255;
	localSkyLight.g*=255;
	localSkyLight.b*=255;
	if (!topDown) {
		SDL_SetRenderDrawColor(renderer, localSkyLight.r, localSkyLight.g, localSkyLight.b, 255);
		for (int i = 0; i < WINDOW_HEIGHT/2; i++) {
			SDL_RenderDrawLine(renderer, 0, i, WINDOW_WIDTH, i);
		}
		SDL_SetRenderDrawColor(renderer, localSkyLight.r/3, localSkyLight.g/3, localSkyLight.b/3, 255);
		for (int i = WINDOW_HEIGHT/2; i < WINDOW_HEIGHT; i++) {
			SDL_RenderDrawLine(renderer, 0, i, WINDOW_WIDTH, i);
		}
	}
}

int processControls() {
		const Uint8 *keys = SDL_GetKeyboardState(NULL);
		cameraSpeedHorziontal /= 1.2;
		cameraSpeedVertical /= 1.2;
		cameraSpeedRotational /= 1.2;
		
		// Forward
        if (keys[SDL_SCANCODE_W]) {
			cameraSpeedHorziontal += 1.0f*sin(degreeToRadian(fovAngle));
			cameraSpeedVertical += 1.0f*cos(degreeToRadian(fovAngle));
		}
		
		// Backward
        if (keys[SDL_SCANCODE_S]) {
			cameraSpeedHorziontal -= 1.0f*sin(degreeToRadian(fovAngle));
			cameraSpeedVertical -= 1.0f*cos(degreeToRadian(fovAngle));
		}
		
		// Left
        if (keys[SDL_SCANCODE_A]) {
			cameraSpeedHorziontal += 1.0f*sin(degreeToRadian(fovAngle+90));
			cameraSpeedVertical += 1.0f*cos(degreeToRadian(fovAngle+90));
		}
		
		// Right
        if (keys[SDL_SCANCODE_D]) {
			cameraSpeedHorziontal += 1.0f*sin(degreeToRadian(fovAngle-90));
			cameraSpeedVertical += 1.0f*cos(degreeToRadian(fovAngle-90));
		}
		
		// Look-Left
        if (keys[SDL_SCANCODE_Q]) {
			cameraSpeedRotational += 1.0f;
		}
		
		// Look-Right
        if (keys[SDL_SCANCODE_E]) {
			cameraSpeedRotational -= 1.0f;
		}
		return 0;
}

int WinMain(int argc, char **argv) {
	printf("Hello, World!\n");
	srand(time(NULL)); 
	
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
			
	skyLight.r = 0.1f;
	skyLight.g = 0.3f;
	skyLight.b = 0.5f;
	
	RayArray = (struct Ray *)calloc(numberOfRays+1, sizeof(struct Ray));
	LineArray = (struct Line *)calloc(numberOfLines+1, sizeof(struct Line));
	LightArray = (struct PointLight *)calloc(numberOfLights+1, sizeof(struct PointLight));
	
	// Camera Origin
	origin.x = WINDOW_WIDTH/2;
	origin.y = WINDOW_HEIGHT/3*2-5;
	fovAngle = 180.0f;
	RayArray[1] = *new Ray();

	// Scene is loaded here
	
	// Lines
	LineArray[1] = *new Line(0					,0					,WINDOW_WIDTH		,0					,0.5, 0.5, 0.5	);
	LineArray[2] = *new Line(WINDOW_WIDTH		,WINDOW_HEIGHT		,WINDOW_WIDTH		,0					,0.5, 0.5, 0.5	);
	LineArray[3] = *new Line(WINDOW_WIDTH		,WINDOW_HEIGHT		,0					,WINDOW_HEIGHT		,0.5, 0.5, 0.5	);
	LineArray[4] = *new Line(0 					,WINDOW_HEIGHT		,0					,0					,0.5, 0.5, 0.5	);
	
	// Smaller Room
	LineArray[5] = *new Line(WINDOW_WIDTH/3		,WINDOW_HEIGHT/3	,WINDOW_WIDTH/9*4	,WINDOW_HEIGHT/3	,1.0, 0.0, 0.0	);
	LineArray[6] = *new Line(WINDOW_WIDTH/9*5	,WINDOW_HEIGHT/3	,WINDOW_WIDTH/3*2	,WINDOW_HEIGHT/3	,1.0, 1.0, 0.0	);
	
	LineArray[7] = *new Line(WINDOW_WIDTH/3*2	,WINDOW_HEIGHT/3*2	,WINDOW_WIDTH/3*2	,WINDOW_HEIGHT/3	,0.0, 1.0, 0.0	);
	LineArray[8] = *new Line(WINDOW_WIDTH/3*2	,WINDOW_HEIGHT/3*2	,WINDOW_WIDTH/3		,WINDOW_HEIGHT/3*2	,0.0, 0.0, 1.0	);
	LineArray[9] = *new Line(WINDOW_WIDTH/3		,WINDOW_HEIGHT/3*2	,WINDOW_WIDTH/3		,WINDOW_HEIGHT/3	,1.0, 1.0, 1.0	);
	
	// Lights
	//LightArray[1] = *new PointLight(WINDOW_WIDTH-5	, 300,1.0	,1.0	,1.0	,1.0f	,128.0f	);
	LightArray[1] = *new PointLight(WINDOW_WIDTH/2		, WINDOW_HEIGHT/2	,1.0	,1.0	,1.0	,1.0f	,512.0f	);
	LightArray[2] = *new PointLight(WINDOW_WIDTH/5		, WINDOW_HEIGHT/5	,1.0	,1.0	,1.0	,1.0f	,512.0f	);
	//LightArray[3] = *new PointLight(WINDOW_WIDTH/5*4	, WINDOW_HEIGHT/3*2	,1.0	,1.0	,1.0	,1.0f	,512.0f	);
	/*LightArray[2] = *new PointLight(WINDOW_WIDTH/4*4	, WINDOW_HEIGHT/4,1.0	,1.0	,1.0	,1.0f	,512.0f	);
	LightArray[3] = *new PointLight(WINDOW_WIDTH/4	, WINDOW_HEIGHT/4*4,1.0	,1.0	,1.0	,1.0f	,512.0f	);
	LightArray[4] = *new PointLight(WINDOW_WIDTH/4*4	, WINDOW_HEIGHT/4*4,1.0	,1.0	,1.0	,1.0f	,512.0f	);
	*/
	/*
	// Light Position
	LightArray[1] = *new PointLight(WINDOW_WIDTH/3	,WINDOW_HEIGHT/2,1.0	,1.0	,1.0	,1.0f	,128.0f	);
	LightArray[2] = *new PointLight(WINDOW_WIDTH/3*2,20.0f			,1.0	,1.0	,1.0	,2.0f	,512.0f	);
	
	RayArray[1] = *new Ray(WINDOW_WIDTH/10,0,45);
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
	
	float maxSpeed = 10.0f;
	float lightMotion = 5.0;
	
    while (running) {
		// LightAnimation
		if (LightArray[2].position.x < WINDOW_WIDTH/5) {
			lightMotion = 5.0f;
		}
		
		if (LightArray[2].position.x > WINDOW_WIDTH/5*4) {
			lightMotion = -5.0;
		}
		
		LightArray[2].position.x+=lightMotion;
		
		// Start new frame
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		clearScreenBuffer();
		
		// Update Origin
		origin.x += cameraSpeedHorziontal;
		origin.y += cameraSpeedVertical;
		fovAngle += cameraSpeedRotational;
		
		if (cameraSpeedHorziontal >= maxSpeed) {
			cameraSpeedHorziontal = maxSpeed;
		}
		if (cameraSpeedVertical >= maxSpeed) {
			cameraSpeedVertical = maxSpeed;
		}
		if (cameraSpeedRotational >= maxSpeed) {
			cameraSpeedRotational = maxSpeed;
		}
		
		if (topDown) {
			// Render Lights
			for (int i = 1; i <= numberOfLights; i++) {
				SDL_SetRenderDrawColor(renderer, LightArray[i].c.r*255, LightArray[i].c.g*255, LightArray[i].c.b*255, 255);
				SDL_RenderDrawPoint(renderer, (int)LightArray[i].position.x, (int)LightArray[i].position.y);
			}
			
			
			// Render Lines
			for (int i = 1; i <= numberOfLines; i++) {
				SDL_SetRenderDrawColor(renderer, LineArray[i].c.r*255, LineArray[i].c.g*255, LineArray[i].c.b*255, 255);
				SDL_RenderDrawLine(renderer, (int)LineArray[i].p1.x, (int)LineArray[i].p1.y, (int)LineArray[i].p2.x, (int)LineArray[i].p2.y);
			}
		}
		
		// Raytrace
		// Go over every pixel in the Field of View
		for (currentPixel = 0; currentPixel < WINDOW_WIDTH; currentPixel++) {
			// Send out a Ray from the camera
			RayArray[1].position = origin;
			// Reset number of successful bounces
			RayArray[1].bounces = 0;
			// This is where the FoV magically appears!
			RayArray[1].direction = (fovAngle + ((fieldOfView/2)*-1)) + (fieldOfView/WINDOW_WIDTH*currentPixel);
			
			for (int i = 0; i <= renderSteps; i++) {
			//while(RayArray[1].bounces < numberOfBounces) {
				Point previousPosition = RayArray[1].position;
				bool finishedLine = RayArray[1].step();
				if (topDown && !(currentPixel%60)) {
					SDL_SetRenderDrawColor(renderer, 255, 0, 0, 20);
					SDL_RenderDrawLine(
						renderer,
						(int)previousPosition.x,
						(int)previousPosition.y,
						(int)RayArray[1].position.x,
						(int)RayArray[1].position.y
					);
				}
				
				// Stop rendering if we hit a wall
				if (finishedLine) {
					break;
				} else {				
					// If we never hit a wall, infinite distance
					if ((i == renderSteps) && (RayArray[1].bounces == 0)) {
						screen[currentPixel].distance = maxDistance;
						break;
					}
				}
			}
			//SDL_RenderPresent(renderer);
			//running = false;
		}
		
		
		// Render result to screen
		drawSky();
		for (currentPixel = 0; currentPixel < WINDOW_WIDTH ; currentPixel++) {
			Color renderColor = screen[currentPixel].c;
			renderColor.r += skyLight.r/4;
			renderColor.g += skyLight.g/4;
			renderColor.b += skyLight.b/4;
			SDL_SetRenderDrawColor(
				renderer,
				(int)(clamp(renderColor.r,0.0f,1.0f)*255),
				(int)(clamp(renderColor.g,0.0f,1.0f)*255),
				(int)(clamp(renderColor.b,0.0f,1.0f)*255),
				255
			);
			
			if (topDown) {
				SDL_RenderDrawLine(
					renderer,
					WINDOW_WIDTH-currentPixel,
					0,
					WINDOW_WIDTH-currentPixel,
					10);
			} else {
				// Weirdly bent for some reason,
				// but I guess that's just down to how I shoot my rays
				float sliceSize = (WINDOW_HEIGHT/6) * log((screen[currentPixel].distance / maxDistance));
				//printf("%f\n",sliceSize);
				SDL_RenderDrawLine(
					renderer,
					WINDOW_WIDTH-currentPixel,
					WINDOW_HEIGHT/2-sliceSize,
					WINDOW_WIDTH-currentPixel,
					WINDOW_HEIGHT/2+sliceSize
				);
			}
			//SDL_RenderPresent(renderer);
		}
		SDL_RenderPresent(renderer);
	
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
		//running = false;
		SDL_Delay(16);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
	return 0;
}