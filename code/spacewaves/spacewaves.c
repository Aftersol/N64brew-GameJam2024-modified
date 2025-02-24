#include <unistd.h>
#include <time.h>
#include <libdragon.h>
#include <display.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include "../../core.h"
#include "../../minigame.h"
#include "world.h"
#include "gfx.h"
#include "station.h"
#include "enemycraft.h"
#include "spacewaves.h"
#include "scoreui.h"
#include "bonus.h"

/*********************************
             Globals
*********************************/

// You need this function defined somewhere in your project
// so that the minigame manager can work
const MinigameDef minigame_def = {
    .gamename = "SpaceWaves",
    .developername = "Spooky Iluha",
    .description = "A station-defense minigame set in various galaxies. Defend yourself from other player's spacecrafts.",
    .instructions = "Z, L/R to fire, A,B to activate bonuses. Hit to win points! Each player will get a round in a station. Requires Expansion Pak."
};

/**
 * Simple example with a 3d-model file created in blender.
 * This uses the builtin model format for loading and drawing a model.
 */
int main()
{
  main_init();
  
  if (!hasStarted) {
    framebuffer = display_get();
    rdpq_attach(framebuffer, NULL);
    goto end;
  }

  T3DMat4 modelMat; // matrix for our model, this is a "normal" float matrix
  t3d_mat4_identity(&modelMat);

  // Now allocate a fixed-point matrix, this is what t3d uses internally.
  // Note: this gets DMA'd to the RSP, so it needs to be uncached.
  // If you can't allocate uncached memory, remember to flush the cache after writing to it instead.
  float rotation = 0.0f;
  PlyNum playerturn = PLAYER_1;
  int countdownseconds = 0;
  gamestatus.winner = -1;
  
  while(true){
    mixer_try_play();
    joypad_poll();
    // ======== Update ======== //
    float modelScale = 0.1f;
    float shakemax = (effects.screenshaketime * 1.5f);
    if(shakemax <= 0.0f) shakemax = 0.0001f;

    if(dither == DITHER_SQUARE_INVSQUARE)
      dither = DITHER_BAYER_BAYER;
    else dither = DITHER_SQUARE_INVSQUARE;

    t3d_viewport_set_projection(&viewport, T3D_DEG_TO_RAD(75.0f), 1.0f, 220.0f);

    // slowly rotate model on start up screen
    t3d_mat4_from_srt_euler(&modelMat,
      (float[3]){modelScale, modelScale, modelScale},
      (float[3]){rotation * 0.2f, rotation, rotation},
      (float[3]){frandr(-shakemax,shakemax),frandr(-shakemax,shakemax),frandr(-shakemax,shakemax)});
    t3d_mat4_to_fixed(modelMatFP, &modelMat);

    // ======== Draw ======== //
    framebuffer = display_get();
    rdpq_attach(framebuffer, NULL);
    t3d_frame_start();
    if(gamestatus.state == GAMESTATE_PLAY || gamestatus.state == GAMESTATE_COUNTDOWN){
        station_update();
        mixer_try_play();
        crafts_update();
        bonus_update();
        effects_update();
    }
    station_apply_camera();

    //t3d_screen_clear_color(RGBA32(100, 80, 80, 0xFF));
    //t3d_screen_clear_depth();
    t3d_matrix_push(modelMatFP);
    // you can use the regular rdpq_* functions with t3d.
    // In this example, the colored-band in the 3d-model is using the prim-color,
    // even though the model is recorded, you change it here dynamically.
    mixer_try_play();
    world_draw();
    if(gamestatus.state == GAMESTATE_PLAY || gamestatus.state == GAMESTATE_PAUSED || gamestatus.state == GAMESTATE_COUNTDOWN){
      bonus_draw();
      crafts_draw();
      effects_draw();
      station_draw();
    }
    world_draw_lensflare();
    userinterface_draw();
    t3d_matrix_pop(1);
    
    // for the actual draw, you can use the generic rspq-api.
    timesys_update();
    for(int c = 0; c < MAXPLAYERS; c++){
      joypad_buttons_t btn = joypad_get_buttons_pressed((joypad_port_t)c);
      if(btn.start && (gamestatus.state == GAMESTATE_PLAY || gamestatus.state == GAMESTATE_PAUSED)){
        gamestatus.paused = !gamestatus.paused;
        gamestatus.state = gamestatus.paused? GAMESTATE_PAUSED : GAMESTATE_PLAY;
        afx_wav64_play_wrapper(snd_button_click2, SFX_CHANNEL_EFFECTS);
        for(int i = 0; i < MAXPLAYERS; i++)
          joypad_set_rumble_active(i, false);
      }
      if (btn.z && gamestatus.state == GAMESTATE_PAUSED) {
        gamestatus.state = GAMESTATE_FINISHED;
        gamestatus.statetime = 0.0f;
      }
      if(btn.start && gamestatus.state == GAMESTATE_GETREADY){
        gamestatus.paused = true;
        gamestatus.state = GAMESTATE_COUNTDOWN;
        gamestatus.statetime = 5.0f;
        currentmusicindex = rand() % MUSIC_COUNT;
        if(!xmplayeropen)
          xm64player_open(&xmplayer, music_path[currentmusicindex]);
        xm64player_play(&xmplayer, SFX_CHANNEL_MUSIC);
        xm64player_set_loop(&xmplayer, true);
        afx_wav64_play_wrapper(snd_button_click1, SFX_CHANNEL_EFFECTS);
        xmplayeropen = true;
      }
    }
    if(gamestatus.state == GAMESTATE_COUNTDOWN && gamestatus.statetime <= 0.0f){
      gamestatus.state = GAMESTATE_PLAY;
      gamestatus.paused = false;
      gamestatus.statetime = 180.0f;
      afx_wav64_play_wrapper(snd_button_click3, SFX_CHANNEL_BONUS);
    }
    if(gamestatus.state == GAMESTATE_COUNTDOWN){
      int newcountseconds = (int)gamestatus.statetime;
      if (newcountseconds != countdownseconds){
          afx_wav64_play_wrapper(snd_button_click3, SFX_CHANNEL_BONUS);
          countdownseconds = newcountseconds;
      }
    }
    
    if(gamestatus.state == GAMESTATE_GETREADY || gamestatus.state == GAMESTATE_TRANSITION)
      rotation += DELTA_TIME_REAL * 0.07;
    else rotation = 0.0f;

    if(gamestatus.state == GAMESTATE_PLAY){
      bool craftsalive = false;
      for(int i = 0; i < 3; i++) if(crafts[i].enabled) craftsalive = true;
      if((gamestatus.statetime <= 0.0f || !craftsalive || station.hp <= 0.0f)){
        for(int i = 0; i < MAXPLAYERS; i++)
          joypad_set_rumble_active(i, false);
        uint32_t count = core_get_playercount();
        playerturn++;
        effects.screenshaketime = 0;
        for(int i = 0; i < MAXPLAYERS; i++) effects.rumbletime[i] = 0;
        if(playerturn < count){
            gamestatus.state = GAMESTATE_TRANSITION;
            gamestatus.paused = false;
            gamestatus.statetime = 10.0f;
        } else {
            int maxscore = 0;
            int secondmaxscore = 0;
            for(int pl = 0; pl < MAXPLAYERS; pl++)
            if(gamestatus.playerscores[pl] > maxscore) {
                secondmaxscore = maxscore;
                maxscore = gamestatus.playerscores[pl];
                gamestatus.winner = pl;
            }
            if(maxscore == secondmaxscore) gamestatus.winner = -1;
            
            core_set_winner(gamestatus.winner);

            gamestatus.state = GAMESTATE_FINISHED;
            gamestatus.paused = false;
            gamestatus.statetime = 10.0f;
        }
      }
    }
    if(gamestatus.state == GAMESTATE_TRANSITION && gamestatus.statetime <= 0.0f){
      gamestatus.state = GAMESTATE_GETREADY;
      gamestatus.paused = false;
      if(xmplayeropen) {
        xm64player_close(&xmplayer);
        xmplayeropen = false;
      }
      rspq_wait();
      world_reinit();
      station_close();
      station_init((PlyNum)playerturn);
      crafts_close();
      crafts_init((PlyNum)playerturn);
      bonus_close();
      bonus_init();
    }
    if(gamestatus.state == GAMESTATE_FINISHED && gamestatus.statetime <= 0.0f){
      if(xmplayeropen) {
        xm64player_close(&xmplayer);
        xmplayeropen = false;
      }
      rspq_wait();
      goto end;
    }

    rdpq_detach_show();
    mixer_try_play();
  }

  end:
    rdpq_detach_show();
    minigame_end();
    return 0;
}

void no_expansion_pak_screen()
{
    bool acknowledged = false;

    float myDT = 0.0f;

    sprite_t* expansion_pak_sprite;
    surface_t expansion_pak_surf;

    sprite_t* galaxy_sprite;
    surface_t galaxy_surf;
    char text_buffer[64];
    

    rdpq_font_t* bold_warning_font;
    rdpq_font_t* normal_warning_font;

    const color_t warning_ui_color = RGBA32(0xBF, 0xFF, 0xBF, 0xFF);
    const color_t black_color = RGBA32(0x00, 0x00, 0x00, 0xFF);

    #ifdef SPACEWAVES_STANDALONE
    // Initialize most subsystems
    asset_init_compression(2);
    asset_init_compression(3);
    dfs_init(DFS_DEFAULT_LOCATION);
    debug_init_usblog();
    debug_init_isviewer();
    joypad_init();
    timer_init();
    rdpq_init();

    audio_init(32000, 3);
    mixer_init(32);
    #endif

    expansion_pak_sprite = sprite_load("rom://spacewaves/expansionPak.rgba16.sprite");
    expansion_pak_surf = sprite_get_pixels(expansion_pak_sprite);

    bold_warning_font = rdpq_font_load("rom://spacewaves/JupiteroidBoldItalic_half.font64");
    normal_warning_font = rdpq_font_load("rom://spacewaves/Jupiteroid_half.font64");

    memset(text_buffer, 0, 64);
    sprintf(text_buffer, "rom://spacewaves/galaxy%i.rgba16.sprite", (rand() % 2) == 0 ? 1 : 3);

    galaxy_sprite = sprite_load(text_buffer);
    galaxy_surf = sprite_get_pixels(galaxy_sprite);

    display_init(RESOLUTION_320x240, DEPTH_16_BPP, DOUBLE_BUFFERED, GAMMA_NONE, FILTERS_RESAMPLE);

    #ifndef SPACEWAVES_STANDALONE
    core_set_winner(PLAYER_1);
    core_set_winner(PLAYER_2);
    core_set_winner(PLAYER_3);
    core_set_winner(PLAYER_4);
    #endif

    rdpq_text_register_font(FONT_TEXT, bold_warning_font);
    rdpq_text_register_font(FONT_HEADING, normal_warning_font);

    rdpq_font_style(bold_warning_font, STYLE_DEFAULT, &(rdpq_fontstyle_t){.color = warning_ui_color});
    rdpq_font_style(normal_warning_font, STYLE_DEFAULT, &(rdpq_fontstyle_t){.color = warning_ui_color});

    while (!acknowledged) {

        myDT += display_get_delta_time();

        surface_t* disp;
        joypad_buttons_t btn;
        while(!(disp = display_try_get()));
        // Attach the buffers to the RDP (No z-buffer needed yet)
        rdpq_attach_clear(disp, NULL);
        rdpq_set_fill_color(black_color);
        
        rdpq_fill_rectangle(0,0, display_get_width(), display_get_height());
        rdpq_set_mode_standard();
        rdpq_tex_upload(TILE1, &galaxy_surf, NULL);
        rdpq_texture_rectangle_scaled(
            TILE1, 
            0, 40*sin(myDT*0.25) - 40,
            display_get_width(), 40*sin(myDT*0.25) + display_get_width() - 40,
            0, 0,
            32, 32
            );

        rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);

        rdpq_tex_upload(TILE2, &expansion_pak_surf, NULL);
        rdpq_texture_rectangle_scaled(
            TILE2, 
            (display_get_width() / 2) - 32, 8*sin(myDT)+152,
            (display_get_width() / 2) + 32, 8*sin(myDT)+216,
            0, 0,
            44, 44
        );

        rdpq_text_printf(&(rdpq_textparms_t){.align = ALIGN_CENTER, .width = 320}, FONT_HEADING, 0, 48,"SPACEWAVES");
        rdpq_text_printf(&(rdpq_textparms_t){.align = ALIGN_CENTER, .width = 320}, FONT_TEXT, 0, 72, "REMEMBER\nSTATION DEFENDERS");
        rdpq_text_printf(&(rdpq_textparms_t){.align = ALIGN_CENTER, .width = 320}, FONT_TEXT, 0, 108, "Spacewaves requires an N64 Expansion Pak\nto be inserted into your N64 Control Deck to play.\nPress A to continue...");

        #ifndef SPACEWAVES_STANDALONE
        joypad_poll();
        for (int i = 0; i < 4; i++) {
            
            btn = joypad_get_buttons_pressed(core_get_playercontroller(i));
            if (btn.a || btn.start) {
                acknowledged = true;
            }
            btn = joypad_get_buttons_pressed(core_get_playercontroller(i));
            if (btn.a || btn.start) {
                acknowledged = true;
            }
        }
        #endif
        rdpq_detach_show();
    }
    #ifndef SPACEWAVES_STANDALONE
    
    surface_free(&expansion_pak_surf);
    surface_free(&galaxy_surf);
    
    sprite_free(expansion_pak_sprite);
    sprite_free(galaxy_sprite);

    rdpq_text_unregister_font(FONT_TEXT);
    rdpq_text_unregister_font(FONT_HEADING);

    rdpq_font_free(bold_warning_font);
    rdpq_font_free(normal_warning_font);

    expansion_pak_sprite = NULL;
    galaxy_sprite = NULL;

    bold_warning_font = NULL;
    normal_warning_font = NULL;

    #endif
}