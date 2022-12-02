#include "headers/particle_system.hpp"
#include <cmath>
#include <iostream>

Particle::Particle(sf::Vector2f position , sf::Vector2f direction , float speed , float duration){
    m_position = position;
    this->movement = direction;
    this->speed = speed;
    m_duration = duration;
    m_alive = true;
    m_vertices.resize(4);
    m_vertices.setPrimitiveType(sf::TriangleStrip);
}

void Particle::move(const sf::Vector2f & movement){
    m_position += movement;
}

bool Particle::isAlive() const {
    return m_alive;
}

void Particle::display(sf::RenderTarget & target , sf::View view) const {
    sf::RenderStates states;
    auto textoffset = (m_texture)?(sf::Vector2f{m_texture->getSize()}*.5f):sf::Vector2f(0,0);
    auto translation = m_position-textoffset-view.getCenter()+view.getSize()*0.5f;
    translation.x = std::round(translation.x*3.f);
    translation.y = std::round(translation.y*3.f);
    sf::Transform transform = sf::Transform::Identity;
    transform.translate(translation);
    transform.scale(sf::Vector2f(3,3));
    states.transform = transform;
    states.texture = &*m_texture;

    target.draw(m_vertices , states);
}

void Particle::setTexture(sf::Texture & texture){
    m_texture = &texture;
    if(m_texture){
        auto ts = m_texture->getSize();
        for(int i = 0;i < m_vertices.getVertexCount();i++){
            sf::Vertex* vertex = &m_vertices[i];
            vertex->position = sf::Vector2f(ts.x*(i%2),ts.y*(i/2));
            vertex->texCoords = sf::Vector2f(ts.x*(i%2),ts.y*(i/2));
        }
    }
}

void Particle::setTexture(sf::Texture* texture){
    m_texture = texture;
    if(m_texture){
        auto ts = m_texture->getSize();
        for(int i = 0;i < m_vertices.getVertexCount();i++){
            sf::Vertex* vertex = &m_vertices[i];
            vertex->position = sf::Vector2f(ts.x*(i%2),ts.y*(i/2));
            vertex->texCoords = sf::Vector2f(ts.x*(i%2),ts.y*(i/2));
        }
    }
}

void Particle::update(float dt){

    if(m_alive){
        move(movement*speed*dt);
        m_duration -= dt;

        if(m_duration <= 0){
            m_alive = false;
        }
    }
}

/////// PARTICLE SYSTEM ///////

ParticleSystem::ParticleSystem(sf::Vector2f startpos){
    m_position = startpos;
    m_particles = {};
    m_spawnRate = 1.f;
    setRange("angle" , 90 , 90);
    setRange("speed" , 1 , 1);
    setRange("duration" , 1 , 1);
}

void ParticleSystem::setPosition(const sf::Vector2f & position){
    m_position = position;
}

// void ParticleSystem::addParticles(unsigned int n , urdf angle , urdf speed , urdf duration){

//     for(int i = 0;i < n;i++){
//         auto a = Random::randFloat(angle)*((float)M_PI/180.f);
//         auto s = Random::randFloat(speed);
//         auto d = Random::randFloat(duration);

//         Particle particle{m_position , {std::cos(a) , -std::sin(a)} , s , d};
//     }
// }

void ParticleSystem::setTexture(sf::Texture & texture){
    m_texture = &texture;
}

void ParticleSystem::setSpawnRate(float rate){
    if(rate > 0){
        m_spawnRate = rate;
    }
}

void ParticleSystem::setRange(std::string type , float min , float max){

    if(type == "angle"){
        urdf a = urdf(std::min(min , max) , std::max(min , max));
        m_urdfs[type] = a;
    } else if(type == "speed"){
        urdf a = urdf(std::min(min , max) , std::max(min , max));
        m_urdfs[type] = a;
    } else if(type == "duration"){

        if(!(min > 0) || !(max > 0))
            return;

        urdf a = urdf(std::min(min , max) , std::max(min , max));
        m_urdfs[type] = a;
    }
}

void ParticleSystem::update(float dt){
    for (int i = m_particles.size()-1;i >= 0;i--){
        m_particles[i].update(dt);
        if(!m_particles[i].isAlive())
            m_particles.erase(m_particles.begin()+i);
    }

    m_timer += dt;
    if(m_timer - m_spawnRate >= 0.f){
        auto a = Random::randFloat(m_urdfs["angle"])*((float)M_PI/180.f);
        auto s = Random::randFloat(m_urdfs["speed"]);
        auto d = Random::randFloat(m_urdfs["duration"]);

        Particle particle{m_position , {std::cos(a) , -std::sin(a)} , s , d};
        particle.setTexture(m_texture);
        m_particles.push_back(particle);
        m_timer = 0.f;
    }
}

void ParticleSystem::display(sf::RenderTarget & target , sf::View view) const{
    for(auto & particle : m_particles){
        particle.display(target , view);
    }
}