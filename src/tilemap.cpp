#include "headers/tilemap.hpp"
#include "headers/utils.hpp"
#include <iostream>
#include <filesystem>
#include <algorithm>
#include <fstream>

Tilemap::Tilemap(){
    m_LayerToDisplay = -1;
    m_colliders = {};
    m_layer_datas = {};
    m_layers = {};
}

const ObjectData & Tilemap::getObject(std::string name) {
    if(m_objects.find(name) != m_objects.end()){
        return m_objects[name];
    } else {
        throw std::runtime_error("Could not find an object with the name : "+name);
    }
}

bool Tilemap::load(std::string tmx_path){

    m_colliders.clear();
    m_layer_datas.clear();
    m_layers.clear();
    m_LayerToDisplay = -1;
    m_tileSize = {0,0};
    m_tmSize = {0,0};

    std::ifstream ftmx{tmx_path};

    if(!ftmx){
        std::cout << "Failed to load tilemap , the path given is incorrect" << std::endl;
        return false;
    }

    nlohmann::json tmx_file = nlohmann::json::parse(ftmx);

    auto pathC = std::filesystem::path(tmx_path);
    pathC.replace_filename(tmx_file["tilesets"][0]["source"]);

    std::ifstream ftsx{pathC.string()};
    nlohmann::json tsx_file = nlohmann::json::parse(ftsx);

    pathC.replace_filename(tsx_file["image"]);

    if(!m_tileset.loadFromFile(pathC.string())){
        return false;
    }

    m_tmSize = sf::Vector2u(tmx_file["width"] , tmx_file["height"]);
    m_tileSize = sf::Vector2u(tmx_file["tilewidth"] , tmx_file["tileheight"]);

    // std::cout << m_tmSize.x << " ; " << m_tmSize.y << std::endl;
    auto layers = tmx_file["layers"];
    int l = -1;
    for(auto layer : layers){

        if(layer["type"] != "tilelayer"){
            if(layer["type"] == "objectgroup"){
                if(layer["name"] == "colliders"){
                    for(auto & object : layer["objects"]){
                        auto collider = sf::FloatRect(sf::Vector2f{object["x"] , object["y"]},sf::Vector2f{object["width"],object["height"]});
                        m_colliders.push_back(collider);
                    }
                } else if (layer["name"] == "objects"){
                    for(auto & object : layer["objects"]){
                        ObjectData obj;
                        obj.name = object["name"];
                        obj.rect.left = object["x"];
                        obj.rect.top = object["y"];
                        obj.rect.width = object["width"];
                        obj.rect.height = object["height"];
                        if(object.contains("properties")){
                            obj.properties = object["properties"];
                        }

                        m_objects[object["name"]] = obj;
                    }
                }
            }
            continue;
        }
        auto layer_data = layer["data"].get<std::vector<unsigned int>>();

        m_layer_datas.push_back(layer_data);
        l++;
        
        sf::VertexArray vlayer;
        vlayer.resize(6*m_tmSize.x*m_tmSize.y);
        vlayer.setPrimitiveType(sf::Triangles);

        for (int j = 0; j < m_tmSize.y; j++)
        {
            for (int i = 0; i < m_tmSize.x; i++)
            {
                int tileNumber = m_layer_datas[l][i + j * m_tmSize.x];
                // std::cout << tileNumber << " ";
                if(!tileNumber){
                    continue;
                }

                int tu = (tileNumber-1) % (m_tileset.getSize().x / 16);
                int tv = (tileNumber-1) / (m_tileset.getSize().x / 16);

                sf::Vertex* tile = &vlayer[(i + j * m_tmSize.x) * 6];

                tile[0].position = sf::Vector2f(i * 16, j * 16);
                tile[1].position = sf::Vector2f((i + 1) * 16, j * 16);
                tile[2].position = sf::Vector2f(i * 16, (j + 1) * 16);

                tile[3].position = sf::Vector2f((i + 1) * 16, j * 16);
                tile[4].position = sf::Vector2f(i * 16, (j + 1) * 16);
                tile[5].position = sf::Vector2f((i + 1) * 16, (j + 1) * 16);

                tile[0].texCoords = sf::Vector2f(tu * 16, tv * 16);
                tile[1].texCoords = sf::Vector2f((tu + 1) * 16, tv * 16);
                tile[2].texCoords = sf::Vector2f(tu * 16, (tv + 1) * 16);

                tile[3].texCoords = sf::Vector2f((tu + 1) * 16, tv * 16);
                tile[4].texCoords = sf::Vector2f(tu * 16, (tv + 1) * 16);
                tile[5].texCoords = sf::Vector2f((tu + 1) * 16, (tv + 1) * 16);
            }

        }

        m_layers.push_back(vlayer);
    
    }

    return true;
}

void Tilemap::draw(sf::RenderTarget & target , sf::View view) const {
    sf::RenderStates states {};
    sf::Vector2f tvpSize { Const::ORIGINAL_WINSIZE };
    auto zoom = sf::Vector2f(tvpSize.x / (view.getSize().x) , tvpSize.y / (view.getSize().y));
    auto translation = getPosition()-view.getCenter()+view.getSize()*0.5f;
    translation.x = std::round(translation.x*zoom.x);
    translation.y = std::round(translation.y*zoom.y);
    sf::Transform transform = sf::Transform::Identity;
    transform.translate(translation);
    transform.scale(sf::Vector2f(zoom.x,zoom.y));
    states.transform = transform;
    states.texture = &m_tileset;

    if(m_LayerToDisplay == -1){
        for (auto & layer : m_layers){
            target.draw(layer , states);
        }
    } else if (m_LayerToDisplay >= 0 && m_LayerToDisplay < m_layers.size()) {
        target.draw(m_layers[m_LayerToDisplay] , states);
    }
}

const sf::Vector2u Tilemap::getSize() const {
    return m_tmSize;
}

const sf::Vector2u Tilemap::getTileSize() const {
    return m_tileSize;
}

void Tilemap::display(sf::RenderWindow & window , sf::View view , int lindex) {
    m_LayerToDisplay = lindex;
    draw(window , view);
}

const std::vector<sf::FloatRect> & Tilemap::getColliders() const {
    return m_colliders;
}