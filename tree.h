//
//  tree.h
//  tree-widget
//
//  Created by user on 12/20/22.
//

#ifndef tree_h
#define tree_h

#include <vector>
#include <string>
#include <map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>


struct Line {
    Line();
    Line(int x1_in, int y1_in, int x2_in, int y2_in)
    : x1(x1_in), y1(y1_in), x2(x2_in), y2(y2_in) {}
    int x1;
    int x2;
    int y1;
    int y2;
};

struct Path {
    Path() : xCoord1(600), yCoord1(0), xCoord2(600), yCoord2(0) {}
    Path(int x1_in, int y1_in, int x2_in, int y2_in, SDL_Renderer* r_in)
    : xCoord1(x1_in), yCoord1(y1_in), xCoord2(x2_in), yCoord2(y2_in), rend(r_in) {}
    std::vector<Line> lines;
    int xCoord1;
    int yCoord1;
    int xCoord2;
    int yCoord2;
    Line prev = Line(0, 0, 0, 0);
    int divergence = 0;
    int skew = 0;
    int max_div = 0;
    
    bool has_ended = false;
    SDL_Renderer* rend;
    
    Line drawPath () {
        if (divergence != 0) {
            if (abs(divergence) == max_div / 2) {
                if (divergence < 0) {
                    skew--;
                } else if (divergence > 0) {
                    skew++;
                }
                divergence = 0;
                yCoord2 += 10;

            } else if (divergence < 0) {
                xCoord2 -= 5;
                yCoord2 += 10;
                divergence--;
            } else {
                xCoord2 += 5;
                yCoord2 += 10;
                divergence++;
            }
        } else {
            int v = rand() % 4;
            switch(v) {
                case 0:
                    if (prev.x1 != xCoord1 + 5) {
                        xCoord2 += 5;
                    }
                    yCoord2 += 10;
                    break;
                case 1:
                    if (prev.x1 != xCoord1 - 5) {
                        xCoord2 -= 5;
                    }
                    yCoord2 += 10;
                    break;
                case 2:
                    xCoord2 = xCoord1;
                    yCoord2 += 10;
                    break;
                case 3:
                    if (prev.x1 != xCoord1 + (5 * skew)) {
                        xCoord2 = xCoord1 + (5 * skew);
                    }
                    yCoord2 += 10;
                    break;
            }
        }
        if (!has_ended) {
            Line l(xCoord1, yCoord1, xCoord2, yCoord2);
            lines.push_back(l);
            prev = l;
            SDL_RenderDrawLine(rend, xCoord1, yCoord1, xCoord2, yCoord2);
            xCoord1 = xCoord2;
            yCoord1 = yCoord2;
        }
        for (int i = 0; i < lines.size(); ++i)
        {
            SDL_RenderDrawLine(rend, lines[i].x1, lines[i].y1, lines[i].x2, lines[i].y2);
        }
                
        return prev;
    }
    
};


struct Boxes {
    Boxes() {};
    Boxes(int speed_in) : speed(speed_in) { }
    std::vector<SDL_Rect> boxes;
    int speed;
    int create = 0;
    int stored = 0;
    int x;
    int y;
    void renderBoxes(SDL_Renderer * rend) {
        if (create != 0) {
            SDL_Rect rect;
            if (stored > 10) {
                rect.x = x + 6 + (18 * abs(stored - 10));
                rect.y = y + 16;
            } else {
                rect.x = x + 6 + (18 * stored);
                rect.y = y;
            }
            rect.w = 8;
            rect.h = 6;
            boxes.push_back(rect);
            SDL_RenderFillRect(rend, &rect);
            if (stored == create) {
                stored = 0;
                create = 0;
            } else {
                stored++;
            }
        }
        for (int i = 0; i < boxes.size(); i++) {
            SDL_RenderDrawRect(rend, &boxes[i]);
        }
    }
    void makePointer(Line l) {
        SDL_Rect pointer;
        pointer.x = l.x1 - 3;
        pointer.y = l.y1 - 3;
        pointer.w = 6;
        pointer.h = 6;
        boxes.push_back(pointer);
    }
};


struct Tree {
    Tree();
    Tree(SDL_Renderer * rend, int speed, int max_diverge_in, int can_end_in) {
        start = Path (600, 0, 600, 0, rend);
        rects = Boxes (speed);
        paths.push_back(start);
        max_diverge = max_diverge_in;
        can_end = can_end_in;
    }
    
    std::vector<Path> paths;
    Path start;
    Boxes rects;
    std::vector<std::pair<SDL_Rect, int>> clickBoxes;
    int color;
    
    int max_diverge;
    int can_end = 1;
    int box_freq = 30;
    int branching_freq = 20;
    int ending_freq = 30;

    
    void mainRender(SDL_Renderer * rend, int index, int background, std::map<std::string, bool> &eventMap) {
        color = 255 - background;
        if (index == 0) {
            eventMap["start"] = true;
        }
        for (int i = 0; i < paths.size(); i++) {
            //SDL_SetRenderDrawColor(rend, 153, 132, 111, 255);
            SDL_SetRenderDrawColor(rend, color, color, color, 255);
            Line l = paths[i].drawPath();
            if (!paths[i].has_ended) {
                int v = rand() % branching_freq;
                if (v == 0 && !(paths[i].divergence) && max_diverge > 1) {
                    Path p(l.x1, l.y1, l.x2, l.y2, rend);
                    paths.push_back(p);
                    if (rand() % 2) {
                        paths[i].divergence = 1;
                    } else {
                        paths[i].divergence = -1;
                    }
                    paths[i].max_div = max_diverge;
                    paths[paths.size() - 1].divergence = paths[i].divergence * (-1);
                    paths[paths.size() - 1].max_div = max_diverge;
                    max_diverge = max_diverge * 0.75;
                    rects.makePointer(l);
                    eventMap["path_split"] = true;
                }
                //draw new boxes
                if ((rand() % box_freq) == 0 && !rects.create) {
                    rects.makePointer(l);
                    rects.create = rand() % 20;
                    rects.x = l.x2;
                    rects.y = l.y2;
                    eventMap["new_boxes"] = true;
                }
                //determine path stopping
                if (can_end && (rand() % ending_freq == 0) && i) {
                    //Mix_PlayChannel( -1, snareLong, 0 );
                    paths[i].has_ended = true;
                    eventMap["path_stopped"] = true;
                }
            }
            SDL_Rect box;
            box.x = l.x2 - 3;
            box.y = l.y2;
            box.w = 6;
            box.h = 6;
            SDL_RenderDrawRect(rend, &box);
            SDL_RenderFillRect(rend, &box);
        }
        //draw boxes
        rects.renderBoxes(rend);
        eventMap["boxes_drawn"] = rects.create;
        //draw random lines
        randomEffects(rend, index);
    }
    
    void clickBox(int x, int y) {
        SDL_Rect box;
        box.w = ((rand() % 9) + 1) * 2;
        box.h = box.w;
        box.x = x - (box.w / 2);
        box.y = y - (box.w / 2);
        clickBoxes.push_back(std::make_pair(box, (box.w * 2)));
    }
    
    void randomEffects(SDL_Renderer * rend, int index) {
        if (index % 2 == 1) {
            Path pathStart = paths[0];
            SDL_RenderDrawLine(rend, 1000, pathStart.yCoord2, pathStart.xCoord2, pathStart.yCoord2);
            SDL_RenderDrawLine(rend, pathStart.xCoord2, 0, pathStart.xCoord2, 1000);
        }
                
        if (clickBoxes.size() > 0) {
            for (int i = 0; i < clickBoxes.size(); i++) {
                SDL_RenderDrawRect(rend, &(clickBoxes[i].first));
                clickBoxes[i].first.w += clickBoxes[i].second * 2;
                clickBoxes[i].first.h += clickBoxes[i].second * 2;
                clickBoxes[i].first.x -= clickBoxes[i].second;
                clickBoxes[i].first.y -= clickBoxes[i].second;
                clickBoxes[i].second--;
                if (clickBoxes[i].first.w < 5) {
                    clickBoxes.erase(clickBoxes.begin() + i);
                }
            }
        }
    }
    
    void clear(SDL_Renderer * rend, int max_diverge_in) {
        paths.clear();
        rects.boxes.clear();
        rects.create = 0;
        rects.stored = 0;
        start = Path (600, 0, 600, 0, rend);
        paths.push_back(start);
        max_diverge = max_diverge_in;
    }
};

#endif /* tree_h */
