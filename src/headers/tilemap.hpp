#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <array>
#include <map>

struct ObjectData{

    std::string name;
    std::string type;
    sf::FloatRect rect;
};

class Tilemap : public sf::Transformable {

    public:

        Tilemap();
        bool load(std::string tmx_path);
        const sf::Vector2u getSize() const;
        const sf::Vector2u getTileSize() const;
        const std::vector<sf::FloatRect> & getColliders() const;
        const ObjectData & getObject(std::string name) ;
        void display(sf::RenderWindow & window , sf::View view , int lindex = -1);

    private:

        std::vector<std::vector<std::array<sf::Vertex , 4>>> m_layers;
        sf::Texture m_tileset;
        sf::Vector2u m_tileSize;
        sf::Vector2u m_tmSize;
        std::vector<std::vector<unsigned int>> m_layer_datas;
        int m_LayerToDisplay;
        std::vector<sf::FloatRect> m_colliders;
        std::map<std::string , ObjectData> m_objects;

        void draw(sf::RenderTarget & target , sf::View view) const;

};