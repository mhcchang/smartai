#ifndef __FACEMODEL_H
#define __FACEMODEL_H

#include "DefGlobalVar.h"
#include <vector>

using namespace std;

#ifndef DLL_EXPORT
#define DLL_EXPORT __declspec(dllexport)
#endif 

class DLL_EXPORT CFaceAlgorithmClass
{
public:
	CFaceAlgorithmClass();
	virtual ~CFaceAlgorithmClass();

	// ��ʼ���㷨��
	virtual bool InitModel();
	
	// ���뵱ǰͼ��·����Ψһid��ǩ����������ֵ���������ꡢid��Ϣ
	virtual bool RegisterFace(char *szName, int id, StruFaceInfo *faceRegister);

	// ʵʱ�����������ȡ����ֵ������������Ϣ hitThresholdһ��Ϊ3��hitThreshold=2ʱ����Щ�������������
	virtual bool FaceDetectAndFeatureExtract(unsigned char *pClrImg, const int &width, const int &height, int hitThreshold, vector<StruFaceInfo> &faceInfo);

	// �������ƶȱȶ�
	virtual float FacePairMatch(unsigned char *pFeature1, unsigned char *pFeature2, const int &featureLength);

private:
	CFaceAlgorithmClass *m_pTreat;
};


#endif // #ifndef __DETECTOR_H