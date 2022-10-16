#include <iostream>
#include <Eigen/Dense>
#include <definitions.h>
#include <math.h>
#include <bitmap_image.hpp>


using Eigen::MatrixXd;
using Eigen::Vector3d;
 

rgb_t FOGCOLOR = {128, 128, 128};

double esfera_Distance(Vector3d punto){
  return punto.norm() - 1;
}

double calculate_Distance(Vector3d punto){
    Vector3d z = punto;
    double dr = 1.0;
    double r = 0.0;
    double ESCAPE_RADIUS = 2.0;
    unsigned char ITERATIONS = 64;
    double theta = 0.0;
    double phi = 0.0;
    double POWER = 8.0; 
    double zr = 0.0;

    for(int x = 0;x < ITERATIONS;x++){
      // Atento a ésto.
      r = z.norm();
      if (r > ESCAPE_RADIUS) {
        break;
      }
      theta = acos(z.z()/r);
      phi = atan2(z.y(),z.x());

      dr = pow(r,POWER - 1.0) * POWER * dr + 1.0;
      zr = pow(r,POWER);
		  theta = theta*POWER;
		  phi = phi*POWER;

      z = zr * Vector3d(sin(theta)*cos(phi), sin(phi)*sin(theta), cos(theta));
		  z = z + punto;
    }
    return 1.1 * log10(r) * (r / dr);

}

Vector3d calculateNormal(Vector3d punto){
  Vector3d gradiente = {1.0,0.0,0.0 };

  gradiente.x() = calculate_Distance(Vector3d { punto.x() + EPSILON, punto.y(), punto.z() }) - calculate_Distance(Vector3d { punto.x() - EPSILON, punto.y(), punto.z() });
  gradiente.y() = calculate_Distance(Vector3d { punto.x(), punto.y() + EPSILON, punto.z() }) - calculate_Distance(Vector3d { punto.x(), punto.y() - EPSILON, punto.z() });
  gradiente.z() = calculate_Distance(Vector3d { punto.x(), punto.y(), punto.z() + EPSILON }) - calculate_Distance(Vector3d { punto.x(), punto.y(), punto.z() - EPSILON});

  gradiente = gradiente * -1.0;

  return gradiente.normalized();
}

rgb_t ilumina(Vector3d punto, double diffuseIntensity, rgb_t colorObjeto){
    rgb_t color = { 50, 0, 50 };
    
    color.red = (colorObjeto.red * diffuseIntensity);
    color.green = (colorObjeto.green * diffuseIntensity);
    color.blue = (colorObjeto.blue * diffuseIntensity);

    return (color);
}

rgb_t raymarching(Vector3d ro, Vector3d rd){
  rgb_t color;
  rgb_t colorObjeto = {80,80,80};
  color.red = 0;
  color.green = 50;
  color.blue = 0;

  Vector3d punto;
  Vector3d directionToLight;
  Vector3d normal;

  double t = 0.0;
  double diffuseIntensity = 2.0;
  double distancia = 0.0;

  Vector3d LIGHT(0.0, -50.0, -130.0);

  for(int x = 0; x < MAXSTEPS; x++){
    punto = ro + rd * t;
    distancia = calculate_Distance(punto);
    //distancia = esfera_Distance(punto);
    if (distancia < MINIMUM_HIT_DISTANCE) {
      directionToLight = (punto - LIGHT).normalized();
      normal = calculateNormal(punto);
      diffuseIntensity = std::max(normal.dot(directionToLight),0.0);
      color = ilumina(punto, diffuseIntensity,  colorObjeto);
      return color;
    }
    t += distancia;
  }
  return FOGCOLOR;
}

int main()
{
  double ndc_x = 0.0;
  double ndc_y = 0.0;

  double pixel_screen_x = 0.0;
  double pixel_screen_y = 0.0;

  double pixel_camera_x = 0.0;
  double pixel_camera_y = 0.0;

  Vector3d EYE(0.0, 0.0, -5.0);
  Vector3d RIGHT(1.0, 0.0, 0.0);
  Vector3d ro(0.0, 0.0, 0.0);
  Vector3d rd(0.0, 0.0, 0.0);
  Vector3d rc(0.0, 0.0, 0.0);

  
  bitmap_image image(IMAGE_WIDTH,IMAGE_HEIGHT);

  double fov = tan(ALPHA / 2.0 * M_PI / 180.0);
  rgb_t color;

  for (int x=0; x < IMAGE_WIDTH; x++)
    for (int y=0; y < IMAGE_HEIGHT; y++)
    {
      ndc_x = (x + CORRECCION) / IMAGE_WIDTH;
      ndc_y = (y + CORRECCION) / IMAGE_HEIGHT;

      pixel_screen_x = 2.0 * ndc_x - 1.0;
      pixel_screen_y = 2.0 * ndc_y - 1.0;

      pixel_camera_x = pixel_screen_x * IMAGE_ASPECT_RATIO ;
      pixel_camera_y = pixel_screen_y;

      ro = EYE;
      rc.x() = pixel_camera_x;
      rc.y() = pixel_camera_y;
      rc.z() = -1.0;

      rd = (rc - ro).normalized();
      color = raymarching(ro,rd);
      
      image.set_pixel(x, y, color);

    }
	  image.save_image("output.bmp");
 
  return 0;
}