#pragma once

#include <SFML/Graphics.hpp>
#include "json.hpp"
#include "animation.hpp"
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <filesystem>


class AssetManager {

    public:
        AssetManager();
        bool loadFromFile(std::string json_path);
        bool load(std::string name , std::string path);
        // bool loadShader(std::string name , std::string vertPath , std::string fragPath);
        sf::Texture& getTexture(std::string name);
        Animation& getAnimation(std::string name);
        // sf::Shader& getShader(std::string name);
    
    private:
        std::map<std::string , sf::Texture> m_textures;
        std::map<std::string , Animation> m_animations;
        // std::map<std::string , sf::Shader> m_shaders;
};