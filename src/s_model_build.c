#include "s_common.h"

#define S_FLOOR_SIZE	6

static int32_t _s_model_builder_add_floor(mod_model_t* model)
{
	mod_quad_t quad = { 0 };
	float y = 0.0f;
	o_vector3_t corners[MOD_QUAD_VERTEX_COUNT] =
	{
		{ -S_FLOOR_SIZE, y, -S_FLOOR_SIZE },
		{ -S_FLOOR_SIZE, y, S_FLOOR_SIZE },
		{ S_FLOOR_SIZE, y, S_FLOOR_SIZE },
		{ S_FLOOR_SIZE, y, -S_FLOOR_SIZE }
	};

	for (size_t i = 0; i < MOD_QUAD_VERTEX_COUNT; ++i)
	{
		o_vertex_t* v = (quad.m_vertex + i);

		v->m_position = corners[i];
		v->m_normal = (o_vector3_t){ 0.0f, 1.0f, 0.0f };
		v->m_color = al_map_rgb(0x80, 0x80, 0xff);
		v->m_uv = (o_vector2_t){ 0.0f, 0.0f };
		v->m_meta = 0;

	}

	if (!mod_model_add_quad(model, &quad, true))
	{
		return -1;
	}

	return 0;
}

static int32_t _s_model_builder_add_cube(mod_model_t* model)
{
	mod_transform_t meta_data =
	{
		.m_translation = (o_vector3_t){ -2.0f, 0.5f, -1.0f},
		.m_scale = (o_vector3_t){ 1.0f, 1.0f, 1.0f},
		.m_rotation = (o_vector3_t){ 0.0f, 0.125f, 0.0f},
		.m_color = (ALLEGRO_COLOR){ 1.0f, 0.0f, 0.0f, 1.0f }
	};

	mod_model_t* cube = mod_cube_generate(1.0f, false);
	if (NULL == cube)
	{
		return -1;
	}
	mod_model_add_model_with_meta_data(model, cube, &meta_data, false);
	mod_model_destroy(cube);

	return 0;
}

static int32_t _s_model_builder_add_sphere(mod_model_t* model)
{
	mod_transform_t meta_data =
	{
		.m_translation = (o_vector3_t){ 3.0f, 0.5f, 2.0f},
		.m_scale = (o_vector3_t){ 1.0f, 1.0f, 1.0f},
		.m_rotation = (o_vector3_t){ 0.0f, 0.0f, 0.0f},
		.m_color = (ALLEGRO_COLOR){ 1.0f, 0.5f, 0.0f, 1.0f }
	};

	mod_model_t* sphere = mod_sphere_generate(0.5f, false, 3);
	if (NULL == sphere)
	{
		return -1;
	}
	mod_model_add_model_with_meta_data(model, sphere, &meta_data, false);
	mod_model_destroy(sphere);

	return 0;
}

static int32_t _s_model_builder_add_pyramid(mod_model_t* model)
{
	float radius = 0.5f;
	float height = radius * sqrtf(2.0f);
	mod_transform_t meta_data =
	{
		.m_translation = (o_vector3_t){ -0.5f, 0.5f * height, 3.0f},
		.m_scale = (o_vector3_t){ 1.0f, 1.0f, 1.0f},
		.m_rotation = (o_vector3_t){ 0.0f, 0.25f, 0.0f},
		.m_color = (ALLEGRO_COLOR){ 0.0f, 1.0f, 1.0f, 1.0f }
	};


	mod_model_t* pyramid = mod_pyramid_generate(radius, height, 3, false);
	if (NULL == pyramid)
	{
		return -1;
	}

	mod_model_add_model_with_meta_data(model, pyramid, &meta_data, false);
	mod_model_destroy(pyramid);
	
	return 0;
}

static int32_t _s_model_builder(mod_model_t* model)
{
	if (_s_model_builder_add_floor(model) < 0)
	{
		return -1;
	}

	if (_s_model_builder_add_cube(model) < 0)
	{
		return -1;
	}

	if (_s_model_builder_add_sphere(model) < 0)
	{
		return -1;
	}

	if (_s_model_builder_add_pyramid(model) < 0)
	{
		return -1;
	}

	return 0;
}


mod_model_t* s_model_builder()
{
	mod_model_t* model = mod_model_create_empty();
	if (NULL == model)
	{
		return NULL;
	}

	if (_s_model_builder(model) < 0)
	{
		mod_model_destroy(model);
		return NULL;
	}

	mod_model_recalculate_normals(model);

	return model;
}
