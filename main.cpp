// SDL2Game.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "CommonFunc.h"
#include "BaseObject.h"
#include "game_map.h"
#include "MainObject.h";
#include "ImpTimer.h"
#include "ThreatsObject.h"
#include "ExplosionObject.h"
#include "TextObject.h"
#include "PlayerPower.h"
#include "Geometric.h"


BaseObject Menu;
TextObject menu[3];
BaseObject back_menu;
BaseObject introduction_;
BaseObject gameover;
BaseObject win;



BaseObject g_background;
TTF_Font* font_time = NULL;
TTF_Font* font_game_menu =NULL;

Mix_Music*  game_sound_total=NULL;
Mix_Chunk* mouse_click=NULL;
Mix_Chunk* time_out = NULL;
Mix_Chunk* you_lose = NULL;
Mix_Chunk* lose = NULL;
Mix_Chunk* you_win= NULL;


bool InitData()
{
    bool success = true;
    int ret = SDL_Init(SDL_INIT_VIDEO);
    if (ret < 0)
        return false;

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    g_window = SDL_CreateWindow("GO_GO!! ",
                                 SDL_WINDOWPOS_UNDEFINED,
                                 SDL_WINDOWPOS_UNDEFINED,
                                 SCREEN_WIDTH, SCREEN_HEIGHT,
                                 SDL_WINDOW_SHOWN);

    if (g_window == NULL)
    {
        success = false;
    }
    else
    {
        g_screen = SDL_CreateRenderer(g_window, -1, SDL_RENDERER_ACCELERATED);
        if (g_screen == NULL)
            success  = false;
        else
        {
            SDL_SetRenderDrawColor(g_screen, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR);
            int imgFlags = IMG_INIT_PNG;
            if (!(IMG_Init(imgFlags) && imgFlags))
                success = false;
        }

        if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1)
        {
            success = false;
        }

        g_sound_bullet[0] = Mix_LoadWAV(g_name_audio_bullet_main1);
        g_sound_bullet[1] = Mix_LoadWAV(g_name_audio_bullet_main2);
        g_sound_explosion = Mix_LoadWAV(g_name_audio_ex_main);
        g_sound_ex_main = Mix_LoadWAV(g_name_audio_ex_threats);

        if (g_sound_bullet[0] == NULL || g_sound_bullet[1] == NULL || g_sound_explosion == NULL)
        {
            return false;
        }
        
        if (TTF_Init() == -1)
        {
            success = false;
        }

        font_time = TTF_OpenFont("font//dlxfont.ttf", 15);
        if (font_time == NULL)
        {
            success = false;
        }
         font_game_menu=TTF_OpenFont("font//dlxfont.ttf",90);
         if(font_game_menu==NULL)
         {
           success=false;
         }
    }

    return success;
}

bool LoadBackground()
{
    bool ret = g_background.LoadImg("img//background.png", g_screen);
    if (ret == false)
        return false;

    return true;
}

void close()
{
    g_background.Free();

    SDL_DestroyRenderer(g_screen);
    g_screen = NULL;

    SDL_DestroyWindow(g_window);
    g_window = NULL;

     Mix_Quit();
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

std::vector<ThreatsObject*> MakeThreadList()
{
    std::vector<ThreatsObject*> list_threats;


    ThreatsObject* dynamic_threats = new ThreatsObject[20];
    for (int i = 0; i < 20; i++)
    {
        ThreatsObject* p_threat = (dynamic_threats + i);
        if (p_threat != NULL)
        {
            p_threat->LoadImg("img//threat_left.png", g_screen);
            p_threat->set_clips();
            p_threat->set_type_move(ThreatsObject::MOVE_IN_SPACE_THREAT);
            p_threat->set_x_pos(500 + i*500);
            p_threat->set_y_pos(200);
            int pos1 = p_threat->get_x_pos() - 60;
            int pos2 = p_threat->get_x_pos() + 60;
            p_threat->SetAnimationPos(pos1, pos2);
            p_threat->set_input_left(1);
            list_threats.push_back(p_threat);
        }
    }



    ThreatsObject* threats_objs = new ThreatsObject[20];
    for (int i = 0; i < 20; i++)
    {
        ThreatsObject* p_threat = (threats_objs + i);
        if (p_threat != NULL)
        {
            p_threat->LoadImg("img//threat_level.png", g_screen);
            p_threat->set_clips();
            p_threat->set_x_pos(700 + i*1200);
            p_threat->set_y_pos(250);
            p_threat->set_type_move(ThreatsObject::STATIC_THREAT);
            p_threat->set_input_left(0);

            BulletObject* p_bullet = new BulletObject();
            p_threat->InitBullet(p_bullet, g_screen);
            list_threats.push_back(p_threat);
        }
    }

    return list_threats;
}

int main(int argc, char* argv[])
{
    ImpTimer fps_timer;

    if (InitData() == false)
        return -1;
     SetPosMenu();

     bool t=Menu.LoadImg("img/menu2.png",g_screen);
     bool x=back_menu.LoadImg("img/Pause.png",g_screen);

    if (LoadBackground() == false)
        return -1;

    game_sound_total=Mix_LoadMUS("sound/backgroundSound.wav");
    Mix_PlayMusic(game_sound_total,-1);

    mouse_click = Mix_LoadWAV("sound/mouse_click.wav");
    time_out =Mix_LoadWAV("sound/timeout.wav");
   you_lose = Mix_LoadWAV("sound/youlose.wav");
   lose =Mix_LoadWAV("sound/loserSound.wav");
   you_win =Mix_LoadWAV("sound/sound_win_sound.wav");

    int quit1=0;
    //show menu
    label:
            menu[0].SetText("Play");
            menu[0].SetColor(TextObject::WHITE_TEXT);
            menu[0].LoadFromRenderText(font_time,g_screen);

            menu[1].SetText("Exit");
            menu[1].LoadFromRenderText(font_time,g_screen);

            menu[2].SetText("Review");
            menu[2].LoadFromRenderText(font_time,g_screen);

    int time_console;


        while(quit1==0)
            {
                Menu.Render(g_screen);
            menu[0].RenderText(g_screen,menu_pos[0].x,menu_pos[0].y);
            menu[1].RenderText(g_screen,menu_pos[1].x,menu_pos[1].y);
            menu[2].RenderText(g_screen,menu_pos[2].x,menu_pos[2].y);
            SDL_RenderPresent(g_screen);
                time_console=SDL_GetTicks()/1000;

                while (SDL_PollEvent(&g_event) != 0) {
                int xm = g_event.motion.x;
                int ym = g_event.motion.y;
                if (g_event.type == SDL_QUIT) {
                    quit1 = 4;
                }
                if (g_event.type == SDL_MOUSEMOTION) {

                    for (int i = 0; i < 3; i++) {
                          menu_pos[i].w=menu[i].GetWidth();
                          menu_pos[i].h=menu[i].GetHeight();
                        if (check_mouse(xm, ym, menu_pos[i])) {

                            menu[i].Free();
                            menu[i].SetColor(TextObject::RED_TEXT);
                            menu[i].LoadFromRenderText(font_time,g_screen);
                        }
                        else {
                                menu[i].Free();
                            menu[i].SetColor(TextObject::WHITE_TEXT);
                            menu[i].LoadFromRenderText(font_time,g_screen);
                        }
                    }
                }
                if (g_event.type == SDL_MOUSEBUTTONDOWN) {
                    for (int i = 0; i < 3; i++) {
                        if (check_mouse(xm, ym, menu_pos[i]) && g_event.button.button == SDL_BUTTON_LEFT) 
                        {
                            Mix_PlayChannel(-1,mouse_click,0);
                            quit1 = i + 1;

                        }
                    }
                }
            }
           
        }

    if(quit1 ==1)
    {
       
    GameMap game_map;
    game_map.LoadMap("map/map01.dat");
    game_map.LoadTiles(g_screen);


    MainObject p_player;
    p_player.LoadImg("img//player_right.png", g_screen);
    p_player.set_clips();


    PlayerPower player_power;
    player_power.Init(g_screen);

    PlayerMoney player_money;
    player_money.Init(g_screen);
    player_money.SetPos(SCREEN_WIDTH*0.5 - 300, 8);

    std::vector<ThreatsObject*> threats_list = MakeThreadList();

    //Boss Threat
   

    ExplosionObject exp_threat;
    bool tRet = exp_threat.LoadImg("img//exp3.png", g_screen);
    if (!tRet) return -1;
    exp_threat.set_clip();

    ExplosionObject exp_main;
    bool mRet = exp_main.LoadImg("img//exp3.png", g_screen);
    exp_main.set_clip();
    if (!mRet) return -1;

    int num_die = 0;

    //Time text
    TextObject time_game;
    time_game.SetColor(TextObject::WHITE_TEXT);

    TextObject mark_game;
    mark_game.SetColor(TextObject::WHITE_TEXT);
    UINT mark_value = 0;

    TextObject money_game;
    money_game.SetColor(TextObject::WHITE_TEXT);

    bool is_quit = false;
    while (!is_quit)
    {
        fps_timer.start();
        while (SDL_PollEvent(&g_event) != 0)
        {
            if (g_event.type == SDL_QUIT)
            {
                is_quit = true;
            }

            p_player.HandelInputAction(g_event, g_screen);

            
         if (g_event.type == SDL_KEYDOWN)
    {
        switch (g_event.key.keysym.sym)
        {
        case SDLK_k:
            {
                quit1 = 0;
                            Mix_PlayChannel(-1,mouse_click,0);
                            goto label;
            }
            break;}}
        }

        SDL_SetRenderDrawColor(g_screen, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR, RENDER_DRAW_COLOR);
        SDL_RenderClear(g_screen);

        g_background.Render(g_screen, NULL);

        Map map_data = game_map.getMap();

        p_player.DoPlayer(map_data);       
        p_player.HandleBullet(g_screen);
        p_player.SetMapXY(map_data.start_x_, map_data.start_y_);

        p_player.Show(g_screen);


        game_map.SetMap(map_data);
        game_map.DrawMap(g_screen);


        //Draw Geometric
        GeometricFormat rectangle_size(0, 0, SCREEN_WIDTH, 40);
        ColorData color_data(255, 100, 28);
        Geometric::RenderRectangle(rectangle_size, color_data, g_screen);

        GeometricFormat outLineSize(1, 1, SCREEN_WIDTH - 1, 38);
        ColorData color_data2(255, 255, 255);

        Geometric::RenderOutline(outLineSize, color_data2, g_screen);


        player_power.Show(g_screen);
        player_money.Show(g_screen);

        for (int i = 0; i < threats_list.size(); i++)
        {
            ThreatsObject* p_threat = threats_list.at(i);
            if (p_threat != NULL)
            {
                p_threat->SetMapXY(map_data.start_x_, map_data.start_y_);
                p_threat->ImpMoveType(g_screen);
                p_threat->DoPlayer(map_data);
                p_threat->MakeBullet(g_screen, SCREEN_WIDTH, SCREEN_HEIGHT);
                p_threat->Show(g_screen);


                SDL_Rect rect_player = p_player.GetRectFrame();
                bool bCol1 = false;
                std::vector<BulletObject*> tBullet_list = p_threat->get_bullet_list();
                for (int jj = 0; jj < tBullet_list.size(); ++jj)
                {
                    BulletObject* pt_bullet = tBullet_list.at(jj);
                    if (pt_bullet)
                    {
                        bCol1 = SDLCommonFunc::CheckCollision(pt_bullet->GetRect(), rect_player);
                        if (bCol1)
                        {
                            p_threat->RemoveBullet(jj);
                            break;
                        }
                    }
                }

                SDL_Rect rect_threat = p_threat->GetRectFrame();
                bool bCol2 = SDLCommonFunc::CheckCollision(rect_player, rect_threat);
                if (bCol1 || bCol2)
                {
                    int width_exp_frame = exp_main.get_frame_width();
                    int heiht_exp_height = exp_main.get_frame_heigh();
                    for (int ex = 0; ex < 4; ex++)
                    {
                        int x_pos = (p_player.GetRect().x + p_player.get_frame_width()*0.5) - width_exp_frame*0.5;
                        int y_pos = (p_player.GetRect().y + p_player.get_frame_height()*0.5) - heiht_exp_height*0.5;

                        exp_main.set_frame(ex);
                        exp_main.SetRect(x_pos, y_pos);
                        exp_main.Show(g_screen);
                        SDL_RenderPresent(g_screen);
                    }

                    Mix_PlayChannel(-1, g_sound_ex_main, 0);

                    num_die++;
                    if (num_die <= 3)
                    {
                        p_player.SetRect(0, 0);
                        p_player.set_comeback_time(60);
                        SDL_Delay(1000);
                        player_power.Decrease();
                        player_power.Render(g_screen);
                        continue;
                    }
                    else
                    {
                        num_die =4;
                    }
                    
                }
            }
        }


        int frame_exp_width = exp_threat.get_frame_width();
        int frame_exp_heigh = exp_threat.get_frame_heigh();

        std::vector<BulletObject*> bullet_arr = p_player.get_bullet_list();
        for (int r = 0; r < bullet_arr.size(); ++r)
        {
            BulletObject* p_bullet = bullet_arr.at(r);
            if (p_bullet != NULL)
            {
                for (int t = 0; t < threats_list.size(); ++t)
                {
                    ThreatsObject* obj_threat = threats_list.at(t);
                    if (obj_threat != NULL)
                    {

                        SDL_Rect tRect;
                        tRect.x = obj_threat->GetRect().x;
                        tRect.y = obj_threat->GetRect().y;
                        tRect.w = obj_threat->get_width_frame();
                        tRect.h = obj_threat->get_height_frame();

                        SDL_Rect bRect = p_bullet->GetRect();

                        bool bCol = SDLCommonFunc::CheckCollision(bRect, tRect);

                        if (bCol)
                        {
                            mark_value++;
                            for (int ex = 0; ex < NUM_FRAME_EXP; ++ex)
                            {
                                int x_pos = p_bullet->GetRect().x - frame_exp_width*0.5;
                                int y_pos = p_bullet->GetRect().y - frame_exp_heigh*0.5;

                                exp_threat.set_frame(ex);
                                exp_threat.SetRect(x_pos, y_pos);
                                exp_threat.Show(g_screen);
                            }

                            p_player.RemoveBullet(r);
                            obj_threat->Free();
                            threats_list.erase(threats_list.begin() + t);

                            Mix_PlayChannel(-1, g_sound_explosion, 0);
                        }
                    }
                }
            }
        }
        if(num_die>=4) {
            Mix_FreeMusic(game_sound_total);
            gameover.LoadImg("img/gameover.png",g_screen);
            gameover.SetRect(0,0);
            gameover.Render(g_screen);
            SDL_RenderPresent(g_screen);
            Mix_PlayChannel(-1,lose,0);
             SDL_Delay(1000);
             Mix_PlayChannel(-1,you_lose,0);
             SDL_Delay(1000);
            close();
            return 0;
            
        }

        if (p_player.CheckEndMap(map_data))
        {
            Mix_FreeMusic(game_sound_total);
             win.LoadImg("img/you_win.png",g_screen);
            win.SetRect(0,0);
            win.Render(g_screen);
            SDL_RenderPresent(g_screen);
            game_sound_total=NULL;
             Mix_PlayChannel(-1,you_win,0);
             SDL_Delay(1000);
              Mix_PlayChannel(-1,you_win,0);
             SDL_Delay(1000);
            close();
            return 0;
        }


        //Show game time
        std::string str_time = "Time: ";
        Uint32 time_val = SDL_GetTicks() / 1000;
        Uint32 val_time = 30 - time_val;
        
        if (val_time <= 0)
        {
                  Mix_FreeMusic(game_sound_total);
            gameover.LoadImg("img/gameover.png",g_screen);
            gameover.SetRect(0,0);
            gameover.Render(g_screen);
            SDL_RenderPresent(g_screen);
            Mix_PlayChannel(-1,time_out,0);
            SDL_Delay(2000);
           close();
           return 0;

        }
        else
        {
            std::string str_val = std::to_string(val_time);
            str_time += str_val;

            time_game.SetText(str_time);
            time_game.LoadFromRenderText(font_time, g_screen);
            time_game.RenderText(g_screen, SCREEN_WIDTH - 200, 15);
        }

        std::string val_str_mark = std::to_string(mark_value);
        std::string strMark("Mark: ");
        strMark += val_str_mark;

        mark_game.SetText(strMark);
        mark_game.LoadFromRenderText(font_time, g_screen);
        mark_game.RenderText(g_screen, SCREEN_WIDTH*0.5 - 50, 15);

        int money_count = p_player.GetMoneyCount();
        std::string money_str = std::to_string(money_count);

        money_game.SetText(money_str);
        money_game.LoadFromRenderText(font_time, g_screen);
        money_game.RenderText(g_screen, SCREEN_WIDTH*0.5 - 250, 15);

        //Show Boss
        
        
        SDL_RenderPresent(g_screen);

        int real_imp_time = fps_timer.get_ticks();
        int time_one_frame = 1000/FRAME_PER_SECOND ;// ms

        if (real_imp_time < time_one_frame)
        {
            int delay_time = time_one_frame - real_imp_time;
            if (delay_time >= 0)
                SDL_Delay(delay_time);
        }

    }

    for (int i = 0; i < threats_list.size(); i++)
    {
        ThreatsObject* p_threat = threats_list.at(i);
        if (p_threat)
        {
            p_threat->Free();
            p_threat = NULL;
        }
    }

    threats_list.clear();

    close();
	return 0;
        }

    else if(quit1==2)
    {
        close();
        return 0;
    }
    else if(quit1==3)
    {
        introduction_.LoadImg("img/intro2.png",g_screen);
        introduction_.SetRect(0,0);
        introduction_.Render(g_screen);
        back_menu.Render(g_screen);
        while(quit1==3)
        {
            while(SDL_PollEvent(&g_event)!=0)
            {
                int xm = g_event.motion.x;
                int ym = g_event.motion.y;
                if(g_event.type == SDL_MOUSEBUTTONDOWN)
                {
                    if (check_mouse(xm, ym, back_menu.GetRect()) && g_event.button.button == SDL_BUTTON_LEFT) {
                            quit1 = 0;
                            Mix_PlayChannel(-1,mouse_click,0);
                            goto label;
                        }
                }

            }
          SDL_RenderPresent(g_screen);
        }
    }


}

