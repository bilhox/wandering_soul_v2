#ifndef ANIMATION_HPP
#define ANIMATION_HPP
#pragma once

#include <SFML/Graphics.hpp>
#include "json.hpp"
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>

class Animation{

    public:
        Animation();
        bool load(std::string anim_path);
        void update(float dt);
        void prepareTexture(sf::VertexArray & vertices);
        sf::Texture & getTexture();
    
    private:
        
        sf::Texture m_sheet;
        std::vector<float> m_timepoints;
        float m_duration;
        float m_timer;
        int m_currentIndex;
        sf::Vector2u m_slice;


};

#endif // ANIMATION_HPP