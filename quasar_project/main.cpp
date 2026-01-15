#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>

struct Vector2{
    float x; // coordinates
    float y;
};

struct Particle
{
    Vector2 position;
    Vector2 velocity;
    int r, g, b;
};

class Quasar{
    private:
        int size;
        Particle *particle;
        sf::VertexArray vertices;

    public:

        Quasar(int count) : size(count), particle(new Particle[size])
        {
            vertices.setPrimitiveType(sf::PrimitiveType::Points);
            vertices.resize(size);

            for (int i = 0; i < size; i++)
            {
                float x = 400.0f + (rand() % 200 - 100);
                float y = 300.0f + (rand() % 200 - 100);

                particle[i].position.x = x;
                particle[i].position.y = y;

                // NEW CODE (The "Spin" Logic)
                float dx = x - 400.0f;
                float dy = y - 300.0f;

                // We flip dx and dy to create a perpendicular vector (-dy, dx)
                // The '2.0f' is the spin speed. Higher number = Wider orbit.
                // NEW (Much slower start)
                particle[i].velocity.x = dy * 0.23f;
                particle[i].velocity.y = -dx * 0.23f;

                vertices[i].position = sf::Vector2f(x, y);
                vertices[i].color = sf::Color::White;
            }

            std::cout << "Construction is completed. " << std::endl;
        }

        ~Quasar()
        {
            delete[] particle;
            std::cout << "Destruction is completed. " << std::endl;
        }

        Quasar(const Quasar & obj): size(obj.size), particle(new Particle[size])
        {
            vertices.setPrimitiveType(sf::PrimitiveType::Points);
            vertices.resize(size);

            for (int i = 0; i < size; i++)
            {
                particle[i] = obj.particle[i];
                vertices[i] = obj.vertices[i];
            }
        }

        Quasar & operator = (const Quasar& other)
        {
            if(this != &other)
            {
                delete[] particle;

                size = other.size;
                particle = new Particle[size];

                vertices.setPrimitiveType(sf::PrimitiveType::Points);
                vertices.resize(size);

                for (int i = 0; i < size; i++ )
                {
                    particle[i] = other.particle[i];
                    vertices[i] = other.vertices[i];
                }
            }
            return *this;
        }

        void update()
    {
        // 1. Real Physics Constants
        // G is scaled up because our pixels are huge compared to atoms
        float G = 1000.0f; 
        float massBH = 250.0f; 
        float dt = 0.016f; // 60 FPS (1/60)

        float centerX = 400.0f;
        float centerY = 300.0f;

        for (int i = 0; i < size; i++)
        {
            // A. Vector to Center
            float dx = centerX - particle[i].position.x;
            float dy = centerY - particle[i].position.y;

            // B. Distance Squared (r^2)
            float r2 = dx*dx + dy*dy;

            // C. Numerical Stability (The "Exception" replacement)
            // If r2 is too small, force it to be bigger so we don't divide by zero
            // This is called "Softening" in Astrophysics simulations
            if (r2 < 200.0f) r2 = 200.0f;

            // D. Inverse Square Law: F = G * M / r^2
            // We assume particle mass = 1, so Force = Acceleration
            float force = (G * massBH) / r2;

            // E. Normalize Direction & Apply Force
            float r = std::sqrt(r2);
            float accX = (dx / r) * force;
            float accY = (dy / r) * force;

            // F. Integrate (Euler Integration)
            particle[i].velocity.x += accX * dt;
            particle[i].velocity.y += accY * dt;

            particle[i].position.x += particle[i].velocity.x * dt;
            particle[i].position.y += particle[i].velocity.y * dt;

            // G. Update Graphics
            vertices[i].position = sf::Vector2f(particle[i].position.x, particle[i].position.y);
            
            // Optional: Color Shift based on Velocity (Heat effect)
            // Fast = Red, Slow = White
            float speed = std::sqrt(particle[i].velocity.x*particle[i].velocity.x + particle[i].velocity.y*particle[i].velocity.y);
            
            if (speed > 50.0f) vertices[i].color = sf::Color::Red;
            else vertices[i].color = sf::Color::White;
        }
    }
    void draw(sf::RenderWindow& window) {
            window.draw(vertices);
        }
};

int main()
{
    // 1. Create Window
    sf::RenderWindow window(sf::VideoMode({800, 600}), "Quasar Simulation");
    window.setFramerateLimit(60); // 60 FPS is smoother for physics

    // 2. Create the Engine (Start with 20,000 particles!)
    Quasar engine(100000); 

    // 3. Main Loop
    while (window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
        }

        // A. Update Physics (Math)
        engine.update(); 

        // B. Render (Graphics)
        window.clear(sf::Color::Black);
        engine.draw(window); // Call your helper function
        window.display();
    }

    return 0;
}