#include "headers/player.hpp"
#include "headers/collision_utils.hpp"
#include "cmath"
#include <iostream>

bool Player::isSoul() const {
    return (m_state == State::SOUL);
}

void Player::update(float dt){

    m_pSys.update(dt);
    if(m_alive){

        m_airtime ++;

        if(m_keys["left"]){
            m_velocity.x = -((m_state == State::SOUL)?50:100) * dt;
            m_flip = true;
        }else if(m_keys["right"]){
            m_velocity.x = ((m_state == State::SOUL)?50:100) * dt;
            m_flip = false;
        }
        else {
            m_velocity.x *= 0.5f;
        }

        if(m_state == State::SOUL){
            if(m_keys["up"]){
                m_velocity.y = -50 * dt;
            }else if(m_keys["down"]){
                m_velocity.y = 50 * dt;
            }
            else {
                m_velocity.y *= 0.5f;
            }
        }

        if(std::abs(m_velocity.x) < 0.001)
            m_velocity.x = 0;
        if(std::abs(m_velocity.y) < 0.001)
            m_velocity.y = 0;
        
    } else {
        m_rotation += 380.f * 5 * dt;
        m_velocity.x = 50.f * dt;
    }

    if(m_state == State::NORMAL){
        float gravityTemp = m_gravityDt;
        m_gravityDt += 2.75*dt;
        m_velocity.y = 20*std::pow(m_gravityDt,2)-20*std::pow(gravityTemp,2);
    }
    
    m_velocity.x = std::max(-10.f , (float) std::min(10.f , m_velocity.x));
    m_velocity.y = std::max(-10.f , (float) std::min(10.f , m_velocity.y));

    for(int i = m_sparks.size()-1;i >= 0;i--){
        m_sparks[i].update(dt);
        m_sparks[i].speedScale -= 3.5f * dt;
        m_sparks[i].speed -= 17.5f * dt;
        if(m_sparks[i].speedScale < 0){
            m_sparks.erase(m_sparks.begin()+i);
        }
    }

    if(m_soulTimer[1] > 0.f){
            m_soulTimer[1] -= dt;
            if(m_soulTimer[1] <= 0.f){
                changeState();
            }
        }

}

bool Player::canEnterTheDoor(Door & door) const {
    return (
        !isSoul() &&
        isAlive() &&
        getRect().intersects(door.door.getRect()) &&
        m_airtime <= 2
    );
}

void Player::die(){
    m_velocity = {0,-2};
    m_keys = {{"left" , false},{"right" , false},{"up" , false},{"down" , false}};
    m_alive = false;
    m_anim = &m_assets->getAnimation("idle");
    m_anim->reset();
    m_texture = m_anim->getTexture();
    m_state = State::NORMAL;
    m_textState = TextState::ANIMATION;
    setSize({7 , 13});
    setTextSize({13 , 17});
    for(int i = 0;i < 16 ;i++){
        float speed = Random::randFloat(urdf(200,250));
        float scale = Random::randFloat(urdf(4.5f,6.5f));
        float angle = Random::randFloat(urdf(0,380));
        Spark s {getPosition()+getOrigin(),(float)(M_PI/180.f)*(angle),speed,scale,speed/100.f};
        s.setColor(sf::Color::White);
        m_sparks.push_back(s);
    }
    m_pSys.setAnimation(m_assets->getAnimation("particle"));
    m_pSys.setContinuous(false);
    m_pSys.setPosition(m_rect.getPosition()+m_rect.getSize()*0.5f);
    m_pSys.setRange("speed" , 100 , 150);
    m_pSys.setRange("angle" , 0 , 360);
    m_pSys.setRange("duration" , 2.5 , 2.5);
    m_pSys.setRange("offsetX" , 0 , 0);
    m_pSys.spawnParticles(25);
    m_pSys.setRange("speed" , 5 , 8);
    m_pSys.setRange("angle" , -90 , -90);
    m_pSys.setRange("duration" , 2.6 , 2.6);
    m_pSys.setRange("offsetX" , -4 , 4);
    m_pSys.setAnimation(m_assets->getAnimation("player_particle"));
    m_gravityDt = -1.25f;
    m_sounds["death"].play();
    m_soulTimer[1] = 0.f;
}

void Player::respawn(sf::Vector2f pos){
    m_velocity = {0,0};
    m_keys = {{"left" , false},{"right" , false},{"up" , false},{"down" , false}};
    m_anim = &m_assets->getAnimation("idle");
    setSize({7 , 13});
    setTextSize({13 , 17});
    setPosition(pos);
    m_anim->reset();
    m_texture = m_anim->getTexture();
    m_textOffset = sf::Vector2f(3,3);
    m_textState = TextState::ANIMATION;
    m_state = State::NORMAL;
    m_alive = true;
    m_rotation = 0.f;
    m_pSys.clear();
    m_gravityDt = 0.f;
}

const sf::FloatRect Player::getRect() const {
    if(m_state == State::NORMAL){
        return m_rect;
    } else {
        return sf::FloatRect(m_rect.getPosition()+sf::Vector2f{0,3},{7,7});
    }
}

bool Player::isAlive() const {
    return m_alive;
}

void Player::changeState() {
    if(m_state == State::SOUL){
        m_state = State::NORMAL;
        resetTextCoords();
        setTextSize({13,17});
        setTextOffset({3,3});
        m_pSys.setContinuous(false);
        m_pSys.setPosition(m_rect.getPosition()+m_rect.getSize()*0.5f);
        m_pSys.setRange("speed" , 5 , 20);
        m_pSys.setRange("angle" , 0 , 360);
        m_pSys.setRange("duration" , 0.5f , 0.8f);
        m_pSys.setRange("offsetX" , 0 , 0);
        m_pSys.spawnParticles(25);
        m_pSys.setRange("speed" , 5 , 8);
        m_pSys.setRange("angle" , -90 , -90);
        m_pSys.setRange("duration" , 2.6 , 2.6);
        m_pSys.setRange("offsetX" , -4 , 4);
        m_gravityDt = 0.f;
        m_sounds["exit_soul"].play();
        m_soulTimer[1] = 0.f;
    } else {
        m_soulTimer[1] = m_soulTimer[0];
        m_gravityDt = 0.f;
        m_state = State::SOUL;
        resetTextCoords();
        setTextSize({9,9});
        setTextOffset({1,-2});
        m_pSys.setContinuous(true);
        m_pSys.setPosition(m_rect.getPosition()+m_rect.getSize()*0.5f);
        m_pSys.setRange("speed" , 5 , 20);
        m_pSys.setRange("angle" , 0 , 360);
        m_pSys.setRange("duration" , 0.5f , 0.8f);
        m_pSys.setRange("offsetX" , 0 , 0);
        m_pSys.spawnParticles(25);
        m_pSys.setRange("speed" , 5 , 8);
        m_pSys.setRange("angle" , -90 , -90);
        m_pSys.setRange("duration" , 2.6 , 2.6);
        m_pSys.setRange("offsetX" , -4 , 4);
        m_sounds["enter_soul"].play();
    }
}

void Player::setSoulReleasingAbility(bool state){
    m_ableToReleaseSoul = state;
}

void Player::events(sf::Event & event , sf::Window & window , float dt){

    if(!window.hasFocus() || !m_alive)
        return;

    switch(event.type){

        case sf::Event::KeyPressed:
            if(m_ableToMove){
                if(event.key.code == sf::Keyboard::Right){
                m_keys["right"] = true;
                } else if(event.key.code == sf::Keyboard::Left){
                    m_keys["left"] = true;
                } else if(event.key.code == sf::Keyboard::Up){
                    m_keys["up"] = true;
                } else if(event.key.code == sf::Keyboard::Down){
                    m_keys["down"] = true;
                }
            }

            if (m_state == State::NORMAL && event.key.code == sf::Keyboard::Up){
                if(m_airtime <= 3){
                    m_gravityDt = -m_jumpAmount;
                    m_sounds["jump"].play();
                    float speed = Random::randFloat(urdf(30,50));
                    float scale = Random::randFloat(urdf(2.5f,4.5f));
                    for(int i = 0;i < 2;i++){
                        Spark s {getPosition()+sf::Vector2f(m_rect.width/2.f-(4*(i?1:-1)) , m_rect.height+1),(float)(M_PI/180.f)*(20.f+140.f*i),speed,scale,speed/50.f};
                        s.setColor(sf::Color::White);
                        m_sparks.push_back(s);
                    }
                }
            }
            break;
        
        case sf::Event::KeyReleased:
            if(m_ableToMove){
                if(event.key.code == sf::Keyboard::Right){
                m_keys["right"] = false;
                } else if(event.key.code == sf::Keyboard::Left){
                    m_keys["left"] = false;
                } else if(event.key.code == sf::Keyboard::Up){
                    m_keys["up"] = false;
                } else if(event.key.code == sf::Keyboard::Down){
                    m_keys["down"] = false;
                }
            }
            break;

        default:
            break;
    }
}

void Player::setMovementAbility(bool state){
    m_ableToMove = state;
    if(!m_ableToMove){
        m_keys = {{"left" , false},{"right" , false},{"up" , false},{"down" , false}};
    }
}

Player::Player(AssetManager* assets) : Entity(assets){
    m_velocity = {0,0};
    m_keys = {{"left" , false},{"right" , false},{"up" , false},{"down" , false}};
    m_jumpAmount = 1.4f;
    m_gravityDt = 0.f;
    m_anim = &m_assets->getAnimation("idle");
    setSize({7 , 13});
    setOrigin({5,7});
    setTextSize({13 , 17});
    m_texture = m_anim->getTexture();
    m_textOffset = sf::Vector2f(3,3);
    m_textState = TextState::ANIMATION;
    m_state = State::NORMAL;
    m_alive = true;
    m_pSys.setAnimation(m_assets->getAnimation("player_particle"));
    m_pSys.setPosition(m_rect.getPosition()+m_rect.getSize()*0.5f);
    m_pSys.setRange("speed" , 5 , 8);
    m_pSys.setRange("angle" , -90 , -90);
    m_pSys.setRange("duration" , 2.6 , 2.6);
    m_pSys.setSpawnRate(.15f);
    m_pSys.setRange("offsetX" , -4 , 4);
    m_pSys.setContinuous(false);
    m_ableToMove = true;
    m_ableToReleaseSoul = false;

    // Loading sounds

    std::array<std::string , 4> soundNames {"jump", "enter_soul" , "exit_soul" , "death"};

    for(auto & sn : soundNames){
        sf::Sound sound {};
        sound.setBuffer(assets->getSoundBuffer(sn));
        m_sounds[sn] = sound;    
    }

}

bool Player::isAbleToReleaseSoul() const {
    return m_ableToReleaseSoul;
}

void Player::collisions(const std::vector<sf::FloatRect> & colliders , sf::View & view){

    sf::FloatRect viewrect{view.getCenter()-view.getSize()*0.5f,view.getSize()};
    m_collisionSide = {{"right" , false},{"left" , false},{"top" , false},{"down" , false}};
    move({m_velocity.x , 0});
    auto collided = collide(colliders , m_rect);
    if(m_alive){
        for (auto & collider : collided){
            if(m_velocity.x < 0){
                setPosition({collider.left+collider.width,m_rect.top});
                m_collisionSide["left"] = true;
            }
            else if (m_velocity.x > 0){
                setPosition({collider.left-m_rect.width,m_rect.top});
                m_collisionSide["right"] = true;
            }
        }
    }

    if(m_state == State::SOUL &&!overlaps(m_rect,viewrect)){
        if(m_velocity.x > 0){
            setPosition({viewrect.left+viewrect.width-m_rect.width,m_rect.top});
        } else {
            setPosition({viewrect.left,m_rect.top});
        }
    }

    move({0,m_velocity.y});
    if(m_alive){
        collided = collide(colliders , m_rect);
        for (auto & collider : collided){
            if(m_velocity.y < 0){
                setPosition({m_rect.left,collider.top+collider.height});
                m_collisionSide["top"] = true;
            }
            else if (m_velocity.y > 0){
                setPosition({m_rect.left,collider.top-m_rect.height});
                m_collisionSide["down"] = true;
            }
        }
    }

    if(m_state == State::SOUL &&!overlaps(m_rect,viewrect)){
        if(m_velocity.y > 0){
            setPosition({m_rect.left,viewrect.top+viewrect.height-m_rect.height});
        } else {
            setPosition({m_rect.left,viewrect.top});
        }
    }
}

void Player::postUpdate(float dt){

    if(m_state == State::SOUL){
        auto rs = getRect();
        m_pSys.setPosition(rs.getPosition()+rs.getSize()*0.5f);
    }

    if(!m_alive)
        return;

    if(m_state == State::NORMAL){
        if(m_collisionSide["down"]){
        m_airtime = 0;
        m_velocity.y = 0;
        m_gravityDt = 0.f;
        } else if (m_collisionSide["top"]){
            m_gravityDt = 0.f;
        }
    }

    if(m_state == State::SOUL){
        if(m_textState == TextState::ANIMATION){
            m_anim->reset();
            m_anim = nullptr;
        }
        resetTextCoords();
        m_textState = TextState::TEXTURE;
        m_texture = &m_assets->getTexture("soul");
        return;
    }

    if(m_airtime > 3){
        if(m_textState == TextState::ANIMATION){
            m_anim->reset();
            m_anim = nullptr;
        }
        resetTextCoords();
        m_textState = TextState::TEXTURE;
        m_texture = &m_assets->getTexture("jumping");
    } else {
        m_textState = TextState::ANIMATION;
    }

    if(m_textState == TextState::ANIMATION && m_airtime < 3){
        if(m_collisionSide["right"] || m_collisionSide["left"] || (std::abs(m_velocity.x) < 0.01)){
            if(!m_anim || m_anim->name != "idle"){
                m_anim = &m_assets->getAnimation("idle");
                m_texture = m_anim->getTexture();
            }
        } else if(std::abs(m_velocity.x) > 0.01){
            if(!m_anim || m_anim->name != "running"){
                m_anim = &m_assets->getAnimation("running");
                m_texture = m_anim->getTexture();
            }
        }
    }

    if(m_textState == TextState::ANIMATION && m_anim){
        m_anim->update(dt);
        auto r = m_anim->getTextRect();
        setTextPos(sf::Vector2f(r.getPosition()),sf::Vector2f(r.getPosition()+r.getSize()));
    }
}

void Player::display(sf::RenderWindow & window , sf::View view , unsigned int to_draw){
    
    switch(to_draw){
        case 0:
            Entity::display(window , view);
            break;

        case 1:
            for(auto & spark : m_sparks){
                spark.draw(window , view);
            }
            m_pSys.display(window , view);
            break;
        
        default:
            break;
    }
}