#include <iostream>
#include <cstdio>
#include <fstream>
#include <cstring>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

using namespace std;

int weight = 980;//界面宽高
int hight = 590;
Uint8 h = 0;//文字亮度
char map[14][14];//地图大小
int people_x, people_y; //人物位置

int init();
void playMusic();
void loadMap(Uint8 level);
void updateMap(SDL_Surface *backgrand, SDL_Surface *people, SDL_Surface *wall, SDL_Surface *aim, SDL_Surface *land, SDL_Surface *boxRed, SDL_Surface *boxYellow, SDL_Renderer *ren, SDL_Texture *tex2);
void destroy(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, SDL_Texture *tex2);

int main(int argc, char **argv)
{
    init();
}

int init()
{
    //初始化SDL

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)

    {

        cout << "SDL_Init Error: " << SDL_GetError() << endl;

        return 1;
    }
    //初始化声音文件
    playMusic();


    SDL_Window *win;   //窗口
    SDL_Texture *tex;  //纹理1
    SDL_Texture *tex2; //纹理2
    SDL_Renderer *ren; //渲染

    //人物与墙壁、箱子
    SDL_Surface *people;
    SDL_Surface *wall;
    SDL_Surface *aim;
    SDL_Surface *land;
    SDL_Surface *boxRed;
    SDL_Surface *boxYellow;

    people = IMG_Load("resources//image//ManLEFT1.jpg");
    aim = IMG_Load("resources//image//aim.jpg");
    wall = IMG_Load("resources//image//wall.jpg");
    land = IMG_Load("resources//image//land.jpg");
    boxRed = IMG_Load("resources//image//BoxRed.jpg");
    boxYellow = IMG_Load("resources//image//BoxYellow.jpg");

    //是不是应该判断一下是否成功加载图片

    
    //创建窗口

    win = SDL_CreateWindow("推箱子",

                           //SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                           0, 0,
                           weight, hight,
                           //  SDL_WINDOW_SHOWN
                           SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE

    );

    if (win == nullptr)

    {

        cout << SDL_GetError() << endl;

        return 1;
    }

    //创建渲染器


    ren = SDL_CreateRenderer(win, -1,

                             SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    if (ren == nullptr)

    {

        cout << SDL_GetError() << endl;

        return 1;
    }

    //创建图片表面

    SDL_Surface *bmp = nullptr;
    SDL_Surface *backgrand = nullptr;


    backgrand = IMG_Load("resources//image//timg.jpg"); //背景图
    bmp = IMG_Load("resources//image//initimg.jpg");    //开始界面

    if (bmp == nullptr || backgrand == nullptr)
    {

        cout << IMG_GetError() << endl;

        return 1;
    }

    //开始界面图片位置
    SDL_Rect dest;
    dest.x = 300;
    dest.y = 100;
    dest.w = 397;
    dest.h = 178;


    //初始化字库

    if (TTF_Init() == -1)
    {
        cout << TTF_GetError() << endl;
        return 1;
    }

    
    //创建文本表面
    SDL_Surface *words = nullptr;

    //BoldItalics.ttf 字库，设字体为30号
    TTF_Font *font = TTF_OpenFont("resources//typeface//BoldItalics.ttf", 30);

    SDL_Color color = {255, 255, 255, 0};

    words = TTF_RenderUTF8_Blended(font, "按空格键进入游戏", color);

    //将文字放置在图片上
    SDL_Color color2 = {255, 255, 255, 200};
    TTF_Font *font2 = TTF_OpenFont("resources//typeface//BoldItalics.ttf", 20);

    SDL_Surface *author = TTF_RenderUTF8_Blended(font2, "—— By QingShan", color2);
    SDL_Rect src = {100, 150};

    SDL_BlitSurface(author, NULL, bmp, &src);
    SDL_FreeSurface(author);

    SDL_UpdateWindowSurface(win);

 
     //文字位置
    SDL_Rect dest2;
    dest2.x = 350;
    dest2.y = 500;


    //创建纹理

    
    tex = SDL_CreateTextureFromSurface(ren, bmp);

    SDL_FreeSurface(bmp);

    

    tex2 = SDL_CreateTextureFromSurface(ren, words);

    SDL_QueryTexture(tex2, NULL, NULL, &dest2.w, &dest2.h);

    SDL_FreeSurface(words);

    //清空渲染器

    SDL_RenderClear(ren);

    //将材质复制到渲染器

    SDL_RenderCopy(ren, tex, NULL, &dest);

    SDL_RenderCopy(ren, tex2, NULL, &dest2);

    //呈现渲染器

    SDL_RenderPresent(ren);

    //窗口延时

    bool done = false;       //main loop
    bool gameStart = false;  //游戏开始
    char last = ' ';         //人物占据的位置之前是什么 初始化为空格

    while ((!done))
    {
        // 消息处理循环
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            // 检查消息
            switch (event.type)
            {
                // exit if the window is closed
            case SDL_QUIT:
                done = true;
                break;

                // check for keypresses
            case SDL_KEYDOWN:
            {

               
                // exit if ESCAPE is pressed
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    done = true;
                    destroy(win, ren, tex, tex2);
                    break;
                case SDLK_SPACE:
                    //空格键进入游戏 更换渲染图片  如果游戏已经开始那么空格键不起作用
                    if (!gameStart)
                    {
                        SDL_RenderClear(ren);
                        tex2 = SDL_CreateTextureFromSurface(ren, backgrand);
                        SDL_RenderCopy(ren, tex2, NULL, NULL);
                        SDL_RenderPresent(ren);

                        gameStart = true;

                        loadMap(1);
                        updateMap(backgrand, people, wall, aim, land, boxRed, boxYellow, ren, tex2);
                        
                        break;
                    }
                //W A S D控制人物走动
                case SDLK_w:
                    //角色下一步为空或者目的地
                    if (map[people_y - 1][people_x] == ' ' || map[people_y - 1][people_x] == 'X')
                    {
                        //人物可以往前面走
                        map[people_y][people_x] = last;
                        last = map[people_y - 1][people_x];
                        map[people_y - 1][people_x] = '@';
                    }
                    //角色下一步是箱子，下下一步是空地
                    else if (map[people_y - 1][people_x] == 'O' && map[people_y - 2][people_x] == ' ')
                    {
                        map[people_y][people_x] = last;
                        last = ' ';
                        map[people_y - 1][people_x] = '@';
                        map[people_y - 2][people_x] = 'O';
                    }
                    //角色下一步是箱子，下下一步是目的地
                    else if (map[people_y - 1][people_x] == 'O' && map[people_y - 2][people_x] == 'X')
                    {
                        map[people_y][people_x] = last;
                        last = ' ';
                        map[people_y - 1][people_x] = '@';
                        map[people_y - 2][people_x] = 'g';
                    }
                    //角色下一步是在目的地箱子，下下一步是空地
                    else if (map[people_y - 1][people_x] == 'g' && map[people_y - 2][people_x] == ' ')
                    {
                        map[people_y][people_x] = last;
                        last = 'X';
                        map[people_y - 1][people_x] = '@';
                        map[people_y - 2][people_x] = 'X';
                    }
                    //角色下一步是在目的地箱子，下下一步是目的地
                    else if (map[people_y - 1][people_x] == 'O' && map[people_y - 2][people_x] == 'X')
                    {
                        map[people_y][people_x] = last;
                        last = 'X';
                        map[people_y - 1][people_x] = '@';
                        map[people_y - 2][people_x] = 'g';
                    }
                    updateMap(backgrand, people, wall, aim, land, boxRed, boxYellow, ren, tex2);
                    break;
                case SDLK_a:
                    //角色下一步为空或者目的地
                    if (map[people_y][people_x - 1] == ' ' || map[people_y][people_x - 1] == 'X')
                    {
                        //人物可以往前面走
                        map[people_y][people_x] = last;
                        last = map[people_y][people_x - 1];
                        map[people_y][people_x - 1] = '@';
                    }
                    //角色下一步是箱子，下下一步是空地
                    else if (map[people_y][people_x - 1] == 'O' && map[people_y][people_x - 2] == ' ')
                    {
                        map[people_y][people_x] = last;
                        last = ' ';
                        map[people_y][people_x - 1] = '@';
                        map[people_y][people_x - 2] = 'O';
                    }
                    //角色下一步是箱子，下下一步是目的地
                    else if (map[people_y][people_x - 1] == 'O' && map[people_y][people_x - 2] == 'X')
                    {
                        map[people_y][people_x] = last;
                        last = ' ';
                        map[people_y][people_x - 1] = '@';
                        map[people_y][people_x - 2] = 'g';
                    }
                    //角色下一步是在目的地箱子，下下一步是空地
                    else if (map[people_y][people_x - 1] == 'g' && map[people_y][people_x - 2] == ' ')
                    {
                        map[people_y][people_x] = last;
                        last = 'X';
                        map[people_y][people_x - 1] = '@';
                        map[people_y][people_x - 2] = 'X';
                    }
                    //角色下一步是在目的地箱子，下下一步是目的地
                    else if (map[people_y][people_x - 1] == 'O' && map[people_y][people_x - 2] == 'X')
                    {
                        map[people_y][people_x] = last;
                        last = 'X';
                        map[people_y][people_x - 1] = '@';
                        map[people_y][people_x - 2] = 'g';
                    }
                    updateMap(backgrand, people, wall, aim, land, boxRed, boxYellow, ren, tex2);
                    break;
                case SDLK_s:
                    //角色下一步为空或者目的地
                    if (map[people_y + 1][people_x] == ' ' || map[people_y + 1][people_x] == 'X')
                    {
                        //人物可以往前面走
                        map[people_y][people_x] = last;
                        last = map[people_y + 1][people_x];
                        map[people_y + 1][people_x] = '@';
                    }
                    //角色下一步是箱子，下下一步是空地
                    else if (map[people_y + 1][people_x] == 'O' && map[people_y + 2][people_x] == ' ')
                    {
                        map[people_y][people_x] = last;
                        last = ' ';
                        map[people_y + 1][people_x] = '@';
                        map[people_y + 2][people_x] = 'O';
                    }
                    //角色下一步是箱子，下下一步是目的地
                    else if (map[people_y + 1][people_x] == 'O' && map[people_y + 2][people_x] == 'X')
                    {
                        map[people_y][people_x] = last;
                        last = ' ';
                        map[people_y + 1][people_x] = '@';
                        map[people_y + 2][people_x] = 'g';
                    }
                    //角色下一步是在目的地箱子，下下一步是空地
                    else if (map[people_y + 1][people_x] == 'g' && map[people_y + 2][people_x] == ' ')
                    {
                        map[people_y][people_x] = last;
                        last = 'X';
                        map[people_y + 1][people_x] = '@';
                        map[people_y + 2][people_x] = 'X';
                    }
                    //角色下一步是在目的地箱子，下下一步是目的地
                    else if (map[people_y + 1][people_x] == 'O' && map[people_y + 2][people_x] == 'X')
                    {
                        map[people_y][people_x] = last;
                        last = 'X';
                        map[people_y + 1][people_x] = '@';
                        map[people_y + 2][people_x] = 'g';
                    }
                    updateMap(backgrand, people, wall, aim, land, boxRed, boxYellow, ren, tex2);
                    break;
                case SDLK_d:
                    //角色下一步为空或者目的地
                    if (map[people_y][people_x + 1] == ' ' || map[people_y][people_x + 1] == 'X')
                    {
                        //人物可以往前面走
                        map[people_y][people_x] = last;
                        last = map[people_y][people_x + 1];
                        map[people_y][people_x + 1] = '@';
                    }
                    //角色下一步是箱子，下下一步是空地
                    else if (map[people_y][people_x + 1] == 'O' && map[people_y][people_x + 2] == ' ')
                    {
                        map[people_y][people_x] = last;
                        last = ' ';
                        map[people_y][people_x + 1] = '@';
                        map[people_y][people_x + 2] = 'O';
                    }
                    //角色下一步是箱子，下下一步是目的地
                    else if (map[people_y][people_x + 1] == 'O' && map[people_y][people_x + 2] == 'X')
                    {
                        map[people_y][people_x] = last;
                        last = ' ';
                        map[people_y][people_x + 1] = '@';
                        map[people_y][people_x + 2] = 'g';
                    }
                    //角色下一步是在目的地箱子，下下一步是空地
                    else if (map[people_y][people_x + 1] == 'g' && map[people_y][people_x + 2] == ' ')
                    {
                        map[people_y][people_x] = last;
                        last = 'X';
                        map[people_y][people_x + 1] = '@';
                        map[people_y][people_x + 2] = 'X';
                    }
                    //角色下一步是在目的地箱子，下下一步是目的地
                    else if (map[people_y][people_x + 1] == 'O' && map[people_y][people_x + 2] == 'X')
                    {
                        map[people_y][people_x] = last;
                        last = 'X';
                        map[people_y][people_x + 1] = '@';
                        map[people_y][people_x + 2] = 'g';
                    }
                    updateMap(backgrand, people, wall, aim, land, boxRed, boxYellow, ren, tex2);
                    break;
                }
            }
            break;
            } // end switch
        }     // 消息处理结束

        //文字闪烁
        if ((!gameStart))
        {
            h += 2;
            if (h >= 255)
            {
                h = 0;
            }

            color = {255, 255, 255, h};
            //TTF_RenderUTF8_Blended 使用该函数使用utf-8字符
            words = TTF_RenderUTF8_Blended(font, "按空格键进入游戏", color);

            SDL_RenderClear(ren);

            tex2 = SDL_CreateTextureFromSurface(ren, words);

            SDL_RenderCopy(ren, tex, NULL, &dest);
            SDL_RenderCopy(ren, tex2, NULL, &dest2);
            SDL_RenderPresent(ren);

            SDL_FreeSurface(words);
            SDL_UpdateWindowSurface(win);
         
        }
    } // end main loop

    
    destroy(win, ren, tex, tex2);
    SDL_Quit();
    return 0;
}

//加载地图 传入关卡
void loadMap(Uint8 level = 1)
{

    string file = "resources//map//" + std::to_string(level) + ".txt";
    
    ifstream srcFile(file, ios::in); //以文本模式打开txt文件
    if (!srcFile.is_open())
    {
        cerr << "未读取到文件" << endl;
    }
    srcFile >> noskipws; //逐个读取包括空格和回车
    char x;
    int flag = 0;
    int i = 0, j = 0;
    while (!srcFile.eof())
    {
        srcFile >> x;
        if (x == '\n')
            continue;
        map[i][j] = x;

        if (j == 1 && map[i][j] == '|')
        {

            break;
        }
        if (map[i][j] == '|')
            flag++;

        j++;
        if (flag == 2)
        {

            i++, j = 0, flag = 0;
        }
    }

    srcFile.close();
}

//根据map[][]更新地图界面
void updateMap(SDL_Surface *backgrand, SDL_Surface *people, SDL_Surface *wall, SDL_Surface *aim, SDL_Surface *land,
               SDL_Surface *boxRed, SDL_Surface *boxYellow, SDL_Renderer *ren, SDL_Texture *tex2)
{

    int flag = 0;
    char c;
    for (int i = 0; i < 14; i++)
    {
        for (int j = 0; j < 14; j++)
        {
            c = map[i][j];
            if (map[i][j + 1] == '|' && c == '|')
                break;
            if (c == '|')
                flag++;
            else if ((c == '|') && flag == 2)
            {
                flag = 0;
                break;
            }
            SDL_Rect lit = {j * 40, i * 40};

            switch (c)
            {
            case '#': //墙壁
                SDL_BlitSurface(wall, NULL, backgrand, &lit);

                break;

            case 'X': //目的地
                SDL_BlitSurface(aim, NULL, backgrand, &lit);

                break;

            case '@': //人物
                SDL_BlitSurface(people, NULL, backgrand, &lit);
                //获取人物位置
                people_x = j, people_y = i;
                break;
            case 'O': //箱子
                SDL_BlitSurface(boxYellow, NULL, backgrand, &lit);

                break;
            case ' ': //空地
                SDL_BlitSurface(land, NULL, backgrand, &lit);

                break;
            case 'g': //在目的地的箱子
                SDL_BlitSurface(boxRed, NULL, backgrand, &lit);

                break;
            }
        }
    }
    SDL_RenderClear(ren);

    tex2 = SDL_CreateTextureFromSurface(ren, backgrand);

    SDL_RenderCopy(ren, tex2, NULL, NULL);
    SDL_RenderPresent(ren);
}
//循环播放背景音乐
void playMusic()
{
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_Music *sound = Mix_LoadMUS("resources//music//bgm.wav");

    Mix_VolumeMusic(80);//控制音量
    
    if (Mix_PlayMusic(sound, -1) == -1) //Mix_PlayMusic()传入-1代表循环播放

    {

        fprintf(stderr, "play %s failure ,%s\n", "音频播放失败", Mix_GetError());
    }
}

//释放资源
void destroy(SDL_Window *win, SDL_Renderer *ren, SDL_Texture *tex, SDL_Texture *tex2)
{
    
    SDL_DestroyTexture(tex);
    SDL_DestroyTexture(tex2);

    SDL_DestroyRenderer(ren);

    SDL_DestroyWindow(win);
    TTF_Quit();
}


