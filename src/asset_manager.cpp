#include "headers/asset_manager.hpp"
#include <iostream>

AssetManager::AssetManager(){
    m_textures = {};
    m_animations = {};
}

bool AssetManager::loadFromFile(std::string json_path){

    std::ifstream fs {json_path};
    if(!fs){
        std::cout << "Failed to load assets from json file at "+json_path << std::endl;
        return false;
    }

    auto source = std::filesystem::path(json_path);

    nlohmann::json assets = nlohmann::json::parse(fs);
    if(assets.contains("textures")){
        for(auto & text : assets["textures"]){
            sf::Texture texture;
            auto textSource = source;
            textSource.replace_filename(text["source"]);
            if(!texture.loadFromFile(textSource.string())){
                continue;
            }
            
            m_textures[text["name"]] = texture;
        }
    }

    if(assets.contains("animations")){
        for(auto & anim : assets["animations"]){
            Animation animation;
            auto animSource = source;
            animSource.replace_filename(anim["source"]);
            if(!animation.load(animSource.string())){
                continue;
            }
            
            m_animations[anim["name"]] = animation;
        }
    }

    return true;
}