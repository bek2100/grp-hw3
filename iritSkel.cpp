#include "stdafx.h"
#include "iritSkel.h"
#include "line.h"
#include <functional>
#include <unordered_map>
/*****************************************************************************
* Skeleton for an interface to a parser to read IRIT data files.			 *
******************************************************************************
* (C) Gershon Elber, Technion, Israel Institute of Technology                *
******************************************************************************
* Written by:  Gershon Elber				Ver 1.0, Feb 2002				 *
* Minimal changes made by Amit Mano			November 2008					 *
******************************************************************************/
std::vector<model> models;
int model_cnt;

// line class hash

std::unordered_map<line, int> lines;
std::unordered_map<vec4, int> vertex;
std::unordered_map<vec4, std::vector<polygon*>> vertex_polygons;

IPFreeformConvStateStruct CGSkelFFCState = {
	FALSE,          /* Talkative */
	FALSE,          /* DumpObjsAsPolylines */
	TRUE,           /* DrawFFGeom */
	FALSE,          /* DrawFFMesh */
	{ 10, 10, 10 }, /* 10 isocurves peru/v/w direction. */
	100,            /* 100 point samples along a curve. */
	SYMB_CRV_APPROX_UNIFORM,  /* CrvApproxMethod */
	FALSE,   /* ShowIntrnal */
	FALSE,   /* CubicCrvsAprox */
	20,      /* Polygonal FineNess */
	FALSE,   /* ComputeUV */
	TRUE,    /* ComputeNrml */
	FALSE,   /* FourPerFlat */
	0,       /* OptimalPolygons */
	FALSE,   /* BBoxGrid */
	TRUE,    /* LinearOnePolyFlag */
	FALSE
};

//CGSkelProcessIritDataFiles(argv + 1, argc - 1);


/*****************************************************************************
* DESCRIPTION:                                                               *
* Main module of skeleton - Read command line and do what is needed...	     *
*                                                                            *
* PARAMETERS:                                                                *
*   FileNames:  Files to open and read, as a vector of strings.              *
*   NumFiles:   Length of the FileNames vector.								 *
*                                                                            *
* RETURN VALUE:                                                              *
*   bool:		false - fail, true - success.                                *
*****************************************************************************/
bool CGSkelProcessIritDataFiles(CString &FileNames, int NumFiles)
{
	IPObjectStruct *PObjects;
	model_cnt = 0;
	IrtHmgnMatType CrntViewMat;

	lines.clear();
	vertex_polygons.clear();
	vertex.clear();

	/* Get the data files: */
	IPSetFlattenObjects(FALSE);
	CStringA CStr(FileNames);
	if ((PObjects = IPGetDataFiles((const char* const *)&CStr, 1/*NumFiles*/, TRUE, FALSE)) == NULL)
		return false;
	PObjects = IPResolveInstances(PObjects);

	if (IPWasPrspMat)
		MatMultTwo4by4(CrntViewMat, IPViewMat, IPPrspMat);
	else
		IRIT_GEN_COPY(CrntViewMat, IPViewMat, sizeof(IrtHmgnMatType));

	/* Here some useful parameters to play with in tesselating freeforms: */
	//CGSkelFFCState.FineNess = 20;   /* Res. of tesselation, larger is finer. */
	CGSkelFFCState.ComputeUV = TRUE;   /* Wants UV coordinates for textures. */
	CGSkelFFCState.FourPerFlat = TRUE;/* 4 poly per ~flat patch, 2 otherwise.*/
	CGSkelFFCState.LinearOnePolyFlag = TRUE;    /* Linear srf gen. one poly. */

	/* Traverse ALL the parsed data, recursively. */
	IPTraverseObjListHierarchy(PObjects, CrntViewMat, 
		CGSkelDumpOneTraversedObject);


	// -------> Y
	// |
	// |
	// |
	// V
	// X

	// determine the primat screen space scale for this group of loaded objects
	mat4 view_space_scale;
	double max_x, max_y, max_z;
	double min_x, min_y, min_z;

	max_x = models.rbegin()[0].max_vec.x;
	max_y = models.rbegin()[0].max_vec.y;
	max_z = models.rbegin()[0].max_vec.z;

	min_x = models.rbegin()[0].min_vec.x;
	min_y = models.rbegin()[0].min_vec.y;
	min_z = models.rbegin()[0].min_vec.z;

	for (int m = 1; m < model_cnt; m++){
		min_x = models.rbegin()[m].min_vec.x < min_x ? models.rbegin()[m].min_vec.x : min_x;
		min_y = models.rbegin()[m].min_vec.y < min_y ? models.rbegin()[m].min_vec.y : min_y;
		min_z = models.rbegin()[m].min_vec.z < min_z ? models.rbegin()[m].min_vec.z : min_z;

		max_x = models.rbegin()[m].max_vec.x > max_x ? models.rbegin()[m].max_vec.x : max_x;
		max_y = models.rbegin()[m].max_vec.y > max_y ? models.rbegin()[m].max_vec.y : max_y;
		max_z = models.rbegin()[m].max_vec.z > max_z ? models.rbegin()[m].max_vec.z : max_z;
	}

	double box_x = max_x - min_x;
	double box_y = max_y - min_y;
	double box_z = max_z - min_z;

	double max_box = max(box_z,max(box_x, box_y));

	if (max_box == 0) max_box = 1;
	view_space_scale[0][0] = (double)1 / max_box;
	view_space_scale[1][1] = (double)1 / max_box;
	view_space_scale[2][2] = (double)1 / max_box;
	view_space_scale[3][3] = 1;

	mat4 depth_transpose;
	depth_transpose[0][0] = 1;
	depth_transpose[1][1] = 1;
	depth_transpose[2][2] = 1;
	depth_transpose[3][2] = 8;

	depth_transpose[3][3] = 1;

	
	CString indx;
	CString file_full_name = FileNames.Mid(FileNames.ReverseFind('\\') + 1);

	int n_tokens_pos = 0;
	CString file_name = file_full_name.Tokenize(_T("."), n_tokens_pos);
	for (int m = 0; m < model_cnt; m++){
		models.rbegin()[m].view_space_trans = view_space_scale;
		models.rbegin()[m].camera_trans = depth_transpose;
		indx.Format(_T("%d"), m);
		models.rbegin()[m].model_name = file_name + indx;
	}

	return true;
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Call back function of IPTraverseObjListHierarchy. Called on every non    *
* list object found in hierarchy.                                            *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:       Non list object to handle.                                   *
*   Mat:        Transformation matrix to apply to this object.               *
*                                                                            *
* RETURN VALUE:                                                              *
*   void									                                 *
*****************************************************************************/
void CGSkelDumpOneTraversedObject(IPObjectStruct *PObj, IrtHmgnMatType Mat)
{
	IPObjectStruct *PObjs;

	if (IP_IS_FFGEOM_OBJ(PObj))
		PObjs = IPConvertFreeForm(PObj, &CGSkelFFCState);
	else
		PObjs = PObj;

	for (PObj = PObjs; PObj != NULL; PObj = PObj -> Pnext)
		if (!CGSkelStoreData(PObj)) 
			exit(1);
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Prints the data from given geometry object.								 *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:       Object to print.                                             *
*   Indent:     Column of indentation.                                       *
*                                                                            *
* RETURN VALUE:                                                              *
*   bool:		false - fail, true - success.                                *
*****************************************************************************/
bool CGSkelStoreData(IPObjectStruct *PObj)
{
	int i;
	const char *Str;
	double RGB[3], Transp;
	IPPolygonStruct *PPolygon;
	IPVertexStruct *PVertex;
	vec4 min_vec;
	vec4 max_vec;
	IPAttributeStruct *Attrs = AttrTraceAttributes(PObj -> Attr, PObj -> Attr);

	min_vec.p = 1;
	max_vec.p = 1;
	model new_model;
	

	if (PObj -> ObjType != IP_OBJ_POLY) {
		AfxMessageBox(_T("Non polygonal object detected and ignored"));
		return true;
	}

	models.push_back(new_model);
	model_cnt++;
	/* You can use IP_IS_POLYGON_OBJ(PObj) and IP_IS_POINTLIST_OBJ(PObj) 
	   to identify the type of the object*/

	if (CGSkelGetObjectColor(PObj, RGB))
	{
		models.back().color = RGB(255*RGB[2], 255*RGB[1], 255*RGB[0]);
	}
	else{
		models.back().color = RGB(0, 0, 0);
	}
	if (CGSkelGetObjectTransp(PObj, &Transp))
	{
		/* transparency code */
	}
	if ((Str = CGSkelGetObjectTexture(PObj)) != NULL)
	{
		/* volumetric texture code */
	}
	if ((Str = CGSkelGetObjectPTexture(PObj)) != NULL)
	{
		/* parametric texture code */
	}
	if (Attrs != NULL) 
	{
		printf("[OBJECT\n");
		while (Attrs) {
			/* attributes code */
			Attrs = AttrTraceAttributes(Attrs, NULL);
		}
	}
	int poly_cnt = 0;
	line cur_line;

	for (PPolygon = PObj -> U.Pl; PPolygon != NULL;	PPolygon = PPolygon -> Pnext) 
	{
			if (PPolygon -> PVertex == NULL) {
				AfxMessageBox(_T("Dump: Attemp to dump empty polygon"));
				return false;
			}
			
			polygon cur_polygon;
			vec4 temp_vert;
			vec4 prev_temp_vert;
			models.back().polygons.push_back(cur_polygon); // create an additional empty polygon

			/* Count number of vertices. */
			for (PVertex = PPolygon -> PVertex -> Pnext, i = 1;
				PVertex != PPolygon -> PVertex && PVertex != NULL;
				PVertex = PVertex -> Pnext, i++);
			/* use if(IP_HAS_PLANE_POLY(PPolygon)) to know whether a normal is defined for the polygon
			   access the normal by the first 3 components of PPolygon->Plane */
			if (IP_HAS_PLANE_POLY(PPolygon) != 0){
				models.back().polygons[poly_cnt].Plane[0] = PPolygon->Plane[0];
				models.back().polygons[poly_cnt].Plane[1] = PPolygon->Plane[1];
				models.back().polygons[poly_cnt].Plane[2] = PPolygon->Plane[2];
				models.back().polygons[poly_cnt].Plane[3] = 0; // scale fix when calculating norm
			}
			PVertex = PPolygon -> PVertex;


			do {			     /* Assume at least one edge in polygon! */
				/* code handeling all vertex/normal/texture coords */
				/* use if(IP_HAS_NORMAL_VRTX(PVertex)) to know whether a normal is defined for the vertex 
				   access the vertex coords by PVertex->Coord
				   access the vertex normal by PVertex->Normal */ 



				temp_vert.x = PVertex->Coord[0];
				temp_vert.y = PVertex->Coord[1];
				temp_vert.z = PVertex->Coord[2];
				temp_vert.p = 1;

				if (temp_vert.x < min_vec.x || models.back().polygons.front().points.size() == 0) min_vec.x = temp_vert.x;
				if (temp_vert.y < min_vec.y || models.back().polygons.front().points.size() == 0) min_vec.y = temp_vert.y;
				if (temp_vert.z < min_vec.z || models.back().polygons.front().points.size() == 0) min_vec.z = temp_vert.z;

				if (temp_vert.x > max_vec.x || models.back().polygons.front().points.size() == 0) max_vec.x = temp_vert.x;
				if (temp_vert.y > max_vec.y || models.back().polygons.front().points.size() == 0) max_vec.y = temp_vert.y;
				if (temp_vert.z > max_vec.z || models.back().polygons.front().points.size() == 0) max_vec.z = temp_vert.z;

				models.back().polygons[poly_cnt].points.push_back(temp_vert); // create an additional vertex
				
				if (IP_HAS_NORMAL_VRTX(PVertex)){
					vec4 vertex1;
					vertex1[0] = PVertex->Normal[0];
					vertex1[1] = PVertex->Normal[1];
					vertex1[2] = PVertex->Normal[2];
					vertex1[3] = 0.0; // scale fix when adding on line below
					if (vertex[temp_vert] == 0){
						vertex[temp_vert] ++;
						models.back().vertex_normals_list.push_back(line(temp_vert, temp_vert + vertex1));
					}

					models.back().polygons[poly_cnt].vertexNormalsGiven[temp_vert] = line(temp_vert, temp_vert + vertex1);
				}


				prev_temp_vert = temp_vert;
				PVertex = PVertex -> Pnext;
			}
			while (PVertex != PPolygon -> PVertex && PVertex != NULL);
			/* Close the polygon. */


			poly_cnt++;
	}

	vec4 p1, p2;

	for (unsigned int p = 0; p < models.back().polygons.size(); p++){
		for (unsigned int pnt = 0; pnt < models.back().polygons[p].points.size(); pnt++){
			p1 = models.back().polygons[p].points[(pnt) % models.back().polygons[p].points.size()];
			p2 = models.back().polygons[p].points[(pnt + 1) % models.back().polygons[p].points.size()];
			cur_line = line(p1, p2);
			if (lines[cur_line] == 0){
				models.back().points_list.push_back(cur_line);
				lines[cur_line]++;
			}
			vertex_polygons[p1].push_back(&models.back().polygons[p]);
		}
	}

	for (unsigned int p = 0; p < models.back().polygons.size(); p++){
		for (unsigned int pnt = 0; pnt < models.back().polygons[p].points.size(); pnt++){
			p1 = models.back().polygons[p].points[(pnt) % models.back().polygons[p].points.size()];
			if (vertex[p1] <= 1){
				vertex[p1]++;
				vec4 avr = vec4(0,0,0,0);
				for (unsigned int j = 0; j <vertex_polygons[p1].size(); j++){
					avr = avr + vertex_polygons[p1][j]->Normal_Val(false);
				}
				avr = avr / vertex_polygons[p1].size();
				p2 = p1 + avr;
				p2[3] = 1;
				cur_line = line(p1, p2);
				for (unsigned int j = 0; j < vertex_polygons[p1].size(); j++){
					vertex_polygons[p1][j]->vertexNormalsCalculated[p1] = cur_line;
				}
				models.back().vertex_normals_list_polygons.push_back(cur_line);
			}
		}
	}

	models.back().max_vec = max_vec;
	models.back().min_vec = min_vec;
	/* Close the object. */
	return true;
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Returns the color of an object.                                          *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:   Object to get its color.                                         *
*   RGB:    as 3 floats in the domain [0, 1].                                *
*                                                                            *
* RETURN VALUE:                                                              *
*   int:    TRUE if object has color, FALSE otherwise.                       *
*****************************************************************************/
int CGSkelGetObjectColor(IPObjectStruct *PObj, double RGB[3])
{
	static int TransColorTable[][4] = {
		{ /* BLACK	*/   0,    0,   0,   0 },
		{ /* BLUE	*/   1,    0,   0, 255 },
		{ /* GREEN	*/   2,    0, 255,   0 },
		{ /* CYAN	*/   3,    0, 255, 255 },
		{ /* RED	*/   4,  255,   0,   0 },
		{ /* MAGENTA 	*/   5,  255,   0, 255 },
		{ /* BROWN	*/   6,   50,   0,   0 },
		{ /* LIGHTGRAY	*/   7,  127, 127, 127 },
		{ /* DARKGRAY	*/   8,   63,  63,  63 },
		{ /* LIGHTBLUE	*/   9,    0,   0, 255 },
		{ /* LIGHTGREEN	*/   10,   0, 255,   0 },
		{ /* LIGHTCYAN	*/   11,   0, 255, 255 },
		{ /* LIGHTRED	*/   12, 255,   0,   0 },
		{ /* LIGHTMAGENTA */ 13, 255,   0, 255 },
		{ /* YELLOW	*/   14, 255, 255,   0 },
		{ /* WHITE	*/   15, 255, 255, 255 },
		{ /* BROWN	*/   20,  50,   0,   0 },
		{ /* DARKGRAY	*/   56,  63,  63,  63 },
		{ /* LIGHTBLUE	*/   57,   0,   0, 255 },
		{ /* LIGHTGREEN	*/   58,   0, 255,   0 },
		{ /* LIGHTCYAN	*/   59,   0, 255, 255 },
		{ /* LIGHTRED	*/   60, 255,   0,   0 },
		{ /* LIGHTMAGENTA */ 61, 255,   0, 255 },
		{ /* YELLOW	*/   62, 255, 255,   0 },
		{ /* WHITE	*/   63, 255, 255, 255 },
		{		     -1,   0,   0,   0 }
	};
	int i, j, Color, RGBIColor[3];

	if (AttrGetObjectRGBColor(PObj,
		&RGBIColor[0], &RGBIColor[1], &RGBIColor[2])) {
			for (i = 0; i < 3; i++)
				RGB[i] = RGBIColor[i] / 255.0;

			return TRUE;
	}
	else if ((Color = AttrGetObjectColor(PObj)) != IP_ATTR_NO_COLOR) {
		for (i = 0; TransColorTable[i][0] >= 0; i++) {
			if (TransColorTable[i][0] == Color) {
				for (j = 0; j < 3; j++)
					RGB[j] = TransColorTable[i][j+1] / 255.0;
				return TRUE;
			}
		}
	}

	return FALSE;
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Returns the volumetric texture of an object, if any.                     *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:   Object to get its volumetric texture.                            *
*                                                                            *
* RETURN VALUE:                                                              *
*   char *:    Name of volumetric texture map to apply, NULL if none.        *
*****************************************************************************/
const char *CGSkelGetObjectTexture(IPObjectStruct *PObj)
{
	return AttrGetObjectStrAttrib(PObj, "texture");
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Returns the parametric texture of an object, if any.                     *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:   Object to get its parametric texture.                            *
*                                                                            *
* RETURN VALUE:                                                              *
*   char *:    Name of parametric texture map to apply, NULL if none.        *
*****************************************************************************/
const char *CGSkelGetObjectPTexture(IPObjectStruct *PObj)
{
	return AttrGetObjectStrAttrib(PObj, "ptexture");
}

/*****************************************************************************
* DESCRIPTION:                                                               *
*   Returns the transparency level of an object, if any.                     *
*                                                                            *
* PARAMETERS:                                                                *
*   PObj:   Object to get its volumetric texture.                            *
*   Transp: Transparency level between zero and one.                         *
*                                                                            *
* RETURN VALUE:                                                              *
*   int:    TRUE if object has transparency, FALSE otherwise.                *
*****************************************************************************/
int CGSkelGetObjectTransp(IPObjectStruct *PObj, double *Transp)
{
	*Transp = AttrGetObjectRealAttrib(PObj, "transp");

	return !IP_ATTR_IS_BAD_REAL(*Transp);
}

