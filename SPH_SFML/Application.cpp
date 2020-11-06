#include "Application.h"
#include <map>

Application::Application(RenderWindow* window) :
	window(window),
	running(true),
	fps(-1),
	//max_particles(356), // debug
	max_particles(2250),
	gravity(0.f, 12000 * 9.8f),
	radius(16.0f),
	radius_squared(pow(radius, 2)),
	mass(65.f),
	viscocity(250.f),
	integration_timestep(0.0008f),
	damping(-0.5f),
	REST_DENSITY(1000.f),
	BOLTZMANN_CONSTANT(2000.f),
	POLY6(315.f / (65.f * PI * pow(radius, 9.f))),
	SPIKY_GRAD(-45.f / (PI * pow(radius, 6.f))),
	VISC_LAP(45.f / (PI * pow(radius, 6.f))),
	scale(4),
	width(W * scale),
	height(H * scale),
	active(false)
{
	//window->setFramerateLimit(60);

	font.loadFromFile("arial.ttf");

	fps_text.setString(to_string(fps));
	fps_text.setCharacterSize(30);
	fps_text.setPosition(width / scale - 80, 20);
	fps_text.setFillColor(sf::Color::White);
	fps_text.setFont(font);

	
	bool vb = vertex_buffer.isAvailable();

	spawnDam();

}

Application::~Application()
{
}

void Application::run()
{
	Clock clock;
	Time fps_timer = Time::Zero;
	int frames = 0;
	while (running)
	{
		Event event;
		while (window->pollEvent(event))
		{
			switch (event.type)
			{
			case Event::Closed:
				running = false;
				break;
			case Event::KeyPressed:
				input.setKeyDown(event.key.code);
				break;
			case Event::KeyReleased:
				input.setKeyUp(event.key.code);
				break;
			case Event::MouseButtonPressed:
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					input.setMouseLeftDown(true);
				}
				break;
			case Event::MouseButtonReleased:
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					input.setMouseLeftDown(false);
				}
				break;
			case Event::MouseMoved:
				input.setMousePosition(event.mouseMove.x, event.mouseMove.y);
				break;
			default:
				break;
			}
		}
		Time delta_time = clock.restart();
		fps_timer += delta_time;
		frames++;
		if (fps_timer > sf::seconds(1))
		{
			fps = frames;
			frames = 0;
			fps_timer -= sf::seconds(1);
		}
		handleInput();
		if (active)
		{
			update();
		}
		render();
	}
	window->close();
}

void Application::handleInput()
{
	if (input.getKeyDown(Keyboard::Escape)) running = false;

	if (input.getKeyDown(Keyboard::Space))
	{
		if (active)
		{
			particles.clear();
			vertices.clear();
			spawnDam();
			active = false;
		}
		else
		{
			active = true;
		}
		input.setKeyUp(Keyboard::Space);
	}

	if (!active)
	{
		if (input.getKeyDown(Keyboard::Num1))
		{
			scale = 1;
			width = W * scale;
			height = H * scale;
			particles.clear();
			vertices.clear();
			spawnDam();
		}
		if (input.getKeyDown(Keyboard::Num2))
		{
			scale = 2;
			width = W * scale;
			height = H * scale;
			particles.clear();
			vertices.clear();
			spawnDam();
		}
		if (input.getKeyDown(Keyboard::Num4))
		{
			scale = 4;
			width = W * scale;
			height = H * scale;
			particles.clear();
			vertices.clear();
			spawnDam();
		}
	}

	if (input.getMouseLeftDown())
	{
		//spawnDrop(input.getMousePosition());
		input.setMouseLeftDown(false);
	}
}

void Application::update()
{
	computeDensityPressure();
	computeForces();
	for (Particle& particle : particles)
	{
		particle.velocity += particle.force / particle.density * integration_timestep;
		particle.position += particle.velocity * integration_timestep;
		edges(particle);
	}
	for (int i = 0; i < particles.size(); i++)
	{
		vertices[i].position.x = (particles[i].position.x / scale);
		vertices[i].position.y = (particles[i].position.y / scale);
	}
	
}



void Application::render()
{
	window->clear(Color::Black);
	if (particles.size() > 0)
	{
		window->draw(&vertices[0], vertices.size(), sf::Points);
	}
	if (fps >= 0)
	{
		fps_text.setString(to_string(fps));
		window->draw(fps_text);
	}
	window->display();
}

void Application::spawnDam()
{
	int row = (width / radius) - 1;
	for (float y = height - radius; y < height; /*y >= radius;*/ y -= radius)
	{
		for (float x = (row / 32 * 0) * radius + radius; x <= (row / 32 * 5) * radius + radius; x += radius)
		{
			if (particles.size() < max_particles)
			{
				float jitter = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
				particles.push_back(Particle(x + jitter, y));
				vertices.push_back(Vertex(Vector2f((x + jitter) / scale, y / scale), sf::Color::White));
			}
			else
			{
				return;
			}
		}
	}
}

void Application::spawnDrop(Vector2i mouse_position)
{
	int drop_radius = radius * 2;
	for (float y = mouse_position.y + drop_radius; y >= mouse_position.y - drop_radius; y -= radius)
	{
		for (float x = mouse_position.x - drop_radius; x <= mouse_position.x + drop_radius; x += radius)
		{
			if (particles.size() < max_particles)
			{
				particles.push_back(Particle(x, y));
				vertices.push_back(Vertex(Vector2f(x, y), sf::Color::White));
			}
		}
	}
}

void Application::edges(Particle& particle)
{
	if (particle.position.x - radius < 0.0f)
	{
		particle.velocity.x *= damping;
		particle.position.x = radius;
	}
	if (particle.position.x + radius > width)
	{
		particle.velocity.x *= damping;
		particle.position.x = width - radius;
	}
	//if (particle.position.y - radius < 0.0f)
	//{
	//	particle.velocity.y *= damping;
	//	particle.position.y = radius;
	//}
	if (particle.position.y + radius > height)
	{
		particle.velocity.y *= damping;
		particle.position.y = height - radius;
	}
}

void Application::computeDensityPressure()
{
	for (Particle& particle : particles)
	{
		particle.density = 0.f;
		for (Particle& other_particle : particles)
		{
			Vector2f difference = other_particle.position - particle.position;
			float distance_squared = pow(difference.x, 2) + pow(difference.y, 2);
			if (distance_squared < radius_squared)
			{
				particle.density += mass * POLY6 * pow(radius_squared - distance_squared, 3.f);
			}
		}
		particle.pressure = BOLTZMANN_CONSTANT * (particle.density - REST_DENSITY);
	}
}

void Application::computeForces()
{
	for (Particle& particle : particles)
	{
		Vector2f pressure_contribution(0.f, 0.f);
		Vector2f viscocity_contribution(0.f, 0.f);
		for (Particle& other_particle : particles)
		{
			if (&particle != &other_particle)
			{
				Vector2f difference = other_particle.position - particle.position;
				float distance = sqrt(pow(difference.x, 2) + pow(difference.y, 2));
				if (distance < radius)
				{
					Vector2f normalized_difference = Vector2f(difference.x / distance, difference.y / distance);
					pressure_contribution += -normalized_difference * mass * (particle.pressure + other_particle.pressure) / (2.f * other_particle.density) * SPIKY_GRAD * pow(radius - distance, 2.f);
					viscocity_contribution += viscocity * mass * (other_particle.velocity - particle.velocity) / other_particle.density * VISC_LAP * (radius - distance);
				}
			}
		}
		Vector2f gravity_contribution = gravity * particle.density;
		particle.force = pressure_contribution + viscocity_contribution + gravity_contribution;
	}
}


