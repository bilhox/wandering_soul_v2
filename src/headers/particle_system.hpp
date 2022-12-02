#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include "utils.hpp"

class Particle{

    public:
        sf::Vector2f movement;
        float speed;

    public:
        Particle(sf::Vector2f startpos , sf::Vector2f direction , float speed , float duration);
        void update(float dt);
        void move(const sf::Vector2f & movement);
        void display(sf::RenderTarget & target , sf::View view) const;
        void setTexture(sf::Texture & texture);
        void setTexture(sf::Texture* texture);
        bool isAlive() const;
    
    private:
        sf::VertexArray m_vertices;
        sf::Texture* m_texture;
        sf::Vector2f m_position;
        bool m_alive;
        float m_duration;

};

class ParticleSystem{
    public:
        ParticleSystem(sf::Vector2f startpos);
        void update(float dt);
        void display(sf::RenderTarget & target , sf::View view) const;
        void setPosition(const sf::Vector2f & position);
        void setSpawnRate(float rate);
        void setTexture(sf::Texture & texture);
        void setRange(std::string type , float min , float max);
    
    private:
        float m_spawnRate;
        float m_timer;
        sf::Vector2f m_position;
        sf::Texture* m_texture;
        std::map<std::string , urdf> m_urdfs;
        std::vector<Particle> m_particles;
};