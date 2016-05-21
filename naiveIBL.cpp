#include "stdafx.h"
#include "CImage.h"
#include "ChildView.h"
#include "InfoDlg.h"
#include <math.h>

void normalize(float *n){
	float mod = sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
	n[0] /= mod;
	n[1] /= mod;
	n[2] /= mod;
}

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
	//将低差异序列第一个值化为角度
	float Phi = 2.0f * 3.1415926f * Xi[0];
	//将第二个值也化为角度
	float CosTheta = (float)(sqrt((1.0 - Xi[1]) / (1.0 + (a*a - 1.0) * Xi[1])));
	float SinTheta = (float)(sqrt(1.0 - CosTheta * CosTheta));

	float* H = new float[3];
	float* UpVector = new float[3];
	float* TangentX = new float[3];
	float* TangentY = new float[3];
	//球坐标转直角坐标
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
	// Tangent to world space 实际是矩阵相乘
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

void vadd(float* v1, float* v2){
	v1[0] = v1[0] + v2[0];
	v1[1] = v1[1] + v2[1];
	v1[2] = v1[2] + v2[2];
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
		//BRDF拆分项计算
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

CImage cube[6];//left,right,up,down,front,back;

void CChildView::PrefilterEnvMap(float* res, float Roughness, float* R){
	//int maxY = imgOriginal.GetHeight(), maxX = imgOriginal.GetWidth();
	int maxY = cube[0].GetHeight(), maxX = cube[0].GetWidth();
	float angle1, angle2, angle3, a, b;
	int plane;
	COLORREF pixel;
	float* N = R;
	float* V = R;
	res[0] = res[1] = res[2] = 0;
	int NumSamples = 512;
	float TotalWeight = 0.0;
	float* Xi = new float[2];
	float* H = new float[3];
	float* L = new float[3];
	float* center = new float[3];
	float* tmp = new float[3];
	float* z = new float[3];
	float* x = new float[3];
	x[0] = 1.0f; x[1] = 0.0f; x[2] = 0.0f;
	z[0] = 0.0f; z[1] = 0.0f; z[2] = 1.0f;
	center[0] = 0.5;
	center[1] = 0.5;
	center[2] = -0.5;
	for (int i = 0; i < NumSamples; i++)
	{
		Hammersley(Xi, i, NumSamples);
		ImportanceSampleGGX(H, Xi, Roughness, N);
		vmul(L, 2.0f * dot(V, H), H);
		vsub(L, V);
		float NoL = saturate(dot(N, L));
		if (NoL > 0){
			//First we calculate the angle of plane x-z,z-y to axis z, x-y to axis x
			//x-z z
			tmp[0] = L[0]; tmp[1] = 0.0f; tmp[2] = L[2];
			normalize(tmp);
			angle1 = tmp[2];
			//x-y y
			tmp[0] = L[0]; tmp[1] = L[1]; tmp[2] = 0.0f;
			normalize(tmp);
			angle2 = tmp[0];
			//y-z z
			tmp[0] = 0.0f; tmp[1] = L[1]; tmp[2] = L[2];
			normalize(tmp);
			angle3 = tmp[2];
			//judge in which plane
			if (angle1 >= 0.71f && angle3 >= 0.71f){
				plane = 2;
				vmul(L, 0.5f / L[2], L);
				a = L[1] + 0.5f;
				b = (L[0] + 0.5f);
			}
			else if (angle1 <= -0.71f && angle3 <= -0.71f){
				plane = 3;
				vmul(L, 0.5f / L[2], L);
				a = 1.0f - (L[1] + 0.5f);
				b = L[0] + 0.5f;
			}
			else if (angle2 >= 0.71f){
				plane = 4;
				vmul(L, 0.5f / L[0], L);
				a = 1.0f - (L[1] + 0.5f);
				b = 1.0f - (L[2] + 0.5f);
			}
			else if (angle2 <= -0.71f){
				plane = 5;
				vmul(L, 0.5f / L[0], L);
				a = 1.0f - (L[1] + 0.5f);
				b = 1.0f - (L[2] + 0.5f);
			}
			else if (L[1] >= 0){
				plane = 1;
				vmul(L, 0.5f / L[1], L);
				a = (L[0] + 0.5f);
				b = (L[2] + 0.5f);
			}
			else if (L[1] < 0){
				plane = 0;
				vmul(L, 0.5f / L[1], L);
				a = L[0] + 0.5f;
				b = 1.0f - (L[2] + 0.5f);
			}
			else{
				plane = 0;
				a = 0;
				b = 0;
				::AfxMessageBox(_T("ERROR"));
			}

			//Eliminate boundary
			if (a > 1.0f || a<0.0f || b>1.0f || b < 0.0f){
				CString tstr;
				tstr.Format(_T("plane: %d a: %lf b: %lf"), plane, a, b);
				//::AfxMessageBox(tstr);
			}

			a = (a < 0.0f) ? 0.0f : a;
			b = (b < 0.0f) ? 0.0f : b;
			a = (a > 1.0f) ? 1.0f : a;
			b = (b > 1.0f) ? 1.0f : b;
			a = a * (maxX-1);
			b = (1.0f-b) * (maxY-1);
			pixel = cube[plane].GetPixel((int)a,(int)b);
			L[0] = GetRValue(pixel);
			L[1] = GetGValue(pixel);
			L[2] = GetBValue(pixel);
			vmul(L, NoL, L);
			vadd(res, L);
			//res += textureCube(cubemap, L).rgb * NoL;
			TotalWeight += NoL;
		}
	}
	vmul(res, 1/TotalWeight, res);
	delete[] Xi;
	delete[] H;
	delete[] L;
	delete[] center;
	delete[] tmp;
	delete[] x;
	delete[] z;
}

void CChildView::OnSrtpCubemap(){
	// TODO:  在此添加命令处理程序代码
	CWaitCursor wait;
	//int maxY = imgOriginal.GetHeight(), maxX = imgOriginal.GetWidth();
	cube[0].Load(_T("left.png"));
	cube[1].Load(_T("right.png"));
	cube[2].Load(_T("up.png"));
	cube[3].Load(_T("buttom.png"));
	cube[4].Load(_T("front.png"));
	cube[5].Load(_T("back.png"));
	float* center = new float[3];
	float* reflect = new float[3];
	float* res = new float[3];
	CImage tmpimg;
	for (int j = 0; j < 6; j++){
		center[0] = 0.5;
		center[1] = 0.5;
		center[2] = 0.5;
		for (int i = 1; i < 8; i++){
			int newY, newX;
			newY = newX = (int)pow(2.0,(8-i));
			tmpimg.Create(newX, newY, 24, 0);
			float Roughness = (i > 4) ? 0.5f * (i - 4) : i*0.1f;
			for (int x = 0; x < newX; x++){ //列循环
				for (int y = 0; y < newY; y++){ //行循环
					switch (j){
					case 0:
						reflect[0] = 1.0f - (float)x * 1.0f / (float)newX;
						reflect[1] = 0.0f;
						reflect[2] = 1.0f - (float)y * 1.0f / (float)newY;
						break;
					case 1:
						reflect[0] = (float)x * 1.0f / (float)newX;
						reflect[1] = 1.0f;
						reflect[2] = 1.0f - (float)y * 1.0f / (float)newY;
						break;
					case 2:
						reflect[0] = 1.0f - (float)y * 1.0f / (float)newY;
						reflect[1] = (float)x * 1.0f / (float)newX;
						reflect[2] = 1.0f;
						break;
					case 3:
						reflect[0] = (float)y * 1.0f / (float)newY;
						reflect[1] = (float)x * 1.0f / (float)newX;
						reflect[2] = 0.0f;
						break;
					case 4:
						reflect[0] = 1.0f;
						reflect[1] = 1.0f - (float)x * 1.0f / (float)newX;
						reflect[2] = 1.0f - (float)y * 1.0f / (float)newY;
						break;
					case 5:
						reflect[0] = 0.0;
						reflect[1] = (float)x * 1.0f / (float)newX;
						reflect[2] = 1.0f - (float)y * 1.0f / (float)newY;
						break;
					}
					vsub(reflect, center);
					normalize(reflect);
					PrefilterEnvMap(res, Roughness, reflect);
					tmpimg.SetPixelRGB(x, y, (byte)res[0], (byte)res[1], (byte)res[2]);
				}
			}
			CString str,pre;
			pre.Format(_T("%d"), j);
			pre = pre + _T("-");
			str.Format(_T("%d"), i);
			str = pre + str + _T(".bmp");
			HRESULT hResult = tmpimg.Save(str);
			if (FAILED(hResult)) {
				CString fmt;
				fmt.Format(_T("Save image failed:\n%x - %s"), hResult, _com_error(hResult).ErrorMessage());
				::AfxMessageBox(fmt);
				return;
			}
			tmpimg.Destroy();
		}
	}
	delete[] center;
	delete[] reflect;
	delete[] res;
	Invalidate();
	UpdateWindow();
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
