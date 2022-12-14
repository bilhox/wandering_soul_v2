#pragma once

#include <SFML/Graphics.hpp>
#include "json.hpp"
#include <string>
#include <fstream>
#include <vector>
#include <filesystem>

class Animation{

    public:
        std::string name;
    public:
        Animation();
        bool load(std::string anim_path);
        void update(float dt);
        void setTexture(sf::Texture & texture);
        void prepareTexture(sf::VertexArray & vertices);
        void reset();
        sf::Texture* getTexture();
        sf::IntRect getTextRect();
        const std::string & getName() const;
    
    private:
        
        sf::Texture* m_sheet;
        std::vector<float> m_timepoints;
        float m_duration;
        float m_timer;
        int m_currentIndex;
        sf::Vector2u m_slice;
        bool m_infinite;
        std::string m_srcName;

};