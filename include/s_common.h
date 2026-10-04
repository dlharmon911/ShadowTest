#ifndef _HEADER_GUARD_SHADOW_TEST_COMMON_H_
#define _HEADER_GUARD_SHADOW_TEST_COMMON_H_

#include <allegro5/allegro5.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_memfile.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_opengl.h>
#include <physfs.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <libogle.h>
#include <libmodeler.h>
#include "s_manifest.h"

typedef struct s_data_main_tag_t s_data_main_t;
typedef struct s_data_render_tag_t s_data_render_t;
typedef struct s_shadow_map_tag_t s_shadow_map_t;

enum S_SHADER_IDS
{
    S_SHADER_ID_NORMAL,
    S_SHADER_ID_LIGHT,
    S_SHADER_ID_DEPTH,
    S_SHADER_COUNT
};

struct s_data_render_tag_t
{
    o_transform_t m_transform_projection;
    o_transform_t m_transform_view;
    o_transform_t m_transform_model;
    o_camera_t m_camera;
    o_light_t m_light;
    ALLEGRO_VERTEX_DECL* m_vertex_decl;
    ALLEGRO_SHADER* m_shader[S_SHADER_COUNT];
    ALLEGRO_BITMAP* m_shadow_map;
    mod_model_t* m_model_world;
    float m_light_angle;
};

struct s_data_main_tag_t
{
    ALLEGRO_DISPLAY* m_display;
    ALLEGRO_TIMER* m_timer;
    ALLEGRO_EVENT_QUEUE* m_event_queue;
    o_input_t* m_input;
    s_data_render_t* m_render;
    float m_delta_time;
    double m_fps;
    bool m_update;
    bool m_draw_stats;
    bool m_running;
    bool m_draw_mouse;
};

void s_data_main_zero(s_data_main_t* data);
int32_t s_data_main_initialize(s_data_main_t* data);
void s_data_main_uninitialize(s_data_main_t* data);

void s_data_render_zero(s_data_render_t* data);
int32_t s_data_render_initialize(s_data_render_t* data);
void s_data_render_uninitialize(s_data_render_t* data);

mod_model_t* s_model_builder();

#endif // _HEADER_GUARD_SHADOW_TEST_COMMON_H_
