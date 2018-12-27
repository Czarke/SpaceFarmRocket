/*
 *  Textures and Lighting
 *
 *  Demonstrates using lighting and textures.
 *
 *  Key bindings:
 *  l          Toggle lighting on/off
 *  t          Change textures
 *  m          Toggles texture mode modulate/replace
 *  a/A        decrease/increase ambient light
 *  d/D        decrease/increase diffuse light
 *  s/S        decrease/increase specular light
 *  e/E        decrease/increase emitted light
 *  n/N        Decrease/increase shininess
 *  []         Lower/rise light
 *  x          Toggle axes
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */



#include <math.h>
#include "CSCIx229.h"
#define PI 3.1415927
int mode=0;       //  Texture mode
int ntex=0;       //  Cube faces
int axes=1;       //  Display axes
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int light=1;      //  Lighting
int rep=1;        //  Repitition
double asp=1;     //  Aspect ratio
double dim=3.0;   //  Size of world
double inc=0;     //  Variable for ship y value increase
double arm=0;     // Variable for arms z value change
int launch = 0;   // Launch button value
// Light values
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shiny   =   1;    // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   0;  // Elevation of light
double t = 0;
double t2 = 0;
unsigned int texture[14]; // Texture names
typedef struct {float r,g,b;} color_t;
color_t color[7] = {{1.0,1.0,1.0},{0.5,0.5,0.5},{0.5,0.0,0.0},{1.0,0.5,0.0},{1.0,1.0,0.0},{0.0,0.5,0.0},{0.0,0.0,0.5}};
int K=5;           //  Color

/*
#include "Flame.h"
#include "Shader.h"
#include <gtc/matrix_transform.hpp>
#include <iostream>

//expand for KDTree implementation
#pragma region KDTree

//recursive constructor finds median of a set using the median of three algorithm
KDTree::KDTree(int _depth, std::vector<Particle*> _particles)
{
  //intialise median to centre of set
  int median = _particles.size()/2;
  //if there are 3 or more particles in the set find the median using median of three algorithm
  if (_particles.size() >= 3)
  {
    int mid;
    //depending on the depth, split by a different axis
    switch(_depth % 3)
    {
    case 1:
      //median of three algorithm
      mid = FindMedian(_particles.front()->GetPos().x, _particles[median]->GetPos().x, _particles.back()->GetPos().x);
      //switch the middle value to the centre of the set
      switch (mid)
      {
      case 1:
        Swap(_particles,0,median);
        break;
      case 3:
        Swap(_particles,_particles.size()-1,median);
        break;
      }
      break;
    case 2:
      mid = FindMedian(_particles.front()->GetPos().y, _particles[median]->GetPos().y, _particles.back()->GetPos().y);
      switch (mid)
      {
      case 1:
        Swap(_particles,0,median);
        break;
      case 3:
        Swap(_particles,_particles.size()-1,median);
        break;
      }
      break;
    case 0:
      mid = FindMedian(_particles.front()->GetPos().z, _particles[median]->GetPos().z, _particles.back()->GetPos().z);
      switch (mid)
      {
      case 1:
        Swap(_particles,0,median);
        break;
      case 3:
        Swap(_particles,_particles.size()-1,median);
        break;
      }
      break;
    }   
  }
  //assign the point to the median of the set
  m_point = _particles[median];
  m_left = nullptr;
  m_right = nullptr;
  m_depth = _depth;
  std::vector<Particle*> leftSubSet;
  std::vector<Particle*> rightSubSet;

  //create subsets out of the left over particles
  for (int i = 0; i < median; i++)
  {
    leftSubSet.push_back(_particles[i]);
  }

  for (int i = median+1; i < _particles.size(); i++)
  {
    rightSubSet.push_back(_particles[i]);
  }

  //if the subsets are larger than 0 recursively create a new tree node
  if (rightSubSet.size() > 0)
  {
    m_right = new KDTree(_depth+1, rightSubSet);
  }
  if (leftSubSet.size() > 0)
  {
    m_left = new KDTree(_depth+1, leftSubSet);
  }
}


int KDTree::FindMedian(float _start, float _middle, float _end)
{
  //median of three algorithm, return 1, 2 or 3 depending on if the median value is the start, middle or end of the set respectively
  if (_start > _end)
  {
    if (_middle > _start)
    {
      return 1;
    }
    if (_end > _middle)
    {
      return 3;
    }
    return 2;
  }
  else
  {
    if (_start > _middle)
    {
      return 1;
    }
    if (_middle > _end)
    {
      return 3;
    }
    return 2;
  }
}

//simple function to swap the given values within a vector
void KDTree::Swap(std::vector<Particle*> _particles, int _index1, int _index2)
{
  Particle* temp = _particles[_index1];
  _particles[_index1] = _particles[_index2];
  _particles[_index2] = temp;
}

//recursively go down the tree and delete nodes on the way back up
void KDTree::DestroyTree()
{
  if(m_right != nullptr)
  {
    m_right->DestroyTree();
    delete m_right;
  }
  if(m_left != nullptr)
  {
    m_left->DestroyTree();
    delete m_left;
  }
}

//print function for debugging
void KDTree::PrintTree()
{
  //print the tree in order from below the node that called the function
  if (m_left != nullptr)
  {
    m_left->PrintTree();
  }

  std::cout << m_point->GetPos().x << " " << m_point->GetPos().y << " " << m_point->GetPos().z << " : " << m_depth << std::endl;

  if (m_right != nullptr)
  {
    m_right->PrintTree();
  }
}

//recursive function to find all the particles within a range of a particle
void KDTree::FindParticlesInRange(float _squaredRange, glm::vec3 _position, std::vector<Particle*> &_particlesInRange, float* _largestDistanceInRange)
{
  ///set the distance between the node being searched and the particle
  float distance = Magnitude(_position - m_point->GetPos());
  //if the distance is smaller than the smallest distance which is greater than the range checked so far, continue with checks
  if (distance < *_largestDistanceInRange)
  {
    //if the distance is within the range, add this node's particle to the vector
    if (distance <= _squaredRange)
    {
      _particlesInRange.push_back(m_point);
    }
    //otherwise lower the smallest distance which is greater than the range
    else
    {
      *_largestDistanceInRange = distance;
    }
    //if there are nodes beneath the one being checked, call the function in those nodes
    if (m_left != nullptr)
    {
      m_left->FindParticlesInRange(_squaredRange,_position, _particlesInRange, _largestDistanceInRange);
    }
    if (m_right != nullptr)
    {
      m_right->FindParticlesInRange(_squaredRange,_position, _particlesInRange, _largestDistanceInRange);
    }
  }
  //else discard this node being checked and it's children
}

#pragma endregion

//expand for Flame implementation
#pragma region Flame

//flame constructor 
Flame::Flame(Material* _material, unsigned int* _deltaTime)
{
  m_deltaTime = _deltaTime;
  m_material = _material;
  //generate a VAO index
  m_VAO = 0;
  glGenVertexArrays( 1, &m_VAO );
  //generate a VBO index
  m_posBuffer = 0;
  glGenBuffers(1, &m_posBuffer);
  //bind the buffer and push initial data to it
  glBindBuffer(GL_ARRAY_BUFFER, m_posBuffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * NUMBEROFPARTICLES * 3, &m_positions[0], GL_STATIC_DRAW);
  m_numParticles = 0;
  //create the first particle
  m_particles.push_back(new Particle);
}

//flame deconstructor
Flame::~Flame()
{
  //delete all particles
  for(unsigned int i = 0; i < m_particles.size(); i++)
  {
    delete m_particles[i];
  }
  //delete the VAO
  glDeleteVertexArrays(1,&m_VAO);
}

//update function to update particles and regenerate KDTree
void Flame::Update()
{
  float maxDist = 100000.0f;
  //fill positions array with -5 in order to not show any unupdated particles above the ground
  std::fill_n(m_positions,NUMBEROFPARTICLES*3,-5);
  //if it won't go over the maximum number of particles add new particles each frame
  if (m_particles.size() < NUMBEROFPARTICLES - 6)
  {
    for (int i = 0; i < 6; i++)
    {
      m_particles.push_back(new Particle);
    }
  }
  //generate a new KDTree of all the particles
  m_root = new KDTree(2,m_particles);
  //for every particle
  for (unsigned int i = 0; i < m_particles.size(); i++)
  {
    //zero the vector of particles within range
    m_particlesInRange.clear();
    //reset the maximum distance used in the FindParticlesInRange() function
    maxDist = 100000.0f;
    //find the particles within a range of 0.3 units
    m_root->FindParticlesInRange(0.3f,m_particles[i]->GetPos(),m_particlesInRange,&maxDist);
    //update the particle
    m_particles[i]->Update(m_particlesInRange, *m_deltaTime);
    //if the particle has reached the end of its lifetime delete it and step back a particle in order to not miss any out
    if (m_particles[i]->GetLifetime() < 0)
    {
      delete m_particles[i];
      m_particles.erase(m_particles.begin()+i);
      i--;
    }
    //otherwise push it's position to the positions array
    else
    {
      m_positions[(i*3)] = m_particles[i]->GetPos().x;
      m_positions[(i*3)+1] = m_particles[i]->GetPos().y;
      m_positions[(i*3)+2] = m_particles[i]->GetPos().z;
    }
  }
  //delete the tree
  m_root->DestroyTree();
  delete m_root;
  //bind the array and buffer object
  glBindVertexArray( m_VAO );
  glBindBuffer(GL_ARRAY_BUFFER, m_posBuffer);
  //use glBufferSubData to stream position data to the graphics card
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * NUMBEROFPARTICLES * 3, &m_positions[0]);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0 );
  //unbind the VAO
  glBindVertexArray(0);
}

void Flame::Draw()
{
  //disable the depth mask in order to stop depth writes and thus blending artifacts while keeping the flame from drawing over other objects
  glDepthMask(false);
  //ready the shader for drawing
  m_material->ReadyForDraw(glm::mat4(1.0f));
  //bind the VAO and draw it
  glBindVertexArray(m_VAO);
  glEnableVertexAttribArray(0);
  glDrawArrays(GL_POINTS, 0, NUMBEROFPARTICLES);
  //unbind appropriate data
  glDisableVertexAttribArray(0);
  glBindVertexArray(0);
  //re-enable the depth mask
  glDepthMask(true);
}

#pragma endregion


*/



static void ground(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(th,1,0,0);
   glScaled(dx,dy,dz);
   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glColor3f(1,1,1);
   glBindTexture(GL_TEXTURE_2D,texture[11]);
   

   glColor3f(0,1,1);
   if (ntex) glBindTexture(GL_TEXTURE_2D,texture[11]);
   glBegin(GL_QUADS);
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   
   //  Undo transformations and textures
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
}

void cylinder(GLfloat radius,
                   GLfloat height,
                   float R, float G,
                   float B, double x, double y, double z, double th)
{

   float black[] = {0,0,0,1};
   float color[] = {R,G,B,1};
   float shiny = 16;
   glColor3f(R,G,B);
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,color);
   glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,color);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,color);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   //  Save transformation
   glPushMatrix();
   glTranslated(x, y, z);
   glRotated(th, 0, 1, 0);

   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glColor3f(1,1,1);
   glBindTexture(GL_TEXTURE_2D, texture[8]);

    GLfloat angle          = 0.0;
    GLfloat angle_stepsize = 0.1;

    /** Draw the tube */
    glBindTexture(GL_TEXTURE_2D,texture[8]);
    glBegin(GL_QUAD_STRIP);
    angle = 0.0;
        while( angle < 2*PI ) {
            x = radius * cos(angle);
            y = radius * sin(angle);
            glNormal3f(x, y, 0);
            glTexCoord2f( angle, 1.0 );
            glVertex3f(x, y , height);
            glTexCoord2f( angle, 0.0 );
            glVertex3f(x, y , 0.0);
            angle = angle + angle_stepsize;
        }
        glVertex3f(radius, 0.0, height);
        glVertex3f(radius, 0.0, 0.0);
    glEnd();

    /** Draw the circle on top of cylinder */
    glBindTexture(GL_TEXTURE_2D,texture[8]);
    glNormal3f(0,0,1);
    glBegin(GL_POLYGON);
    angle = 0.0;
        while( angle < 2*PI ) {
            x = radius * cos(angle);
            y = radius * sin(angle);
            glNormal3f(x,y,0);
            glTexCoord2f( 0.5f * cos(angle) + 0.5f, 0.5f * sin(angle) + 0.5f );
            glVertex3f(x, y , height);
            angle = angle + angle_stepsize;
        }
        glVertex3f(radius, 0.0, height);
    glEnd();
    /** Draw the circle on bottom of cylinder*/
    glBindTexture(GL_TEXTURE_2D,texture[8]);
    glBegin(GL_POLYGON);
    angle = 0.0;
        while( angle < 2*PI ) {
            x = radius * cos(angle);
            y = radius * sin(angle);
            glNormal3f(x,y,0);
            glTexCoord2f( 0.5f * cos(angle) + 0.5f, 0.5f * sin(angle) + 0.5f );
            glVertex3f(x, y , 0);
            angle = angle + angle_stepsize;
        }
        glVertex3f(radius, 0.0, height);
    glEnd();
    glPopMatrix();
}

void cone(GLfloat radius,
                   GLfloat height,
                   float R, float G,
                   float B, double x, double y, double z, double th)
{

   float black[] = {0,0,0,1};
   float color[] = {R,G,B,1};
   float shiny = 16;
   glColor3f(R,G,B);
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,color);
   glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,color);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,color);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   //  Save transformation
   glPushMatrix();
   glTranslated(x, y, z);
   glRotated(th, 0, 1, 0);

   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glColor3f(1,1,1);
   glBindTexture(GL_TEXTURE_2D, texture[9]);

    GLfloat angle          = 0.0;
    GLfloat angle_stepsize = 0.1;

    /** Draw the top */
    glBindTexture(GL_TEXTURE_2D,texture[9]);
    glBegin(GL_TRIANGLE_STRIP);
    angle = 0.0;
        while( angle <= 2*PI + angle_stepsize ) {
            x = radius * cos(angle);
            y = radius * sin(angle);
            glNormal3f(x, y, 0);
            glTexCoord2f( angle, 1.0 );
            glVertex3f(0, 0, height);
            glTexCoord2f( angle, 0.0 );
            glVertex3f(x, y , 0.0);
            angle = angle + angle_stepsize;
        }

    glEnd();


    /** Draw the circle on bottom of cylinder*/
    glBindTexture(GL_TEXTURE_2D,texture[9]);
    glBegin(GL_POLYGON);
    angle = 0.0;
        while( angle < 2*PI ) {
            x = radius * cos(angle);
            y = radius * sin(angle);
            glNormal3f(x,y,0);
            glTexCoord2f( 0.5f * cos(angle) + 0.5f, 0.5f * sin(angle) + 0.5f );
            glVertex3f(x, y , 0);
            angle = angle + angle_stepsize;
        }
    glEnd();
    glPopMatrix();
}

void ring(GLfloat radius, GLfloat iradius,
                   GLfloat height,
                   float R, float G,
                   float B, double x, double y, double z, double th, double botSize)
{

   float black[] = {0,0,0,1};
   float color[] = {R,G,B,1};
   float shiny = 16;
   glColor3f(R,G,B);
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,color);
   glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,color);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,color);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   //  Save transformation
   glPushMatrix();
   glTranslated(x, y, z);
   glRotated(th, 0, 1, 0);

   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glColor3f(1,1,1);
   glBindTexture(GL_TEXTURE_2D, texture[9]);

    GLfloat angle          = 0.0;
    GLfloat angle_stepsize = 0.1;

    /** Draw the tube */
    glBindTexture(GL_TEXTURE_2D,texture[9]);
    glBegin(GL_QUAD_STRIP);
    angle = 0.0;
        while( angle < 2*PI + angle_stepsize) {
            x = radius * cos(angle);
            y = radius * sin(angle);
            glNormal3f(x, y, 0);
            glTexCoord2f( angle, 1.0 );
            glVertex3f(x, y , height);
            glTexCoord2f( angle, 0.0 );
            glVertex3f(x * botSize, y * botSize, 0.0);
            angle = angle + angle_stepsize;
        }
        glVertex3f(radius, 0.0, 0.0);
    glEnd();

    /** Draw the inner tube */
    glBindTexture(GL_TEXTURE_2D,texture[9]);
    glBegin(GL_QUAD_STRIP);
    angle = 0.0;
        while( angle < 2*PI + angle_stepsize) {
            x = (radius - iradius) * cos(angle);
            y = (radius - iradius) * sin(angle);
            glNormal3f(-x, -y, 0);
            glTexCoord2f( angle, 1.0 );
            glVertex3f(x, y , height);
            glTexCoord2f( angle, 0.0 );
            glVertex3f(x * botSize, y * botSize, 0.0);
            angle = angle + angle_stepsize;
        }
        glVertex3f(iradius, 0.0, 0.0);
    glEnd();

    /** Draw the end **/
    glBindTexture(GL_TEXTURE_2D,texture[9]);
    glBegin(GL_QUAD_STRIP);
    angle = 0.0;
        while( angle < 2*PI + angle_stepsize) {
            x = (radius) * cos(angle);
            y = (radius) * sin(angle);
            glTexCoord2f( angle, 0.0 );
            glVertex3f(x * botSize, y * botSize , 0.0);
            x = (radius - iradius) * cos(angle);
            y = (radius - iradius) * sin(angle);
            glTexCoord2f(angle, 0.0);
            glVertex3f(x * botSize, y * botSize, 0.0);
            angle = angle + angle_stepsize;
        }
        glVertex3f(iradius, 0.0, 0.0);
    glEnd();

        /** Draw the top **/
    glBindTexture(GL_TEXTURE_2D,texture[9]);
    glBegin(GL_QUAD_STRIP);
    angle = 0.0;
        while( angle < 2*PI + angle_stepsize) {
            x = (radius) * cos(angle);
            y = (radius) * sin(angle);
            glTexCoord2f( angle, 1.0 );
            glVertex3f(x, y , height);
            x = (radius - iradius) * cos(angle);
            y = (radius - iradius) * sin(angle);
            glTexCoord2f(angle, 0.0);
            glVertex3f(x, y, height);
            angle = angle + angle_stepsize;
        }
        glVertex3f(iradius, 0.0, 0.0);
    glEnd();



    glPopMatrix();
}

void triangle(double x, double y, double z, double dx, double dy, double dz,
   double th, double ph, float R, float G, float B){

   float black[] = {0,0,0,1};
   float color[] = {R,G,B,1};
   float shiny = 16;
   glColor3f(R,G,B);
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_AMBIENT,color);
   glMaterialfv(GL_FRONT_AND_BACK,GL_DIFFUSE,color);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,color);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glColor3f(1,1,1);
   glBindTexture(GL_TEXTURE_2D,texture[12]);

   glPushMatrix();
   glTranslated(x, y, z);
   glScaled(dx, dy, dz);
   glRotated(ph, 0, 1, 0);
   glRotated(th, 1, 0, 0); // rotate the pyramid
   //glColor3f(R, G, B);

   glBegin(GL_POLYGON);
   glNormal3f(0, 0, 1);
   glTexCoord2f(0.5,0);
   glVertex3f(0, 1, 0);
   glTexCoord2f(0,0);
   glVertex3f(1, 0, 0);
   glTexCoord2f(0.5,0.5);
   glVertex3f(-1, 0, 0);
   glEnd();

   glPopMatrix();

}

static void cube(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);

   glRotated(th,0,0,1);
   glScaled(dx,dy,dz);
   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glColor3f(1,1,1);
   glBindTexture(GL_TEXTURE_2D,texture[8]);
   //  Front
   if (ntex) glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_QUADS);
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
   glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
   glTexCoord2f(0,1); glVertex3f(-1,+1, 1);
   glEnd();
   //  Back
   if (ntex) glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_QUADS);
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
   glEnd();
   //  Right
   if (ntex) glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_QUADS);
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,+1);
   glEnd();
   //  Left
   if (ntex) glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   //  Top
   if (ntex) glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_QUADS);
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   //  Bottom
   if (ntex) glBindTexture(GL_TEXTURE_2D,texture[8]);
   glBegin(GL_QUADS);
   glNormal3f( 0,-1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
   glEnd();
   //  Undo transformations and textures
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
}

static void base(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float Emission[]  = {0.0,0.0,0.01*emission,1.0};
   glMaterialf(GL_FRONT_AND_BACK,GL_SHININESS,shiny);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);

   glRotated(th,0,0,1);
   glScaled(dx,dy,dz);
   //  Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,mode?GL_REPLACE:GL_MODULATE);
   glColor3f(1,1,1);
   glBindTexture(GL_TEXTURE_2D,texture[13]);
   //  Front
   if (ntex) glBindTexture(GL_TEXTURE_2D,texture[13]);
   glBegin(GL_QUADS);
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
   glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
   glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
   glTexCoord2f(0,1); glVertex3f(-1,+1, 1);
   glEnd();
   //  Back
   if (ntex) glBindTexture(GL_TEXTURE_2D,texture[13]);
   glBegin(GL_QUADS);
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
   glEnd();
   //  Right
   if (ntex) glBindTexture(GL_TEXTURE_2D,texture[13]);
   glBegin(GL_QUADS);
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(+1,+1,+1);
   glEnd();
   //  Left
   if (ntex) glBindTexture(GL_TEXTURE_2D,texture[13]);
   glBegin(GL_QUADS);
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
   glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   //  Top
   if (ntex) glBindTexture(GL_TEXTURE_2D,texture[13]);
   glBegin(GL_QUADS);
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
   glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
   glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
   glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
   glEnd();
   //  Bottom
   if (ntex) glBindTexture(GL_TEXTURE_2D,texture[13]);
   glBegin(GL_QUADS);
   glNormal3f( 0,-1, 0);
   glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
   glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
   glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
   glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
   glEnd();
   //  Undo transformations and textures
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
}


void pyramid(double x, double y, double z, double dx, double dy,
   double dz, double th, double ph, float R, float G, float B){

   glPushMatrix();
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glRotated(ph,1,0,0);
   glScaled(dx,dy,dz);

   triangle(0,0,0, .75,1,1, -48.75,0, R,G,B);
   triangle(.75,0,-.75, 1,1,.75, -48.75,90, R,G,B);
   triangle(0, 0, -1.5, .75,1,1, -48.75,180, R,G,B);
   triangle(-.75,0,-.75, 1,1,.75, -48.75,-90, R,G,B);

   glPopMatrix();
}


/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius r
 */
static void ball(double x,double y,double z,double r)
{
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glScaled(r,r,r);
   //  White ball
   glColor3f(1,1,1);
   glutSolidSphere(1.0,16,16);
   //  Undo transofrmations
   glPopMatrix();
}

void landscape(){
  glPushMatrix();
  ground(0,-1,0, 1,1,1, 0);
  ground(2,-1,0, 1,1,1, 0);
  ground(0,-1,2, 1,1,1, 0);
  ground(2,-1,2, 1,1,1, 0);
  ground(-2,-1,0, 1,1,1, 0);
  ground(0,-1,-2, 1,1,1, 0);
  ground(-2,-1,2, 1,1,1, 0);
  ground(2,-1,-2, 1,1,1, 0);
  ground(-2,-1,-2, 1,1,1, 0);
  glPopMatrix();
}



void arms(double th){
  glPushMatrix();
  glTranslated(0, th, 0); 
  cube(.12,.14,.68, .04,.08,.04,0);
  cube(-.12,.14,.68, .04,.08,.04,0);
  glPopMatrix();
}


void movetower(){
  glPushMatrix();
  glRotated(270,1,0,0);
   cube(.12,.25,.5, .04,.04,.5, 0);
   cube(-.12,.25,.5, .04,.04,.5, 0);
   cube(.12,.50,.5, .04,.04,.5, 0);
   cube(-.12,.50,.5, .04,.04,.5, 0);
   cube(.12,.375, .04, .04,.12,.04, 0);
   cube(.12,.375, .20, .04,.12,.04, 0);
   cube(.12,.375, .36, .04,.12,.04, 0);
   cube(.12,.375, .52, .04,.12,.04, 0);
   cube(.12,.375, .68, .04,.12,.04, 0);
   cube(.12,.375, .84, .04,.12,.04, 0);
   cube(-.12,.375, .04, .04,.12,.04, 0);
   cube(-.12,.375, .20, .04,.12,.04, 0);
   cube(-.12,.375, .36, .04,.12,.04, 0);
   cube(-.12,.375, .52, .04,.12,.04, 0);
   cube(-.12,.375, .68, .04,.12,.04, 0);
   cube(-.12,.375, .84, .04,.12,.04, 0);
   cube(0,.25,.04, .12,.04,.04, 0);
   cube(0,.25,.20, .12,.04,.04, 0);
   cube(0,.25,.36, .12,.04,.04, 0);
   cube(0,.25,.52, .12,.04,.04, 0);
   cube(0,.25,.68, .12,.04,.04, 0);
   cube(0,.25,.84, .12,.04,.04, 0);
   cube(0,.5,.04, .12,.04,.04, 0);
   cube(0,.5,.20, .12,.04,.04, 0);
   cube(0,.5,.36, .12,.04,.04, 0);
   cube(0,.5,.52, .12,.04,.04, 0);
   cube(0,.5,.68, .12,.04,.04, 0);
   cube(0,.5,.84, .12,.04,.04, 0);
   if (inc < .3){
    arms(inc);
   }
   else {
    arms(.2);
   }
   glPopMatrix();
}

void platform(){
  glPushMatrix();
  cube(0,.2,0, 1,.2,1, 0);
  glPopMatrix();
}

void barn(){
  glPushMatrix();
  base(-2,.3,.5, .4,.3,.3, 0);
  pyramid(-2,.6,.8, .54,.4,.4, 0,0, 220,220,220);


  base(-2,.3,-.4, .4,.3,.3, 0);
  pyramid(-2,.6,-.1, .54,.4,.4, 0,0, 220,220,220);

  base(-2,.3,1.4, .4,.3,.3, 0);
  pyramid(-2,.6,1.7, .54,.4,.4, 0,0, 220,220,220);

  glPopMatrix();
}

void tower(){
  glPushMatrix();
  glRotated(270,1,0,0);
   cube(.12,.25,.9, .04,.04,.5, 0);
   cube(-.12,.25,.9, .04,.04,.5, 0);
   cube(.12,.50,.9, .04,.04,.5, 0);
   cube(-.12,.50,.9, .04,.04,.5, 0);
   cube(.12,.375, .44, .04,.12,.04, 0);
   cube(.12,.375, .60, .04,.12,.04, 0);
   cube(.12,.375, .76, .04,.12,.04, 0);
   cube(.12,.375, .92, .04,.12,.04, 0);
   cube(.12,.375, 1.08, .04,.12,.04, 0);
   cube(.12,.375, 1.24, .04,.12,.04, 0);
   cube(-.12,.375, .44, .04,.12,.04, 0);
   cube(-.12,.375, .60, .04,.12,.04, 0);
   cube(-.12,.375, .76, .04,.12,.04, 0);
   cube(-.12,.375, .92, .04,.12,.04, 0);
   cube(-.12,.375, 1.08, .04,.12,.04, 0);
   cube(-.12,.375, 1.24, .04,.12,.04, 0);
   cube(0,.25,.44, .12,.04,.04, 0);
   cube(0,.25,.60, .12,.04,.04, 0);
   cube(0,.25,.76, .12,.04,.04, 0);
   cube(0,.25,.92, .12,.04,.04, 0);
   cube(0,.25,1.08, .12,.04,.04, 0);
   cube(0,.25,1.24, .12,.04,.04, 0);
   cube(0,.5,.44, .12,.04,.04, 0);
   cube(0,.5,.60, .12,.04,.04, 0);
   cube(0,.5,.76, .12,.04,.04, 0);
   cube(0,.5,.92, .12,.04,.04, 0);
   cube(0,.5,1.08, .12,.04,.04, 0);
   cube(0,.5,1.24, .12,.04,.04, 0);
   arms(0);
   glPopMatrix();
}

void boost(float R, float G, float B){
   cylinder(0.15, 1.0, R,G,B, 0, 0, .2, 0);
   cylinder(0.1, .3, R,G,B, .25, 0, .2, 0);
   cylinder(.1, .3, R,G,B, -.25, 0, .2, 0);
   cone(0.15, .3, R,G,B, 0, 0, 1.2, 0);
   cone(.1, .2, R,G,B, .25, 0, .5, 0);
   cone(.1, .2, R,G,B, -.25, 0, .5, 0);
   ring(0.13, .03, .2, R,G,B, 0, 0, 0, 0, 1.25);
   ring(.08, .02, .15, R,G,B, .25, 0, .05, 0, 1.15);
   ring(.08, .02, .15, R,G,B, -.25, 0, .05, 0, 1.15);
}


void ship(double x, double y, double z, double dx, double dy,
   double dz, double th, double ph, float R, float G, float B)
{
   glPushMatrix();
   glTranslated(x,y,z);
   if (inc > 1){
    glTranslated(x,th-1,z);
   }
   glRotated(ph,1,0,0);
   glScaled(dx,dy,dz);

   boost(R, G, B);

   glPopMatrix();
}



/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   glClearColor(0, 0, 0, 0);
   //  Length of axes
   const double len=2.0;
   //  Eye position
   double Ex = -2*dim*Sin(th)*Cos(ph);
   double Ey = +2*dim        *Sin(ph);
   double Ez = +2*dim*Cos(th)*Cos(ph);
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);
   //  Set perspective
   glLoadIdentity();
   if (launch == 1){
    gluLookAt(Ex,inc,Ez, 0,inc,0, 0,Cos(ph),0);
   }
   else if (launch != 1){
    gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   //  Light switch
   if (light)
   {
      //  Translate intensity to color vectors
      float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
      float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
      float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
      //  Light direction
      float Position[]  = {5*Cos(zh),ylight,5*Sin(zh),1};
      //  Draw light position as ball (still no lighting here)
      glColor3f(1,1,1);
      ball(Position[0],Position[1],Position[2] , 0.1);
      //  OpenGL should normalize normal vectors
      glEnable(GL_NORMALIZE);
      //  Enable lighting
      glEnable(GL_LIGHTING);
      //  glColor sets ambient and diffuse color materials
      glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
      glEnable(GL_COLOR_MATERIAL);
      //  Enable light 0
      glEnable(GL_LIGHT0);
      //  Set ambient, diffuse, specular components and position of light 0
      glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
      glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
      glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
      glLightfv(GL_LIGHT0,GL_POSITION,Position);
   }
   else
      glDisable(GL_LIGHTING);

    ship(0,.4,0, 1,1,1, inc,-90, color[K].r,color[K].g,color[K].b);
    movetower();
    landscape();
    barn();
    platform();
   


   //  Draw axes - no lighting from here on
   glDisable(GL_LIGHTING);
   glColor3f(1,1,1);
   if (axes)
   {
      glBegin(GL_LINES);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(len,0.0,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,len,0.0);
      glVertex3d(0.0,0.0,0.0);
      glVertex3d(0.0,0.0,len);
      glEnd();
      //  Label axes
      glRasterPos3d(len,0.0,0.0);
      Print("X");
      glRasterPos3d(0.0,len,0.0);
      Print("Y");
      glRasterPos3d(0.0,0.0,len);
      Print("Z");
   }
   //  Display parameters
   glWindowPos2i(5,5);
   Print("Angle=%d,%d  Dim=%.1f Light=%s Texture=%s",th,ph,dim,light?"On":"Off",mode?"Replace":"Modulate");
   if (light)
   {
      glWindowPos2i(5,25);
      Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",ambient,diffuse,specular,emission,shiny);
   }
   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{
   //  Elapsed time in seconds
   t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360.0);
   if (t < 1000){
      arm = t/12;
    }
    if (launch == 1){
      inc = pow(t-t2, 2)/15;
    }
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
   //  Right arrow key - increase angle by 5 degrees
   if (key == GLUT_KEY_RIGHT)
      th += 5;
   //  Left arrow key - decrease angle by 5 degrees
   else if (key == GLUT_KEY_LEFT)
      th -= 5;
   //  Up arrow key - increase elevation by 5 degrees
   else if (key == GLUT_KEY_UP)
      ph += 5;
   //  Down arrow key - decrease elevation by 5 degrees
   else if (key == GLUT_KEY_DOWN)
      ph -= 5;
   //  PageUp key - increase dim
   else if (key == GLUT_KEY_PAGE_DOWN)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_UP && dim>1)
      dim -= 0.1;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project(45,asp,dim);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
   //  Exit on ESC
   if (ch == 27)
      exit(0);
   //  Reset view angle
   else if (ch == '0')
      th = ph = 0;
   //  Toggle texture mode
   else if (ch == 'm' || ch == 'M')
      mode = 1-mode;
   //  Toggle axes
   else if (ch == 'x' || ch == 'X')
      axes = 1-axes;
   //  Toggle lighting
   else if (ch == 'l' || ch == 'L')
      light = 1-light;
   //  Toggle textures mode
   else if (ch == 't')
      ntex = 1-ntex;
   //  Light elevation
   else if (ch=='[')
      ylight -= 0.1;
   else if (ch==']')
      ylight += 0.1;
   //  Ambient level
   else if (ch=='a' && ambient>0)
      ambient -= 5;
   else if (ch=='A' && ambient<100)
      ambient += 5;
   //  Diffuse level
   else if (ch=='d' && diffuse>0)
      diffuse -= 5;
   else if (ch=='D' && diffuse<100)
      diffuse += 5;
   //  Specular level
   else if (ch=='s' && specular>0)
      specular -= 5;
   else if (ch=='S' && specular<100)
      specular += 5;
   //  Emission level
   else if (ch=='e' && emission>0)
      emission -= 5;
   else if (ch=='E' && emission<100)
      emission += 5;
   //  Shininess level
   else if (ch=='n' && shininess>-1)
      shininess -= 1;
   else if (ch=='N' && shininess<7)
      shininess += 1;
   //  Repitition
   else if (ch=='+')
      rep++;
   else if (ch=='-' && rep>1)
      rep--;
   //  Launch button
   else if (ch=='g')
   {
      t2=t;
      launch+= 1; 
    }
   //  Translate shininess power to value (-1 => 0)
   shiny = shininess<0 ? 0 : pow(2.0,shininess);
   //  Reproject
   Project(45,asp,dim);
   //  Tell GLUT it is necessary to redisplay the scene
   glutPostRedisplay();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
   //  Ratio of the width to the height of the window
   asp = (height>0) ? (double)width/height : 1;
   //  Set the viewport to the entire window
   glViewport(0,0, width,height);
   //  Set projection
   Project(45,asp,dim);
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
   //  Initialize GLUT
   glutInit(&argc,argv);
   //  Request double buffered, true color window with Z buffering at 600x600
   glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
   glutInitWindowSize(600,600);
   glutCreateWindow("Textures and Lighting");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   //  Load textures
   texture[0] = LoadTexBMP("crate.bmp");
   texture[1] = LoadTexBMP("img1.bmp");
   texture[2] = LoadTexBMP("img2.bmp");
   texture[3] = LoadTexBMP("img3.bmp");
   texture[4] = LoadTexBMP("img4.bmp");
   texture[5] = LoadTexBMP("img5.bmp");
   texture[6] = LoadTexBMP("img6.bmp");
   texture[7] = LoadTexBMP("logtex_64x64.bmp");
   texture[8] = LoadTexBMP("shiphull.bmp");
   texture[9] = LoadTexBMP("shipcap.bmp");
   texture[10] = LoadTexBMP("towertex.bmp");
   texture[11] = LoadTexBMP("grass.bmp");
   texture[12] = LoadTexBMP("black.bmp");
   texture[13] = LoadTexBMP("barnside.bmp");
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}
