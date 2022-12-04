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
    m_textState = TextState::TEXTURE;
}

void Particle::move(const sf::Vector2f & movement){
    m_position += movement;
}

bool Particle::isAlive() const {
    return m_alive;
}

void Particle::display(sf::RenderTarget & target , sf::View view) {
    sf::RenderStates states;
    sf::Vector2f tvpSize { Const::ORIGINAL_WINSIZE };
    auto zoom = sf::Vector2f(tvpSize.x / (view.getSize().x) , tvpSize.y / (view.getSize().y));
    auto textoffset = sf::Vector2f{m_textSize}*.5f;
    auto translation = m_position-textoffset-view.getCenter()+view.getSize()*0.5f;
    translation.x = std::round(translation.x*zoom.x);
    translation.y = std::round(translation.y*zoom.y);
    sf::Transform transform = sf::Transform::Identity;
    transform.translate(translation);
    transform.scale(sf::Vector2f(zoom.x,zoom.y));
    states.transform = transform;
    if(m_textState == TextState::TEXTURE){
        states.texture = &*m_texture;
    } else if (m_textState == TextState::ANIMATION){
        states.texture = m_anim.getTexture();
    }

    target.draw(m_vertices , states);
}

void Particle::setTextPos(sf::Vector2f start , sf::Vector2f end){

    sf::Vector2f ts = end-start;

    for(int i = 0;i < m_vertices.getVertexCount();i++){
        sf::Vertex* vertex = &m_vertices[i];
        vertex->position = sf::Vector2f(ts.x*(i%2),ts.y*(i/2));
        vertex->texCoords = start+sf::Vector2f(ts.x*(i%2),ts.y*(i/2));
    }

    m_textSize = ts;
}

void Particle::setTexture(sf::Texture & texture){
    m_texture = &texture;
    m_textState = TextState::TEXTURE;
    if(m_texture){
        setTextPos({0,0},sf::Vector2f{m_texture->getSize()});
    }
}


sf::VertexArray & Particle::getVertices(){
    return m_vertices;
}

void Particle::update(float dt){

    if(m_alive){
        move(movement*speed*dt);
        m_duration -= dt;

        if(m_textState == TextState::ANIMATION){
            m_anim.update(dt);
            auto r = m_anim.getTextRect();
            setTextPos(sf::Vector2f(r.getPosition()),sf::Vector2f(r.getPosition()+r.getSize()));
        }

        if(m_duration <= 0){
            m_alive = false;
        }
    }
}

void Particle::setAnimation(Animation & animation){
    m_anim = animation;
    auto r = m_anim.getTextRect();
    setTextPos(sf::Vector2f(r.getPosition()) , sf::Vector2f(r.getPosition()+r.getSize()));
    m_textState = TextState::ANIMATION;
}

const sf::Vector2f & Particle::getPosition() const {
    return m_position;
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
    m_textState = TextState::TEXTURE;
    m_light = sf::CircleShape();
    m_light.setScale(sf::Vector2f(3,3));
}

void ParticleSystem::setPosition(const sf::Vector2f & position){
    m_position = position;
}

void ParticleSystem::setTexture(sf::Texture & texture){
    m_texture = &texture;
    m_textState = TextState::TEXTURE;
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
    m_textState = TextState::ANIMATION;
    m_anim = animation;
}

void ParticleSystem::update(float dt){

    for (int i = m_particles.size()-1;i >= 0;i--){
        m_particles[i].update(dt);
        if(!m_particles[i].isAlive())
            m_particles.erase(m_particles.begin()+i);
    }

    if(m_continuous){
        m_timer += dt;
        if(m_timer - m_spawnRate >= 0.f){
            auto a = Random::randFloat(m_urdfs["angle"])*((float)M_PI/180.f);
            auto s = Random::randFloat(m_urdfs["speed"]);
            auto d = Random::randFloat(m_urdfs["duration"]);
            auto offX = Random::randFloat(m_urdfs["offsetX"]);
            auto offY = Random::randFloat(m_urdfs["offsetY"]);

            Particle particle{m_position+sf::Vector2f{offX , offY} , {std::cos(a) , -std::sin(a)} , s , d};
            particle.setAnimation(m_anim);
            m_particles.push_back(particle);
            m_timer = 0.f;
        }
    }
}

void ParticleSystem::clear(){
    m_particles.clear();
}

void ParticleSystem::setContinuous(bool state){
    m_continuous = state;
}

void ParticleSystem::setLightRadius(float radius){
    m_light.setRadius(radius);
}

void ParticleSystem::setLightColor(sf::Color color){
    m_light.setFillColor(color);
}

void ParticleSystem::spawnParticles(unsigned int n){
    for (int i = 0; i < n; i++)
    {
        auto a = Random::randFloat(m_urdfs["angle"])*((float)M_PI/180.f);
        auto s = Random::randFloat(m_urdfs["speed"]);
        auto d = Random::randFloat(m_urdfs["duration"]);
        auto offX = Random::randFloat(m_urdfs["offsetX"]);
        auto offY = Random::randFloat(m_urdfs["offsetY"]);

        Particle particle{m_position+sf::Vector2f{offX , offY} , {std::cos(a) , -std::sin(a)} , s , d};
        particle.setAnimation(m_anim);
        m_particles.push_back(particle);
    }
    
}

void ParticleSystem::display(sf::RenderTarget & target , sf::View view) {
    sf::BlendMode b {};
    b.alphaEquation = sf::BlendMode::Add;
    sf::Vector2f tvpSize { Const::ORIGINAL_WINSIZE };
    auto zoom = sf::Vector2f(tvpSize.x / (view.getSize().x) , tvpSize.y / (view.getSize().y));
    for(auto & particle : m_particles){
        auto pos = particle.getPosition();
        sf::Vector2f p = (pos-sf::Vector2f{m_light.getRadius(),m_light.getRadius()}-view.getCenter()+view.getSize()*0.5f);
        m_light.setPosition({p.x*zoom.x , p.y*zoom.y});
        m_light.setScale(zoom);
        target.draw(m_light , sf::BlendAdd);
        particle.display(target , view);
    }
}