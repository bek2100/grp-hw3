// CGWorkView.cpp : implementation of the CCGWorkView class
//
#include "stdafx.h"
#include "CGWork.h"

#include "CGWorkDoc.h"
#include "CGWorkView.h"

#include <iostream>
#include <functional>
using std::cout;
using std::endl;
#include "MaterialDlg.h"
#include "LightDialog.h"
#include <algorithm> 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "PngWrapper.h"
#include "iritSkel.h"
#include "MouseSensetiveDialog.h"
#include "mat4.h"
#include "ColorSelectionDialog.h"
#include "PrespectiveControlDialog.h"
#include "OtherOptionsDialog.h"
#include <math.h>
#include "line.h"
#include <unordered_map>
#include <vector>
// For Status Bar access
#include "MainFrm.h"

extern std::vector<model> models;
extern IPFreeformConvStateStruct CGSkelFFCState;


// Use this macro to display text messages in the status bar.
#define STATUS_BAR_TEXT(str) (((CMainFrame*)GetParentFrame())->getStatusBar().SetWindowText(str))

#define IN_RANGE(x, y) ((1 <= x) && (x < (m_WindowHeight - 1)) && (1 <= y) && (y < (m_WindowWidth - 1)))


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView

IMPLEMENT_DYNCREATE(CCGWorkView, CView)

BEGIN_MESSAGE_MAP(CCGWorkView, CView)
	//{{AFX_MSG_MAP(CCGWorkView)
	ON_WM_ERASEBKGND()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_MESSAGE(WM_MOUSEMOVE, OnMouseMovement)
	ON_COMMAND(ID_FILE_LOAD, OnFileLoad)
	ON_COMMAND(ID_WIREFRAME_COLOUR, OnWriteframeColor)
	ON_COMMAND(ID_VIEW_ORTHOGRAPHIC, OnViewOrthographic)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ORTHOGRAPHIC, OnUpdateViewOrthographic)
	ON_COMMAND(ID_VIEW_PERSPECTIVE, OnViewPerspective)
	ON_UPDATE_COMMAND_UI(ID_VIEW_PERSPECTIVE, OnUpdateViewPerspective)
	ON_COMMAND(ID_ACTION_ROTATE, OnActionRotate)
	ON_UPDATE_COMMAND_UI(ID_ACTION_ROTATE, OnUpdateActionRotate)
	ON_COMMAND(ID_ACTION_SCALE, OnActionScale)
	ON_UPDATE_COMMAND_UI(ID_ACTION_SCALE, OnUpdateActionScale)
	ON_COMMAND(ID_ACTION_TRANSLATE, OnActionTranslate)
	ON_UPDATE_COMMAND_UI(ID_ACTION_TRANSLATE, OnUpdateActionTranslate)
	ON_COMMAND(ID_ACTION_RESETVIEW, OnActionResetView)
	ON_COMMAND(ID_AXIS_X, OnAxisX)
	ON_UPDATE_COMMAND_UI(ID_AXIS_X, OnUpdateAxisX)
	ON_COMMAND(ID_AXIS_Y, OnAxisY)
	ON_UPDATE_COMMAND_UI(ID_AXIS_Y, OnUpdateAxisY)
	ON_COMMAND(ID_AXIS_Z, OnAxisZ)
	ON_UPDATE_COMMAND_UI(ID_AXIS_Z, OnUpdateAxisZ)
	ON_COMMAND(ID_AXIS_XY, OnAxisXY)
	ON_UPDATE_COMMAND_UI(ID_AXIS_XY, OnUpdateAxisXY)
	ON_COMMAND(ID_BOUNDBOX, OnBoundBox)
	ON_UPDATE_COMMAND_UI(ID_BOUNDBOX, OnUpdateBoundBox)
	ON_COMMAND(ID_LIGHT_SHADING_FLAT, OnLightShadingFlat)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_SHADING_FLAT, OnUpdateLightShadingFlat)
	ON_COMMAND(ID_LIGHT_SHADING_GOURAUD, OnLightShadingGouraud)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_SHADING_GOURAUD, OnUpdateLightShadingGouraud)
	ON_COMMAND(ID_LIGHT_CONSTANTS, OnLightConstants)
	ON_COMMAND(ID_BUTTON_TRANS_TOGGLE, OnActionToggleView)
	ON_COMMAND(ID_OPTIONS_MOUSESENSITIVITY, OnOptionMouseSensetivity)
	ON_COMMAND(ID_OPTIONS_PERSPECTIVECONTROL, OnOptionPrespectiveControl)
	ON_COMMAND(ID_OPTIONS_OTHEROPTIONS, OnOptionOthers)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_TRANS_TOGGLE, OnUpdateActionToggleView)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_POLYGON_GIVEN, &CCGWorkView::OnPolygonGiven)
	ON_COMMAND(ID_POLYGON_CALCULATED, &CCGWorkView::OnPolygonCalculated)
	ON_UPDATE_COMMAND_UI(ID_POLYGON_CALCULATED, &CCGWorkView::OnUpdatePolygonCalculated)
	ON_UPDATE_COMMAND_UI(ID_POLYGON_GIVEN, &CCGWorkView::OnUpdatePolygonGiven)
	ON_COMMAND(ID_VERTEX_GIVEN, &CCGWorkView::OnVertexGiven)
	ON_UPDATE_COMMAND_UI(ID_VERTEX_GIVEN, &CCGWorkView::OnUpdateVertexGiven)
	ON_COMMAND(ID_VERTEX_CALCULATED, &CCGWorkView::OnVertexCalculated)
	ON_UPDATE_COMMAND_UI(ID_VERTEX_CALCULATED, &CCGWorkView::OnUpdateVertexCalculated)
END_MESSAGE_MAP()


// A patch to fix GLaux disappearance from VS2005 to VS2008
void auxSolidCone(GLdouble radius, GLdouble height) {
        GLUquadric *quad = gluNewQuadric();
        gluQuadricDrawStyle(quad, GLU_FILL);
        gluCylinder(quad, radius, 0.0, height, 20, 20);
        gluDeleteQuadric(quad);
}

/////////////////////////////////////////////////////////////////////////////
// CCGWorkView construction/destruction

CCGWorkView::CCGWorkView()
{
	// Set default values
	m_nAxis = ID_AXIS_X;
	m_nAction = ID_ACTION_ROTATE;
	m_nView = ID_VIEW_ORTHOGRAPHIC;	
	m_screen = NULL;
	z_buffer = NULL;
	polygon_normal = NULL;

	m_object_space_trans = false;
	m_bound_box = false;
	m_mouse_sensetivity = 1;
	CGSkelFFCState.FineNess = 20;

	m_bIsPerspective = false;
	m_tarnsform[0][0] = 1;
	m_tarnsform[1][1] = 1;
	m_tarnsform[2][2] = 1;
	m_tarnsform[3][3] = 1;

	m_presepctive_d = 1;
	m_presepctive_alpha = 0.3;
	
	m_prespective_trans[0][0] = 1;
	m_prespective_trans[1][1] = 1;
	m_prespective_trans[2][2] = 1;
	m_prespective_trans[2][3] = 1 / m_presepctive_d;
	


	m_color_wireframe = RGB(0, 0, 0);
	m_background_color = RGB(255, 255, 255);
	m_boundbox_color = RGB(0, 0, 0);
	m_polygon_norm_color = RGB(0, 0, 0);
	m_vertex_norm_color = RGB(0, 0, 0);

	m_nLightShading = ID_LIGHT_SHADING_FLAT;

	m_lMaterialAmbient = 0.2;
	m_lMaterialDiffuse = 0.8;
	m_lMaterialSpecular = 1.0;
	m_nMaterialCosineFactor = 32;


	//init the first light to be enabled
	m_lights[LIGHT_ID_1].enabled=true;
}

CCGWorkView::~CCGWorkView()
{
}


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView diagnostics

#ifdef _DEBUG
void CCGWorkView::AssertValid() const
{
	CView::AssertValid();
}

void CCGWorkView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCGWorkDoc* CCGWorkView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCGWorkDoc)));
	return (CCGWorkDoc*)m_pDocument;
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView Window Creation - Linkage of windows to CGWork

BOOL CCGWorkView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	// An CGWork window must be created with the following
	// flags and must NOT include CS_PARENTDC for the
	// class style.

	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}

int CCGWorkView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	InitializeCGWork();

	return 0;
}


// This method initialized the CGWork system.
BOOL CCGWorkView::InitializeCGWork()
{
	m_pDC = new CClientDC(this);

	m_hDC = CreateCompatibleDC(m_pDC->GetSafeHdc());

	if (NULL == m_hDC) { // failure to get DC
		::AfxMessageBox(CString("Couldn't get a valid DC."));
		return FALSE;
	}

	if ( NULL == m_pDC ) { // failure to get DC
		::AfxMessageBox(CString("Couldn't get a valid DC."));
		return FALSE;
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView message handlers


void CCGWorkView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);

	if ( 0 >= cx || 0 >= cy ) {
		return;
	}

	// save the width and height of the current window
	m_WindowWidth = cx;
	m_WindowHeight = cy;

	// compute the aspect ratio
	// this will keep all dimension scales equal
	m_AspectRatio = (GLdouble)m_WindowWidth/(GLdouble)m_WindowHeight;
}


BOOL CCGWorkView::SetupViewingFrustum(void)
{
    return TRUE;
}


// This viewing projection gives us a constant aspect ration. This is done by
// increasing the corresponding size of the ortho cube.
BOOL CCGWorkView::SetupViewingOrthoConstAspect(void)
{
	return TRUE;
}

BOOL CCGWorkView::OnEraseBkgnd(CDC* pDC) 
{
	// Windows will clear the window with the background color every time your window 
	// is redrawn, and then CGWork will clear the viewport with its own background color.

	// return CView::OnEraseBkgnd(pDC);
	return true;
}



/////////////////////////////////////////////////////////////////////////////
// CCGWorkView drawing
/////////////////////////////////////////////////////////////////////////////

void CCGWorkView::OnDraw(CDC* pDC)
{
	CCGWorkDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	delete m_screen;
	m_screen = (COLORREF*)calloc(m_WindowWidth * m_WindowHeight, sizeof(COLORREF));
	z_buffer = (int*)calloc(m_WindowWidth * m_WindowHeight, sizeof(int));

	mat4 screen_space_scale;
	mat4 screen_space_translate;


	int min_axis = min(m_WindowHeight, m_WindowWidth);

	screen_space_scale[0][0] = (double)min_axis*0.6;
	screen_space_scale[1][1] = (double)min_axis*0.6;
	screen_space_scale[2][2] = (double)min_axis*0.6;
	screen_space_scale[3][3] = 1;

	screen_space_translate[0][0] = 1;
	screen_space_translate[3][0] = 0.5 * m_WindowHeight;

	screen_space_translate[1][1] = 1;
	screen_space_translate[3][1] = 0.5 * m_WindowWidth;

	screen_space_translate[2][2] = 1;
	screen_space_translate[3][3] = 1;

	m_screen_space_trans = screen_space_scale * screen_space_translate;

	RenderScene();

	if (!pDoc)
	    return;
}


/////////////////////////////////////////////////////////////////////////////
// CCGWorkView CGWork Finishing and clearing...

void CCGWorkView::OnDestroy() 
{
	CView::OnDestroy();
	if (m_screen){
		delete m_screen;
	}
	if (z_buffer){
		delete z_buffer;
	}

	if (m_hDC){
		DeleteDC(m_hDC);
	}
	// delete the DC
	if ( m_pDC ) {
		delete m_pDC;
	}
}



/////////////////////////////////////////////////////////////////////////////
// User Defined Functions
bool CCGWorkView::InRange(int x, int y, int width, int height){
	return (0 <= x && x < height && 0 <= y && y < width);
};

bool past_pressed;
LRESULT CCGWorkView::OnMouseMovement(WPARAM wparam, LPARAM lparam){
	int xPos = GET_X_LPARAM(lparam);
	int yPos = GET_Y_LPARAM(lparam);

	if (wparam == MK_LBUTTON && past_pressed)
	{
		mat4 temp_transform;
		mat4 temp_transform_xy;

		// x mouse movement params
		double diff_x = (xPos - m_mouse_xpos);
		
		double teta_x = m_mouse_sensetivity * 10 * asin((double)diff_x / m_WindowWidth);

		double sinx = sin(teta_x);
		
		double cosx = cos(teta_x); // sin(x) ~= x movements are very small

		// x mouse movement params
		double diff_y = (yPos - m_mouse_ypos);
		
		double teta_y = m_mouse_sensetivity * 10 * asin((double)diff_y / m_WindowHeight);  //TODO scales...
		double siny = sin(teta_y);
		
		double cosy = cos(teta_y); // sin(y) ~= y movements are very small

		if (m_nAction == ID_ACTION_ROTATE){
			if (m_nAxis == ID_AXIS_X){
				temp_transform[0][0] = 1;

				temp_transform[1][1] = cosx;
				temp_transform[1][2] = sinx;

				temp_transform[2][1] = -sinx;
				temp_transform[2][2] = cosx;

				temp_transform[3][3] = 1;

			}
			if (m_nAxis == ID_AXIS_Y){
				temp_transform[0][0] = cosy;
				temp_transform[0][2] = siny;

				temp_transform[1][1] = 1;

				temp_transform[2][0] = -siny;
				temp_transform[2][2] = cosy;

				temp_transform[3][3] = 1;
			}
			if (m_nAxis == ID_AXIS_Z){
				temp_transform[0][0] = cosx;
				temp_transform[0][1] = sinx;

				temp_transform[1][0] = -sinx;
				temp_transform[1][1] = cosx;

				temp_transform[2][2] = 1;

				temp_transform[3][3] = 1;
			}
			if (m_nAxis == ID_AXIS_XY){
				temp_transform[0][0] = 1;

				temp_transform[1][1] = cosx;
				temp_transform[1][2] = sinx;

				temp_transform[2][1] = -sinx;
				temp_transform[2][2] = cosx;

				temp_transform[3][3] = 1;

				temp_transform_xy[0][0] = cosy;
				temp_transform_xy[0][2] = siny;

				temp_transform_xy[1][1] = 1;

				temp_transform_xy[2][0] = -siny;
				temp_transform_xy[2][2] = cosy;

				temp_transform_xy[3][3] = 1;

				temp_transform = temp_transform_xy * temp_transform;
			}
		}
		else if (m_nAction == ID_ACTION_TRANSLATE){
			if (m_nAxis == ID_AXIS_X){
				temp_transform[0][0] = 1;
				temp_transform[3][0] = m_mouse_sensetivity * (double)diff_x / m_WindowWidth;

				temp_transform[1][1] = 1;

				temp_transform[2][2] = 1;

				temp_transform[3][3] = 1;
			}
			if (m_nAxis == ID_AXIS_Y){
				temp_transform[0][0] = 1;

				temp_transform[1][1] = 1;
				temp_transform[3][1] = m_mouse_sensetivity * (double)diff_y / m_WindowHeight;

				temp_transform[2][2] = 1;

				temp_transform[3][3] = 1;
			}
			if (m_nAxis == ID_AXIS_Z){
				temp_transform[0][0] = 1;

				temp_transform[1][1] = 1;

				temp_transform[2][2] = 1;
				temp_transform[3][2] = m_mouse_sensetivity * (double)diff_x / m_WindowWidth;

				temp_transform[3][3] = 1;
			}
			if (m_nAxis == ID_AXIS_XY){
				temp_transform[0][0] = 1;
				temp_transform[3][0] = m_mouse_sensetivity * (double)diff_x / m_WindowWidth;

				temp_transform[1][1] = 1;
				temp_transform[3][1] = m_mouse_sensetivity * (double)diff_y / m_WindowHeight;

				temp_transform[2][2] = 1;

				temp_transform[3][3] = 1;
			}
		}
		else if (m_nAction == ID_ACTION_SCALE){
			if (m_nAxis == ID_AXIS_X){
				temp_transform[0][0] = 1 + m_mouse_sensetivity * (double)diff_x / m_WindowWidth;

				temp_transform[1][1] = 1;

				temp_transform[2][2] = 1;

				temp_transform[3][3] = 1;
			}
			if (m_nAxis == ID_AXIS_Y){
				temp_transform[0][0] = 1;

				temp_transform[1][1] = 1 + m_mouse_sensetivity * (double)diff_y / m_WindowHeight;

				temp_transform[2][2] = 1;

				temp_transform[3][3] = 1;
			}
			if (m_nAxis == ID_AXIS_Z){
				temp_transform[0][0] = 1;

				temp_transform[1][1] = 1;

				temp_transform[2][2] = 1 + m_mouse_sensetivity * (double)diff_x / m_WindowWidth;

				temp_transform[3][3] = 1;
			}
			if (m_nAxis == ID_AXIS_XY){
				temp_transform[0][0] = 1 + m_mouse_sensetivity * (double)diff_x / m_WindowWidth;

				temp_transform[1][1] = 1 + m_mouse_sensetivity * (double)diff_y / m_WindowHeight;

				temp_transform[2][2] = 1;

				temp_transform[3][3] = 1;
			}
		}
		else {
			temp_transform[0][0] = 1;

			temp_transform[1][1] = 1;

			temp_transform[2][2] = 1;

			temp_transform[3][3] = 1;
		}
		m_tarnsform = temp_transform;

		for (unsigned int m = 0; m < models.size(); m++){
			if (models[m].active_model){
				if (m_object_space_trans)
					models[m].obj_coord_trans = m_tarnsform * models[m].obj_coord_trans;
				else
					models[m].camera_trans = models[m].camera_trans * m_tarnsform;
			}
		}

		RenderScene();

		m_mouse_xpos = xPos;
		m_mouse_ypos = yPos;
	}
	else if (wparam == MK_LBUTTON){
		past_pressed = true;
		m_mouse_xpos = xPos;
		m_mouse_ypos = yPos;
	}
	else{
		past_pressed = false;
	}

	return 0;
};

static int Depth(std::vector<vec4> q, int x, int y){
	
	vec4 p1, p2;
	std::vector<vec4> points;
	unsigned int i;
	for (i = 0; i < q.size(); i++){
		p1 = q[i];
		if (i + 1 == q.size())
			p2 = q[0];
		else p2 = q[i + 1];
		double y1 = max(p1.y / p1.p, p2.y / p2.p);
		double y2 = min(p1.y / p1.p, p2.y / p2.p);
		if (y <= y1 && y >= y2){
			if (x == static_cast<int> (p1.x / p1.p))
				return static_cast<int>(p1.z / p1.p);

			if (x == static_cast<int>(p2.x / p1.p))
				return static_cast<int>(p2.z / p2.p);

			double m = (p1.x / p1.p - p2.x / p2.p) / (p1.y / p1.p - p2.y / p2.p);
			double x1 = m*y - m*(p1.y / p1.p) + (p1.x / p1.p);
			double d1 = sqrt(pow(p1.y / p1.p - y, 2) + pow(p1.x / p1.p - x1, 2));
			double d = sqrt(pow(p1.y / p1.p - p2.y / p2.p, 2) + pow(p1.x / p1.p - p2.x / p2.p, 2));
			double z = (p1.z / p1.p)*d1 / d + (1 - d1 / d)*(p2.z / p2.p);
			if (x == static_cast<int>(x1))
				return static_cast<int>(z);
			points.push_back(vec4(x1, y, z, 1));
		}
	}
	
	for (i = 0; i < points.size(); i++){
		p1 = points[i];
		if (i + 1 == points.size())
			p2 = points[0];
		else p2 = points[i + 1];
		int x1 = static_cast<int> (max(p1.x, p2.x));
		int x2 = static_cast<int> (min(p1.x, p2.x));
		if (x <= x1 && x >= x2){
			double d1 = p1.x - x;
			double d = p1.x - p2.x;
			double z = p1.z*(d1 / d) + (1 - (d1 / d))*p2.z;
			return static_cast<int>(z);
		}
	}
	return NULL;
}

static int LinePointDepth(vec4 &p1, vec4 &p2, int x, int y){
	double m = (p1.x / p1.p - p2.x / p2.p) / (p1.y / p1.p - p2.y / p2.p);
	double x1 = m*y - m*(p1.y / p1.p) + (p1.x / p1.p);
	double d1 = sqrt(pow(p1.y / p1.p - y, 2) + pow(p1.x / p1.p - x1, 2));
	double d = sqrt(pow(p1.y / p1.p - p2.y / p2.p, 2) + pow(p1.x / p1.p - p2.x / p2.p, 2));
	double z = (p1.z / p1.p)*d1 / d + (1 - d1 / d)*(p2.z / p2.p);
	return static_cast<int>(z);
}

void CCGWorkView::DrawLine(int* z_arr, COLORREF *arr, vec4 &p1, vec4 &p2, COLORREF color, std::unordered_map<int, std::vector<int>>* x_y){
	
	// if the line is beyond the screen space, dont bother drawing it
	if (!(((p1.z > m_presepctive_d && p2.z > m_presepctive_d) && !(p1.x <= 0 && p2.x <= 0) && !(p1.y <= 0 && p2.y <= 0))
		&& (m_nView == ID_VIEW_PERSPECTIVE) ||
		(m_nView == ID_VIEW_ORTHOGRAPHIC)))
		return;
	bool xy = true;
	if (x_y == NULL)
		xy = false;
	// algorithm vars
	int x1, x2, y1, y2, dx, dy, d;
	int north_er, north_west_er, west_er, south_west_er, south_er, south_east_er, east_er, north_east_er;

	// draw location variables
	int x, y;

	// midpoint algorithm

	x1 = static_cast<int>(p1.x / p1.p < p2.x / p2.p ? p1.x / p1.p : p2.x / p2.p);
	x2 = static_cast<int>(p1.x / p1.p < p2.x / p2.p ? p2.x / p2.p : p1.x / p1.p);

	if (x1 != x2){
		y1 = static_cast<int>(p1.x / p1.p < p2.x / p2.p ? p1.y / p1.p : p2.y / p2.p);
		y2 = static_cast<int>(p1.x / p1.p < p2.x / p2.p ? p2.y / p2.p : p1.y / p1.p);
	}
	else{
		y1 = static_cast<int>(min(p1.y / p1.p, p2.y / p2.p));
		y2 = static_cast<int>(max(p1.y / p1.p, p2.y / p2.p));
	}


	dx = x2 - x1;
	dy = y2 - y1;

	x = x1;
	y = y1;

	east_er = 2 * dy;
	north_east_er = 2 * (dy - dx);
	north_er = -2 * dx;
	north_west_er = -2 * (dy - dx);
	west_er = -2 * dy;
	south_west_er = 2 * dx - 2 * dy;
	south_er = 2 * dx;
	south_east_er = 2 * dx + 2 * dy;
	
	int z;
	if (IN_RANGE(x, y) && arr[y + m_WindowWidth * x] != color){
		z = LinePointDepth(p1, p2, x, y);
		if (z && (z > z_arr[y + m_WindowWidth * x] || (z_arr[y + m_WindowWidth * x] == NULL))){
			arr[y + m_WindowWidth * x] = color;
			z_arr[y + m_WindowWidth * x] = z;
		}
	}
	if (xy){
		(*x_y)[y].push_back(x);
	}
	// select the correct midpoint algorithm (direction and incline)
	if (dx == 0){ // horizontal y line or line in z direction only
		//move in positive y direction only
			
		while (y < y2){
			y = y + 1;	
			if (IN_RANGE(x, y) && arr[y + m_WindowWidth * x] != color){
				z = LinePointDepth(p1, p2, x, y);
				if (z && (z > z_arr[y + m_WindowWidth * x] || (z_arr[y + m_WindowWidth * x] == NULL))){
					arr[y + m_WindowWidth * x] = color;
					z_arr[y + m_WindowWidth * x] = z;
				}
			}
			if (xy){
				(*x_y)[y].push_back(x);
			}
		}
		return;
	}

	double incline = (double)dy / (double)dx;

	if (incline > 1){
		d = dy - 2 * dx; // try to move in positive y direction only
		while (y < y2){
			if (d > 0){
				d = d + north_er;
				y = y + 1;
			}
			else{
				d = d + north_east_er;
				x = x + 1;
				y = y + 1;
			}
			if (IN_RANGE(x, y) && arr[y + m_WindowWidth * x] != color){
				z = LinePointDepth(p1, p2, x, y);
				if (z && (z > z_arr[y + m_WindowWidth * x] || (z_arr[y + m_WindowWidth * x] == NULL))){
					arr[y + m_WindowWidth * x] = color;
					z_arr[y + m_WindowWidth * x] = z;
				}
			}
			if (xy){
				(*x_y)[y].push_back(x);
			}
		}
	}
	else if (0 < incline && incline <= 1)
	{
		d = 2 * dy - dx; // try to move in positive x direction only, possibly positive y
		while (x < x2){
			if (d < 0){
				d = d + east_er;
				x = x + 1;
			}
			else{
				d = d + north_east_er;
				x = x + 1;
				y = y + 1;
			}
			if (IN_RANGE(x, y) && arr[y + m_WindowWidth * x] != color){
				z = LinePointDepth(p1, p2, x, y);
				if (z && (z > z_arr[y + m_WindowWidth * x] || (z_arr[y + m_WindowWidth * x] == NULL))){
					arr[y + m_WindowWidth * x] = color;
					z_arr[y + m_WindowWidth * x] = z;
				}
			}
			if (xy){
				(*x_y)[y].push_back(x);
			}
		}
	}
	else if (-1 < incline && incline <= 0){
		d = dx + 2 * dy; // try to move in positive x direction only, possibly negative y
		while (x < x2){
			if (d > 0){
				d = d + east_er;
				x = x + 1;
			}
			else{
				d = d + south_east_er;
				x = x + 1;
				y = y - 1;
			}
			if (IN_RANGE(x, y) && arr[y + m_WindowWidth * x] != color){
				z = LinePointDepth(p1, p2, x, y);
				if (z && (z > z_arr[y + m_WindowWidth * x] || (z_arr[y + m_WindowWidth * x] == NULL))){
					arr[y + m_WindowWidth * x] = color;
					z_arr[y + m_WindowWidth * x] = z;
				}
			}
			if (xy){
				(*x_y)[y].push_back(x);
			}
		}
	}
	else if (incline <= -1){ // condition unneccessary, exists to make conditions clear
		d = 2 * dx + dy; // try to move in negative y direction only
		while (y > y2){
			if (d < 0){
				d = d + south_er;
				y = y - 1;
			}
			else{
				d = d + south_east_er;
				x = x + 1;
				y = y - 1;
			}
			if (IN_RANGE(x, y) && arr[y + m_WindowWidth * x] != color){
				z = LinePointDepth(p1, p2, x, y);
				if (z && (z > z_arr[y + m_WindowWidth * x] || (z_arr[y + m_WindowWidth * x] == NULL))){
					arr[y + m_WindowWidth * x] = color;
					z_arr[y + m_WindowWidth * x] = z;
				}
			}
			if (xy){
				(*x_y)[y].push_back(x);
			}
		}
	}
	return;
}



void CCGWorkView::ScanConversion(int *z_arr,COLORREF *arr, polygon &p, mat4 cur_transform, COLORREF color){
	vec4 p1, p2;
	int min_y = m_WindowWidth - 1;
	int max_y = 1;
	std::vector<vec4> q;
	std::unordered_map<int, std::vector<int>> x_y = std::unordered_map<int, std::vector<int>>();
	for (unsigned int pnt = 0; pnt < p.points.size(); pnt++){
		p1 = p.points[pnt]* cur_transform;
		if (pnt + 1 == p.points.size()) p2 = p.points[0] * cur_transform;
		else p2 = p.points[pnt + 1] * cur_transform;
		q.push_back(p1);
		if (p1.y / p1.p < p2.y / p2.p){
			min_y = static_cast<int> (p1.y / p1.p < min_y ? p1.y / p1.p : min_y);
			max_y = static_cast<int> (p2.y / p2.p > max_y ? p2.y / p2.p : max_y);
		}
		else{
			min_y = static_cast<int>(p2.y / p1.p < min_y ? p2.y / p2.p : min_y);
			max_y = static_cast<int>(p1.y / p1.p > max_y ? p1.y / p1.p : max_y);
		}
		DrawLine(z_arr, arr, p1, p2, color, &x_y);
	}

	for (int y = min_y; y <= max_y; y++){
		if (!x_y[y].empty()){
			std::sort(x_y[y].begin(), x_y[y].end());
			//bool draw = true;
			for (unsigned int i = 0; i < x_y[y].size() -1; i++){
				for (int x = x_y[y][i]; x <= x_y[y][i + 1]; x++){
					if (IN_RANGE(x, y)){
						int z = Depth(q, x, y);
						if (z && (z > z_arr[y + m_WindowWidth * x] || (z_arr[y + m_WindowWidth * x] == NULL))){
							arr[y + m_WindowWidth * x] = color;
							z_arr[y + m_WindowWidth * x] = z;
						}
					}
				}
			}
		}
	}
}

void CCGWorkView::DrawBoundBox(int *z_arr, COLORREF *arr, model &model, mat4 cur_transform, COLORREF color){

	//mat4 cur_transform = model.obj_coord_trans * model.view_space_trans * m_screen_space_trans;

	double minx = model.min_vec.x;
	double miny = model.min_vec.y;
	double minz = model.min_vec.z;

	double maxx = model.max_vec.x;
	double maxy = model.max_vec.y;
	double maxz = model.max_vec.z;

	vec4 xmin_ymin_zmin(minx, miny, minz, 1.0);
	vec4 xmin_ymin_zmax(minx, miny, maxz, 1.0);

	vec4 xmin_ymax_zmin(minx, maxy, minz, 1.0);
	vec4 xmin_ymax_zmax(minx, maxy, maxz, 1.0);

	vec4 xmax_ymin_zmin(maxx, miny, minz, 1.0);
	vec4 xmax_ymin_zmax(maxx, miny, maxz, 1.0);

	vec4 xmax_ymax_zmin(maxx, maxy, minz, 1.0);
	vec4 xmax_ymax_zmax(maxx, maxy, maxz, 1.0);

	// zmin rectangle first
	DrawLine(z_arr, arr, xmin_ymin_zmin * cur_transform, xmin_ymax_zmin * cur_transform, color);
	DrawLine(z_arr, arr, xmin_ymax_zmin * cur_transform, xmax_ymax_zmin * cur_transform, color);
	DrawLine(z_arr, arr, xmax_ymax_zmin * cur_transform, xmax_ymin_zmin * cur_transform, color);
	DrawLine(z_arr, arr, xmax_ymin_zmin * cur_transform, xmin_ymin_zmin * cur_transform, color);

	// zmax rectangle second
	DrawLine(z_arr, arr, xmin_ymin_zmax * cur_transform, xmin_ymax_zmax * cur_transform, color);
	DrawLine(z_arr, arr, xmin_ymax_zmax * cur_transform, xmax_ymax_zmax * cur_transform, color);
	DrawLine(z_arr, arr, xmax_ymax_zmax * cur_transform, xmax_ymin_zmax * cur_transform, color);
	DrawLine(z_arr, arr, xmax_ymin_zmax * cur_transform, xmin_ymin_zmax * cur_transform, color);

	// connect the two rectangles next
	DrawLine(z_arr, arr, xmin_ymin_zmin * cur_transform, xmin_ymin_zmax * cur_transform, color);
	DrawLine(z_arr, arr, xmin_ymax_zmin * cur_transform, xmin_ymax_zmax * cur_transform, color);
	DrawLine(z_arr, arr, xmax_ymin_zmin * cur_transform, xmax_ymin_zmax * cur_transform, color);
	DrawLine(z_arr, arr, xmax_ymax_zmin * cur_transform, xmax_ymax_zmax * cur_transform, color);
}

void CCGWorkView::RenderScene() {
	
	std::fill_n(m_screen, m_WindowWidth * m_WindowHeight, m_background_color);
	std::fill_n(z_buffer, m_WindowWidth*m_WindowHeight, NULL);
	vec4 p1, p2;
	polygon cur_polygon;
	mat4 cur_transform;
	for (unsigned int m = 0; m < models.size(); m++){
		
		if (m_nView == ID_VIEW_ORTHOGRAPHIC){
			cur_transform = models[m].obj_coord_trans * models[m].camera_trans * models[m].view_space_trans * m_screen_space_trans;
		}
		else if (m_nView == ID_VIEW_PERSPECTIVE){
			cur_transform = models[m].obj_coord_trans * models[m].camera_trans * models[m].view_space_trans * m_prespective_trans * m_screen_space_trans;
		}
		if (polygon_normal == ID_POLYGON_GIVEN || polygon_normal == ID_POLYGON_CALCULATED){
			for (unsigned int count = 0; count < models[m].polygons.size(); count++){
				cur_polygon = models[m].polygons[count];
				p1 = cur_polygon.Normal(given_polygon_normal).p_a * cur_transform;
				p2 = cur_polygon.Normal(given_polygon_normal).p_b * cur_transform;
				DrawLine(z_buffer, m_screen, p1, p2, m_polygon_norm_color);
			}
		}

		if (vertex_normal == ID_VERTEX_GIVEN|| vertex_normal == ID_VERTEX_CALCULATED){
			std::vector<line> vertex_normal = models[m].Normal(given_vertex_normal);
			for (unsigned int count = 0; count < vertex_normal.size(); count++){
				p1 = vertex_normal[count].p_a * cur_transform;
				p2 = vertex_normal[count].p_b * cur_transform;
				DrawLine(z_buffer, m_screen, p1, p2, m_vertex_norm_color);
			}
		}
		/*for (unsigned int pnt = 0; pnt < models[m].points_list.size(); pnt++){
			p1 = (models[m].points_list[pnt].p_a)* cur_transform;
			p2 = (models[m].points_list[pnt].p_b)* cur_transform;
			DrawLine(m_screen, p1, p2, models[m].color);
		}*/
		for (unsigned int pol = 0; pol < models[m].polygons.size(); pol++){
			ScanConversion(z_buffer, m_screen, models[m].polygons[pol], cur_transform, models[m].color);
		}
		if (m_bound_box){
			DrawBoundBox(z_buffer, m_screen, models[m], cur_transform, m_boundbox_color);
		}
	}


	m_map = CreateBitmap(m_WindowWidth,		 // width
		m_WindowHeight,		 // height
		1,			 // Color Planes, unfortanutelly don't know what is it actually. Let it be 1
		8 * 4,		 // Size of memory for one pixel in bits (in win32 4 bytes = 4*8 bits)
		(void*)m_screen); // pointer to array

	SelectObject(m_hDC, m_map); // Inserting picture into our temp HDC

	// Copy image from temp HDC to window
	BitBlt(m_pDC->GetSafeHdc(), // Destination
		0,  // x and
		0,  // y - upper-left corner of place, where we'd like to copy
		m_WindowWidth, // width of the region
		m_WindowHeight, // height
		m_hDC, // source
		0,   // x and
		0,   // y of upper left corner  of part of the source, from where we'd like to copy
		SRCCOPY); // Defined DWORD to juct copy pixels. Watch more on msdn;

	DeleteObject(m_map);

	return;
}


void CCGWorkView::OnFileLoad() 
{
	TCHAR szFilters[] = _T ("IRIT Data Files (*.itd)|*.itd|All Files (*.*)|*.*||");

	CFileDialog dlg(TRUE, _T("itd"), _T("*.itd"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY ,szFilters);

	if (dlg.DoModal () == IDOK) {
		m_strItdFileName = dlg.GetPathName();		// Full path and filename
		PngWrapper p;
		CGSkelProcessIritDataFiles(m_strItdFileName, 1);

		Invalidate();	// force a WM_PAINT for drawing.
	} 

}

// VIEW HANDLERS ///////////////////////////////////////////

// Note: that all the following Message Handlers act in a similar way.
// Each control or command has two functions associated with it.

void CCGWorkView::OnViewOrthographic() 
{
	m_nView = ID_VIEW_ORTHOGRAPHIC;
	m_bIsPerspective = false;
	Invalidate();		// redraw using the new view.
}

void CCGWorkView::OnUpdateViewOrthographic(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nView == ID_VIEW_ORTHOGRAPHIC);
}

void CCGWorkView::OnViewPerspective() 
{
	m_nView = ID_VIEW_PERSPECTIVE;
	m_bIsPerspective = true;
	Invalidate();
}

void CCGWorkView::OnUpdateViewPerspective(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nView == ID_VIEW_PERSPECTIVE);
}

// ACTION HANDLERS ///////////////////////////////////////////

void CCGWorkView::OnActionRotate() 
{
	m_nAction = ID_ACTION_ROTATE;
}

void CCGWorkView::OnUpdateActionRotate(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_ROTATE);
}

void CCGWorkView::OnActionTranslate() 
{
	m_nAction = ID_ACTION_TRANSLATE;
}

void CCGWorkView::OnUpdateActionTranslate(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_TRANSLATE);
}

void CCGWorkView::OnActionScale() 
{
	m_nAction = ID_ACTION_SCALE;
}

void CCGWorkView::OnUpdateActionScale(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAction == ID_ACTION_SCALE);
}

void CCGWorkView::OnActionResetView()
{
	mat4 reset_transform;
	reset_transform[0][0] = 1;
	reset_transform[1][1] = 1;
	reset_transform[2][2] = 1;
	reset_transform[3][3] = 1;

	m_tarnsform = reset_transform;
	for (unsigned int m = 0; m < models.size(); m++){
		models[m].obj_coord_trans = reset_transform;
		models[m].camera_trans = reset_transform;
	}
	
	RenderScene();
}

// OBJ/CAMERA VIEW TOGGLE //////////////////////////////////

void CCGWorkView::OnActionToggleView()
{
	m_object_space_trans = !m_object_space_trans;
}



void CCGWorkView::OnUpdateActionToggleView(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_object_space_trans);
}

// AXIS HANDLERS ///////////////////////////////////////////


// Gets calles when the X button is pressed or when the Axis->X menu is selected.
// The only thing we do here is set the ChildView member variable m_nAxis to the 
// selected axis.
void CCGWorkView::OnAxisX() 
{
	m_nAxis = ID_AXIS_X;
}

// Gets called when windows has to repaint either the X button or the Axis pop up menu.
// The control is responsible for its redrawing.
// It sets itself disabled when the action is a Scale action.
// It sets itself Checked if the current axis is the X axis.
void CCGWorkView::OnUpdateAxisX(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_X);
}


void CCGWorkView::OnAxisY() 
{
	m_nAxis = ID_AXIS_Y;
}

void CCGWorkView::OnUpdateAxisY(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_Y);
}


void CCGWorkView::OnAxisZ() 
{
	m_nAxis = ID_AXIS_Z;
}

void CCGWorkView::OnUpdateAxisZ(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_Z);
}

void CCGWorkView::OnAxisXY()
{
	m_nAxis = ID_AXIS_XY;
}

void CCGWorkView::OnUpdateAxisXY(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nAxis == ID_AXIS_XY);
}



// OPTIONS HANDLERS ///////////////////////////////////////////

void CCGWorkView::OnWriteframeColor()
{
	ColorSelectionDialog dlg(m_color_wireframe, m_boundbox_color, m_background_color, m_vertex_norm_color, m_polygon_norm_color);
	if (dlg.DoModal() == IDOK){

		m_color_wireframe = RGB(GetBValue(dlg.wireframe_color), GetGValue(dlg.wireframe_color), GetRValue(dlg.wireframe_color));
		m_boundbox_color = RGB(GetBValue(dlg.boundbox_color), GetGValue(dlg.boundbox_color), GetRValue(dlg.boundbox_color));
		m_background_color = RGB(GetBValue(dlg.background_color), GetGValue(dlg.background_color), GetRValue(dlg.background_color));
		m_vertex_norm_color = RGB(GetBValue(dlg.vertex_norm_color), GetGValue(dlg.vertex_norm_color), GetRValue(dlg.vertex_norm_color));
		m_polygon_norm_color = RGB(GetBValue(dlg.polygon_norm_color), GetGValue(dlg.polygon_norm_color), GetRValue(dlg.polygon_norm_color));

		for (unsigned int m = 0; m < models.size(); m++){
			models[m].color = m_color_wireframe;
		}
		Invalidate();
	}
}

void CCGWorkView::OnBoundBox()
{
	m_bound_box = !m_bound_box;
	Invalidate();
}

void CCGWorkView::OnUpdateBoundBox(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_bound_box);
}

void CCGWorkView::OnOptionMouseSensetivity(){
	MouseSensetiveDialog dlg(m_mouse_sensetivity);
	if (dlg.DoModal() == IDOK){
		m_mouse_sensetivity = dlg.m_mouse_sensetivity;
	}
}

void CCGWorkView::OnOptionPrespectiveControl(){
	PrespectiveControlDialog dlg(m_presepctive_d);
	if (dlg.DoModal() == IDOK){
		m_presepctive_d = dlg.d;

		m_prespective_trans[0][0] = 1;
		m_prespective_trans[1][1] = 1;
		m_prespective_trans[2][2] = 1;
		m_prespective_trans[2][3] = 1 / m_presepctive_d;

		Invalidate();
	}
}

void CCGWorkView::OnOptionOthers(){

	CString models_list;
	std::vector<bool> active_models_list;
	for (unsigned int m = 0; m < models.size(); m++){
		models_list += models[m].model_name + "\n";
		active_models_list.push_back(models[m].active_model);
	}
	OtherOptionsDialog dlg(CGSkelFFCState.FineNess, models_list, active_models_list);
	if (dlg.DoModal() == IDOK){
		CGSkelFFCState.FineNess = dlg.finess;
		for (unsigned int m = 0; m < models.size(); m++){
			models[m].active_model = dlg.active_modules[m];
		}
	}
}
// LIGHT SHADING HANDLERS ///////////////////////////////////////////

void CCGWorkView::OnLightShadingFlat() 
{
	m_nLightShading = ID_LIGHT_SHADING_FLAT;
}

void CCGWorkView::OnUpdateLightShadingFlat(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nLightShading == ID_LIGHT_SHADING_FLAT);
}


void CCGWorkView::OnLightShadingGouraud() 
{
	m_nLightShading = ID_LIGHT_SHADING_GOURAUD;
}

void CCGWorkView::OnUpdateLightShadingGouraud(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_nLightShading == ID_LIGHT_SHADING_GOURAUD);
}

// LIGHT SETUP HANDLER ///////////////////////////////////////////

void CCGWorkView::OnLightConstants() 
{
	CLightDialog dlg;

	for (int id=LIGHT_ID_1;id<MAX_LIGHT;id++)
	{	    
	    dlg.SetDialogData((LightID)id,m_lights[id]);
	}
	dlg.SetDialogData(LIGHT_ID_AMBIENT,m_ambientLight);

	if (dlg.DoModal() == IDOK) 
	{
	    for (int id=LIGHT_ID_1;id<MAX_LIGHT;id++)
	    {
		m_lights[id] = dlg.GetDialogData((LightID)id);
	    }
	    m_ambientLight = dlg.GetDialogData(LIGHT_ID_AMBIENT);
	}	
	Invalidate();
}

void CCGWorkView::OnPolygonGiven()
{
	// TODO: Add your command handler code here
	if (polygon_normal == ID_POLYGON_GIVEN)
		polygon_normal = NULL;
	else
		polygon_normal = ID_POLYGON_GIVEN;
	given_polygon_normal = true;
	Invalidate();
}


void CCGWorkView::OnPolygonCalculated()
{
	// TODO: Add your command handler code here
	if (polygon_normal == ID_POLYGON_CALCULATED)
		polygon_normal = NULL;
	else
		polygon_normal = ID_POLYGON_CALCULATED;
	given_polygon_normal = false;
	Invalidate();
}


void CCGWorkView::OnUpdatePolygonCalculated(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(polygon_normal == ID_POLYGON_CALCULATED);
}


void CCGWorkView::OnUpdatePolygonGiven(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(polygon_normal == ID_POLYGON_GIVEN);
}


void CCGWorkView::OnVertexGiven()
{
	// TODO: Add your command handler code here
	if (vertex_normal == ID_VERTEX_GIVEN)
		vertex_normal = NULL;
	else
		vertex_normal = ID_VERTEX_GIVEN;
	given_vertex_normal = true;
	Invalidate();
}


void CCGWorkView::OnUpdateVertexGiven(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(vertex_normal == ID_VERTEX_GIVEN);

}


void CCGWorkView::OnVertexCalculated()
{
	// TODO: Add your command handler code here
	if (vertex_normal == ID_VERTEX_CALCULATED)
		vertex_normal = NULL;
	else
		vertex_normal = ID_VERTEX_CALCULATED;
	given_vertex_normal = false;
	Invalidate();
}


void CCGWorkView::OnUpdateVertexCalculated(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(vertex_normal == ID_VERTEX_CALCULATED);
}
