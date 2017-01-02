#pragma once

#include "Light.h"
// CLightDialog dialog



class CLightDialog : public CDialog
{
	DECLARE_DYNAMIC(CLightDialog)

public:
	CLightDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CLightDialog();

	//dialog interface
	void SetDialogData(LightID id,const LightParams& light);
	void SetLightConstants(double ambient_mod, double diffuse_mod, double spcular_mod);
	LightParams GetDialogData(LightID id);
	double m_ambient_mod;
	double m_diffuse_mod;
	double m_specular_mod;

// Dialog Data
	enum { IDD = IDD_LIGHTS_DLG };

protected:
	LightParams m_lights[MAX_LIGHT];
	LightParams m_ambiant;
	int m_currentLightIdx;
	int GetCurrentLightIndex();

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedRadioLight();
//    afx_msg void On();
    virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedLightEnabled();
	afx_msg void OnCbnSelchangeLightType();
	afx_msg void OnEnChangeLightColorR();
};
