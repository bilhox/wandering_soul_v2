#include "headers/enemy.hpp"
#include <iostream>

Eye::Eye(AssetManager* assets) : Entity(assets){
    m_assets = assets;
    m_attack = AttackState::ARROW;
    m_attacks = 0;
    m_spawnRate = 0.2f;
    m_formDelay = 2.f;
    m_eventTimer = m_formDelay;
    m_vertices.resize(8);
    m_vertices.setPrimitiveType(sf::TriangleFan);
    setSize({60 , 30});
    setVertexPositions();
    m_spawnForm = true;
    m_nProjSpawned = 0;
    m_targetPos = {0,0};
    m_stateDelay = 5.f;
    m_dead = false;
    m_deathTimer = 4.f;
}

void Eye::setTargetPos(const sf::Vector2f & pos){
    m_targetPos = pos;
}

void Eye::setVertexPositions(){
    m_vertices[0].position = {0 , 0};
    m_vertices[1].position = {-getSize().x/2.f , 0};
    m_vertices[2].position = {-getSize().x/4.f , -getSize().y/2.f};
    m_vertices[3].position = {getSize().x/4.f , -getSize().y/2.f};
    m_vertices[4].position = {getSize().x/2.f , 0};
    m_vertices[5].position = {getSize().x/4.f , getSize().y/2.f};
    m_vertices[6].position = {-getSize().x/4.f , getSize().y/2.f};
    m_vertices[7].position = {-getSize().x/2.f , 0};
}

void Eye::resetAttacks(){
    m_attack = AttackState::ARROW;
    m_attacks = 0;
    m_stateDelay = 5.f;
    m_formDelay = 2.f;
    m_eventTimer = m_formDelay;
    m_nProjSpawned = 0;
    m_deathTimer = 4.f;
    m_spawnRate = 0.2f;
    m_dead = false;
    setTargetPos(player->getPosition());
}

bool Eye::isDead(){
    return m_dead;
}

void Eye::update(float dt) {
    m_spawnRate -= dt;
    if(m_attacks == 10 && m_attack == AttackState::ARROW){
        m_attack = AttackState::CIRCLE;
        m_formDelay = 1.25f;
        m_stateDelay = 5.f;
    }else if(m_attacks == 15 && m_attack == AttackState::CIRCLE){
        m_attack = AttackState::SPIRAL;
        m_stateDelay = 5.f;
    } else if(m_attacks == 16 && m_attack == AttackState::SPIRAL){
        m_attack = AttackState::CIRCLEV2;
        m_stateDelay = 7.5f;
    } else if(m_attacks == 17 && m_attack == AttackState::CIRCLEV2){
        m_attack = AttackState::SPIRALV2;
        m_stateDelay = 7.5f;
    } else if(m_attacks == 18 && m_attack == AttackState::SPIRALV2){
        m_attack = AttackState::NONE;
        m_stateDelay = 5.f;
    }

    if(m_stateDelay < 0.f){
        setSize({60 , getSize().y+(30-getSize().y)/20.f});
        setVertexPositions();
        if(m_attack == AttackState::ARROW && m_spawnForm){
            if(m_spawnRate < 0.f){
                auto v = m_targetPos-getPosition();
                if(v.x*v.x+v.y*v.y != 0){
                    for(int i = -1;i < 2;i+=2){
                        auto proj = instanciateProjectile(m_assets);
                        auto angle = std::atan2(v.y , v.x);
                        proj.movement = sf::Vector2f{std::cos(angle) , std::sin(angle)}*125.f;
                        auto orthoOff = sf::Vector2f{-std::sin(angle),std::cos(angle)}*(float)m_nProjSpawned*4.f*(float)i;
                        proj.projectile.setPosition(getPosition()+orthoOff);
                        projectiles->push_back(proj);
                        lights->push_back(proj.projectile.getLightDatas());
                    }
                    for(int i = 0;i < 6 ;i++){
                        float sp = Random::randFloat(urdf(200,250));
                        float sc = Random::randFloat(urdf(7.f,8.5f));
                        float ang = Random::randFloat(urdf(0,360))*(M_PI/180.f);
                        auto off = sf::Vector2f{std::cos(ang),std::sin(ang)}*4.f;
                        Spark s {getPosition()+off,ang,sp,sc,sp/100.f};
                        s.setColor(sf::Color::Black);
                        m_sparks.push_back(s);
                    }
                    m_nProjSpawned ++;
                    m_spawnRate = 0.1f;
                }
            }
            if(m_nProjSpawned == 10){
                m_spawnForm = false;
                m_nProjSpawned = 0;
                m_attacks ++;
            }
        } else if (m_attack == AttackState::CIRCLE && m_spawnForm){
            for(int i = 0;i < 36;i++){
                auto proj = instanciateProjectile(m_assets);
                float angle = (M_PI*2)*(float)i/36.f;
                proj.movement = sf::Vector2f{std::cos(angle) , std::sin(angle)}*45.f;
                proj.projectile.setPosition(getPosition());
                projectiles->push_back(proj);
                        lights->push_back(proj.projectile.getLightDatas());
            }
            for(int i = 0;i < 6 ;i++){
                float sp = Random::randFloat(urdf(200,250));
                float sc = Random::randFloat(urdf(7.f,8.5f));
                float ang = Random::randFloat(urdf(0,360))*(M_PI/180.f);
                auto off = sf::Vector2f{std::cos(ang),std::sin(ang)}*4.f;
                Spark s {getPosition()+off,ang,sp,sc,sp/100.f};
                s.setColor(sf::Color::Black);
                m_sparks.push_back(s);
            }
            m_attacks ++;    
            m_spawnForm = false;
        } else if (m_attack == AttackState::SPIRAL && m_spawnForm){
            int n = m_nProjSpawned / 6;
            if(m_spawnRate < 0.f){
                for(int i = 0;i < 6;i++){
                    auto proj = instanciateProjectile(m_assets);
                    float angle = (M_PI*2) * (i / 6.f + n / 25.f);
                    proj.movement = sf::Vector2f{std::cos(angle) , std::sin(angle)}*30.f;
                    proj.projectile.setPosition(getPosition());
                    projectiles->push_back(proj);
                        lights->push_back(proj.projectile.getLightDatas());
                    m_nProjSpawned ++;
                }
                
                for(int i = 0;i < 6 ;i++){
                    float sp = Random::randFloat(urdf(200,250));
                    float sc = Random::randFloat(urdf(7.f,8.5f));
                    float ang = Random::randFloat(urdf(0,360))*(M_PI/180.f);
                    auto off = sf::Vector2f{std::cos(ang),std::sin(ang)}*4.f;
                    Spark s {getPosition()+off,ang,sp,sc,sp/100.f};
                    s.setColor(sf::Color::Black);
                    m_sparks.push_back(s);
                }
                m_spawnRate = 0.1f;
                
            }
            if(m_nProjSpawned >= 550){
                m_spawnForm = false;
                m_nProjSpawned = 0;
                m_attacks ++;
            }
        } else if (m_attack == AttackState::CIRCLEV2 && m_spawnForm){
            if(m_spawnRate < 0.f){
                for(int j = -1;j < 2;j+=2){

                    float n = ((m_nProjSpawned / 12) / 25.f) * j;

                    for(int i = 0;i < 6;i++){
                    auto proj = instanciateProjectile(m_assets);
                    float angle = (M_PI*2) * (n + i / 6.f);
                    proj.movement = sf::Vector2f{std::cos(angle) , std::sin(angle)}*30.f;
                    proj.projectile.setPosition(getPosition());
                    projectiles->push_back(proj);
                    lights->push_back(proj.projectile.getLightDatas());
                    m_nProjSpawned ++;
                    }
                }
                
                for(int i = 0;i < 6 ;i++){
                    float sp = Random::randFloat(urdf(200,250));
                    float sc = Random::randFloat(urdf(7.f,8.5f));
                    float ang = Random::randFloat(urdf(0,360))*(M_PI/180.f);
                    auto off = sf::Vector2f{std::cos(ang),std::sin(ang)}*4.f;
                    Spark s {getPosition()+off,ang,sp,sc,sp/100.f};
                    s.setColor(sf::Color::Black);
                    m_sparks.push_back(s);
                }
                m_spawnRate = 0.2f;
                
            }
            if(m_nProjSpawned >= 1000){
                m_spawnForm = false;
                m_nProjSpawned = 0;
                m_attacks ++;
            }
        } else if (m_attack == AttackState::SPIRALV2 && m_spawnForm){
            if(m_spawnRate < 0.f){
                float n = m_nProjSpawned / 3;
                for(int i = 0;i < 3;i++){
                    auto proj = instanciateProjectile(m_assets);
                    float angle = (M_PI*2) * (i / 3.f + n / 150.f);
                    proj.movement = sf::Vector2f{std::cos(angle) , std::sin(angle)}*30.f;
                    proj.projectile.setPosition(getPosition());
                    projectiles->push_back(proj);
                    lights->push_back(proj.projectile.getLightDatas());
                    m_nProjSpawned ++;
                }
                
                
                for(int i = 0;i < 6 ;i++){
                    float sp = Random::randFloat(urdf(200,250));
                    float sc = Random::randFloat(urdf(7.f,8.5f));
                    float ang = Random::randFloat(urdf(0,360))*(M_PI/180.f);
                    auto off = sf::Vector2f{std::cos(ang),std::sin(ang)}*4.f;
                    Spark s {getPosition()+off,ang,sp,sc,sp/100.f};
                    s.setColor(sf::Color::Black);
                    m_sparks.push_back(s);
                }
                m_spawnRate = 0.05f;
                
            }
            if(m_nProjSpawned >= 600){
                m_spawnForm = false;
                m_nProjSpawned = 0;
                m_attacks ++;
            }
        } else if(m_attack == AttackState::NONE){
            m_deathTimer -= dt;
            auto off = urdi(0 , 8);
            setTextOffset(sf::Vector2f(Random::randInt(off)-4, Random::randInt(off)-4));

            if(m_deathTimer < 0.f){
                m_dead = true;
            }
        }
        else {
            m_eventTimer -= dt;
            m_targetPos = player->getPosition();
            if(m_eventTimer < 0.f){
                m_eventTimer = m_formDelay;
                m_spawnForm = true;
            }
        }
    } else {
        m_stateDelay -= dt;
        setSize({60 , getSize().y+(3-getSize().y)/20.f});
        setVertexPositions();
    }

    for (int i = m_sparks.size()-1;i >= 0;i--){
            
        m_sparks[i].speedScale -= 3.5f * dt;
        m_sparks[i].speed -= 20.5f * dt;
        m_sparks[i].update(dt);
        if(m_sparks[i].speedScale <= 0){
            m_sparks.erase(m_sparks.begin()+i);
        }
        
    }
}

void Eye::display(sf::RenderWindow & window , sf::View view) {
    Entity::display(window , view);
    if(getSize().y-2 > 16){
        sf::Vector2f tvpSize { Const::ORIGINAL_WINSIZE };
        auto zoom = sf::Vector2f(tvpSize.x / (view.getSize().x) , tvpSize.y / (view.getSize().y));
        sf::Vector2f p = (getPosition()-view.getCenter()+view.getSize()*0.5f+m_pupil.offset);
        m_pupil.setScale(zoom);
        m_pupil.setPosition(p*zoom);
        window.draw(m_pupil.red);
        window.draw(m_pupil.black);
    }
    for(auto & spark : m_sparks){
        spark.draw(window , view);
    }
}

void Eye::updatePupil(const sf::Vector2f & player_pos){
    auto v = player_pos-(getPosition()+getTextOffset());
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