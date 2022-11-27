#ifndef PLAYER_HPP
#define PLAYER_HPP
#pragma once

#include "entity.hpp"
#include "animation.hpp"
#include "asset_manager.hpp"
#include <map>

class Player : public Entity {

    public:
        Player();
        void update(float dt);
        void events(sf::Event & event , sf::Window & window , float dt);
        void postUpdate(float dt);
    
    private:

        enum class TextState{
            TEXTURE,
            ANIMATION
        };

        std::map<std::string , bool> m_keys;
        float m_gravity;
        float m_jumpAmount;
        int m_airtime;
        TextState textState;
        AssetManager m_assets;
        Animation m_anim;
        
};

#endif // PLAYER_HPP