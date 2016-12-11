#include "model.h"


model::model()
{
	obj_coord_trans[0].x = 1;

	obj_coord_trans[1].y = 1;

	obj_coord_trans[2].z = 1;
	
	obj_coord_trans[3].p = 1;

	inv_obj_coord_trans[0].x = 1;

	inv_obj_coord_trans[1].y = 1;

	inv_obj_coord_trans[2].z = 1;

	inv_obj_coord_trans[3].p = 1;

	camera_trans[0].x = 1;

	camera_trans[1].y = 1;

	camera_trans[2].z = 1;

	camera_trans[3].p = 1;
	active_model = true;
	color = RGB(255, 255, 255);
}


model::~model()
{
}

std::vector<line> model::Normal(bool given){
	if (given)
		return vertex_normals_list;
	return vertex_normals_list_polygons;
}