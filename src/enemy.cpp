#include "headers/enemy.hpp"
#include <iostream>

Eye::Eye(AssetManager* assets){
    m_assets = assets;
    m_attack = AttackState::ARROW;
    m_timer = 0.f;
    m_spawnRate = 0.2f;
    m_formDelay = 2.f;
    m_vertices.resize(8);
    m_vertices.setPrimitiveType(sf::TriangleFan);
    setSize({80 , 50});
    m_vertices[0].position = {0 , 0};
    m_vertices[1].position = {-40 , 0};
    m_vertices[2].position = {-20 , -25};
    m_vertices[3].position = {20 , -25};
    m_vertices[4].position = {40 , 0};
    m_vertices[5].position = {20 , 25};
    m_vertices[6].position = {-20 , 25};
    m_vertices[7].position = {-40 , 0};
}

void Eye::update(float dt , std::vector<EntityData> & projectiles , const sf::Vector2f & player_pos){
    Entity::update(dt);
    m_timer += dt;

    if(m_timer < 30.f){
        m_attack = AttackState::ARROW;
    } else if(m_timer < 90.f){
        m_attack = AttackState::CIRCLE;
    } else if(m_timer < 120.f){
        m_attack = AttackState::CIRCLEV2;
    } else if(m_timer < 150.f){
        m_attack = AttackState::SPIRAL;
    }

    auto v = player_pos-getPosition();
    std::cout << "oui" << std::endl;
    if(v.x*v.x+v.y*v.y != 0){
        auto proj = instanciateProjectile(m_assets->getTexture("projectile"));
        auto length = std::sqrt(v.x*v.x+v.y*v.y);
        proj.movement = sf::Vector2f{std::cos(v.x / length) , std::sin(v.y / length)}*50.f;
        proj.projectile.setPosition(getPosition());
        projectiles.push_back(proj);
    }
}

void Eye::display(sf::RenderWindow & window , sf::View view) {
    Entity::display(window , view);
    sf::Vector2f tvpSize { Const::ORIGINAL_WINSIZE };
    auto zoom = sf::Vector2f(tvpSize.x / (view.getSize().x) , tvpSize.y / (view.getSize().y));
    sf::Vector2f p = (getPosition()-view.getCenter()+view.getSize()*0.5f+m_pupil.offset);
    m_pupil.setScale(zoom);
    m_pupil.setPosition(p*zoom);
    window.draw(m_pupil.red);
    window.draw(m_pupil.black);
}

void Eye::updatePupil(const sf::Vector2f & player_pos){
    auto v = player_pos-getPosition();
    if(v.x+v.y != 0){
        auto n = std::atan2(v.y , v.x);
        m_pupil.offset = sf::Vector2f{std::cos(n) , std::sin(n)}*4.f;
    }
}


Pupil::Pupil(float radius){
    red = sf::CircleShape(radius);
    black = sf::CircleShape(radius*0.8);
    red.setFillColor({244 , 89 , 120});
    black.setFillColor(sf::Color::Black);
    red.setOrigin({red.getRadius(),red.getRadius()});
    black.setOrigin({black.getRadius(),black.getRadius()});
}

void Pupil::setPosition(sf::Vector2f pos){
    red.setPosition(pos);
    black.setPosition(pos);
}

void Pupil::setScale(sf::Vector2f scale){
    red.setScale(scale);
    black.setScale(scale);
}