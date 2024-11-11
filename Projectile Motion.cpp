#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <cmath>
#include <vector>
#include <fstream>
#include <sstream>

#define PI 3.14159

class Ball {
public:
    sf::CircleShape shape;
    sf::Vector2f velocity;
    float gravity;

    Ball(float radius, sf::Color color, sf::Vector2f position, float g)
        : velocity(0, 0), gravity(g) {
        shape.setRadius(radius);
        shape.setFillColor(color);
        shape.setPosition(position);
        shape.setOrigin(radius, radius);
    }

    bool hit_ground(float ground_y, sf::Sound hit_ground, int threshold_vel) {

        if (shape.getPosition().y + shape.getRadius() >= ground_y) {
            //hit_ground.play();
            shape.setPosition(shape.getPosition().x, ground_y - shape.getRadius());

            if (std::abs(velocity.x) < threshold_vel) {
                velocity.x = 0;
            }

            if (std::abs(velocity.y) <= threshold_vel) {
                velocity.y = 0;
                
                return false;
            }
           
            if (velocity.y)
                velocity.y = -std::abs(velocity.y) * 0.7f;  
            velocity.x *= 0.7f;  

            return true;
        }

        
        
    }


    std::vector<sf::CircleShape> trajectory(float g, int width_bg, sf::Vector2f initial_vel, int ground_y, float starting_pos) {
        std::vector<sf::CircleShape> points;
        float start_x = starting_pos;
        float start_y = ground_y - shape.getRadius();
        float factor = 1;
        if (initial_vel.x == 0) {
            factor = 5;
        }
        else if (initial_vel.y == 0) {
            factor = 1;
        }
        else {
            factor = std::abs(initial_vel.y / initial_vel.x);
        }
        

        for (float x = start_x; x <= width_bg && x >= 0; x += (initial_vel.x > 0 ? 5/factor :-5/factor)) {
            float t = (x - start_x) / initial_vel.x;

            float y = start_y + (initial_vel.y * t) + (0.5f * g * t * t);

            if (y >= ground_y) break;

            sf::CircleShape point(1);
            point.setPosition(x, y);
            point.setFillColor(sf::Color::Yellow);
            points.push_back(point);
        }

        return points;
    }

    void display_constrain(int bg_width) {
        if (shape.getPosition().x >= bg_width - shape.getRadius()) {
            shape.setPosition(sf::Vector2f(bg_width - shape.getRadius(), shape.getPosition().y));
            velocity.x *= -1;
        }

        if (shape.getPosition().x <= shape.getRadius()) {
            shape.setPosition(sf::Vector2f(shape.getRadius(), shape.getPosition().y));
            velocity.x *= -1;
        }

        if (shape.getPosition().y <= shape.getRadius()) {
            shape.setPosition(sf::Vector2f(shape.getPosition().x, shape.getRadius()));
            velocity.y *= -1;
        }
    }
};

int main() {
    // Universal Variables
    unsigned int width_bg = 1080;
    unsigned int height_bg = 720;
    float ground_y = height_bg - 50;
    float gravity = 500;

    sf::Clock clock;
    

    sf::ContextSettings setting;
    setting.antialiasingLevel = 10;
    sf::RenderWindow window(sf::VideoMode(width_bg, height_bg), "Projectile Motion: Hand Gestures", sf::Style::Default, setting);
    window.setFramerateLimit(120);

    // Ground Line
    sf::RectangleShape g_line(sf::Vector2f(width_bg, height_bg - ground_y));
    g_line.setFillColor(sf::Color(80, 80, 90));
    g_line.setPosition(0, ground_y);

    // Ball setup
    float radius = 12.5f;
    sf::Vector2f initialVelocity(300, -500);  // initial x and y velocities
    Ball ball(radius, sf::Color(90, 160, 90), sf::Vector2f(300, ground_y - radius), gravity);

    bool isLaunched = false;
    int scale = 3;
    bool is_bouncing = false;
    int threshold_vel = 50;

    sf::SoundBuffer buffer;
    buffer.loadFromFile("C:\\Users\\Akshay\\Desktop\\Projectile\\Projectile Motion\\hitHurt.wav");
    sf::Sound bounce;
    bounce.setBuffer(buffer);
    
    sf::Font arial;
    arial.loadFromFile("C:\\Windows\\Fonts\\AGENCYR.TTF");

    sf::Text vel_y("Vy = 0", arial);
    sf::Text vel_x("Vx = 0", arial);
    vel_x.setCharacterSize(20);
    vel_y.setCharacterSize(20);
    vel_y.rotate(-90);


    // Lines and Overlays
    sf::RectangleShape line_x(sf::Vector2f(width_bg , 0.5));
    sf::RectangleShape line_y(sf::Vector2f(0.5, height_bg));
    line_x.setFillColor(sf::Color(200, 160, 160));
    line_y.setFillColor(sf::Color(200, 160, 160));

    // Main Game Loop
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    ball.velocity = initialVelocity;
                    isLaunched = true;
                    is_bouncing = true;
                }
                if (event.key.code == sf::Keyboard::Right) {
                    initialVelocity.x += 300 * clock.getElapsedTime().asSeconds();
                }
                if (event.key.code == sf::Keyboard::Left) {
                    initialVelocity.x -= 300 * clock.getElapsedTime().asSeconds();
                }
                if (event.key.code == sf::Keyboard::Up) {
                    initialVelocity.y -= 300 * clock.getElapsedTime().asSeconds();
                }
                if (event.key.code == sf::Keyboard::Down) {
                    initialVelocity.y += 300 * clock.getElapsedTime().asSeconds();
                }
                if (event.key.code == sf::Keyboard::R) {
                    ball.shape.setPosition(50, ground_y - radius);
                    ball.velocity = sf::Vector2f(0, 0);
                    isLaunched = false;
                }
            }
        }

        sf::Time elapsedTime = clock.restart();
        float deltaTime = elapsedTime.asSeconds();

        if (isLaunched) {
            ball.shape.move(ball.velocity.x * deltaTime, ball.velocity.y * deltaTime);
            ball.velocity.y += ball.gravity * deltaTime;
        }

        

        std::ifstream inFile("hand_coordinates.txt");
        int index_x = 0, index_y = 0, thumb_x = 0, thumb_y = 0;
        std::string line = "";

        if (inFile.is_open()) {
            if (std::getline(inFile, line)) {
                int comma_pos = line.find(',');
                index_x = std::stoi(line.substr(0, comma_pos));
                index_y = std::stoi(line.substr(comma_pos + 1));
            }

            if (std::getline(inFile, line)) {
                int comma_pos = line.find(',');
                thumb_x = std::stoi(line.substr(0, comma_pos));
                thumb_y = std::stoi(line.substr(comma_pos + 1));
            }
        }

        

        // float length = pow((pow((thumb_x - index_x), 2) + pow((thumb_y - index_y), 2)), 0.5);

        // Updates
        std::string score_x = std::to_string(int(ball.velocity.x));
        std::string score_y = std::to_string(-int(ball.velocity.y));
        vel_x.setString("Vx = " + score_x);
        vel_y.setString("Vy = " + score_y);
        vel_x.setPosition(ball.shape.getPosition().x + 40, ball.shape.getPosition().y - 30);
        vel_y.setPosition(ball.shape.getPosition().x - 30, ball.shape.getPosition().y - 50);
        
        int x_comp = (index_x - thumb_x);
        int y_comp = (index_y - thumb_y);

        initialVelocity.x = x_comp * scale;
        initialVelocity.y = y_comp * scale;

        line_x.setPosition(0, ball.shape.getPosition().y);
        line_y.setPosition(ball.shape.getPosition().x, 0);
        ball.display_constrain(width_bg);
        is_bouncing = ball.hit_ground(ground_y, bounce, threshold_vel);



        std::vector<sf::CircleShape> points = ball.trajectory(gravity, width_bg, initialVelocity, ground_y, ball.shape.getPosition().x);

        window.clear(sf::Color(30, 30, 30));
        window.draw(g_line);

        if (!is_bouncing) {
            for (const auto& point : points) {
                window.draw(point);
            }
        }

        window.draw(line_x);
        window.draw(line_y);
        window.draw(vel_y);
        window.draw(vel_x);
        window.draw(ball.shape);
        window.display();
    }

    return 0;
}