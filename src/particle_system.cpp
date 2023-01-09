#include "headers/particle_system.hpp"
#include <cmath>
#include <iostream>

Particle::Particle(sf::Vector2f direction , float speed , float duration){
    this->movement = direction;
    this->speed = speed;
    m_duration = duration;
    m_alive = true;
}

bool Particle::isAlive() const {
    return m_alive;
}

sf::IntRect Particle::getTextRect(){
    return m_anim.getTextRect();
}

void Particle::update(float dt){

    if(m_alive){
        position += movement*speed*dt;
        m_duration -= dt;

        m_anim.update(dt);

        if(m_duration <= 0){
            m_alive = false;
        }
    }
}

void Particle::setAnimation(Animation & animation){
    m_anim = animation;
}

/////// PARTICLE SYSTEM ///////

ParticleSystem::ParticleSystem(sf::Vector2f startpos){
    m_position = startpos;
    m_particles = {};
    m_spawnRate = 1.f;
    setRange("angle" , 90 , 90);
    setRange("speed" , 1 , 1);
    setRange("duration" , 1 , 1);
    setRange("offsetX" , 0 , 0);
    setRange("offsetY" , 0 , 0);
    m_vertices = {};
    m_color = sf::Color::White;
}

void ParticleSystem::setPosition(const sf::Vector2f & position){
    m_position = position;
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
    } else if(type == "offsetX"){
        urdf a = urdf(std::min(min , max) , std::max(min , max));
        m_urdfs[type] = a;
    } else if(type == "offsetY"){
        urdf a = urdf(std::min(min , max) , std::max(min , max));
        m_urdfs[type] = a;
    }
}

void ParticleSystem::setAnimation(Animation & animation){
    m_anim = animation;
}

void ParticleSystem::setColor(sf::Color color){
    m_color = color;
}

void ParticleSystem::update(float dt){
    for (int i = m_particles.size()-1;i >= 0;i--){
        m_particles[i].update(dt);
        updateParticle(i , m_particles[i].getTextRect());
        if(!m_particles[i].isAlive()){
            m_particles.erase(m_particles.begin()+i);
            m_vertices.erase(m_vertices.begin()+i*6 , m_vertices.begin()+(i+1)*6);
        }
    }

    if(m_continuous){
        m_timer += dt;
        if(m_timer - m_spawnRate >= 0.f){
            spawnParticles(1);
            m_timer = 0.f;
        }
    }
}

void ParticleSystem::clear(){
    m_particles.clear();
    m_vertices.clear();
}

void ParticleSystem::setContinuous(bool state){
    m_continuous = state;
}

void ParticleSystem::spawnParticles(unsigned int n){
    for (int i = 0; i < n; i++)
    {
        auto a = Random::randFloat(m_urdfs["angle"])*((float)M_PI/180.f);
        auto s = Random::randFloat(m_urdfs["speed"]);
        auto d = Random::randFloat(m_urdfs["duration"]);
        auto offX = Random::randFloat(m_urdfs["offsetX"]);
        auto offY = Random::randFloat(m_urdfs["offsetY"]);

        Particle particle{{std::cos(a) , -std::sin(a)} , s , d};
        particle.position = m_position+sf::Vector2f{offX , offY};
        particle.setAnimation(m_anim);
        m_vertices.insert(m_vertices.end() , 6 , sf::Vertex{});
        for(int i = 1;i < 7;i++){
            m_vertices[m_vertices.size()-i].color = m_color;
        }
        m_particles.push_back(particle);
    }
    
}

void ParticleSystem::updateParticle(size_t index , sf::IntRect pRect){
    
    sf::Vector2f pp = m_particles[index].position;
    sf::Vector2f ppt = sf::Vector2f(pRect.getPosition());

    m_vertices[index*6].position = pp-sf::Vector2f(pRect.getSize())*0.5f;
    m_vertices[index*6+1].position = pp+sf::Vector2f(pRect.width*0.5f,-pRect.height*0.5f);
    m_vertices[index*6+2].position = pp+sf::Vector2f(-pRect.width*0.5f,pRect.height*0.5f);
    m_vertices[index*6+3].position = pp+sf::Vector2f(pRect.width*0.5f,-pRect.height*0.5f);
    m_vertices[index*6+4].position = pp+sf::Vector2f(-pRect.width*0.5f,pRect.height*0.5f);
    m_vertices[index*6+5].position = pp+sf::Vector2f(pRect.width*0.5f,pRect.height*0.5f);

    /*
    Vertex disposition
        1/ --------2/4
        |            |
        |            |
        |            |
        |            |
        3/5--------4/6
    
    */

    m_vertices[index*6].texCoords = ppt;
    m_vertices[index*6+1].texCoords = ppt+sf::Vector2f(pRect.width,0.f);
    m_vertices[index*6+2].texCoords = ppt+sf::Vector2f(0.f,pRect.height);
    m_vertices[index*6+3].texCoords = ppt+sf::Vector2f(pRect.width,0.f);
    m_vertices[index*6+4].texCoords = ppt+sf::Vector2f(0.f,pRect.height);
    m_vertices[index*6+5].texCoords = ppt+sf::Vector2f(pRect.width,pRect.height);
}

void ParticleSystem::display(sf::RenderTarget & target , sf::View view) {
    sf::RenderStates states;
    sf::Vector2f tvpSize { Const::ORIGINAL_WINSIZE };
    auto zoom = sf::Vector2f(tvpSize.x / (view.getSize().x) , tvpSize.y / (view.getSize().y));
    auto translation = -view.getCenter()+view.getSize()*0.5f;
    translation.x = std::round(translation.x*zoom.x);
    translation.y = std::round(translation.y*zoom.y);
    sf::Transform transform = sf::Transform::Identity;
    transform.translate(translation);
    transform.scale(zoom);
    states.transform = transform;
    states.texture = m_anim.getTexture();

    target.draw(&m_vertices[0] , m_vertices.size() , sf::Triangles , states);
}