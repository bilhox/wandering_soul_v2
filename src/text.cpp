#include "headers/text.hpp"
#include "headers/utils.hpp"
#include <iostream>

FontData loadFont(sf::Texture & font){
    sf::Image fimage = font.copyToImage();
    FontData fdata {};
    fdata.height = fimage.getSize().y;
    unsigned int fwidth = fimage.getSize().x;

    std::vector<char> letters {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','.','-',',',':','+','\'','!','?','0','1','2','3','4','5','6','7','8','9','(',')','/','_','=','\\','[',']','*','"','<','>',';'};

    unsigned int lwidth = 0;
    int lindex = 0;
    for (int i = 0; i <= fwidth ; i++){
        auto cpixel = fimage.getPixel(i , 0);
        if(cpixel == sf::Color::Red){
            fdata.letter_widths[letters.at(lindex)] = lwidth;
            fdata.letter_bpos[letters.at(lindex)] = i-lwidth;
            lindex ++;
            lwidth = 0;
        } else {
            lwidth ++;
        }
    }

    return fdata;
}

Text::Text(AssetManager& assets){
    m_font = &assets.getTexture("small font");
    m_letter_spacing = 1;
    m_vertices.setPrimitiveType(sf::Triangles);
    m_fdata = loadFont(*m_font);
    m_size = {0 , m_fdata.height};
}

void Text::setText(std::string text){
    m_text = text;
    m_vertices.clear();
    m_vertices.resize(m_text.length()*6);

    int n = 0;
    unsigned int bpos = 0;
    for(auto c : m_text){
        
        if(c == ' ' || c == '\n'){
            n++;
            bpos += 2+m_letter_spacing;
            continue;
        }

        sf::Vertex* vertex = &m_vertices[n*6];

        vertex[0].texCoords = sf::Vector2f{m_fdata.letter_bpos[c] , 0.f};
        vertex[1].texCoords = sf::Vector2f{m_fdata.letter_bpos[c]+m_fdata.letter_widths[c] , 0.f};
        vertex[2].texCoords = sf::Vector2f{m_fdata.letter_bpos[c] , m_fdata.height};
        vertex[3].texCoords = sf::Vector2f{m_fdata.letter_bpos[c]+m_fdata.letter_widths[c] , 0.f};
        vertex[4].texCoords = sf::Vector2f{m_fdata.letter_bpos[c] , m_fdata.height};
        vertex[5].texCoords = sf::Vector2f{m_fdata.letter_bpos[c]+m_fdata.letter_widths[c] , m_fdata.height};

        vertex[0].position = {bpos , 0.f};
        vertex[1].position = {bpos+m_fdata.letter_widths[c] , 0.f};
        vertex[2].position = {bpos , m_fdata.height};
        vertex[3].position = {bpos+m_fdata.letter_widths[c] , 0.f};
        vertex[4].position = {bpos , m_fdata.height};
        vertex[5].position = {bpos+m_fdata.letter_widths[c] , m_fdata.height};

        bpos += m_fdata.letter_widths[c]+m_letter_spacing;

        n++;
    }

    m_size.x = bpos;
    setColor(m_color);
}

sf::Vector2u Text::getSize() const {
    return m_size;
}

void Text::setColor(sf::Color color){

    m_color = color;
    for (int i = 0; i < m_vertices.getVertexCount();i++){
        m_vertices[i].color = m_color;
    }
}

void Text::display(sf::RenderTarget & target , sf::View view){
    sf::RenderStates states;
    sf::Vector2f tvpSize { Const::ORIGINAL_WINSIZE };
    auto zoom = sf::Vector2f(tvpSize.x / (view.getSize().x) , tvpSize.y / (view.getSize().y));
    auto translation = getPosition()-getOrigin()-view.getCenter()+view.getSize()*0.5f;
    translation.x = std::round(translation.x*zoom.x);
    translation.y = std::round(translation.y*zoom.y);
    sf::Transform transform = sf::Transform::Identity;
    transform.translate(translation);
    transform.scale(zoom*getScale());
    auto origin = getOrigin();
    transform.rotate(getRotation() , origin.x , origin.y);
    states.transform = transform;
    states.texture = m_font;

    target.draw(m_vertices , states);
}