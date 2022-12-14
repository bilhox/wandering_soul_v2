#pragma once

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <string>
#include "asset_manager.hpp"

struct FontData{
    unsigned int height;
    std::unordered_map<char , unsigned int> letter_widths;
    std::unordered_map<char , unsigned int> letter_bpos;
};

FontData loadFont(sf::Texture & font);

class Text : public sf::Transformable {

    public:
        Text(AssetManager & assets);
        void setColor(sf::Color color);
        void setText(std::string text);
        void setShadowOffset(const sf::Vector2f & offset);
        void setShadowColor(sf::Color color);
        sf::Vector2u getSize() const;
        void display(sf::RenderTarget & target , sf::View view);
    
    private:
        sf::Texture* m_font = nullptr;
        sf::VertexArray m_vertices;
        std::string m_text;
        sf::Color m_color;
        unsigned int m_letter_spacing;
        sf::Vector2u m_size;
        FontData m_fdata;
        
        // Shadow
        sf::Vector2f m_sOffset;
        sf::Color m_sColor;

        void updateColor(sf::Color color);

};