#pragma once
#include "Input.h"
#include <iostream>

using namespace sf;
using namespace std;

#define W 528
#define H 528

class Application
{
public:
	Application(RenderWindow* window);
	~Application();

public:
	void run();

private:

	struct Particle {
		Particle(float _x, float _y) : position(_x, _y), velocity(0.f, 0.f), force(0.f, 0.f), density(0), pressure(0.f) {}
		Vector2f position, velocity, force;
		float density, pressure;
	};

	void handleInput();
	void update();
	void render();

	void spawnDam();
	void spawnDrop(Vector2i mouse_position);
	void edges(Particle& particle);
	void computeDensityPressure();
	void computeForces();

	const float PI = 3.14159265358979323846f;

	RenderWindow* window;
	Input input;
	bool running;

	int fps;
	Font font;
	Text fps_text;

	vector<Particle> particles;
	vector<Vertex> vertices;
	VertexBuffer vertex_buffer{ sf::Points, sf::VertexBuffer::Static };
	

	const int max_particles;

	const Vector2f gravity; 
	const float radius; 
	const float radius_squared;
	const float mass; 
	const float viscocity; 
	const float integration_timestep; 
	const float damping;

	const float REST_DENSITY; // rest density
	const float BOLTZMANN_CONSTANT; // const for equation of state

	// smoothing kernels defined in Müller and their gradients
	const float POLY6;
	const float SPIKY_GRAD;
	const float VISC_LAP;

	int scale;
	int width;
	int height;

	bool active;
};

