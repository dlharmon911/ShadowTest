#define ALLEGRO_UNSTABLE

#include "s_common.h"

static const o_light_t g_light =
{
    .m_position = { 20.0f, 20.0f, 20.0f },
    .m_ambient = { 0.6f, 0.6f, 0.6f },
    .m_diffuse = { 1.0f, 1.0f, 1.0f },
    .m_specular = { 0.4f, 0.4f, 0.4f }
};

static const o_camera_t g_camera =
{
    .m_position = { 0.0f, 2.0f, 10.0f },
    .m_lookat = { 0.0f, 0.0f, 0.0f },
    .m_up = { 0.0f, 1.0f, 0.0f }
};

#define S_SHADOW_MAP_WIDTH 1024
#define S_SHADOW_MAP_HEIGHT 1024

static const char* S_SHADER_VERTEX_FILENAME = "assets/shaders/vertex_%s.glsl";
static const char* S_SHADER_PIXEL_FILENAME = "assets/shaders/pixel_%s.glsl";

static const char* S_SHADER_PREFIXES[S_SHADER_COUNT] =
{
	"normal",
	"light",
	"depth"
};

void s_data_render_zero(s_data_render_t* data)
{
	if (!data)
	{
		return;
	}

	memset(data, 0, sizeof(s_data_render_t));
}

static int32_t _s_data_render_initialize(s_data_render_t* data)
{
	if (NULL == data)
	{
		return -1;
	}

	data->m_vertex_decl = ogle_vertex_decl_create();
	if (NULL == data->m_vertex_decl)
	{
		ogle_do_log(OGLE_LOG_LEVEL_ERROR, "Failed to create vertex declaration");
		return -1;
	}

	memcpy(&data->m_light, &g_light, sizeof(o_light_t));
	memcpy(&data->m_camera, &g_camera, sizeof(o_camera_t));

	al_set_new_bitmap_flags(ALLEGRO_VIDEO_BITMAP);
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ABGR_F32);
	al_set_new_bitmap_depth(24);
	data->m_shadow_map = al_create_bitmap(S_SHADOW_MAP_WIDTH, S_SHADOW_MAP_HEIGHT);
	al_set_new_bitmap_depth(0);
	al_set_new_bitmap_format(ALLEGRO_PIXEL_FORMAT_ANY);
	if (NULL == data->m_shadow_map)
	{
		ogle_do_log(OGLE_LOG_LEVEL_ERROR, "Failed to create shadow map bitmap");
		return -1;
	}

	data->m_model_world = s_model_builder();
	if (NULL == data->m_model_world)
	{
		return -1;
	}

	for (int32_t i = 0; i < S_SHADER_COUNT; ++i)
	{
		const char* shader_prefix = S_SHADER_PREFIXES[i];
		char vertex_filename[256] = { 0 };
		char pixel_filename[256] = { 0 };
		snprintf(vertex_filename, sizeof(vertex_filename), S_SHADER_VERTEX_FILENAME, shader_prefix);
		snprintf(pixel_filename, sizeof(pixel_filename), S_SHADER_PIXEL_FILENAME, shader_prefix);
		data->m_shader[i] = al_create_shader(ALLEGRO_SHADER_GLSL);
		if (!data->m_shader[i])
		{
			ogle_do_log(OGLE_LOG_LEVEL_ERROR, "Failed to create shader: %s", shader_prefix);
			return -1;
		}
		if (!al_attach_shader_source_file(data->m_shader[i], ALLEGRO_VERTEX_SHADER, vertex_filename))
		{
			ogle_do_log(OGLE_LOG_LEVEL_ERROR, "Failed to attach vertex shader source file: %s", vertex_filename);
			return -1;
		}
		if (!al_attach_shader_source_file(data->m_shader[i], ALLEGRO_PIXEL_SHADER, pixel_filename))
		{
			ogle_do_log(OGLE_LOG_LEVEL_ERROR, "Failed to attach pixel shader source file: %s", pixel_filename);
			return -1;
		}
		if (!al_build_shader(data->m_shader[i]))
		{
			const char* log = al_get_shader_log(data->m_shader[i]);
			if (log)
			{
				ogle_do_log(OGLE_LOG_LEVEL_ERROR, "Failed to build shader: %s\n%s", shader_prefix, log);
			}
			else
			{
				ogle_do_log(OGLE_LOG_LEVEL_ERROR, "Failed to build shader: %s", shader_prefix);
			}
			return -1;
		}
	}

	return 0;
}

int32_t s_data_render_initialize(s_data_render_t* data)
{
	if (NULL == data)
	{
		return -1;
	}
	
	s_data_render_zero(data);
	
	if (_s_data_render_initialize(data) != 0)
	{
		s_data_render_uninitialize(data);
		return -1;
	}

	return 0;
}

void s_data_render_uninitialize(s_data_render_t* data)
{
	if (NULL == data)
	{
		return;
	}

	if (data->m_shadow_map)
	{
		al_destroy_bitmap(data->m_shadow_map);
		data->m_shadow_map = NULL;
	}

	for (int32_t i = 0; i < S_SHADER_COUNT; ++i)
	{
		if (data->m_shader[i])
		{
			al_destroy_shader(data->m_shader[i]);
			data->m_shader[i] = NULL;
		}
	}

	if (data->m_model_world)
	{
		mod_model_destroy(data->m_model_world);
		data->m_model_world = NULL;
	}

	if (data->m_vertex_decl)
	{
		ogle_vertex_decl_destroy(data->m_vertex_decl);
		data->m_vertex_decl = NULL;
	}
}
