#include "headers/player.hpp"
#include "cmath"

void Player::update(float dt){

    m_airtime ++;

    if(m_keys["left"]){
        m_velocity.x = -100 * dt;
        m_flip = true;
    }else if(m_keys["right"]){
        m_velocity.x = 100 * dt;
        m_flip = false;
    }
    else {
        m_velocity.x *= 0.5f;
    }

    m_velocity.y = m_velocity.y + m_gravity * dt;

    if(std::abs(m_velocity.x) < 0.001)
        m_velocity.x = 0;
    if(std::abs(m_velocity.y) < 0.001)
        m_velocity.y = 0;
    
    m_velocity.x = std::max(-10.f , (float) std::min(10.f , m_velocity.x));
    m_velocity.y = std::max(-10.f , (float) std::min(10.f , m_velocity.y));

    m_anim.update(dt);
    m_anim.prepareTexture(m_vertices);
}

void Player::events(sf::Event & event , sf::Window & window , float dt){

    if(!window.hasFocus())
        return;

    switch(event.type){

        case sf::Event::KeyPressed:
            if(event.key.code == sf::Keyboard::Right){
                m_keys["right"] = true;
            } else if(event.key.code == sf::Keyboard::Left){
                m_keys["left"] = true;
            }

            if (event.key.code == sf::Keyboard::S){
                if(m_airtime <= 3)
                    m_velocity.y = -m_jumpAmount;
            }

            if(event.key.code == sf::Keyboard::R){
                setPosition({300 , 50});
                m_velocity = {0,0};
            }
            break;
        
        case sf::Event::KeyReleased:
            if(event.key.code == sf::Keyboard::Right){
                m_keys["right"] = false;
            } else if(event.key.code == sf::Keyboard::Left){
                m_keys["left"] = false;
            } 
            break;

        default:
            break;
    }
}

Player::Player(){
    m_velocity = {0,0};
    m_keys = {{"left" , false},{"right" , false}};
    m_jumpAmount = 1.75f;
    m_gravity = 3.f;
    m_assets = AssetManager();
    m_assets.loadFromFile("../assets/player_assets.json");
    m_anim = Animation();
    m_anim.load("../assets/images/animations/config2.json");
    setSize({7 , 13});
    setTextSize({13 , 17});
    m_texture = &m_anim.getTexture();
    m_textOffset = sf::Vector2f(3,3);
}

void Player::postUpdate(float dt){
    if(m_collisionSide["down"]){
        m_airtime = 0;
        m_velocity.y = 0;
    } else if (m_collisionSide["top"]){
        m_velocity.y = 1.f;
    }
}