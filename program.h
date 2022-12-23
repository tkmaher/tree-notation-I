//
//  program.h
//  tree-widget
//
//  Created by user on 12/20/22.
//

#ifndef program_h
#define program_h

#include <iostream>
#include "tree.h"
#include <dirent.h>

struct audioHandler {
    audioHandler() {
        soundMap["start"] = 0;
        soundMap["path_split"] = 0;
        soundMap["new_boxes"] = 0;
        soundMap["boxes_drawn"] = 0;
        soundMap["path_stopped"] = 0;
        soundMap["mouse_click"] = 0;
        soundMap["every_other_tick"] = 0;
    }
    
    std::map<std::string, int> channelMap{
        {"start", 1},
        {"path_split", 2},
        {"new_boxes", 3},
        {"boxes_drawn", 4},
        {"path_stopped", 5},
        {"every_other_tick", 6},
        {"mouse_click", 7},
    };
    
    audioHandler(std::vector<Mix_Chunk*> wavs_in) : wavs(wavs_in) { }
    
    std::vector<Mix_Chunk*> wavs;
    std::map<std::string, int> soundMap;
    
    //soundmap is a map of events to the sounds they correspond to, in ints
    //the ints in soundmap point to wavs in the vector "wavs", which contains the actual sounds
    //eventmap lists a series of events as "true" or false". if the event is true, the corresponding
    //sound is found in soundmap and played
    
    
    void audioPlay(int index, std::map<std::string, bool> &eventMap) {
        if (index % 2 == 1) {
            Mix_PlayChannel( -1, wavs[soundMap["every_other_tick"]], 0 );
        }
        
        for (std::map<std::string,bool>::iterator it= eventMap.begin(); it != eventMap.end(); ++it) {
            if (it->second == true) {
                Mix_HaltChannel(channelMap[it->first]);
                Mix_PlayChannel( channelMap[it->first], wavs[soundMap[it->first]], 0 );
                it->second = false;
            }
        }
    }
};

struct Program {
    
    Program(SDL_Renderer * rend_in, const Uint8 * keystates_in) : rend(rend_in), keystates(keystates_in) {

        
        Mix_OpenAudio( 48000, MIX_DEFAULT_FORMAT, 2, 4096 );
        
        DIR *dir = opendir("./sounds");
        if (!dir) {
            printf("ERROR: no valid sounds directory. Create a folder with name 'sounds'./n");
        } else {
            dirent *entry;
            while((entry = readdir(dir))!= NULL)
            {
                std::string s = entry->d_name;
                std::string suffix = "wav";
                if((s.size() >= suffix.size()) && equal(suffix.rbegin(), suffix.rend(), s.rbegin()))
                {
                    wav_names.push_back(s);
                }
            }
            std::sort(wav_names.begin(), wav_names.end());
            for (int i = 0; i < wav_names.size(); i++) {
                wavs.push_back(Mix_LoadWAV(("./sounds/" + wav_names[i]).c_str()));
            }
            closedir(dir);
            
            audio = audioHandler(wavs);
        }
    };
    
    SDL_Renderer * rend;
    const Uint8 * keystates;
    
    
    // controls animation loop
    int speed = 50;
    int max_diverge = 25;
    int can_end = 1;
    bool paused = false;
    bool hideMenu = false;
    int close = 0;
    int background = rand() % 100;
    int new_background = background;
    int index = 0;
    std::map<std::string, bool> eventMap;
    
    //handle text
    int pointerY = 20;
    TTF_Font * font = TTF_OpenFont("Roboto-Thin.ttf", 12);
    std::vector<std::pair<SDL_Surface*, SDL_Texture*>> surfaces;
    std::map<int, std::pair<int*, std::pair<int, int>>> yposMap;
    
    //handle audio
    Mix_Music *sfx = NULL;
    audioHandler audio;

    //The sound effects that will be used
    std::vector<Mix_Chunk*> wavs;
    std::vector<std::string> wav_names;
    
    void poll(SDL_Event event, Tree& tree) {
        if (SDL_PollEvent( &event ))
        {
            switch (event.type) {
                    
                case SDL_QUIT:
                    // handling of close button
                    close = 1;
                    break;
                        
                case SDL_MOUSEBUTTONDOWN:
                    int xM, yM;
                    SDL_GetMouseState( &xM, &yM );
                    tree.clickBox(xM, yM);
                    eventMap["mouse_click"] = true;
                    break;
                    
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_ESCAPE) {
                        close = 1;
                        break;
                    }
                    
                    if ((!paused && !hideMenu) || event.key.keysym.sym == SDLK_SPACE || event.key.keysym.sym == SDLK_TAB) {
                        switch(event.key.keysym.sym){
                            case SDLK_TAB:
                                hideMenu = !hideMenu;
                                break;
                            case SDLK_SPACE:
                                if (paused) {
                                    paused = false;
                                    Mix_Resume(-1);
                                } else {
                                    paused = true;
                                    Mix_Pause(-1);
                                }
                                break;
                            case SDLK_DOWN:
                                if (pointerY == 145) {
                                    pointerY = 200;
                                } else if (pointerY != 350) {
                                    pointerY += 25;
                                } else {
                                    pointerY = 20;
                                }
                                break;
                            case SDLK_UP:
                                if (pointerY == 200) {
                                    pointerY = 145;
                                } else if (pointerY != 20) {
                                    pointerY -= 25;
                                } else {
                                    pointerY = 350;
                                }
                                break;
                            case SDLK_RIGHT:
                                if (*(yposMap[pointerY].first) < yposMap[pointerY].second.second) {
                                    *(yposMap[pointerY].first) += 1;
                                } else {
                                    *(yposMap[pointerY].first) = yposMap[pointerY].second.first;
                                }
                                break;
                            case SDLK_LEFT:
                                if (*(yposMap[pointerY].first) > yposMap[pointerY].second.first) {
                                    *(yposMap[pointerY].first) -= 1;
                                } else {
                                    *(yposMap[pointerY].first) = yposMap[pointerY].second.second;
                                }
                                break;
                            case SDLK_r:
                                for (std::map<int, std::pair<int*, std::pair<int, int>>>::iterator it= yposMap.begin(); it != yposMap.end(); ++it) {
                                    if (rand() % 2) {
                                        *((it->second).first) += (rand() % 5) + 1;
                                        if (*((it->second).first) > (it->second).second.second) {
                                            *((it->second).first) = (it->second).second.second;
                                        }
                                    } else {
                                        *((it->second).first) -= (rand() % 5) + 1;
                                        if (*((it->second).first) < (it->second).second.first) {
                                            *((it->second).first) = (it->second).second.first;
                                        }
                                    }
                                }
                                break;
                            }
                    }
                break;

                }
        }
        
    }

    void displayText(std::string text, int x, int y, uint8_t background) {
        SDL_Color color = {background, background, background};
        SDL_Surface * surface = TTF_RenderText_Solid(font, text.c_str(), color);
        SDL_Texture * texture = SDL_CreateTextureFromSurface(rend, surface);
        int texW = 0;
        int texH = 0;
        SDL_QueryTexture(texture, NULL, NULL, &texW, &texH);
        SDL_Rect dstrect = { x, y, texW, texH };
        SDL_RenderCopy(rend, texture, NULL, &dstrect);
        surfaces.push_back(std::make_pair(surface, texture));
    }
    
    void renderText(const Tree tree) {
        
        displayText("r: randomize", 20, 743, (255 - background));
        displayText("tab: hide/show UI", 20, 768, (255 - background));
        displayText(">", 10, pointerY, (255 - background));
        displayText("ms/tick: " + std::to_string(speed), 20, 20, (255 - background));
        displayText("tree width: " + std::to_string(max_diverge), 20, 45, (255 - background));
        displayText("branching frequency: 1/" + std::to_string(tree.branching_freq), 20, 70, (255 - background));
        displayText("box frequency: 1/" + std::to_string(tree.box_freq), 20, 95, (255 - background));
        displayText("ending on/off: " + std::to_string(tree.can_end), 20, 120, (255 - background));
        displayText("ending frequency: 1/" + std::to_string(tree.ending_freq), 20, 145, (255 - background));
        displayText("play on tree start: " + wav_names[audio.soundMap["start"]], 20, 200, (255 - background));
        displayText("play on tree split: " + wav_names[audio.soundMap["path_split"]], 20, 225, (255 - background));
        displayText("play on box creation: " + wav_names[audio.soundMap["new_boxes"]], 20, 250, (255 - background));
        displayText("play on box scroll: " + wav_names[audio.soundMap["boxes_drawn"]], 20, 275, (255 - background));
        displayText("play on path end: " + wav_names[audio.soundMap["path_stopped"]], 20, 300, (255 - background));
        displayText("play every other tick: " + wav_names[audio.soundMap["every_other_tick"]], 20, 325, (255 - background));
        displayText("play on click: " + wav_names[audio.soundMap["mouse_click"]], 20, 350, (255 - background));
    }
    
    void makeMap(Tree &tree) {
        //map: yposition maps to respective int, with min and max
        yposMap[20] = std::make_pair(&speed, std::make_pair(5, 999));
        yposMap[45] = std::make_pair(&max_diverge, std::make_pair(1, 9999));
        yposMap[70] = std::make_pair(&(tree.branching_freq), std::make_pair(1, 9999));
        yposMap[95] = std::make_pair(&(tree.box_freq), std::make_pair(1, 9999));
        yposMap[120] = std::make_pair(&(tree.can_end), std::make_pair(0, 1));
        yposMap[145] = std::make_pair(&(tree.ending_freq), std::make_pair(1, 9999));
        yposMap[200] = std::make_pair(&(audio.soundMap["start"]), std::make_pair(0, wavs.size() - 1));
        yposMap[225] = std::make_pair(&(audio.soundMap["path_split"]), std::make_pair(0, wavs.size() - 1));
        yposMap[250] = std::make_pair(&(audio.soundMap["new_boxes"]), std::make_pair(0, wavs.size() - 1));
        yposMap[275] = std::make_pair(&(audio.soundMap["boxes_drawn"]), std::make_pair(0, wavs.size() - 1));
        yposMap[300] = std::make_pair(&(audio.soundMap["path_stopped"]), std::make_pair(0, wavs.size() - 1));
        yposMap[325] = std::make_pair(&(audio.soundMap["every_other_tick"]), std::make_pair(0, wavs.size() - 1));
        yposMap[350] = std::make_pair(&(audio.soundMap["mouse_click"]), std::make_pair(0, wavs.size() - 1));
    }
    
    void mainLoop() {
        Tree tree(rend, speed, max_diverge, can_end);
        makeMap(tree);
        
        while (!close) {
            SDL_Event event;
            // Events management
            poll(event, tree);
            
            //render cycle
            
            if (index == 81 && (paused == false)) {
                index = 0;
                tree.clear(rend, max_diverge);
                SDL_RenderClear(rend);
                SDL_RenderPresent(rend);
                if (rand() % 2) {
                    new_background = rand() % 100;
                } else {
                    new_background = (rand() % 100) + 155;
                }
            }
            
            if (index < 81 && (paused == false)) {
                //SDL_SetRenderDrawColor(rend, 213, 191, 154, 255);
                if (background != new_background && background < new_background) {
                    background += 10;
                } else if (background != new_background) {
                    background -= 10;
                }
                if (abs(new_background - background) < 10) {
                    background = new_background;
                }
                
                SDL_SetRenderDrawColor(rend, background, background, background, 255);
                SDL_RenderClear(rend);

                //show text
                if (!hideMenu) {
                    renderText(tree);
                    displayText("otherseas - tree notation I", 850, 768, (255 - background));
                } else {
                    displayText("otherseas - tree notation I", 20, 768, (255 - background));
                }
                
                
                //render paths
                tree.mainRender(rend, index, background, eventMap);
                audio.audioPlay(index, eventMap);
                
                index++;
                
         
                // triggers the double buffers
                // for multiple rendering
                SDL_RenderPresent(rend);
                SDL_Delay(speed);
                
                for (int i = 0; i < surfaces.size(); i++) {
                    SDL_DestroyTexture(surfaces[i].second);
                    SDL_FreeSurface(surfaces[i].first);
                }
                surfaces.clear();
            }
         
        }
     
        index = 0;
        
        //destroy sounds
        for (int i = 0; i < wavs.size(); i++) {
            Mix_FreeChunk(wavs[i]);
            wavs[i] = NULL;
        }
        Mix_FreeMusic( sfx );
        sfx = NULL;
     
        //destroy text
        TTF_CloseFont(font);
        TTF_Quit();
    }
};


#endif /* program_h */
