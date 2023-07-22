#include "../../includes/includes.hpp"
#include <corecrt_math_defines.h>
// defines
namespace defines {
	inline DWORD64 base_address;
	inline bool debug = true;
	inline HWND game_window;
}

extern int screen_width;
extern int screen_height;
class vector2
{
public:
	vector2() : x(0.f), y(0.f)
	{

	}

	vector2(double _x, double _y) : x(_x), y(_y)
	{

	}
	~vector2()
	{

	}

	double x;
	double y;

	/*inline ImVec2 ConvImVec2()
	{
		return ImVec2(x, y);
	}*/
	inline bool valid_location() {
		if (x > screen_width or y > screen_height)
			return false;
		else if (x <= 0 or y <= 0)
			return false;
		else if (std::isnan(x) or std::isnan(y))
			return false;
		else
			return true;

	}

};
class vector3
{
public:
	vector3() : x(0.f), y(0.f), z(0.f)
	{

	}

	vector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z)
	{

	}
	~vector3()
	{

	}

	double x;
	double y;
	double z;

	inline double dot(vector3 v)
	{
		return x * v.x + y * v.y + z * v.z;
	}

	inline double distance(vector3 v)
	{
		return double(sqrtf(powf(v.x - x, 2.0) + powf(v.y - y, 2.0) + powf(v.z - z, 2.0)));
	}

	inline double length() {
		return sqrt(x * x + y * y + z * z);
	}

	vector3 operator+(vector3 v)
	{
		return vector3(x + v.x, y + v.y, z + v.z);
	}

	vector3 operator-(vector3 v)
	{
		return vector3(x - v.x, y - v.y, z - v.z);
	}

	vector3 operator*(double flNum) { return vector3(x * flNum, y * flNum, z * flNum); }
	void set_random_coordinates() {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_real_distribution<double> dis(0.0, 360.0);

		x = dis(gen);
		y = dis(gen);
		z = dis(gen);
	}
};
struct fquat
{
	double x;
	double y;
	double z;
	double w;
};
struct frotator
{
	double Pitch;
	double Yaw;
	double Roll;
};
template<class T>
class tarray
{
public:
	int Length() const
	{
		return m_nCount;
	}

	bool IsValid() const
	{
		if (m_nCount > m_nMax)
			return false;
		if (!m_Data)
			return false;
		return true;
	}

	uint64_t GetAddress() const
	{
		return m_Data;
	}
	T operator[](int i) {
		return read<T>(m_Data + i * sizeof(T));
	};
protected:
	uint64_t m_Data;
	uint32_t m_nCount;
	uint32_t m_nMax;
};

struct FTransform
{
	fquat rot;
	vector3 translation;
	char pad[4];
	vector3 scale;
	char pad1[4];
	D3DMATRIX ToMatrixWithScale()
	{
		D3DMATRIX m;
		m._41 = translation.x;
		m._42 = translation.y;
		m._43 = translation.z;

		float x2 = rot.x + rot.x;
		float y2 = rot.y + rot.y;
		float z2 = rot.z + rot.z;

		float xx2 = rot.x * x2;
		float yy2 = rot.y * y2;
		float zz2 = rot.z * z2;
		m._11 = (1.0f - (yy2 + zz2)) * scale.x;
		m._22 = (1.0f - (xx2 + zz2)) * scale.y;
		m._33 = (1.0f - (xx2 + yy2)) * scale.z;

		float yz2 = rot.y * z2;
		float wx2 = rot.w * x2;
		m._32 = (yz2 - wx2) * scale.z;
		m._23 = (yz2 + wx2) * scale.y;

		float xy2 = rot.x * y2;
		float wz2 = rot.w * z2;
		m._21 = (xy2 - wz2) * scale.y;
		m._12 = (xy2 + wz2) * scale.x;

		float xz2 = rot.x * z2;
		float wy2 = rot.w * y2;
		m._31 = (xz2 + wy2) * scale.z;
		m._13 = (xz2 - wy2) * scale.x;

		m._14 = 0.0f;
		m._24 = 0.0f;
		m._34 = 0.0f;
		m._44 = 1.0f;

		return m;
	}
};
inline D3DMATRIX MatrixMultiplication(D3DMATRIX pM1, D3DMATRIX pM2)
{
	D3DMATRIX pOut;
	pOut._11 = pM1._11 * pM2._11 + pM1._12 * pM2._21 + pM1._13 * pM2._31 + pM1._14 * pM2._41;
	pOut._12 = pM1._11 * pM2._12 + pM1._12 * pM2._22 + pM1._13 * pM2._32 + pM1._14 * pM2._42;
	pOut._13 = pM1._11 * pM2._13 + pM1._12 * pM2._23 + pM1._13 * pM2._33 + pM1._14 * pM2._43;
	pOut._14 = pM1._11 * pM2._14 + pM1._12 * pM2._24 + pM1._13 * pM2._34 + pM1._14 * pM2._44;
	pOut._21 = pM1._21 * pM2._11 + pM1._22 * pM2._21 + pM1._23 * pM2._31 + pM1._24 * pM2._41;
	pOut._22 = pM1._21 * pM2._12 + pM1._22 * pM2._22 + pM1._23 * pM2._32 + pM1._24 * pM2._42;
	pOut._23 = pM1._21 * pM2._13 + pM1._22 * pM2._23 + pM1._23 * pM2._33 + pM1._24 * pM2._43;
	pOut._24 = pM1._21 * pM2._14 + pM1._22 * pM2._24 + pM1._23 * pM2._34 + pM1._24 * pM2._44;
	pOut._31 = pM1._31 * pM2._11 + pM1._32 * pM2._21 + pM1._33 * pM2._31 + pM1._34 * pM2._41;
	pOut._32 = pM1._31 * pM2._12 + pM1._32 * pM2._22 + pM1._33 * pM2._32 + pM1._34 * pM2._42;
	pOut._33 = pM1._31 * pM2._13 + pM1._32 * pM2._23 + pM1._33 * pM2._33 + pM1._34 * pM2._43;
	pOut._34 = pM1._31 * pM2._14 + pM1._32 * pM2._24 + pM1._33 * pM2._34 + pM1._34 * pM2._44;
	pOut._41 = pM1._41 * pM2._11 + pM1._42 * pM2._21 + pM1._43 * pM2._31 + pM1._44 * pM2._41;
	pOut._42 = pM1._41 * pM2._12 + pM1._42 * pM2._22 + pM1._43 * pM2._32 + pM1._44 * pM2._42;
	pOut._43 = pM1._41 * pM2._13 + pM1._42 * pM2._23 + pM1._43 * pM2._33 + pM1._44 * pM2._43;
	pOut._44 = pM1._41 * pM2._14 + pM1._42 * pM2._24 + pM1._43 * pM2._34 + pM1._44 * pM2._44;

	return pOut;
}
#define PI 3.14159265358979323846f
struct _MATRIX {
	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;

		};
		float m[4][4];
	};
};
inline _MATRIX Matrix(vector3 Vec4, vector3 origin = vector3(0, 0, 0))
{
	double radPitch = (Vec4.x * double(PI) / 180.f);
	double radYaw = (Vec4.y * double(PI) / 180.f);
	double radRoll = (Vec4.z * double(PI) / 180.f);

	double SP = sinf(radPitch);
	double CP = cosf(radPitch);
	double SY = sinf(radYaw);
	double CY = cosf(radYaw);
	double SR = sinf(radRoll);
	double CR = cosf(radRoll);

	_MATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;

	return matrix;
}

typedef struct Actors {
	uint64_t Actor;
	DWORD_PTR IsVisible;
	uint64_t SkeletalMesh;
	uint64_t PlayerState;
	char bIsKnocked;
	int TeamIndex;
}Actors;
std::vector<Actors> inline ActorList;

struct Engine {
	uintptr_t
		UnrealWorld,
		GameInstance,
		LocalPlayers, LocalPlayer,
		AcknowledgedPawn,
		PlayerState,
		RootComponent,
		PlayerController,
		ULevel,
		BoneArray,
		Actors,
		ActorArray;
	int
		TeamIndex;
	vector3
		RelativeLocation;

}; std::unique_ptr<Engine> inline Pointers = std::make_unique<Engine>();

struct CamewaDescwipsion {
	vector3 Location;
	vector3 Rotation;
	float FieldOfView;
	char Useless[0x18];
};
inline CamewaDescwipsion GetViewPoint() {
	char v1; // r8
	CamewaDescwipsion ViewPoint = driver.read<CamewaDescwipsion>(defines::base_address + 0xE9AD420);
	BYTE* v2 = (BYTE*)&ViewPoint;
	int i; // edx
	__int64 result; // rax

	v1 = 0x40;
	for (i = 0; i < 0x40; ++i)
	{
		*v2 ^= v1;
		result = (unsigned int)(i + 0x17);
		v1 += i + 0x17;
		v2++;
	}

	return ViewPoint;
}
inline float fov;
inline vector3 w2s(vector3 WorldLocation)
{
	CamewaDescwipsion vCamera = GetViewPoint();

	fov = vCamera.FieldOfView;

	_MATRIX tempMatrix = Matrix(vCamera.Rotation, vector3(0, 0, 0));
	vector3 vAxisX = vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	vector3 vAxisY = vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	vector3 vAxisZ = vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);
	vector3 vDelta = WorldLocation - vCamera.Location;
	vector3 vTransformed = vector3(vDelta.dot(vAxisY), vDelta.dot(vAxisZ), vDelta.dot(vAxisX));
	if (vTransformed.z < 1.f) vTransformed.z = 1.f;
	return vector3((screen_width / 2.0f) + vTransformed.x * (((screen_width / 2.0f) / tanf(vCamera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z, (screen_height / 2.0f) - vTransformed.y * (((screen_width / 2.0f) / tanf(vCamera.FieldOfView * (float)M_PI / 360.f))) / vTransformed.z, 0);
}

inline vector3 get_bone(uintptr_t mesh, int bone_id)
{
	uintptr_t bone_array = driver.read<uintptr_t>(mesh + 0x600);
	if (bone_array == NULL) bone_array = driver.read<uintptr_t>(mesh + 0x600 + 0x10);
	FTransform bone = driver.read<FTransform>(bone_array + (bone_id * 0x60));
	FTransform component_to_world = driver.read<FTransform>(mesh + 0x240);
	D3DMATRIX matrix = MatrixMultiplication(bone.ToMatrixWithScale(), component_to_world.ToMatrixWithScale());
	return vector3(matrix._41, matrix._42, matrix._43);
}

inline bool is_visible(uintptr_t mesh) {
	if (!mesh)
		return false;

	float last_sumbit_time = driver.read<float>(mesh + 0x360);
	float last_render_time_on_screen = driver.read<float>(mesh + 0x368);
	const float fVisionTick = 0.06f;
	bool visible = last_render_time_on_screen + fVisionTick >= last_sumbit_time;
	return visible;
}

inline std::wstring MBytesToWString(const char* lpcszString)
{
	int len = strlen(lpcszString);
	int unicodeLen = ::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, NULL, 0);
	wchar_t* pUnicode = new wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, (LPWSTR)pUnicode, unicodeLen);
	std::wstring wString = (wchar_t*)pUnicode;
	delete[] pUnicode;
	return wString;
}
inline std::string WStringToUTF8(const wchar_t* lpwcszWString)
{
	char* pElementText;
	int iTextLen = ::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, NULL, 0, NULL, NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));
	::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, pElementText, iTextLen, NULL, NULL);
	std::string strReturn(pElementText);
	delete[] pElementText;
	return strReturn;
}
inline void DrawString(float fontSize, int x, int y, ImColor color, bool bCenter, bool stroke, const char* pText, ...)
{
	va_list va_alist;
	char buf[1024] = { 0 };
	va_start(va_alist, pText);
	_vsnprintf_s(buf, sizeof(buf), pText, va_alist);
	va_end(va_alist);
	std::string text = WStringToUTF8(MBytesToWString(buf).c_str());
	if (bCenter)
	{
		ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
		x = x - textSize.x / 2;
		y = y - textSize.y;
	}
	if (stroke)
	{
		ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
	}
	ImGui::GetForegroundDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x, y), color, text.c_str());
}

inline double GetCrossDistance(double x1, double y1, double x2, double y2) {
	return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
}