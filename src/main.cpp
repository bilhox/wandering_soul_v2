#include <SFML/Graphics.hpp>
#include "headers/entity.hpp"
#include <iostream>
#include "headers/tilemap.hpp"
#include "headers/player.hpp"
#include <cmath>

int main()
{

    auto map = Tilemap();
    map.load("../assets/map_test.json");
    auto window = sf::RenderWindow{ { 900u, 600u }, "XML parser test"};
    window.setFramerateLimit(120);

    Player player {};
    player.setPosition({300 , 100});

    sf::Clock clock{};
    float dt = 0;

    std::vector<sf::FloatRect> colliders{};
    
    sf::View camera {sf::FloatRect({0,0},{300 , 200})};
    // sf::View IntCamera;
    // window.setView(camera);
    
    // for (auto & rect : map.getColliders()){
    //     std::cout << rect.left << " ; " << rect.top << " ; " << rect.width << " ; " << rect.height << std::endl;
    // }

    float dtAvg = 0.f;
    int tick = 0;

    while (window.isOpen())
    {
        dt = clock.getElapsedTime().asSeconds();
        clock.restart();

        dtAvg += dt;
        tick ++;
        if(dtAvg >= 0.2f){
            float fps = 1/(dtAvg/(float)tick);
            tick = 0;
            dtAvg = 0.f;
            window.setTitle("FPS : " + std::to_string(fps));
        }

        for (auto event = sf::Event{}; window.pollEvent(event);)
        {
            switch(event.type){
                case sf::Event::Closed:
                    window.close();
                    break;
                
                case sf::Event::Resized:
                    
                    {   
                        auto viewSize = window.getView().getSize();
                        auto winView = window.getView();
                        winView.setViewport({
                            0.5f-(viewSize.x/2.f)/(float)event.size.width,
                            0.5f-(viewSize.y/2.f)/(float)event.size.height,
                            (viewSize.x)/(float)event.size.width,
                            (viewSize.y)/(float)event.size.height
                        });
                        window.setView(winView);
                    }
                    break;
                
                default:
                    break;
            }
            if(window.hasFocus()){
                player.events(event , window , dt);
            }
        }

        player.update(dt);
        player.collisions(map.getColliders());
        player.postUpdate(dt);
        // std::cout << player.getPosition().x << " ; " << player.getPosition().y << std::endl;

        camera.setCenter(player.getPosition()+player.getSize()*.5f);
        sf::Vector2f topleftCameraCorner = (camera.getCenter()-camera.getSize()/2.f);

        if(topleftCameraCorner.x < 0){
            camera.setCenter({(camera.getSize()/2.f).x , camera.getCenter().y});
        } else if ((camera.getCenter()+camera.getSize()*0.5f).x > map.getSize().x*map.getTileSize().x){
            camera.setCenter({map.getSize().x*map.getTileSize().x-(camera.getSize()/2.f).x,camera.getCenter().y});
        }
        if (topleftCameraCorner.y < 0){
            camera.setCenter({camera.getCenter().x,(camera.getSize()/2.f).y});
        } else if ((camera.getCenter()+camera.getSize()*0.5f).y > map.getSize().y*map.getTileSize().y){
            camera.setCenter({camera.getCenter().x,map.getSize().y*map.getTileSize().y-(camera.getSize()/2.f).y});
        }

        // auto camPos = camera.getCenter();
        // camera.setCenter({std::trunc(camPos.x) , std::trunc(camPos.y)});

        window.clear();
        map.display(window , camera , 0);
        player.display(window , camera);
        map.display(window , camera , 1);
        window.display();
    }

    return 0;
}