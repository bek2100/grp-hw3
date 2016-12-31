// CGWorkView.h : interface of the CCGWorkView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CGWORKVIEW_H__5857316D_EA60_11D5_9FD5_00D0B718E2CD__INCLUDED_)
#define AFX_CGWORKVIEW_H__5857316D_EA60_11D5_9FD5_00D0B718E2CD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "gl\gl.h"    // Include the standard CGWork  headers
#include "gl\glu.h"   // Add the utility library


#include "Light.h"

#include "vec4.h"
#include "line.h"
#include <unordered_map>
#include "polygon.h"
#include <vector>
#include "model.h"
#include "PngWrapper.h"

class CCGWorkView : public CView
{
protected: // create from serialization only
	CCGWorkView();
	DECLARE_DYNCREATE(CCGWorkView)

// Attributes
public:
	CCGWorkDoc* GetDocument();

// Operations
public:

private:
	int m_nAxis;				// Axis of Action, X Y or Z
	int m_nAction;				// Rotate, Translate, Scale
	int m_nView;				// Orthographic, perspective
	int polygon_normal;
	int vertex_normal;
	bool given_polygon_normal;
	bool given_vertex_normal;
	bool m_bIsPerspective;			// is the view perspective
	int render_type;				// rendering type
	int m_render_target;			// render to a file or to the screent
	
	CString m_strItdFileName;		// file name of IRIT data

	int m_nLightShading;			// shading: Flat, Gouraud.

	double m_lMaterialAmbient;		// The Ambient in the scene
	double m_lMaterialDiffuse;		// The Diffuse in the scene
	double m_lMaterialSpecular;		// The Specular in the scene
	int m_nMaterialCosineFactor;		// The cosine factor for the specular

	
	class x_z_c_n_point {
		friend bool operator<(const x_z_c_n_point& l, const x_z_c_n_point& r){
			return (l.x < r.x);
		}

	public:
		double x;
		double z;
		COLORREF c;
		vec4 n;
	};

	// our functions
	void DrawLine(double *z_arr, COLORREF *arr, vec4 &p1, vec4 &p2, COLORREF p1_color, vec4* p1_normal = NULL, COLORREF p2_color = NULL, vec4* p2_normal = NULL, std::unordered_map<int, std::vector<x_z_c_n_point>>* x_y = NULL);
	void DrawBoundBox(double *z_arr, COLORREF *arr, model &m, mat4 cur_transform, COLORREF color);
	void ScanConversion(double *z_arr, COLORREF *arr, polygon &p, mat4 no_presp_trans, mat4 cur_transform, COLORREF color);
	void SetBackgound();
	COLORREF ApplyLight(COLORREF in_color, vec4 normal, vec4 pos);
	COLORREF m_color_wireframe;
	COLORREF m_background_color;
	COLORREF m_boundbox_color;
	COLORREF m_vertex_norm_color;
	COLORREF m_polygon_norm_color;

	LightParams m_lights[MAX_LIGHT];	//configurable lights array
	LightParams m_ambientLight;		//ambient light (only RGB is used)


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCGWorkView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void OnWriteframeColor();			// The wireframe colour
	protected:
	//}}AFX_VIRTUAL

// Implementation

public:
	virtual ~CCGWorkView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	BOOL InitializeCGWork();
	BOOL SetupViewingFrustum(void);
	BOOL SetupViewingOrthoConstAspect(void);
	virtual void RenderScene();
	virtual LRESULT OnMouseMovement(WPARAM wparam, LPARAM lparam);

	mat4 m_tarnsform;
	mat4 m_screen_space_trans;
	mat4 m_prespective_trans;
	mat4 m_screen_space_scale;
	mat4 m_screen_space_translate;
	vec4 m_camera_pos;
	vec4 m_camera_at;
	vec4 m_camera_up;
	double m_presepctive_d;
	double m_presepctive_alpha;
	double m_ambient_k;
	double m_diffuse_k;
	double m_speculr_k;
	std::vector<bool> active_modules;
	std::string m_pic_name;

	PngWrapper m_pngHandle; // Png handle class for writing
	PngWrapper m_pngBackground; // Png handle class for background

	bool m_active_background;
	bool m_valid_background;
	int m_background_type;

	int m_mouse_xpos;
	int m_mouse_ypos;
	bool m_object_space_trans;
	bool m_bound_box;
	HGLRC    m_hRC;			// holds the Rendering Context
	HDC		 m_hDC;
	CDC*     m_pDC;			// holds the Device Context
	int m_WindowWidth;		// hold the windows width
	int m_WindowHeight;		// hold the windows height
	int m_OriginalWindowWidth;		// hold the original window width
	int m_OriginalWindowHeight;		// hold the original window height
	double m_AspectRatio;		// hold the fixed Aspect Ration
	double m_mouse_sensetivity;
	COLORREF *m_screen;
	double *z_buffer;
	HBITMAP m_map;

// Generated message map functions
protected:
	//{{AFX_MSG(CCGWorkView)
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileLoad();
	afx_msg void OnViewOrthographic();
	afx_msg void OnUpdateViewOrthographic(CCmdUI* pCmdUI);
	afx_msg void OnViewPerspective();
	afx_msg void OnUpdateViewPerspective(CCmdUI* pCmdUI);
	afx_msg void OnActionRotate();
	afx_msg void OnUpdateActionRotate(CCmdUI* pCmdUI);
	afx_msg void OnActionScale();
	afx_msg void OnUpdateActionScale(CCmdUI* pCmdUI);
	afx_msg void OnActionTranslate();
	afx_msg void OnUpdateActionTranslate(CCmdUI* pCmdUI);
	afx_msg void OnAxisX();
	afx_msg void OnUpdateAxisX(CCmdUI* pCmdUI);
	afx_msg void OnAxisY();
	afx_msg void OnUpdateAxisY(CCmdUI* pCmdUI);
	afx_msg void OnAxisZ();
	afx_msg void OnUpdateAxisZ(CCmdUI* pCmdUI);
	afx_msg void OnAxisXY();
	afx_msg void OnUpdateAxisXY(CCmdUI* pCmdUI);
	afx_msg void OnBoundBox();
	afx_msg void OnUpdateBoundBox(CCmdUI* pCmdUI);
	afx_msg void OnLightShadingFlat();
	afx_msg void OnUpdateLightShadingFlat(CCmdUI* pCmdUI);
	afx_msg void OnLightShadingGouraud();
	afx_msg void OnUpdateLightShadingGouraud(CCmdUI* pCmdUI);
	afx_msg void OnLightConstants();
	afx_msg void OnActionToggleView();
	afx_msg void OnUpdateActionToggleView(CCmdUI* pCmdUI);
	afx_msg void OnActionResetView();
	afx_msg void OnActionClearAll();
	afx_msg void OnOptionMouseSensetivity();
	afx_msg void OnOptionPrespectiveControl();
	afx_msg void OnOptionOthers();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPolygonGiven();
	afx_msg void OnPolygonCalculated();
	afx_msg void OnUpdatePolygonCalculated(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePolygonGiven(CCmdUI *pCmdUI);
	afx_msg void OnVertexGiven();
	afx_msg void OnUpdateVertexGiven(CCmdUI *pCmdUI);
	afx_msg void OnVertexCalculated();
	afx_msg void OnUpdateVertexCalculated(CCmdUI *pCmdUI);
	afx_msg void OnViewWireframe();
	afx_msg void OnUpdateViewWireframe(CCmdUI *pCmdUI);
	afx_msg void OnUpdateViewSolid(CCmdUI *pCmdUI);
	afx_msg void OnViewSolid();
	afx_msg void OnViewZ();
	afx_msg void OnUpdateViewZ(CCmdUI *pCmdUI);
	afx_msg void OnLightShadingPhong();
	afx_msg void OnUpdateLightShadingPhong(CCmdUI *pCmdUI);
	afx_msg void OnRenderTofile();
	afx_msg void OnUpdateRenderTofile(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRenderToscreen(CCmdUI *pCmdUI);
	afx_msg void OnRenderToscreen();
	afx_msg void OnBackgroundSetimage();
	afx_msg void OnBackgroundActive();
	afx_msg void OnUpdateBackgroundActive(CCmdUI *pCmdUI);
	afx_msg void OnBackgroundRepeat();
	afx_msg void OnUpdateBackgroundRepeat(CCmdUI *pCmdUI);
	afx_msg void OnBackgroundStretch();
	afx_msg void OnUpdateBackgroundStretch(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // debug version in CGWorkView.cpp
inline CCGWorkDoc* CCGWorkView::GetDocument()
   { return (CCGWorkDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CGWORKVIEW_H__5857316D_EA60_11D5_9FD5_00D0B718E2CD__INCLUDED_)

