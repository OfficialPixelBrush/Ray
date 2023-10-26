#include <iostream>  
#include <math.h>
#include <SDL2/SDL.h>

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

float RayStepSize = 5.0f;
int renderSteps = 100;

int numberOfRays = 1;
int numberOfLines = 20;
int numberOfLights = 3;

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

struct Ray *RayArray;
struct Line *LineArray;
struct PointLight *LightArray;
Color screen[WINDOW_WIDTH];
int currentPixel = 0;
Point origin;
int smallestValue = 0;
int largestValue = 0;
float fieldOfView = 90.0f;
bool running = true;
float fovAngle = 0.0f;
float cameraSpeedHorziontal = 0.0f;
float cameraSpeedVertical = 0.0f;
float cameraSpeedRotational = 0.0f;

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
		Line(float x1, float y1, float x2, float y2) {
			p1.x = x1;
			p1.y = y1;
			p2.x = x2;
			p2.y = y2;
			c.r = 1.0;
			c.g = 1.0;
			c.b = 1.0;
		}
		
		Line(Point _p1, Point _p2) {
			p1 = _p1;
			p2 = _p2;
			c.r = 1.0;
			c.g = 1.0;
			c.b = 1.0;
		}
		
		Line(float x1, float y1, float x2, float y2, int r, int g, int b) {
			p1.x = x1;
			p1.y = y1;
			p2.x = x2;
			p2.y = y2;
			c.r = r;
			c.g = g;
			c.b = b;
		}
		
		Line(Point _p1, Point _p2, Color _c) {
			p1 = _p1;
			p2 = _p2;
			c = _c;
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

class Ray {
	public:
		Point position;
		float direction;
		Ray(float x, float y, float _direction) {
			position.x = x;
			position.y = y;
			direction = _direction;
		}
		
		int step() {
			Point previousPosition = position;
			float rad = degreeToRadian(direction);
			position.x += (RayStepSize*sin(rad));
			position.y += (RayStepSize*cos(rad));
			//printf("%d %d -> %d %d\n", (int)previousPosition.x, (int)previousPosition.y, (int)position.x, (int)position.y);
			
			// Check for intersection
			int intersectedLineID = checkIfAnyLinesIntersect(previousPosition,position);
			if (intersectedLineID != 0) {
				// Reset Position
				position = previousPosition;
				
				// Get Intersection Point 
				Line RayLine(previousPosition,position);
				Point intersectionPoint = getIntersectionPoint(RayLine,LineArray[intersectedLineID]);
				
				// Check if we hit anything on our way to a lightsource
				for (int currentLightID = 1; currentLightID <= numberOfLights; currentLightID++) {
					// Get current Light
					PointLight currentLight = LightArray[currentLightID];
					int lightLineID = checkIfAnyLinesIntersect(position,currentLight.position);
					if (lightLineID) {
						//printf("%d: Line #%d Intersects\n",currentPixel,lightLineID);
						//float normalizedShade = getDistance(light,position);
						screen[currentPixel].r += 0;
						screen[currentPixel].g += 0;
						screen[currentPixel].b += 0;
					} else {
						if (getDistance(intersectionPoint,currentLight.position) < currentLight.distance) {
							float distanceToLight = getDistance(intersectionPoint,currentLight.position);
							float normalizedLight = (1.0-(distanceToLight/currentLight.distance)) * currentLight.brightness;
							//printf("NL: %f\n", normalizedLight);
							screen[currentPixel].r += LineArray[intersectedLineID].c.r * normalizedLight;
							screen[currentPixel].g += LineArray[intersectedLineID].c.g * normalizedLight;
							screen[currentPixel].b += LineArray[intersectedLineID].c.b * normalizedLight;
							//printf("%f, %f, %f\n", screen[currentPixel].r, screen[currentPixel].g, screen[currentPixel].b);
							//running = 0;
						}
					}
				}
				
				// Distance Based Shade
				/*
				float maxDistance = renderSteps*RayStepSize;
				//printf("%f\t%f\t=\t%f\n", getDistance(origin,position), maxDistance, (getDistance(origin,position)/maxDistance));
				float normalizedShade = (getDistance(origin,position)/maxDistance)*-1;
				screen[currentPixel].r = LineArray[intersectedLineID].c.r * normalizedShade;
				screen[currentPixel].g = LineArray[intersectedLineID].c.g * normalizedShade;
				screen[currentPixel].b = LineArray[intersectedLineID].c.b * normalizedShade;
				*/
				
				// Line Normals
				//float normalRadian = calculateNormalAngle(LineArray[intersectedLineID]);
				//SDL_RenderDrawLine(renderer, position.x, position.y, (position.x + (20*cos(normalRadian))), position.y + (20*sin(normalRadian)));
				
				// WIP Bouncelight
				// Reflect
				/*
				printf("Ray intersects with Line %d\n", intersectedLineID);
				
				float normalRadian = calculateNormalAngle(LineArray[intersectedLineID]);
				SDL_RenderDrawLine(renderer, position.x, position.y, (position.x + (20*cos(normalRadian))), position.y + (20*sin(normalRadian)));
				
				// Apply new direction based on normal
				direction += radianToDegree(normalRadian) + 180.0f;
				*/
				return 1;
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
		screen[i].r = 0;
		screen[i].g = 0;
		screen[i].b = 0;
	}
	return 0;
}

int WinMain(int argc, char **argv) {
	printf("Hello, World!\n");
	
    SDL_Init(SDL_INIT_VIDEO);
    SDL_CreateWindowAndRenderer(WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
	
	RayArray = (struct Ray *)calloc(numberOfRays+1, sizeof(struct Ray));
	LineArray = (struct Line *)calloc(numberOfLines+1, sizeof(struct Line));
	LightArray = (struct PointLight *)calloc(numberOfLights+1, sizeof(struct PointLight));
	
	// Camera Origin
	origin.x = WINDOW_WIDTH/2;
	origin.y = 10.0f;
	
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
	
	
    while (running) {
		// Start new frame
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		SDL_RenderClear(renderer);
		clearScreenBuffer();
		
		// Update Origin
		origin.x += cameraSpeedHorziontal;
		origin.y += cameraSpeedVertical;
		fovAngle += cameraSpeedRotational;
		
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		
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
		
		// Raytrace
		// Go over every pixel in the Field of View
		for (currentPixel = 0; currentPixel < WINDOW_WIDTH; currentPixel++) {
			// Send out a Ray from the camera
			RayArray[1].position = origin;
			// This is where the FoV magically appears!
			RayArray[1].direction = (fovAngle + ((fieldOfView/2)*-1)) + (fieldOfView/WINDOW_WIDTH*currentPixel);
			
			//
			for (int i = 0; i <= renderSteps; i++) {
				Point previousPosition = RayArray[1].position;
				bool finishedLine = RayArray[1].step();
				SDL_SetRenderDrawColor(renderer, 255, 0, 0, 20);
				SDL_RenderDrawLine(renderer, (int)previousPosition.x, (int)previousPosition.y, (int)RayArray[1].position.x, (int)RayArray[1].position.y);
				if (finishedLine) {
					break;
				}
			}
			//SDL_RenderPresent(renderer);
		}
		
		
		// Render result to screen
		for (currentPixel = 0; currentPixel < WINDOW_WIDTH; currentPixel++) {
			SDL_SetRenderDrawColor(
				renderer,
				(int)(clamp(screen[currentPixel].r,0.0f,1.0f)*255),
				(int)(clamp(screen[currentPixel].g,0.0f,1.0f)*255),
				(int)(clamp(screen[currentPixel].b,0.0f,1.0f)*255),
				255
			);
			SDL_RenderDrawLine(renderer, currentPixel, 0, currentPixel, 10);
			//SDL_RenderPresent(renderer);
		}
	
		// Event Handling
        if (SDL_PollEvent(&event)) {
                switch( event.type ){
                    /* Keyboard event */
                    /* Pass the event data onto PrintKeyInfo() */
                    case SDL_KEYDOWN:
						switch(event.key.keysym.scancode) {
							case 0x1A: // W
								cameraSpeedVertical -= 1.0f;
								break;
							case 0x04: // A
								cameraSpeedHorziontal -= 1.0f;
								break;
							case 0x16: // S
								cameraSpeedVertical += 1.0f;
								break;
							case 0x07: // D
								cameraSpeedHorziontal += 1.0f;
								break;
							case 0x14: // Q
								cameraSpeedRotational -= 1.0f;
								break;
							case 0x08: // E
								cameraSpeedRotational += 1.0f;
								break;
						}
						break;
                    case SDL_KEYUP:
						cameraSpeedVertical = 0;
						cameraSpeedHorziontal = 0;
						cameraSpeedRotational = 0;
                        //PrintKeyInfo( &event.key );
                        break;

                    /* SDL_QUIT event (window close) */
                    case SDL_QUIT:
                        running = 0;
						break;
						
					default:
						break;
                }
		}
		
		SDL_RenderPresent(renderer);
		SDL_Delay(16);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
	return 0;
}