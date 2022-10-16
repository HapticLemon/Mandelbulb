#include <Eigen/Dense>

int IMAGE_WIDTH = 800;
int IMAGE_HEIGHT = 600;

// Ángulo para el FOV. Actúa como una especie de zoom.
float ALPHA = 55.0;
float CORRECCION = 0.5;
int MAXSTEPS = 64;
float EPSILON = 0.001;
float MINIMUM_HIT_DISTANCE = 0.0025;

float IMAGE_ASPECT_RATIO = IMAGE_WIDTH / IMAGE_HEIGHT;

