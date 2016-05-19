#include "stdafx.h"
#include "CImage.h"
#include "ChildView.h"
#include "InfoDlg.h"
#include <math.h>

void cross(float *TangentX,float* UpVector,float* N){
	float* tmp = new float[3];
	float tmpl;
	tmp[0] = UpVector[1] * N[2] - UpVector[2] * N[1];  //[a2b3-a3b2,a3b1-a1b3, a1b2-a2b1]
	tmp[1] = UpVector[2] * N[0] - UpVector[0] * N[2];
	tmp[2] = UpVector[0] * N[1] - UpVector[1] * N[0];
	tmpl = tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2];
	tmpl = sqrt(tmpl);
	TangentX[0] = tmp[0] / tmpl;
	TangentX[1] = tmp[1] / tmpl;
	TangentX[2] = tmp[2] / tmpl;
	delete[] tmp;
}

void ImportanceSampleGGX(float* res, float* Xi, float Roughness, float* N){
	float a = Roughness * Roughness;
	float Phi = 2.0f * 3.1415926f * Xi[0];
	float CosTheta = (float)(sqrt((1.0 - Xi[1]) / (1.0 + (a*a - 1.0) * Xi[1])));
	float SinTheta = (float)(sqrt(1.0 - CosTheta * CosTheta));
	float* H = new float[3];
	float* UpVector = new float[3];
	float* TangentX = new float[3];
	float* TangentY = new float[3];
	H[0] = SinTheta * cos(Phi);
	H[1] = SinTheta * sin(Phi);
	H[2] = CosTheta;
	if (abs(N[2]) < 0.999){
		UpVector[0] = UpVector[1] = 0.0;
		UpVector[2] = 1.0;
	}
	else{
		UpVector[1] = UpVector[2] = 0.0;
		UpVector[0] = 1.0;
	}
	cross(TangentX,UpVector, N);
	cross(TangentY, N, TangentX);
	// Tangent to world space
	res[0] = TangentX[0] * H[0] + TangentY[0] * H[1] + N[0] * H[2];
	res[1] = TangentX[1] * H[0] + TangentY[1] * H[1] + N[1] * H[2];
	res[2] = TangentX[2] * H[0] + TangentY[2] * H[1] + N[2] * H[2];
	delete[] H;
	delete[] UpVector;
	delete[] TangentX;
	delete[] TangentY;
}

float saturate(float f){
	return (float)(min(1.0, max(0.0, f)));
}

void Hammersley(float* Xi, int i, int N){
	Xi[0] = i*1.0f / N;
	Xi[1] = 0.0f;
	int base = 2;
	float inv_base = 1.0f / base;
	float f = inv_base;//Base is 2
	while (i>0){
		Xi[1] += f*(i%2);
		i = i / base;
		f *= inv_base;
	}
}

float dot(float* a, float* b){
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void vmul(float* res, float a, float* v){
	res[0] = a*v[0];
	res[1] = a*v[1];
	res[2] = a*v[2];
}

void vsub(float* v1, float* v2){
	v1[0] = v1[0] - v2[0];
	v1[1] = v1[1] - v2[1];
	v1[2] = v1[2] - v2[2];
}

void IntegrateBRDF(float* res, float roughness, float NoV){
	float* V = new float[3];
	float *Xi = new float[2];
	float *H = new float[3];
	float *L = new float[3];
	float *N = new float[3];
	V[0] = sqrt(1.0f - NoV * NoV); // sin
	V[1] = 0.0;
	V[2] = NoV; // cos
	N[0] = 0.0;
	N[1] = 0.0;
	N[2] = 1.0; // cos
	float A = 0.0;
	float B = 0.0;
	int NumSamples = 1024;
	for (int i = 0; i < NumSamples; i++){
		Hammersley(Xi, i, NumSamples);
		ImportanceSampleGGX(H, Xi, roughness, N);
		vmul(L, 2.0f * dot(V, H), H);
		vsub(L,V);
		float NoL = saturate(L[2]);
		float NoH = saturate(H[2]);
		float VoH = saturate(dot(V, H));
		if (NoL > 0.0){
			float G = (VoH == 0) ? 1.0f : min(1.0f, min(2.0f*NoH*NoV / VoH, 2.0f*NoH*NoL / VoH));
			float G_Vis = G * VoH / (NoH * NoV);
			float Fc = (float)pow(1.0 - VoH, 5.0);
			A += (1.0f - Fc) * G_Vis;
			B += Fc * G_Vis;
		}
	}
	delete[] V;
	delete[] Xi;
	delete[] H;
	delete[] N;
	delete[] L;
	res[0] = A / NumSamples;
	res[1] = B / NumSamples;
}

void ApproximateSpecularIBL(float* res, float NoV, float roughness, float* rf){
	float* EnvBRDF = new float[2];
	IntegrateBRDF(EnvBRDF, roughness,NoV);
	res[0] = EnvBRDF[1] * rf[0] + EnvBRDF[2];
	res[1] = EnvBRDF[1] * rf[1] + EnvBRDF[2];
	res[2] = EnvBRDF[1] * rf[2] + EnvBRDF[2];
	delete[] EnvBRDF;
}

void CChildView::OnSrtpEnvironmentbrdf()
{
	CWaitCursor wait;//在函数执行过程中使鼠标图标变成等待图标
	int newX = 256, newY = 256;
	CImage tmpimg;
	tmpimg.Create(newX, newY, 24, 0);
	float NoV, roughness;
	float* res = new float[2];
	for (int x = 0; x < newX; x++){
		NoV = x*1.0f / newX;
		for (int y = 0; y < newY; y++){
			roughness = y*2.0f / newY;
			IntegrateBRDF(res, roughness, NoV);
			tmpimg.SetPixelRGB(x, y, (byte)(res[0] * 255), (byte)(res[1] * 255), 0);
		}
	}
	delete[] res;
	imgOriginal.Destroy();
	imgOriginal.Create(newX, newY, tmpimg.GetBPP());//根据新的大小建立CImage，GetBPP是获取其大小
	for (int x = 0; x < newX; x++){
		for (int y = 0; y < newY; y++){
			imgOriginal.SetPixel(x, y, tmpimg.GetPixel(x, y));
		}
	}
	tmpimg.Destroy();
	//刷新显示图像
	Invalidate();
	UpdateWindow();
}
