// CGWorkView.cpp : implementation of the CCGWorkView class
//
#include "stdafx.h"
#include "CGWork.h"
#include <limits>
#include "CGWorkDoc.h"
#include "CGWorkView.h"

#include <iostream>
#include <functional>
#include <string>
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
#include "FileRenderDlg.h"
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

#define IN_RANGE(x, y) ((1 <= x) && (x < (m_WindowWidth - 1)) && (1 <= y) && (y < (m_WindowHeight - 1)))
#define SCREEN_SPACE(x, y) (x + m_WindowWidth * y)
#define BGR(x) RGB(GetBValue(x), GetGValue(x), GetRValue(x));
#define SET_RGB(r,g,b) ((r)<<24|(g)<<16|(b)<<8|0)
#define SET_RGBA(r,g,b,a) ((r)<<24|(g)<<16|(b)<<8|a)
#define GET_R(x) (((x)&0xff000000)>>24)
#define GET_G(x) (((x)&0x00ff0000)>>16)
#define GET_B(x) (((x)&0x0000ff00)>>8)
#define GET_A(x) ((x)&0x000000ff)

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
	ON_COMMAND(ID_ACTION_CLEARALL, OnActionClearAll)
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
	ON_COMMAND(ID_VIEW_WIREFRAME, &CCGWorkView::OnViewWireframe)
	ON_UPDATE_COMMAND_UI(ID_VIEW_WIREFRAME, &CCGWorkView::OnUpdateViewWireframe)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SOLID, &CCGWorkView::OnUpdateViewSolid)
	ON_COMMAND(ID_VIEW_SOLID, &CCGWorkView::OnViewSolid)
	ON_COMMAND(ID_VIEW_Z, &CCGWorkView::OnViewZ)
	ON_UPDATE_COMMAND_UI(ID_VIEW_Z, &CCGWorkView::OnUpdateViewZ)
	ON_COMMAND(ID_LIGHT_SHADING_PHONg, &CCGWorkView::OnLightShadingPhong)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_SHADING_PHONg, &CCGWorkView::OnUpdateLightShadingPhong)
	ON_COMMAND(ID_RENDER_TOFILE, &CCGWorkView::OnRenderTofile)
	ON_UPDATE_COMMAND_UI(ID_RENDER_TOFILE, &CCGWorkView::OnUpdateRenderTofile)
	ON_UPDATE_COMMAND_UI(ID_RENDER_TOSCREEN, &CCGWorkView::OnUpdateRenderToscreen)
	ON_COMMAND(ID_RENDER_TOSCREEN, &CCGWorkView::OnRenderToscreen)
	ON_COMMAND(ID_BACKGROUND_SETIMAGE, &CCGWorkView::OnBackgroundSetimage)
	ON_COMMAND(ID_BACKGROUND_ACTIVE, &CCGWorkView::OnBackgroundActive)
	ON_UPDATE_COMMAND_UI(ID_BACKGROUND_ACTIVE, &CCGWorkView::OnUpdateBackgroundActive)
	ON_COMMAND(ID_BACKGROUND_REPEAT, &CCGWorkView::OnBackgroundRepeat)
	ON_UPDATE_COMMAND_UI(ID_BACKGROUND_REPEAT, &CCGWorkView::OnUpdateBackgroundRepeat)
	ON_COMMAND(ID_BACKGROUND_STRETCH, &CCGWorkView::OnBackgroundStretch)
	ON_UPDATE_COMMAND_UI(ID_BACKGROUND_STRETCH, &CCGWorkView::OnUpdateBackgroundStretch)
	ON_COMMAND(ID_OPTIONS_BACKFACECULLING, &CCGWorkView::OnOptionsBackfaceculling)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_BACKFACECULLING, &CCGWorkView::OnUpdateOptionsBackfaceculling)
	ON_COMMAND(ID_OPTIONS_NORMALINVERSE, &CCGWorkView::OnOptionsNormalinverse)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_NORMALINVERSE, &CCGWorkView::OnUpdateOptionsNormalinverse)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_OVERRIDEGIVENNORMAL, &CCGWorkView::OnUpdateOptionsOverridegivennormal)
	ON_COMMAND(ID_OPTIONS_OVERRIDEGIVENNORMAL, &CCGWorkView::OnOptionsOverridegivennormal)
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

	m_presepctive_d = 2;
	m_pic_name = "Default Name.png";
	m_pngHandle.SetFileName(m_pic_name.c_str());

	m_active_background = false;
	m_valid_background = false;

	// initilize the cameras' position and direction in the view space
	// used later
	m_camera_pos = vec4(0, 0, -1, 1);
	m_camera_at = vec4(0, 0, 0, 1);
	m_camera_up = vec4(0, -1, 0, 1);

	m_color_wireframe = RGB(0, 0, 0);
	m_background_color = RGB(255, 255, 255);
	m_boundbox_color = RGB(0, 0, 0);
	m_polygon_norm_color = RGB(0, 0, 0);
	m_vertex_norm_color = RGB(0, 0, 0);

	m_nLightShading = ID_LIGHT_SHADING_FLAT;
	render_type = ID_VIEW_SOLID;
	m_render_target = ID_RENDER_TOSCREEN;

	m_background_type = ID_BACKGROUND_REPEAT;
	//init the coesffiecents of the lights
	m_ambient_k = 0.3;
	m_diffuse_k = 0.7;
	m_speculr_k = 0.7;

	m_lMaterialAmbient = 0.2;
	m_lMaterialDiffuse = 0.8;
	m_lMaterialSpecular = 1.0;
	m_nMaterialCosineFactor = 32;

	for (int l = LIGHT_ID_1; l < MAX_LIGHT; l++){
		m_lights[LIGHT_ID_1].enabled = false;
	}

	//init the first light to be enabled
	m_lights[LIGHT_ID_1].enabled = true;
	m_lights[LIGHT_ID_1].type = LIGHT_TYPE_POINT;
	m_lights[LIGHT_ID_1].posX = -4000;
	m_lights[LIGHT_ID_1].posY = -4000;
	m_lights[LIGHT_ID_1].posZ = -04000;

	m_ambientLight.colorR = 30;
	m_ambientLight.colorG = 30;
	m_ambientLight.colorB = 30;

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

	if (NULL == m_pDC) { // failure to get DC
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

	if (0 >= cx || 0 >= cy) {
		return;
	}

	// save the width and height of the current window
	m_WindowWidth = cx;
	m_WindowHeight = cy;

	m_OriginalWindowWidth = m_WindowWidth;
	m_OriginalWindowHeight = m_WindowHeight;

	// compute the aspect ratio
	// this will keep all dimension scales equal
	m_AspectRatio = (GLdouble)m_WindowWidth / (GLdouble)m_WindowHeight;
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
	z_buffer = (double*)calloc(m_WindowWidth * m_WindowHeight, sizeof(double));

	mat4 screen_space_scale;
	mat4 screen_space_translate;

	// set the screen scaling transformation
	int min_axis = min(m_WindowHeight, m_WindowWidth);
	screen_space_scale[0][0] = (double)min_axis*0.6;
	screen_space_scale[1][1] = (double)min_axis*0.6;
	screen_space_scale[2][2] = (double)min_axis*0.6;
	screen_space_scale[3][3] = 1;

	m_screen_space_scale = screen_space_scale;

	// set the screen translation transformation
	screen_space_translate[0][0] = 1;
	screen_space_translate[3][0] = 0.5 * m_WindowWidth;

	screen_space_translate[1][1] = 1;
	screen_space_translate[3][1] = 0.5 * m_WindowHeight;

	screen_space_translate[2][2] = 1;

	screen_space_translate[3][3] = 1;

	m_screen_space_translate = screen_space_translate;


	// set the screens prespective transformation
	mat4 reset;
	m_prespective_trans = reset;

	double screen_space_d = m_presepctive_d * (double)min_axis*0.6;
	double screen_space_alpha = 0.6 * screen_space_d;

	m_prespective_trans[0][0] = 1;
	m_prespective_trans[1][1] = 1;
	m_prespective_trans[2][2] = screen_space_d / (screen_space_d - screen_space_alpha);
	m_prespective_trans[2][3] = 1 / screen_space_d;
	m_prespective_trans[3][2] = -screen_space_alpha * screen_space_d / (screen_space_d - screen_space_alpha);

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
	if (m_pDC) {
		delete m_pDC;
	}
}

/////////////////////////////////////////////////////////////////////////////
// User Defined Functions

bool past_pressed;
LRESULT CCGWorkView::OnMouseMovement(WPARAM wparam, LPARAM lparam){
	int xPos = GET_X_LPARAM(lparam);
	int yPos = GET_Y_LPARAM(lparam);
	
	if (wparam == MK_RBUTTON){
		std::string str(("X - " + std::to_string(xPos) + "\n" + "Y - " + std::to_string(yPos)));
		CString notice(str.c_str());
		this->MessageBox(notice);
	}

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

				temp_transform[1][1] = cosy;
				temp_transform[1][2] = -siny;

				temp_transform[2][1] = siny;
				temp_transform[2][2] = cosy;

				temp_transform[3][3] = 1;

			}
			if (m_nAxis == ID_AXIS_Y){
				temp_transform[0][0] = cosx;
				temp_transform[0][2] = -sinx;

				temp_transform[1][1] = 1;

				temp_transform[2][0] = sinx;
				temp_transform[2][2] = cosx;

				temp_transform[3][3] = 1;
			}
			if (m_nAxis == ID_AXIS_Z){
				temp_transform[0][0] = cosx;
				temp_transform[0][1] = -sinx;

				temp_transform[1][0] = sinx;
				temp_transform[1][1] = cosx;

				temp_transform[2][2] = 1;

				temp_transform[3][3] = 1;
			}
			if (m_nAxis == ID_AXIS_XY){
				temp_transform[0][0] = 1;

				temp_transform[1][1] = cosy;
				temp_transform[1][2] = siny;

				temp_transform[2][1] = -siny;
				temp_transform[2][2] = cosy;

				temp_transform[3][3] = 1;

				temp_transform_xy[0][0] = cosx;
				temp_transform_xy[0][2] = sinx;

				temp_transform_xy[1][1] = 1;

				temp_transform_xy[2][0] = -sinx;
				temp_transform_xy[2][2] = cosx;

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

static double Depth(std::vector<vec4> q, int x, int y){
	vec4 p1, p2;
	std::vector<vec4> points;
	unsigned int i;

	if (x == static_cast<int>(p1.x / p1.p) && y == static_cast<int>(p1.y / p1.p)) return p1.z / p1.p;
	if (x == static_cast<int>(p2.x / p2.p) && y == static_cast<int>(p2.y / p2.p)) return p2.z / p2.p;

	for (i = 0; i < q.size(); i++){
		p1 = q[i];
		p2 = q[(i + 1) % q.size()];
		if (y == static_cast<int>(p1.y / p1.p)) points.push_back(vec4(p1.x / p1.p, y, p1.z / p1.p, 1));
		if (y == static_cast<int>(p2.y / p2.p)) points.push_back(vec4(p2.x / p2.p, y, p2.z / p2.p, 1));
		double y1 = max(p1.y / p1.p, p2.y / p2.p);
		double y2 = min(p1.y / p1.p, p2.y / p2.p);
		if (y < y1 && y > y2){
			if (x == static_cast<int> (p1.x / p1.p))
				return p1.z / p1.p;
			if (x == static_cast<int>(p2.x / p1.p))
				return p2.z / p2.p;
			double m = (p1.x / p1.p - p2.x / p2.p) / (p1.y / p1.p - p2.y / p2.p);
			double x1 = m*y - m*(p1.y / p1.p) + (p1.x / p1.p);
			double d1 = sqrt(pow(p1.y / p1.p - y, 2) + pow(p1.x / p1.p - x1, 2));
			double d = sqrt(pow(p1.y / p1.p - p2.y / p2.p, 2) + pow(p1.x / p1.p - p2.x / p2.p, 2));
			double z = (p2.z / p2.p)*(d1 / d) + (1 - d1 / d)*(p1.z / p1.p);
			//double z = LinePointDepth(p1, p2, x, y);
			if (x == static_cast<int>(x1))
				return z;
			points.push_back(vec4(x1, y, z, 1));
		}
	}

	for (i = 0; i < points.size(); i++){
		p1 = points[i];
		p2 = points[(i + 1) % points.size()];
		double x1 = max(p1.x, p2.x);
		double x2 = min(p1.x, p2.x);
		if (x1 == x2) return p1.z / p1.p;
		if (x < static_cast<int>(x1) && x > static_cast<int>(x2)){
			double d1 = p1.x -x;
			double d = p1.x - p2.x;
			double z = p1.z*(1- (d1 / d)) + (d1 / d)*p2.z;
			return z;
		}
	}

	return p2.z; //cheating 
}

static double LinePointDepth(vec4 &p1, vec4 &p2, int x, int y){

	double p2_x = static_cast<int>(p2.x / p2.p);
	double p2_y = static_cast<int>(p2.y / p2.p);
	double p2_z = (p2.z / p2.p);

	double p1_x = static_cast<int>(p1.x / p1.p);
	double p1_y = static_cast<int>(p1.y / p1.p);
	double p1_z = (p1.z / p1.p);

	double z_delta = p1_z - p2_z;
	double y_delta = p1_y - p2_y;
	double x_delta = p1_x - p2_x;
	double z;

	if (y_delta != 0) // line does not change in y
		z = (z_delta / y_delta) * (y - p2_y) + p2_z;
	else if (x_delta != 0) // line does not change in x
		z = (z_delta / x_delta) * (x - p2_x) + p2_z;
	else // line does not change in y and x, meaning we only draw p1 on screen
		z = max(p1_z, p2_z);

	return z;
	
}

static vec4 LinePointNormal(vec4 &p1, vec4 &p2, vec4 p1_normal, vec4 p2_normal, int x, int y){

	double p2_x = static_cast<int>(p2.x / p2.p);
	double p2_y = static_cast<int>(p2.y / p2.p);
	double p2_z = (p2.z / p2.p);

	double p1_x = static_cast<int>(p1.x / p1.p);
	double p1_y = static_cast<int>(p1.y / p1.p);
	double p1_z = (p1.z / p1.p);

	vec4 n_delta = p1_normal - p2_normal;
	double y_delta = p1_y - p2_y;
	double x_delta = p1_x - p2_x;
	vec4 n;

	if (y_delta != 0) // line does not change in y
		n = (n_delta / y_delta) * (y - p2_y) + p2_normal;
	else if (x_delta != 0) // line does not change in x
		n = (n_delta / x_delta) * (x - p2_x) + p2_normal;
	else // line does not change in y and x, meaning we only draw p1 on screen
		n = p1_normal;

	return n;

}

static COLORREF LinePointLight(vec4 &p1, vec4 &p2, COLORREF p1_color, COLORREF p2_color, int x, int y){

	double p2_x = static_cast<int>(p2.x / p2.p);
	double p2_y = static_cast<int>(p2.y / p2.p);

	double p1_x = static_cast<int>(p1.x / p1.p);
	double p1_y = static_cast<int>(p1.y / p1.p);

	double c_red_delta = GetRValue(p1_color) - GetRValue(p2_color);
	double c_grn_delta = GetGValue(p1_color) - GetGValue(p2_color);
	double c_blu_delta = GetBValue(p1_color) - GetBValue(p2_color);
	double y_delta = p1_y - p2_y;
	double x_delta = p1_x - p2_x;
	double cred;
	double cgrn;
	double cblu;
	COLORREF c;

	if (y_delta != 0){ // line does not change in y
		cred = (c_red_delta / y_delta) * (y - p2_y) + GetRValue(p2_color);
		cgrn = (c_grn_delta / y_delta) * (y - p2_y) + GetGValue(p2_color);
		cblu = (c_blu_delta / y_delta) * (y - p2_y) + GetBValue(p2_color);
		c = RGB(static_cast<int>(min(cred, 255)),
			static_cast<int>(min(cgrn, 255)),
			static_cast<int>(min(cblu, 255)));
	}
	else if (x_delta != 0){ // line does not change in x
		cred = (c_red_delta / x_delta) * (x - p2_x) + GetRValue(p2_color);
		cgrn = (c_grn_delta / x_delta) * (x - p2_x) + GetGValue(p2_color);
		cblu = (c_blu_delta / x_delta) * (x - p2_x) + GetBValue(p2_color);
		c = RGB(static_cast<int>(min(cred, 255)),
			static_cast<int>(min(cgrn, 255)),
			static_cast<int>(min(cblu, 255)));
	}
	else // line does not change in y and x, meaning we only draw p1 on screen
		c = max(p1_color, p2_color);

	return c;

}

void CCGWorkView::DrawLine(double* z_arr, COLORREF *arr, vec4 &p1, vec4 &p2, COLORREF p1_color, vec4* p1_normal, COLORREF p2_color, vec4* p2_normal, std::unordered_map<int, std::vector<x_z_c_n_point>>* x_y){

	// if the line is beyond the screen space, dont bother drawing it
	if (!(((p1.z > m_presepctive_d && p2.z > m_presepctive_d) && !(p1.x <= 0 && p2.x <= 0) && !(p1.y <= 0 && p2.y <= 0))
		&& (m_nView == ID_VIEW_PERSPECTIVE) ||
		(m_nView == ID_VIEW_ORTHOGRAPHIC)))
		return;
	int xy = (x_y != NULL);
	// algorithm vars
	int x1, x2, y1, y2, dx, dy, d;
	int north_er, north_west_er, west_er, south_west_er, south_er, south_east_er, east_er, north_east_er;

	// draw location variables
	int x, y;

	// midpoint algorithm
	double p1_x = p1.x / p1.p;
	double p2_x = p2.x / p2.p;
	double p1_y = p1.y / p1.p;
	double p2_y = p2.y / p2.p;
	double p1_z = p1.z / p1.p;
	double p2_z = p2.z / p2.p;

	x1 = static_cast<int>(min(p1_x, p2_x));
	x2 = static_cast<int>(max(p1_x, p2_x));

	if (x1 != x2){
		y1 = static_cast<int>(p1_x < p2_x ? p1_y : p2_y);
		y2 = static_cast<int>(p1_x < p2_x ? p2_y : p1_y);
	}
	else{
		y1 = static_cast<int>(min(p1_y, p2_y));
		y2 = static_cast<int>(max(p1_y, p2_y));
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

	double z;
	x_z_c_n_point xzcn_point;
	COLORREF c;
	vec4 n = *p1_normal;

	z = LinePointDepth(p1, p2, x, y);
	if (m_nLightShading == ID_LIGHT_SHADING_PHONg) n = LinePointNormal(p1, p2, *p1_normal, *p2_normal, x, y);
	if (m_nLightShading == ID_LIGHT_SHADING_GOURAUD) c = LinePointLight(p1, p2, p1_color, p2_color ,x, y);
	else c = ApplyLight(p1_color, n, vec4(x, y, z, 1));
	if (xy) {
		xzcn_point.x = x;
		xzcn_point.z = z;
		xzcn_point.c = c;
		xzcn_point.n = n;
		(*x_y)[y].push_back(xzcn_point);
	}
	if (IN_RANGE(x, y)){
		if (z < z_arr[SCREEN_SPACE(x, y)]){
			arr[SCREEN_SPACE(x, y)] = c;
			z_arr[SCREEN_SPACE(x, y)] = z;
			if (m_render_target == ID_RENDER_TOFILE){
				//COLORREF png_c = BGR(c);
				//m_pngHandle.SetValue(x, y, png_c);
				m_pngHandle.SetValue(x, y, c);
			}
		}		
	}
	// select the correct midpoint algorithm (direction and incline)
	if (dx == 0){ // horizontal y line or line in z direction only
		//move in positive y direction only

		while (y < y2){
			y = y + 1;
			z = LinePointDepth(p1, p2, x, y);
			if (m_nLightShading == ID_LIGHT_SHADING_PHONg) n = LinePointNormal(p1, p2, *p1_normal, *p2_normal, x, y);
			if (m_nLightShading == ID_LIGHT_SHADING_GOURAUD) c = LinePointLight(p1, p2, p1_color, p2_color, x, y);
			else c = ApplyLight(p1_color, n, vec4(x, y, z, 1));
			if (xy) {
				xzcn_point.x = x;
				xzcn_point.z = z;
				xzcn_point.c = c;
				xzcn_point.n = n;
				(*x_y)[y].push_back(xzcn_point);
			}
			if (IN_RANGE(x, y)){
				if (z < z_arr[SCREEN_SPACE(x, y)]){
					arr[SCREEN_SPACE(x, y)] = c;
					z_arr[SCREEN_SPACE(x, y)] = z;
					if (m_render_target == ID_RENDER_TOFILE){
						//COLORREF png_c = BGR(c);
						//m_pngHandle.SetValue(x, y, png_c);
						m_pngHandle.SetValue(x, y, c);
					}
				}
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
			z = LinePointDepth(p1, p2, x, y);
			if (m_nLightShading == ID_LIGHT_SHADING_PHONg) n = LinePointNormal(p1, p2, *p1_normal, *p2_normal, x, y);
			if (m_nLightShading == ID_LIGHT_SHADING_GOURAUD) c = LinePointLight(p1, p2, p1_color, p2_color, x, y);
			else c = ApplyLight(p1_color, n, vec4(x, y, z, 1));
			if (xy) {
				xzcn_point.x = x;
				xzcn_point.z = z;
				xzcn_point.c = c;
				xzcn_point.n = n;
				(*x_y)[y].push_back(xzcn_point);
			}
			if (IN_RANGE(x, y)){
				if (z < z_arr[SCREEN_SPACE(x, y)]){
					arr[SCREEN_SPACE(x, y)] = c;
					z_arr[SCREEN_SPACE(x, y)] = z;
					if (m_render_target == ID_RENDER_TOFILE){
						//COLORREF png_c = BGR(c);
						//m_pngHandle.SetValue(x, y, png_c);
						m_pngHandle.SetValue(x, y, c);
					}
				}
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
			z = LinePointDepth(p1, p2, x, y);
			if (m_nLightShading == ID_LIGHT_SHADING_PHONg) n = LinePointNormal(p1, p2, *p1_normal, *p2_normal, x, y);
			if (m_nLightShading == ID_LIGHT_SHADING_GOURAUD) c = LinePointLight(p1, p2, p1_color, p2_color, x, y);
			else c = ApplyLight(p1_color, n, vec4(x, y, z, 1));
			if (xy) {
				xzcn_point.x = x;
				xzcn_point.z = z;
				xzcn_point.c = c;
				xzcn_point.n = n;
				(*x_y)[y].push_back(xzcn_point);
			}
			if (IN_RANGE(x, y)){
				if (z < z_arr[SCREEN_SPACE(x, y)]){
					arr[SCREEN_SPACE(x, y)] = c;
					z_arr[SCREEN_SPACE(x, y)] = z;
					if (m_render_target == ID_RENDER_TOFILE){
						//COLORREF png_c = BGR(c);
						//m_pngHandle.SetValue(x, y, png_c);
						m_pngHandle.SetValue(x, y, c);
					}
				}
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
			z = LinePointDepth(p1, p2, x, y);
			if (m_nLightShading == ID_LIGHT_SHADING_PHONg) n = LinePointNormal(p1, p2, *p1_normal, *p2_normal, x, y);
			if (m_nLightShading == ID_LIGHT_SHADING_GOURAUD) c = LinePointLight(p1, p2, p1_color, p2_color, x, y);
			else c = ApplyLight(p1_color, n, vec4(x, y, z, 1));
			if (xy && (incline != 0 || x == x2)) {
				xzcn_point.x = x;
				xzcn_point.z = z;
				xzcn_point.c = c;
				xzcn_point.n = n;
				(*x_y)[y].push_back(xzcn_point);
			}
			if (IN_RANGE(x, y)){
				if (z < z_arr[SCREEN_SPACE(x, y)]){
					arr[SCREEN_SPACE(x, y)] = c;
					z_arr[SCREEN_SPACE(x, y)] = z;
					if (m_render_target == ID_RENDER_TOFILE){
						//COLORREF png_c = BGR(c);
						//m_pngHandle.SetValue(x, y, png_c);
						m_pngHandle.SetValue(x, y, c);
					}
				}
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
			z = LinePointDepth(p1, p2, x, y);
			z = LinePointDepth(p1, p2, x, y);
			if (m_nLightShading == ID_LIGHT_SHADING_PHONg) n = LinePointNormal(p1, p2, *p1_normal, *p2_normal, x, y);
			if (m_nLightShading == ID_LIGHT_SHADING_GOURAUD) c = LinePointLight(p1, p2, p1_color, p2_color, x, y);
			else c = ApplyLight(p1_color, n, vec4(x, y, z, 1));
			if (xy) {
				xzcn_point.x = x;
				xzcn_point.z = z;
				xzcn_point.c = c;
				xzcn_point.n = n;
				(*x_y)[y].push_back(xzcn_point);
			}
			if (IN_RANGE(x, y)){
				if (z < z_arr[SCREEN_SPACE(x, y)]){
					arr[SCREEN_SPACE(x, y)] = c;
					z_arr[SCREEN_SPACE(x, y)] = z;
					if (m_render_target == ID_RENDER_TOFILE){
						//COLORREF png_c = BGR(c);
						//m_pngHandle.SetValue(x, y, png_c);
						m_pngHandle.SetValue(x, y, c);
					}
				}
			}
		}
	}
	return;
}

void CCGWorkView::ScanConversion(double *z_arr, COLORREF *arr, polygon &p, mat4 no_presp_trans, mat4 cur_transform, COLORREF color){
	vec4 p1, p2;
	//std::vector<vec4> q;
	std::unordered_map<int, std::vector<x_z_c_n_point>> x_y;

	////////////////////////////////////////////
	// create the normal of this polygon
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	line p1_normal_line;
	line p2_normal_line;
	vec4 p1_normal;
	vec4 p2_normal;
	if (m_nLightShading == ID_LIGHT_SHADING_FLAT){
		p2_normal_line = p1_normal_line = p.Normal(!m_override_normals);
		vec4 normal_pa = p1_normal_line.p_a * cur_transform;
		vec4 normal_pb = p1_normal_line.p_b * cur_transform;
		p1_normal = (normal_pa / normal_pa.p) - (normal_pb / normal_pb.p);
	}

	////////////////////////////////////////////
	// "draw" the lines on screen and save where the x's of each y row are
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	for (unsigned int pnt = 0; pnt < p.points.size(); pnt++){
		p1 = p.points[pnt];
		if (pnt + 1 == p.points.size()) p2 = p.points[0];
		else p2 = p.points[pnt + 1];
		COLORREF c1 = color;
		COLORREF c2 = color;
		if (m_nLightShading == ID_LIGHT_SHADING_GOURAUD || m_nLightShading == ID_LIGHT_SHADING_PHONg){
			p1_normal_line = p.VertexNormal(!m_override_normals)[p1];
			p2_normal_line = p.VertexNormal(!m_override_normals)[p2];
			vec4 p1_normal_pa = p1_normal_line.p_a * cur_transform;
			vec4 p1_normal_pb = p1_normal_line.p_b * cur_transform;
			vec4 p2_normal_pa = p2_normal_line.p_a * cur_transform;
			vec4 p2_normal_pb = p2_normal_line.p_b * cur_transform;
			p1_normal = (p1_normal_pa / p1_normal_pa.p) - (p1_normal_pb / p1_normal_pb.p);
			p2_normal = (p2_normal_pa / p2_normal_pa.p) - (p2_normal_pb / p2_normal_pb.p);
		}
		p1 = p1 * cur_transform;
		p2 = p2 * cur_transform;
		if (m_nLightShading == ID_LIGHT_SHADING_GOURAUD){
			c1 = ApplyLight(color, p1_normal, p1 / p1.p);
			c2 = ApplyLight(color, p2_normal, p2 / p2.p);
		}
		//q.push_back(p1 / p1.p);

		// skip polygons behind the camera
		if (!(((p1.z > m_presepctive_d && p2.z > m_presepctive_d) && !(p1.x <= 0 && p2.x <= 0) && !(p1.y <= 0 && p2.y <= 0))
			&& (m_nView == ID_VIEW_PERSPECTIVE) ||
			(m_nView == ID_VIEW_ORTHOGRAPHIC)))
			return;
		if (m_back_face_culling && p1_normal.z > 0) return;
		DrawLine(z_arr, arr, p1, p2, c1, &p1_normal, c2, &p2_normal, &x_y);
	}
	
	////////////////////////////////////////////
	// z_buffer drawing
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	vec4 scan_p1, scan_p2 ,n1 ,n2;
	COLORREF c1, c2;
	double z;
	for (auto iter = x_y.begin(); iter != x_y.end(); ++iter){
		std::sort(iter->second.begin(), iter->second.end());
		int y = iter->first;
		bool draw = true;
		for (unsigned int i = 0; i <= iter->second.size() - 2; i++){
			if (iter->second.size() > 1){
				scan_p1 = vec4(iter->second[i].x, y, iter->second[i].z, 1);
				scan_p2 = vec4(iter->second[i +1].x, y, iter->second[i + 1].z, 1);
				c1 = iter->second[i].c;
				c2 = iter->second[i + 1].c;
				n1 = iter->second[i].n;
				n2 = iter->second[i + 1].n;
				for (int x = static_cast<int>(iter->second[i].x); x <= iter->second[i + 1].x; x++){
					if (IN_RANGE(x, y) && draw){
						z = LinePointDepth(scan_p1, scan_p2, x, y);
						if (z < z_arr[SCREEN_SPACE(x, y)]){
							vec4 n = p1_normal;
							if (m_nLightShading == ID_LIGHT_SHADING_PHONg) n = LinePointNormal(scan_p1, scan_p2, n1, n2, x, y);
							if (m_nLightShading == ID_LIGHT_SHADING_GOURAUD) arr[SCREEN_SPACE(x, y)] = LinePointLight(scan_p1, scan_p2, c1, c2, x, y);
							else arr[SCREEN_SPACE(x, y)] = ApplyLight(color, n, vec4(x, y, z, 1));
							z_arr[SCREEN_SPACE(x, y)] = z;

							if (m_render_target == ID_RENDER_TOFILE){
								//COLORREF png_c = BGR(arr[SCREEN_SPACE(x, y)]);
								//m_pngHandle.SetValue(x, y, png_c);
								m_pngHandle.SetValue(x, y, arr[SCREEN_SPACE(x, y)]);
							}
						}
					}
				}
			}
		}
		draw = !draw;
	}
}

void CCGWorkView::DrawBoundBox(double *z_arr, COLORREF *arr, model &model, mat4 cur_transform, COLORREF color){

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

	vec4 psudo_normal = vec4(0, 0, 0, 1);

	// zmin rectangle first
	DrawLine(z_arr, arr, xmin_ymin_zmin * cur_transform, xmin_ymax_zmin * cur_transform, color, &psudo_normal);
	DrawLine(z_arr, arr, xmin_ymax_zmin * cur_transform, xmax_ymax_zmin * cur_transform, color, &psudo_normal);
	DrawLine(z_arr, arr, xmax_ymax_zmin * cur_transform, xmax_ymin_zmin * cur_transform, color, &psudo_normal);
	DrawLine(z_arr, arr, xmax_ymin_zmin * cur_transform, xmin_ymin_zmin * cur_transform, color, &psudo_normal);

	// zmax rectangle second
	DrawLine(z_arr, arr, xmin_ymin_zmax * cur_transform, xmin_ymax_zmax * cur_transform, color, &psudo_normal);
	DrawLine(z_arr, arr, xmin_ymax_zmax * cur_transform, xmax_ymax_zmax * cur_transform, color, &psudo_normal);
	DrawLine(z_arr, arr, xmax_ymax_zmax * cur_transform, xmax_ymin_zmax * cur_transform, color, &psudo_normal);
	DrawLine(z_arr, arr, xmax_ymin_zmax * cur_transform, xmin_ymin_zmax * cur_transform, color, &psudo_normal);

	// connect the two rectangles next
	DrawLine(z_arr, arr, xmin_ymin_zmin * cur_transform, xmin_ymin_zmax * cur_transform, color, &psudo_normal);
	DrawLine(z_arr, arr, xmin_ymax_zmin * cur_transform, xmin_ymax_zmax * cur_transform, color, &psudo_normal);
	DrawLine(z_arr, arr, xmax_ymin_zmin * cur_transform, xmax_ymin_zmax * cur_transform, color, &psudo_normal);
	DrawLine(z_arr, arr, xmax_ymax_zmin * cur_transform, xmax_ymax_zmax * cur_transform, color, &psudo_normal);
}

COLORREF CCGWorkView::ApplyLight(COLORREF in_color, vec4 normal, vec4 pos){
	// initilize as "no light"
	int inentisity = 0;

	// apply amient light
	double red_inentsity = m_ambient_k * ((double)m_ambientLight.colorR / 255) * GetRValue(in_color);
	double grn_inentsity = m_ambient_k * ((double)m_ambientLight.colorG / 255) * GetGValue(in_color);
	double blu_inentsity = m_ambient_k * ((double)m_ambientLight.colorB / 255) * GetBValue(in_color);


	double cos_teta = 1;
	double len_light_dist = 1;
	double len_normal = sqrt(pow(normal.x, 2.0) +
		pow(normal.y, 2.0) +
		pow(normal.z, 2.0));

	// draw z buffer depth using the blue channel
	if (render_type == ID_VIEW_Z)
		return RGB(static_cast<int>(255 * (pos.z / 4000)), 0, 0);

	// when you don't want light effect, tner a o length normal
	if (len_normal == 0)
		return 0;

	// apply diffuse reflection
	for (int l = LIGHT_ID_1; l < MAX_LIGHT; l++){
		if (m_lights[l].enabled){
			if (m_lights[l].type == LIGHT_TYPE_POINT){

				len_light_dist = sqrt(pow(pos.x - m_lights[l].posX, 2.0) +
					pow(pos.y - m_lights[l].posY, 2.0) +
					pow(pos.z - m_lights[l].posZ, 2.0));

				cos_teta = ((pos.x - m_lights[l].posX) * (normal.x) +
					(pos.y - m_lights[l].posY) * (normal.y) +
					(pos.z - m_lights[l].posZ) * (normal.z)) /
					(len_normal * len_light_dist);

				//diffuse light from source
				// when the normal is facing the light source, add the calculation
				// when the normal is not facing the light source, ignore it
				red_inentsity += m_diffuse_k * ((double)m_lights[l].colorR / 255) * GetRValue(in_color) * ((cos_teta < 0) ? -cos_teta : 0);
				grn_inentsity += m_diffuse_k * ((double)m_lights[l].colorG / 255) * GetGValue(in_color) * ((cos_teta < 0) ? -cos_teta : 0);
				blu_inentsity += m_diffuse_k * ((double)m_lights[l].colorB / 255) * GetBValue(in_color) * ((cos_teta < 0) ? -cos_teta : 0);

				//specular light from source
				//...
			}
			else if (m_lights[l].type == LIGHT_TYPE_DIRECTIONAL){
				// the "light distance" here is the size of the direction
				len_light_dist = sqrt(pow(m_lights[l].dirX, 2.0) +
					pow(m_lights[l].dirY, 2.0) +
					pow(m_lights[l].dirZ, 2.0));
				
				cos_teta = ((m_lights[l].dirX) * (normal.x) +
					(m_lights[l].dirY) * (normal.y) +
					(m_lights[l].dirZ) * (normal.z)) /
					(len_normal * len_light_dist);

				//diffuse light from source
				// when the normal is facing the light source, add the calculation
				// when the normal is not facing the light source, ignore it
				red_inentsity += m_diffuse_k * ((double)m_lights[l].colorR / 255) * GetRValue(in_color) * ((cos_teta < 0) ? -cos_teta : 0);
				grn_inentsity += m_diffuse_k * ((double)m_lights[l].colorG / 255) * GetGValue(in_color) * ((cos_teta < 0) ? -cos_teta : 0);
				blu_inentsity += m_diffuse_k * ((double)m_lights[l].colorB / 255) * GetBValue(in_color) * ((cos_teta < 0) ? -cos_teta : 0);

				//specular light from source
				//...
			}
			else if (m_lights[l].type == LIGHT_TYPE_SPOT){
				// not needed for hw2
				//len_light_dist = pow(m_lights[l].dirX - m_lights[l].posX, 2.0) +
				//	pow(m_lights[l].dirY - m_lights[l].posY, 2.0) +
				//	pow(m_lights[l].dirZ - m_lights[l].posZ, 2.0);

				//double point_dis = sqrt(pow(pos.x - m_lights[l].posX, 2)
				//	+ pow(pos.y - m_lights[l].posY, 2)
				//	+ pow(pos.z - m_lights[l].posZ, 2));

				//cos_teta = ((pos.x - m_lights[l].posX) * (normal.x) +
				//	(pos.y - m_lights[l].posY) * (normal.y) +
				//	(pos.z - m_lights[l].posZ) * (normal.z)) /
				//	(len_normal * len_light_dist);

				//double cos_dis_teta = ((pos.x - m_lights[l].posX) * (m_lights[l].dirX - m_lights[l].posX) +
				//	(pos.y - m_lights[l].posY) * (m_lights[l].dirY - m_lights[l].posY) +
				//	(pos.z - m_lights[l].posZ) * (m_lights[l].dirZ - m_lights[l].posZ)) /
				//	(point_dis * len_light_dist);


				//if (cos_dis_teta <= 1 && cos_dis_teta >= -1){
				//	if ((cos_teta < 0 && cos_dis_teta > 0) || (cos_teta > 0 && cos_dis_teta < 0)){
				//		red_inentsity += ((double)m_lights[l].colorR / 255) * GetRValue(in_color)*-cos_teta * cos_dis_teta;
				//		grn_inentsity += ((double)m_lights[l].colorG / 255) * GetGValue(in_color)*-cos_teta * cos_dis_teta;
				//		blu_inentsity += ((double)m_lights[l].colorB / 255) * GetBValue(in_color)*-cos_teta * cos_dis_teta;
				//	}
				//	else{
				//		red_inentsity += ((double)m_lights[l].colorR / 255) * GetRValue(in_color) *cos_teta * cos_dis_teta;
				//		grn_inentsity += ((double)m_lights[l].colorG / 255) * GetGValue(in_color) *cos_teta * cos_dis_teta;
				//		blu_inentsity += ((double)m_lights[l].colorB / 255) * GetBValue(in_color) *cos_teta * cos_dis_teta;
				//	}
				//}
			}
		}
	}

	COLORREF out_color = RGB(static_cast<int>(min(red_inentsity, 255)),
						 	 static_cast<int>(min(grn_inentsity, 255)),
							 static_cast<int>(min(blu_inentsity, 255)));
	return out_color;
}

void CCGWorkView::SetBackgound(){
	if (m_active_background && m_valid_background){
		int bpp = m_pngBackground.GetNumChannels();
		int screen_indx;
		int cur_color;
		int pic_width = m_pngBackground.GetWidth();
		int pic_height = m_pngBackground.GetHeight();
		if (bpp == 3 || bpp == 4){
			for (int x = 0; x < m_WindowWidth; x++)
				for (int y = 0; y < m_WindowHeight; y++){
					if (m_background_type == ID_BACKGROUND_REPEAT)
						cur_color = m_pngBackground.GetValue(
						x % pic_width,
						y % pic_height
						);
					else if (m_background_type == ID_BACKGROUND_STRETCH)
						cur_color = m_pngBackground.GetValue(
						static_cast<int>(((double)pic_width / m_WindowWidth) * x),
						static_cast<int>(((double)pic_height / m_WindowHeight) * y)
						);

					m_screen[SCREEN_SPACE(x, y)] = RGB(GET_B(cur_color), GET_G(cur_color), GET_R(cur_color));
				}
		}
		else
			for (int x = 0; x < m_WindowWidth; x++)
				for (int y = 0; y < m_WindowHeight; y++){
					if (m_background_type == ID_BACKGROUND_REPEAT)
						cur_color = m_pngBackground.GetValue(
						x % pic_width,
						y % pic_height
						);
					else if (m_background_type == ID_BACKGROUND_STRETCH)
						cur_color = m_pngBackground.GetValue(
						static_cast<int>(((double)pic_width / m_WindowWidth) * x),
						static_cast<int>(((double)pic_height / m_WindowHeight) * y)
						);

					m_screen[SCREEN_SPACE(x, y)] = m_pngBackground.GetValue(x, y);
				}
	}
	else
		std::fill_n(m_screen, m_WindowWidth * m_WindowHeight, m_background_color);

}

void CCGWorkView::RenderScene() {

	if (m_render_target == ID_RENDER_TOFILE){
		m_pngHandle.SetWidth(m_WindowWidth);
		m_pngHandle.SetHeight(m_WindowHeight);
		if (!m_pngHandle.InitWritePng()){
			//TODO send error
			return;
		}

	}

	vec4 psudo_normal = vec4(0, 0, 0, 1);

	SetBackgound();

	
	std::fill_n(z_buffer, m_WindowWidth * m_WindowHeight, std::numeric_limits<double>::infinity());
	vec4 p1, p2;
	polygon cur_polygon;
	mat4 cur_transform;
	mat4 no_presp_trans;
	for (unsigned int m = 0; m < models.size(); m++){
		no_presp_trans = models[m].obj_coord_trans * models[m].camera_trans * models[m].view_space_trans * m_screen_space_scale * m_screen_space_translate;
		if (m_nView == ID_VIEW_ORTHOGRAPHIC){
			cur_transform = models[m].obj_coord_trans * models[m].camera_trans * models[m].view_space_trans * m_screen_space_scale * m_screen_space_translate;
		}
		else if (m_nView == ID_VIEW_PERSPECTIVE){
			cur_transform = models[m].obj_coord_trans * models[m].camera_trans * models[m].view_space_trans * m_screen_space_scale * m_prespective_trans * m_screen_space_translate;
		}
		if (render_type == ID_VIEW_SOLID || render_type == ID_VIEW_Z)
			for (unsigned int pol = 0; pol < models[m].polygons.size(); pol++){
				p1 = cur_polygon.Normal(given_polygon_normal).p_a * cur_transform;
				p2 = cur_polygon.Normal(given_polygon_normal).p_b * cur_transform;
				ScanConversion(z_buffer, m_screen, models[m].polygons[pol], no_presp_trans, cur_transform, models[m].color);
			}
		else if (render_type == ID_VIEW_WIREFRAME)
			for (unsigned int pnt = 0; pnt < models[m].points_list.size(); pnt++){
			p1 = (models[m].points_list[pnt].p_a)* cur_transform;
			p2 = (models[m].points_list[pnt].p_b)* cur_transform;
			int prev_shading = m_nLightShading;
			m_nLightShading = ID_LIGHT_SHADING_FLAT;
			DrawLine(z_buffer, m_screen, p1, p2, models[m].color, &psudo_normal);
			m_nLightShading = prev_shading;
			}

		if (polygon_normal == ID_POLYGON_GIVEN || polygon_normal == ID_POLYGON_CALCULATED){
			for (unsigned int count = 0; count < models[m].polygons.size(); count++){
				cur_polygon = models[m].polygons[count];
				p1 = cur_polygon.Normal(given_polygon_normal).p_a * cur_transform;
				p2 = cur_polygon.Normal(given_polygon_normal).p_b * cur_transform;
				int prev_shading = m_nLightShading;
				m_nLightShading = ID_LIGHT_SHADING_FLAT;
				DrawLine(z_buffer, m_screen, p1, p2, m_polygon_norm_color, &psudo_normal);
				m_nLightShading = prev_shading;
			}
		}

		if (vertex_normal == ID_VERTEX_GIVEN || vertex_normal == ID_VERTEX_CALCULATED){
			std::vector<line> vertex_normal = models[m].Normal(given_vertex_normal);
			for (unsigned int count = 0; count < vertex_normal.size(); count++){
				p1 = vertex_normal[count].p_a * cur_transform;
				p2 = vertex_normal[count].p_b * cur_transform;
				int prev_shading = m_nLightShading;
				m_nLightShading = ID_LIGHT_SHADING_FLAT;
				DrawLine(z_buffer, m_screen, p1, p2, m_vertex_norm_color, &psudo_normal);
				m_nLightShading = prev_shading;
			}
		}

		if (m_bound_box){
			DrawBoundBox(z_buffer, m_screen, models[m], cur_transform, m_boundbox_color);
		}
	}


	if (m_render_target == ID_RENDER_TOFILE){
		m_pngHandle.WritePng();
	}
	else {

		m_map = CreateBitmap(m_WindowWidth,	// width
			m_WindowHeight,					// height
			1,								// Color Planes, unfortanutelly don't know what is it actually. Let it be 1
			8 * 4,							// Size of memory for one pixel in bits (in win32 4 bytes = 4*8 bits)
			(void*)m_screen);				// pointer to array

		SelectObject(m_hDC, m_map);			// Inserting picture into our temp HDC

		// Copy image from temp HDC to window
		BitBlt(m_pDC->GetSafeHdc(),			// Destination
			0,								// x and
			0,								// y - upper-left corner of place, where we'd like to copy
			m_WindowWidth,					// width of the region
			m_WindowHeight,					// height
			m_hDC,							// source
			0,								// x and
			0,								// y of upper left corner  of part of the source, from where we'd like to copy
			SRCCOPY);						// Defined DWORD to juct copy pixels. Watch more on msdn;

	}

	DeleteObject(m_map);
	return;
}

void CCGWorkView::OnFileLoad()
{
	TCHAR szFilters[] = _T("IRIT Data Files (*.itd)|*.itd|All Files (*.*)|*.*||");

	CFileDialog dlg(TRUE, _T("itd"), _T("*.itd"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	if (dlg.DoModal() == IDOK) {
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

void CCGWorkView::OnActionClearAll()
{
	models.clear();
	RenderScene();
}

void CCGWorkView::OnActionResetView()
{
	mat4 reset_transform;
	reset_transform[0][0] = 1;
	reset_transform[1][1] = 1;
	reset_transform[2][2] = 1;
	reset_transform[3][2] = 1;
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
		m_prespective_trans[2][2] = m_presepctive_d / (m_presepctive_d - m_presepctive_alpha);
		m_prespective_trans[2][3] = 1 / m_presepctive_d;
		m_prespective_trans[3][2] = -m_presepctive_alpha * m_presepctive_d / (m_presepctive_d - m_presepctive_alpha);

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
	Invalidate();
}

void CCGWorkView::OnUpdateLightShadingFlat(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nLightShading == ID_LIGHT_SHADING_FLAT);
}

void CCGWorkView::OnLightShadingGouraud()
{
	m_nLightShading = ID_LIGHT_SHADING_GOURAUD;
	Invalidate();
}

void CCGWorkView::OnUpdateLightShadingGouraud(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_nLightShading == ID_LIGHT_SHADING_GOURAUD);
}

// LIGHT SETUP HANDLER ///////////////////////////////////////////

void CCGWorkView::OnLightConstants()
{
	CLightDialog dlg;

	for (int id = LIGHT_ID_1; id<MAX_LIGHT; id++)
	{
		dlg.SetDialogData((LightID)id, m_lights[id]);
	}
	dlg.SetDialogData(LIGHT_ID_AMBIENT, m_ambientLight);
	dlg.SetLightConstants(m_ambient_k, m_diffuse_k, m_speculr_k);

	if (dlg.DoModal() == IDOK)
	{
		for (int id = LIGHT_ID_1; id<MAX_LIGHT; id++)
		{
			m_lights[id] = dlg.GetDialogData((LightID)id);
		}
		m_ambientLight = dlg.GetDialogData(LIGHT_ID_AMBIENT);

		m_ambient_k = dlg.m_ambient_mod;
		m_diffuse_k = dlg.m_diffuse_mod;
		m_speculr_k = dlg.m_specular_mod;
	}
	Invalidate();
}

void CCGWorkView::OnPolygonGiven()
{
	if (polygon_normal == ID_POLYGON_GIVEN)
		polygon_normal = NULL;
	else
		polygon_normal = ID_POLYGON_GIVEN;
	given_polygon_normal = true;
	Invalidate();
}

void CCGWorkView::OnPolygonCalculated()
{
	if (polygon_normal == ID_POLYGON_CALCULATED)
		polygon_normal = NULL;
	else
		polygon_normal = ID_POLYGON_CALCULATED;
	given_polygon_normal = false;
	Invalidate();
}

void CCGWorkView::OnUpdatePolygonCalculated(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(polygon_normal == ID_POLYGON_CALCULATED);
}

void CCGWorkView::OnUpdatePolygonGiven(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(polygon_normal == ID_POLYGON_GIVEN);
}

void CCGWorkView::OnVertexGiven()
{
	if (vertex_normal == ID_VERTEX_GIVEN)
		vertex_normal = NULL;
	else
		vertex_normal = ID_VERTEX_GIVEN;
	given_vertex_normal = true;
	Invalidate();
}

void CCGWorkView::OnUpdateVertexGiven(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(vertex_normal == ID_VERTEX_GIVEN);
}

void CCGWorkView::OnVertexCalculated()
{
	if (vertex_normal == ID_VERTEX_CALCULATED)
		vertex_normal = NULL;
	else
		vertex_normal = ID_VERTEX_CALCULATED;
	given_vertex_normal = false;
	Invalidate();
}

void CCGWorkView::OnUpdateVertexCalculated(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(vertex_normal == ID_VERTEX_CALCULATED);
}

void CCGWorkView::OnViewWireframe()
{
	render_type = ID_VIEW_WIREFRAME;
	Invalidate();
}

void CCGWorkView::OnUpdateViewWireframe(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(render_type == ID_VIEW_WIREFRAME);
}

void CCGWorkView::OnViewSolid()
{
	render_type = ID_VIEW_SOLID;
	Invalidate();
}

void CCGWorkView::OnUpdateViewSolid(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(render_type == ID_VIEW_SOLID);
}

void CCGWorkView::OnViewZ()
{
	render_type = ID_VIEW_Z;
	Invalidate();
}

void CCGWorkView::OnUpdateViewZ(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(render_type == ID_VIEW_Z);
}

void CCGWorkView::OnLightShadingPhong()
{
	m_nLightShading = ID_LIGHT_SHADING_PHONg;
	Invalidate();
}

void CCGWorkView::OnUpdateLightShadingPhong(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_nLightShading == ID_LIGHT_SHADING_PHONg);
}

void CCGWorkView::OnRenderTofile()
{
	CString window_pic_name(m_pic_name.c_str());
	FileRenderDlg dlg(m_WindowWidth, m_WindowHeight, window_pic_name);
	m_OriginalWindowHeight = m_WindowHeight;
	m_OriginalWindowWidth = m_WindowWidth;

	if (dlg.DoModal() == IDOK)
	{
		m_WindowHeight = dlg.m_pic_height;
		m_WindowWidth = dlg.m_pic_width;

		//// Convert a TCHAR string to a LPCSTR
		// required for data type conversions
		CT2CA pszConvertedAnsiString(dlg.m_pic_name);
		m_pic_name = std::string(pszConvertedAnsiString);

		m_pngHandle.SetFileName(m_pic_name.c_str());

		m_render_target = ID_RENDER_TOFILE;
		
	}

	Invalidate();
}

void CCGWorkView::OnUpdateRenderTofile(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_render_target == ID_RENDER_TOFILE);
}

void CCGWorkView::OnRenderToscreen()
{
	m_render_target = ID_RENDER_TOSCREEN;
	m_WindowHeight = m_OriginalWindowHeight;
	m_WindowHeight = m_OriginalWindowWidth;
	Invalidate();
}

void CCGWorkView::OnUpdateRenderToscreen(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_render_target == ID_RENDER_TOSCREEN);
}

void CCGWorkView::OnBackgroundSetimage()
{
	TCHAR szFilters[] = _T("PNG Data Files (*.png)|*.png|All Files (*.*)|*.*||");

	CFileDialog dlg(TRUE, _T("png"), _T("*.png"), OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, szFilters);

	if (dlg.DoModal() == IDOK) {	
		CT2CA pszConvertedAnsiString(dlg.GetPathName()); // Full path and filename

		std::string string_filename = std::string(pszConvertedAnsiString);

		m_pngBackground.SetFileName(string_filename.c_str());
		
		
		if (m_pngBackground.ReadPng()){
			m_valid_background = true;
			m_pngBackground.ReadPng();
		}
		else {
			m_valid_background = false;
			// TODO eror message
		}

		Invalidate();	// force a WM_PAINT for drawing.
	}

}


void CCGWorkView::OnBackgroundActive()
{
	m_active_background = !m_active_background;
	Invalidate();
}


void CCGWorkView::OnUpdateBackgroundActive(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_active_background);
}


void CCGWorkView::OnBackgroundRepeat()
{
	m_background_type = ID_BACKGROUND_REPEAT;
	Invalidate();
}


void CCGWorkView::OnUpdateBackgroundRepeat(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_background_type == ID_BACKGROUND_REPEAT);
}


void CCGWorkView::OnBackgroundStretch()
{
	m_background_type = ID_BACKGROUND_STRETCH;
	Invalidate();
}


void CCGWorkView::OnUpdateBackgroundStretch(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(m_background_type == ID_BACKGROUND_STRETCH);
}


void CCGWorkView::OnOptionsBackfaceculling()
{
	// TODO: Add your command handler code here
	
	m_back_face_culling = !m_back_face_culling;
	Invalidate();
}


void CCGWorkView::OnUpdateOptionsBackfaceculling(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_back_face_culling);
}


void CCGWorkView::OnOptionsNormalinverse()
{
	// TODO: Add your command handler code here
}


void CCGWorkView::OnUpdateOptionsNormalinverse(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
}


void CCGWorkView::OnUpdateOptionsOverridegivennormal(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->SetCheck(m_override_normals);
}


void CCGWorkView::OnOptionsOverridegivennormal()
{
	m_override_normals = !m_override_normals;
	Invalidate();
}
