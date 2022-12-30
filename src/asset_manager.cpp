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
            auto textSource = source;
            textSource.replace_filename(text["source"]);
            load(text["name"] , textSource.string());
        }
    }

    if(assets.contains("animations")){
        for(auto & anim : assets["animations"]){
            auto animSource = source;
            animSource.replace_filename(anim["source"]);
            load(anim["name"] , animSource.string());
        }
    }

    // if(assets.contains("shaders")){
    //     for(auto & shader : assets["shaders"]){
    //         auto fragSource = source;
    //         if(fragSource.string() != "default")
    //             fragSource.replace_filename(shader["fragment"]);
    //         auto vertSource = source;
    //         if(vertSource.string() != "default")
    //             vertSource.replace_filename(shader["vertex"]);
    //         loadShader(shader["name"] , vertSource.string() , fragSource.string());
    //     }
    // }

    return true;
}

// bool AssetManager::loadShader(std::string name , std::string vertPath , std::string fragPath){

//     if(vertPath == "default" && fragPath == "default"){
//         std::cout << "vertex and fragment shaders cannot be DEFAULT in the same time !" << std::endl;
//         return false;
//     }

//     sf::Shader shader;
//     if(vertPath == "default"){
//         if(!shader.loadFromFile(fragPath , sf::Shader::Fragment))
//             return false;
//     } else if(fragPath == "default"){
//         if(!shader.loadFromFile(vertPath , sf::Shader::Vertex))
//             return false;
//     } else {
//         if(!shader.loadFromFile(vertPath , fragPath))
//             return false;
//     }

//     m_shaders.emplace(name , shader);
//     return true;
// }

bool AssetManager::load(std::string name , std::string path){

    auto source = std::filesystem::path(path);

    if(source.extension() == ".png" || source.extension() == ".jpg"){
        sf::Texture texture;
        if(!texture.loadFromFile(path))
            return false;
        
        m_textures[name] = texture;
        return true;
    } else if (source.extension() == ".json"){
        Animation animation;
    
        if(!animation.load(path))
            return false;
        
        animation.setTexture(getTexture(animation.getName()));

        animation.name = name;
        m_animations[name] = animation;
        return true;
    }
    return false;
}


sf::Texture & AssetManager::getTexture(std::string name){
    if(m_textures.find(name) != m_textures.end())
        return m_textures[name];
    
    throw std::runtime_error("With AssetManager object : Could not find a texture with the key '"+name+"'");
}

// sf::Shader& AssetManager::getShader(std::string name){
//     return m_shaders[name];
// }


Animation & AssetManager::getAnimation(std::string name){
    if(m_animations.find(name) != m_animations.end())
        return m_animations[name];
    
    throw std::runtime_error("With AssetManager object : Could not find an animation with the key '"+name+"'");
}