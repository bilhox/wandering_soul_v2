#include <SFML/Graphics.hpp>
#include "headers/entity.hpp"
#include <iostream>
#include "headers/tilemap.hpp"
#include "headers/player.hpp"
#include "headers/utils.hpp"
#include "headers/enemy.hpp"
#include "headers/light.hpp"
#include "headers/text.hpp"
#include "headers/mana.hpp"
#include <cmath>
#include <random>
#include <array>
#include <thread>

int main()
{

    auto map = Tilemap();

    map.load("../assets/levels/entrance.json");

    sf::FloatRect mapRect = {{0,0},sf::Vector2f{(float)map.getSize().x*map.getTileSize().x , (float)map.getSize().y*map.getTileSize().y}};
    auto window = sf::RenderWindow{ {Const::ORIGINAL_WINSIZE.x , Const::ORIGINAL_WINSIZE.y} , "XML parser test"};
    // window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(120);
    window.setKeyRepeatEnabled(false);

    AssetManager assets{};
    assets.loadFromFile("../assets/assets.json");

    Player player {&assets};

    auto startPos = map.getObject("player_position");
    player.setPosition(startPos.rect.getPosition());
    float death_time = 2.f;
    float proj_spawn = 0.5f;

    sf::Clock clock{};
    float dt = 0;

    std::vector<Text> texts;

    for(auto obj : map.getObjects("text")){
        Text t {assets};
        t.setPosition(obj.rect.getPosition());
        t.setText(obj.properties[0]["value"]);
        t.setColor({255 , 255 , 255});
        t.setShadowColor({0, 152, 219});
        t.setShadowOffset({0 , 1});
        t.setOrigin(sf::Vector2f(t.getSize())/2.f);
        texts.push_back(t);
    }

    std::vector<ManaOrb> orbs;

    // Orb shader things

    sf::Shader orbShader {};
    sf::Clock orbShaderClock {};

    if(!orbShader.loadFromFile("../assets/manaOrb.frag" , sf::Shader::Fragment)){
        throw std::runtime_error("Failed to shader manaOrb.");
    }

    orbShader.setUniform("u_resolution" , sf::Vector2f(16.f , 16.f));

    // Level orbs

    for(auto obj : map.getObjects("mana")){
        ManaOrb orb{orbShader};
        orb.setPosition(obj.rect.getPosition());
        orbs.emplace_back(std::move(orb));
    }

    std::vector<sf::FloatRect> colliders{};
    
    sf::View camera {sf::FloatRect({0,0},{300 , 200})};

    auto dObj = map.getObject("door");

    auto door = instanciateDoor(&assets);
    door.door.setPosition(dObj.rect.getPosition()+sf::Vector2f{4,0});
    door.destination = "../assets/levels/"+dObj.properties[0]["value"].get<std::string>();
    door.destination += ".json";
    door.visible = dObj.properties[1]["value"].get<bool>();

    int level = 0;

    std::vector<Spark> sparks;

    sf::Texture proj;
    proj.loadFromFile("../assets/images/projectile.png");

    float dtAvg = 0.f;
    int tick = 0;

    bool projSpawning = false;
    bool invincible = true;
    float slowTime = 0.f;
    float zoomTime = 1.2f;
    float game_timer = 0.f;
    float transition_time = 0.f;
    float lspt = 0.f;
    bool transition = false;
    bool level_finished = false;
    bool paused = false;

    sf::VertexArray black_filter;
    black_filter.resize(4);
    black_filter.setPrimitiveType(sf::TriangleStrip);

    // Particle system

    ParticleSystem pSys {{0,0}};

    pSys.setAnimation(assets.getAnimation("player_particle"));
    pSys.setContinuous(false);
    pSys.setRange("speed" , 80 , 120);
    pSys.setRange("angle" , 0 , 360);
    pSys.setRange("duration" , 0.6f , 1.2f);
    pSys.setRange("offsetX" , 0 , 0);

    for(int i = 0;i < black_filter.getVertexCount();i++){
        sf::Vertex & vertex = black_filter[i];
        vertex.position = sf::Vector2f((i%2)*900.f , (i/2)*600.f);
        vertex.color = sf::Color(0,0,0,0);
    }

    std::vector<EntityData> projectiles{};
    std::vector<sf::Vector3f> lights{};

    // Eye enemy in level 3

    Eye eye {&assets};
    eye.player = &player;
    eye.projectiles = &projectiles;
    eye.lights = &lights;
    eye.setPosition({324 , 365});
    eye.setTargetPos(player.getPosition());

    camera.setCenter((player.getPosition()+player.getSize()*.5f));

    // Lightning system

    sf::RenderTexture lrt;
    lrt.create(300 , 200);
    lrt.clear({0,0,0,0});

    sf::VertexArray result;
    result.resize(4);
    result.setPrimitiveType(sf::TriangleStrip);

    for(int i = 0;i < result.getVertexCount();i++){
        sf::Vertex & vertex = result[i];
        vertex.position = sf::Vector2f((i%2)*300.f , (i/2)*200.f);
    }

    sf::VertexArray lightRendering;
    lightRendering.resize(4);
    lightRendering.setPrimitiveType(sf::TriangleStrip);

    for(int i = 0;i < lightRendering.getVertexCount();i++){
        sf::Vertex & vertex = lightRendering[i];
        vertex.position = sf::Vector2f((i%2)*900.f , (i/2)*600.f);
        vertex.texCoords = sf::Vector2f((i%2)*300.f , (i/2)*200.f);
    }

    sf::RenderStates lightRenderingStates;
    lightRenderingStates.texture = &lrt.getTexture();
    lightRenderingStates.blendMode = sf::BlendAdd;

    sf::Shader lightningShader;

    if (!sf::Shader::isAvailable())
    {
        std::cerr << "Shader are not available" << std::endl;
        return -1;
    }


    if(!lightningShader.loadFromFile("../assets/lightning.frag" , sf::Shader::Fragment)){
        std::cout << "Failed to load fragment lightningShader" << std::endl;
        return -1;
    }

    // Define lights 

    std::vector<Light> lightTypes;

    Light light {};
    light.radius = 20.f*20.f;
    light.color = {12.f/255.f , 2.f/255.f , 6.f/255.f};
    lightTypes.push_back(light);

    lightningShader.setUniform("lights[0].squaredRadius" , light.radius);
    lightningShader.setUniform("lights[0].color" , light.color);

    // Define UI texts

    Text manaCounterText {assets};
    manaCounterText.setPosition({10 , 20});
    manaCounterText.setText("Mana :");
    manaCounterText.setColor({255 , 255 , 255});
    manaCounterText.setScale({3 , 3});
    manaCounterText.setShadowColor({0, 152, 219});
    manaCounterText.setShadowOffset({0 , 1});

    std::vector<ManaOrb> orbAmount;
    int ySize = manaCounterText.getSize().y;
    {
        ManaOrb orb {orbShader};
        orb.setPosition(sf::Vector2f{10.f,35+ySize});
        orb.setScale({3 , 3});
        orbAmount.emplace_back(std::move(orb));
    }

    Text soulTutoText {assets};
    soulTutoText.setPosition(sf::Vector2f(Const::ORIGINAL_WINSIZE)/2.f-sf::Vector2f(0 , 50.f));
    soulTutoText.setText("Press z to release/unrelease soul");
    soulTutoText.setColor(sf::Color::White);
    soulTutoText.setScale({3 , 3});
    soulTutoText.setShadowColor({0 , 152 , 219});
    soulTutoText.setShadowOffset({0 , 1});
    soulTutoText.setOrigin(sf::Vector2f(soulTutoText.getSize())*3.f/2.f);

    unsigned int manaCounter = 1;

    std::array<float , 2> playerSoulTimer {10.f , 0.f};

    // Sounds 

    std::unordered_map<std::string , sf::Sound> gameSounds;
    std::array<std::string , 3> sbNames {"door" , "mana_1" , "mana_2"};

    for(auto & sn : sbNames){
        sf::Sound sound {};
        sound.setBuffer(assets.getSoundBuffer(sn));
        gameSounds[sn] = sound;    
    }

    while (window.isOpen())
    {
        dt = clock.getElapsedTime().asSeconds();
        clock.restart();

        dtAvg += dt;
        tick ++;
        if(dtAvg >= 1.f){
            float fps = 1/(dtAvg/(float)tick);
            tick = 0;
            dtAvg = 0.f;
            window.setTitle("FPS : " + std::to_string(fps));
        }

        if(slowTime > 0.f){
            slowTime -= dt;
            dt *= 0.5f;
        }

        game_timer += dt;

        for (auto event = sf::Event{}; window.pollEvent(event);)
        {
            if(window.hasFocus() && !transition && !paused){
                player.events(event , window , dt);
            }

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
                
                case sf::Event::KeyPressed:
                    if(event.key.code == sf::Keyboard::Z){
                        
                        if(paused){
                            slowTime = 1.f;
                            paused = false;
                            player.changeState();
                            player.setSoulReleasingAbility(true);
                            player.setMovementAbility(true);
                            playerSoulTimer[1] = playerSoulTimer[0];
                        } else if (player.isAbleToReleaseSoul()){
                            if(!player.isSoul() && manaCounter > 0){
                                player.changeState();
                                manaCounter --;
                                orbAmount.pop_back();
                                playerSoulTimer[1] = playerSoulTimer[0];
                            } else if(player.isSoul()) {
                                player.changeState();
                                playerSoulTimer[1] = 0.f;
                            }
                        }
                    }
                    break;

                default:
                    break;
            }
        }

        orbShader.setUniform("u_time" , orbShaderClock.getElapsedTime().asSeconds());

        if(playerSoulTimer[1] > 0.f){
            playerSoulTimer[1] -= dt;
            if(playerSoulTimer[1] <= 0.f){
                player.changeState();
            }
        }

        if(!paused)
            pSys.update(dt);

        if(level == 2){

            if(game_timer > 30.f && game_timer < 40.f){
                if(lspt == 0.f){
                    int dir = Random::randInt(urdi(0 , 1));
                    for(int i = 0;i < 22;i++){
                        sf::FloatRect camRect = {camera.getCenter()-camera.getSize()*0.5f,camera.getSize()};
                        auto entData = instanciateProjectile(&assets);
                        auto pp = sf::Vector2f{(dir)?(0):((float)(map.getSize().x*map.getTileSize().x)),((dir)?0:50)+(((float)(map.getSize().y*map.getTileSize().y) / 20.f)*i)};
                        entData.projectile.setPosition(pp);
                        entData.movement = sf::Vector2f((dir)?1:-1 , 0)*50.f;
                        projectiles.push_back(entData);
                        lights.push_back(entData.projectile.getLightDatas());
                        for(int i = 0;i < 6 ;i++){
                            float sp = Random::randFloat(urdf(150,200));
                            float sc = Random::randFloat(urdf(5.5f,7.5f));
                            float ang = Random::randFloat(urdf(-30,30));
                            Spark s {{(dir)?(camRect.left):camRect.left+camRect.width,pp.y},(float)(M_PI/180.f)*(ang+(dir)?0:180),(dir)?sp:-sp,sc,sp/100.f};
                            s.setColor(sf::Color::Black);
                            sparks.push_back(s);
                        }
                    }
                }
                lspt += dt;
                if(lspt > 1.f)
                    lspt = 0.f;
            } else if (game_timer > 45.f && !level_finished) {
                door.visible = true;
                pSys.setPosition(door.door.getPosition()+door.door.getSize()*0.5f);
                pSys.spawnParticles(25);
                level_finished = true;
            }
            
        }


        if(transition){
            transition_time += dt;
            camera.setSize(sf::Vector2f(300.f , 200.f)*interpolate(sf::Vector2f(1.0 , 1.0) , {0.4 , 0.4} , (std::min(1.0f , transition_time))));
            for(int i = 0;i < black_filter.getVertexCount();i++){
                sf::Vertex & vertex = black_filter[i];
                vertex.color = sf::Color(0,0,0,(int)(std::min(transition_time , 1.f)*255));
            }
            if(transition_time > 1.f){
                map.load(door.destination);
                texts.clear();

                for(auto obj : map.getObjects("text")){
                    Text t {assets};
                    t.setPosition(obj.rect.getPosition());
                    t.setText(obj.properties[0]["value"]);
                    t.setColor({255 , 255 , 255});
                    t.setShadowColor({0, 152, 219});
                    t.setShadowOffset({0 , 1});
                    t.setOrigin(sf::Vector2f(t.getSize())/2.f);
                    texts.push_back(t);
                }
                
                player.respawn(map.getObject("player_position").rect.getPosition());
                projectiles.clear();
                lights.clear();
                orbs.clear();
                auto dObj = map.getObject("door");

                door.door.setPosition(dObj.rect.getPosition()+sf::Vector2f(4,0));
                door.destination = "../assets/levels/"+dObj.properties[0]["value"].get<std::string>();
                door.destination += ".json";
                door.visible = dObj.properties[1]["value"].get<bool>();
                transition = false;
                if(level != 4 && level != 1)
                    projSpawning = true;
                if(level == 3)
                    eye.setTargetPos(player.getPosition());
                level_finished = false;
                player.setMovementAbility(true);
                mapRect = {{0,0},sf::Vector2f{(float)map.getSize().x*map.getTileSize().x , (float)map.getSize().y*map.getTileSize().y}};
                for(auto obj : map.getObjects("mana")){
                    ManaOrb orb{orbShader};
                    orb.setPosition(obj.rect.getPosition());
                    orbs.emplace_back(std::move(orb));
                }
            }
        } else if (transition_time > 0.f){
            transition_time -= dt;
            camera.setSize(sf::Vector2f(300.f , 200.f)*interpolate(sf::Vector2f(1.0 , 1.0) , {0.4 , 0.4} , std::max(0.0f , transition_time)));
            for(int i = 0;i < black_filter.getVertexCount();i++){
                sf::Vertex & vertex = black_filter[i];
                vertex.color = sf::Color(0,0,0,(int)(std::max(transition_time , 0.f)*255));
            }
        } else {
            transition_time = 0.f;
            camera.setSize(sf::Vector2f(300.f , 200.f));
        }

        if(!paused){
            player.update(dt);
            player.collisions(map.getColliders() , camera);
            player.postUpdate(dt);
        }

        if(level == 3 && !transition){
            eye.update(dt);
            eye.updatePupil(player.getPosition());
        }

        if(level == 1 && player.isAlive() && !transition && !projSpawning && map.getObject("ps1").rect.getPosition().x < player.getPosition().x+player.getSize().x){
            for(int i = 0;i < 22;i++){
                sf::FloatRect camRect = {camera.getCenter()-camera.getSize()*0.5f,camera.getSize()};
                auto entData = instanciateProjectile(&assets);
                auto pp = sf::Vector2f{camRect.left+camRect.width,(((float)(map.getSize().y*map.getTileSize().y) / 20.f)*i)};
                entData.projectile.setPosition(pp);
                entData.movement = sf::Vector2f(-1 , 0)*50.f;
                projectiles.push_back(entData);
                lights.push_back(entData.projectile.getLightDatas());
                for(int i = 0;i < 6 ;i++){
                    float sp = Random::randFloat(urdf(200,250));
                    float sc = Random::randFloat(urdf(8.f,10.5f));
                    float ang = Random::randFloat(urdf(-20,20));
                    Spark s {{pp.x,pp.y},(float)(M_PI/180.f)*(ang+180),sp,sc,sp/100.f};
                    s.setColor(sf::Color::Black);
                    sparks.push_back(s);
                }
            }
            paused = true;
            projSpawning = true;
            player.setMovementAbility(false);
        }

        if(level == 3 && eye.isDead() && !level_finished){
            door.visible = true;

            pSys.setAnimation(assets.getAnimation("particle"));
            pSys.setRange("speed" , 100 , 150);
            pSys.setRange("angle" , 0 , 360);
            pSys.setRange("duration" , 2.f , 2.5f);
            pSys.setRange("offsetX" , 0 , 0);
            pSys.setPosition(eye.getPosition());
            pSys.spawnParticles(40);

            pSys.setAnimation(assets.getAnimation("player_particle"));
            pSys.setRange("speed" , 80 , 120);
            pSys.setRange("angle" , 0 , 360);
            pSys.setRange("duration" , 0.6f , 1.2f);
            pSys.setRange("offsetX" , 0 , 0);
            pSys.setPosition(door.door.getPosition()+door.door.getSize()*0.5f);
            pSys.spawnParticles(25);
            level_finished = true;
            projSpawning = false;
        }

        if(!paused){
            for(int i = projectiles.size() -1 ; i >= 0 ; i--){
                projectiles[i].projectile.move(projectiles[i].movement*dt);
                lights[i] = projectiles[i].projectile.getLightDatas();
                if(projSpawning && !transition){
                    if(!invincible && player.isAlive() && player.getRect().intersects(projectiles[i].projectile.getRect())){
                        player.die();
                        projectiles.clear();
                        lights.clear();

                        if(level == 3){
                            eye.resetAttacks();
                        } 

                        game_timer = 0.f;
                        if(level == 1){
                            projSpawning = false;
                        } else {
                            door.visible = false;
                        }
                        zoomTime = 0.f;
                        break;
                    }
                }
                if(!projectiles[i].projectile.getRect().intersects(mapRect)){
                        projectiles.erase(projectiles.begin()+i);
                        lights.erase(lights.begin()+i);
                    }
            }

            for(int i = sparks.size()-1;i >= 0;i--){
            
                sparks[i].speedScale -= 3.5f * dt;
                sparks[i].speed -= 17.5f * dt;
                sparks[i].update(dt);
                if(sparks[i].speedScale <= 0){
                    sparks.erase(sparks.begin()+i);
                }
            }
        }

        lightningShader.setUniformArray("positions" , &lights[0] , lights.size());

        lightningShader.setUniform("nLight" , (int)lights.size());
        lightningShader.setUniform("viewOrigin" , (camera.getCenter()-camera.getSize()/2.f));

        if(!player.isAlive()){
            death_time -= dt;
            if(death_time <= 0.f){
                death_time = 2.f;
                player.respawn(map.getObject("player_position").rect.getPosition());
            }
        }

        if(!player.isSoul() && player.isAlive())
            camera.move(((player.getPosition()+player.getSize()*.5f)-camera.getCenter())*0.1f);
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

        if(projSpawning && !paused && !transition && player.isAlive()){
            proj_spawn -= dt;
            if(proj_spawn <= 0.f){
                auto p = instanciateProjectile(&assets);
                int angle = Random::randInt(urdi(60,120));
                sf::FloatRect camRect = {camera.getCenter()-camera.getSize()*0.5f,camera.getSize()};
                int randXpos = Random::randInt(urdi(0,(int)camRect.width));
                p.movement = sf::Vector2f{std::cos((float)(M_PI/180)*angle),std::sin((float)(M_PI/180)*angle)}*50.f;
                p.projectile.setPosition({camRect.left+randXpos,camRect.top});
                projectiles.push_back(p);
                lights.push_back(p.projectile.getLightDatas());
                if(level == 1){
                    proj_spawn = 0.5f;
                }
                else if (level == 2) {
                    proj_spawn = 0.3f;
                }
                else if (level == 3){
                    proj_spawn = 0.2f;
                }
                for(int i = 0;i < 6 ;i++){
                    float sp = Random::randFloat(urdf(150,200));
                    float sc = Random::randFloat(urdf(6.f,9.5f));
                    float ang = Random::randFloat(urdf(-30,30));
                    Spark s {{camRect.left+randXpos,camRect.top},(float)(M_PI/180.f)*(ang+angle),sp,sc,sp/100.f};
                    s.setColor(sf::Color::Black);
                    sparks.push_back(s);
                }
            }
        }

        // Test if the player entered the door

        if(player.isAlive() && !transition && door.visible && !player.isSoul() && player.getRect().intersects(door.door.getRect())){
            transition = true;
            level += 1;
            game_timer = 0.f;
            player.setMovementAbility(false);
            gameSounds["door"].play();
        }

        if(player.isAlive()){
            for (int i = orbs.size()-1;i>=0;i--){
                if(orbs[i].collideRect(player.getRect())){
                    manaCounter ++;
                    gameSounds["mana_1"].play();
                    gameSounds["mana_2"].play();

                    ManaOrb orb {orbShader};
                    orb.setPosition(sf::Vector2f{10.f+(16.f)*orbAmount.size()*3.f,35+ySize});
                    orb.setScale({3 , 3});
                    orbAmount.emplace_back(std::move(orb));

                    pSys.setAnimation(assets.getAnimation("player_particle"));
                    pSys.setRange("speed" , 100 , 175);
                    pSys.setRange("angle" , 0 , 360);
                    pSys.setRange("duration" , 3.f , 3.5f);
                    pSys.setRange("offsetX" , 0 , 0);
                    pSys.setPosition(orbs[i].getPosition());
                    pSys.spawnParticles(40);
                    orbs.erase(orbs.begin()+i);
                }
            }
        }

        window.clear({20, 19, 39});
        lrt.clear({0,0,0,0});
        map.display(window , camera , 0);
        if(door.visible)
            door.door.display(window , camera);
        if(level == 3 && !eye.isDead() && !transition)
            eye.display(window , camera);
        if(!player.isSoul())
            player.display(window , camera , 0);
        map.display(window , camera , 1);
        player.display(window , camera , 1);
        if(player.isSoul() && player.isAlive()){
            player.display(window , camera , 0);
        }
        for(auto & orb : orbs){
            orb.display(window , camera);
        }

        for(auto & p : projectiles){
            p.projectile.display(window , camera);
        }
        for(auto & spark : sparks){
            spark.draw(window , camera);
        }
        pSys.display(window , camera);
        
        for(auto & text : texts){
            text.display(window , camera);
        }

        if(paused)
            soulTutoText.display(window , window.getDefaultView());
        manaCounterText.display(window , window.getDefaultView());
        for(auto & orb : orbAmount){
            orb.display(window , window.getDefaultView());
        }

        lrt.draw(result , &lightningShader);
        lrt.display();
        window.draw(lightRendering , lightRenderingStates);
        window.draw(black_filter);
        window.display();
    }

    return 0;
}