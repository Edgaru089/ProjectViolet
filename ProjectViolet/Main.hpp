#pragma once

#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <atomic>
#include <vector>
#include <queue>
#include <mutex>
#include <list>
#include <map>
#include <set>

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#ifdef SFML_SYSTEM_WINDOWS
#include <Windows.h>
#endif

#include "ImGui/imgui-setup.h"

#include "LogSystem.hpp"
#include "Uuid.hpp"
#include "Lockable.hpp"
#include "ThreadPool.hpp"

#define USING_NAMESPACE \
using namespace std;\
using namespace sf;

USING_NAMESPACE;

// Compile Flags
//#define USE_DEFAULT_WINMAIN_ENTRYPOINT
#define USE_ASYNC_RENDERING
//#define USE_DISCRETE_GPU

#define OUTPUT_LOG_TO_STDOUT
//#define OUTPUT_LOG_TO_FILE
#define LOG_FILENAME "latest.log"
#define LOG_IGNORE_LEVEL -1

//Marcos & Typedefs
#define var auto
#define AUTOLOCK(a) lock_guard<mutex> __mutex_lock(a)
#define AUTOLOCKTYPE(type, a) lock_guar1d<type> __type_lock(a)
#define AUTOLOCKABLE(a) lock_guard<Lockable> __lockable_lock(a)
#define AUTOLOCKABLE_NAMED(a, name) lock_guard<Lockable> name(a)
#define AUTOLOCK_ALL_SYSTEM \
AUTOLOCKABLE_NAMED(terrainManager, tml);\
AUTOLOCKABLE_NAMED(particleSystem, psl);\
AUTOLOCKABLE_NAMED(entityManager, eml)
#define AUTOPTR(type) shared_ptr<type>
typedef Vector2<double> Vector2d;
typedef sf::Rect<Uint32> UintRect;
typedef sf::Rect<double> DoubleRect;

//Constants
const int majorVersion = 0, minorVersion = 0, patchVersion = 0;
const string projectCode = "Project Violet", projectSuffix = "Internal Release", releaseStage = "Alpha";
const string compileTime = string(__DATE__) + " " + string(__TIME__);

const double PI = 3.1415926535897932385;
const double eps = 3e-6;

//Resources
RenderWindow win;
#ifdef OUTPUT_LOG_TO_FILE
ofstream logout(LOG_FILENAME);
#endif // OUTPUT_LOG_TO_FILE

//Locks & Mutexs
mutex renderLock, logicDataLock;

//Global Variables
atomic_bool isProgramRunning;
int logicTickPerSecond, logicTickCounter, framePerSecond, frameCounter, eventTickPerSecond, eventTickCounter;
Clock logicTickCounterClock, frameCounterClock, eventTickCounterClock;
Time logicThreadTickTime;
Time appRenderTime, runImGuiTime, imGuiRenderTime, imGuiUpdateTime, renderThreadTickTime;
Clock renderClock;
Time renderTime = microseconds(16667);
atomic_bool isReady;
mt19937 randomEngine((random_device())());

Clock desktopUpdate;

//Utilities
const String operator""S(const char* c, std::size_t l) { return String::fromUtf8(c, c + l); }
String u8ToSfString(string u8string) { return String::fromUtf8(u8string.begin(), u8string.end()); }
String u8ToSfString(char* u8string) { return String::fromUtf8(u8string, u8string + strlen(u8string)); }

const double sqr(double x) { return x * x; }

const double getDisSquared(double posX1, double posY1, double posX2, double posY2) {
	return fabs(posX1 - posX2)*fabs(posX1 - posX2) + fabs(posY1 - posY2)*fabs(posY1 - posY2);
}

const double getDisSquared(Vector2d posX, Vector2d posY) {
	return getDisSquared(posX.x, posX.y, posY.x, posY.y);
}

const double getDis(double posX1, double posY1, double posX2, double posY2) {
	return sqrt(getDisSquared(posX1, posY1, posX2, posY2));
}

const double getDis(Vector2d posX, Vector2d posY) {
	return getDis(posX.x, posX.y, posY.x, posY.y);
}

const bool isSame(double x, double y) {
	if (abs(x - y) < eps)
		return true;
	else
		return false;
}

const double getAngle(Vector2d begin, Vector2d end) {
	return atan((begin.y - end.y) / (begin.x - end.x)) / PI * 180.0;
}

template<class Type>
void resetVector2D(vector<vector<Type>>& vec, int Xsize, int Ysize, Type value) {
	vec.resize(Xsize);
	for (vector<Type>& i : vec) {
		i.resize(Ysize, value);
	}
}

template<typename Type1, typename Type2>
void waitUntilEqual(const Type1& val, const Type2 equal) {
	while (val != equal) sleep(milliseconds(2));
}

template<typename Type>
class Vector2Less {
public:
	const bool operator() (const Vector2<Type>& x, const Vector2<Type>& y) const {
		if (x.x < y.x)
			return true;
		else if (x.x == y.x&&x.y < y.y)
			return true;
		else
			return false;
	}
};

template<typename Type>
VertexArray renderRect(Rect<Type> rect, Color color = Color::White) {
	VertexArray arr;
	arr.append(Vertex(Vector2f(rect.left, rect.top), color));
	arr.append(Vertex(Vector2f(rect.left + rect.width, rect.top), color));
	arr.append(Vertex(Vector2f(rect.left + rect.width, rect.top + rect.height), color));
	arr.append(Vertex(Vector2f(rect.left, rect.top + rect.height), color));
	arr.append(Vertex(Vector2f(rect.left, rect.top), color));
	arr.setPrimitiveType(PrimitiveType::LinesStrip);
	return arr;
}

template<typename RectType, typename ValType>
Rect<RectType> operator * (Rect<RectType> rect, ValType val) {
	return Rect<RectType>(rect.left*val, rect.top*val, rect.width*val, rect.height*val);
}

double rand01() {
	return uniform_real_distribution<double>(0.0, 1.0)(randomEngine);
}

// [x, y]
int rand(int x, int y) {
	return uniform_int_distribution<int>(x, y)(randomEngine);
}

template<typename Type1, typename Type2>
Type2 min(const Type1& x, const Type2& y) { if (x < y)return x; else return y; }

template<typename Type1, typename Type2>
Type2 max(const Type1& x, const Type2& y) { if (x > y)return x; else return y; }

string toupper(const string& str) { string copy = str; for (char& c : copy)c = toupper(c); return copy; }


#define ENTER_IMGUI_DEBUG \
imgui::PushStyleColor(ImGuiCol_ChildBg, Color(0, 0, 0, 0));\
imgui::PushStyleColor(ImGuiCol_FrameBg, Color(0, 0, 0, 0));\
imgui::PushStyleColor(ImGuiCol_FrameBgActive, Color(0, 0, 0, 0));\
imgui::PushStyleColor(ImGuiCol_FrameBgHovered, Color(0, 0, 0, 0));\
imgui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);\
imgui::SetNextWindowPos(ImVec2(-1, -1), ImGuiCond_Always);\
imgui::SetNextWindowSize(Vector2f(win.getView().getSize()), ImGuiCond_Always);\
imgui::SetNextWindowBgAlpha(0);\
imgui::Begin("Debug", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|\
ImGuiWindowFlags_NoMove| ImGuiWindowFlags_NoScrollbar| ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoInputs)

#define LEAVE_IMGUI_DEBUG \
imgui::End();\
imgui::PopStyleColor(4);\
imgui::PopStyleVar()

const ImWchar* getGlyphRangesCustomChinese() {
	static ImVector<ImWchar> range;
	if (range.size() == 0) {
		ImFontAtlas::GlyphRangesBuilder builder;
		builder.AddRanges(imgui::GetIO().Fonts->GetGlyphRangesChineseSimplifiedCommon());
		builder.AddText(   // 教育部于1988年发布的1000次常用字，与2500常用字↑共同覆盖了约99.48%的现代汉语汉字
u8R"(匕刁丐歹戈夭仑讥冗邓艾夯凸卢叭叽皿凹囚矢乍尔冯玄邦迂邢芋芍吏夷吁吕吆屹廷迄臼仲伦伊肋旭匈凫妆亥汛讳讶讹讼诀弛阱驮驯纫玖玛韧抠扼汞扳抡坎坞抑拟抒
芙芜苇芥芯芭杖杉巫杈甫匣轩卤肖吱吠呕呐吟呛吻吭邑囤吮岖牡佑佃伺囱肛肘甸狈鸠彤灸刨庇吝庐闰兑灼沐沛汰沥沦汹沧沪忱诅诈罕屁坠妓姊妒纬玫卦坷坯拓坪坤拄拧拂
拙拇拗茉昔苛苫苟苞茁苔枉枢枚枫杭郁矾奈奄殴歧卓昙哎咕呵咙呻咒咆咖帕账贬贮氛秉岳侠侥侣侈卑刽刹肴觅忿瓮肮肪狞庞疟疙疚卒氓炬沽沮泣泞泌沼怔怯宠宛衩祈诡帚
屉弧弥陋陌函姆虱叁绅驹绊绎契贰玷玲珊拭拷拱挟垢垛拯荆茸茬荚茵茴荞荠荤荧荔栈柑栅柠枷勃柬砂泵砚鸥轴韭虐昧盹咧昵昭盅勋哆咪哟幽钙钝钠钦钧钮毡氢秕俏俄俐侯
徊衍胚胧胎狰饵峦奕咨飒闺闽籽娄烁炫洼柒涎洛恃恍恬恤宦诫诬祠诲屏屎逊陨姚娜蚤骇耘耙秦匿埂捂捍袁捌挫挚捣捅埃耿聂荸莽莱莉莹莺梆栖桦栓桅桩贾酌砸砰砾殉逞哮
唠哺剔蚌蚜畔蚣蚪蚓哩圃鸯唁哼唆峭唧峻赂赃钾铆氨秫笆俺赁倔殷耸舀豺豹颁胯胰脐脓逛卿鸵鸳馁凌凄衷郭斋疹紊瓷羔烙浦涡涣涤涧涕涩悍悯窍诺诽袒谆祟恕娩骏琐麸琉
琅措捺捶赦埠捻掐掂掖掷掸掺勘聊娶菱菲萎菩萤乾萧萨菇彬梗梧梭曹酝酗厢硅硕奢盔匾颅彪眶晤曼晦冕啡畦趾啃蛆蚯蛉蛀唬啰唾啤啥啸崎逻崔崩婴赊铐铛铝铡铣铭矫秸秽
笙笤偎傀躯兜衅徘徙舶舷舵敛翎脯逸凰猖祭烹庶庵痊阎阐眷焊焕鸿涯淑淌淮淆渊淫淳淤淀涮涵惦悴惋寂窒谍谐裆袱祷谒谓谚尉堕隅婉颇绰绷综绽缀巢琳琢琼揍堰揩揽揖彭
揣搀搓壹搔葫募蒋蒂韩棱椰焚椎棺榔椭粟棘酣酥硝硫颊雳翘凿棠晰鼎喳遏晾畴跋跛蛔蜒蛤鹃喻啼喧嵌赋赎赐锉锌甥掰氮氯黍筏牍粤逾腌腋腕猩猬惫敦痘痢痪竣翔奠遂焙滞
湘渤渺溃溅湃愕惶寓窖窘雇谤犀隘媒媚婿缅缆缔缕骚瑟鹉瑰搪聘斟靴靶蓖蒿蒲蓉楔椿楷榄楞楣酪碘硼碉辐辑频睹睦瞄嗜嗦暇畸跷跺蜈蜗蜕蛹嗅嗡嗤署蜀幌锚锥锨锭锰稚颓
筷魁衙腻腮腺鹏肄猿颖煞雏馍馏禀痹廓痴靖誊漓溢溯溶滓溺寞窥窟寝褂裸谬媳嫉缚缤剿赘熬赫蔫摹蔓蔗蔼熙蔚兢榛榕酵碟碴碱碳辕辖雌墅嘁踊蝉嘀幔镀舔熏箍箕箫舆僧孵
瘩瘟彰粹漱漩漾慷寡寥谭褐褪隧嫡缨撵撩撮撬擒墩撰鞍蕊蕴樊樟橄敷豌醇磕磅碾憋嘶嘲嘹蝠蝎蝌蝗蝙嘿幢镊镐稽篓膘鲤鲫褒瘪瘤瘫凛澎潭潦澳潘澈澜澄憔懊憎翩褥谴鹤憨
履嬉豫缭撼擂擅蕾薛薇擎翰噩橱橙瓢蟥霍霎辙冀踱蹂蟆螃螟噪鹦黔穆篡篷篙篱儒膳鲸瘾瘸糙燎濒憾懈窿缰龍壕藐檬檐檩檀礁磷瞭瞬瞳瞪曙蹋蟋蟀嚎赡镣魏簇儡徽爵朦臊鳄
糜癌懦豁臀藕藤瞻嚣鳍癞瀑襟璧戳攒孽蘑藻鳖蹭蹬簸簿蟹靡癣羹鬓攘蠕巍鳞糯譬霹躏髓蘸镶瓤矗…)"); // ???字符集里没有省略号?
		builder.AddText(u8R"(霰狙洩鵺)"); //看到没有的字就往这里加进去吧
		builder.BuildRanges(&range);
	}
	return range.Data;
}

//void DebugSliderInt(int& val, char name[], int min = 0, int max = 100) {
//	imgui::Begin("Debug Variables");
//	imgui::SliderInt(name, &val, min, max);
//	imgui::End();
//}
//
//int DebugSliderInt(char name[], int fallback = 0, int min = 0, int max = 100) {
//	DebugSliderInt(fallback, name, min, max); return fallback;
//}
//
//void DebugSliderFloat(float& val, char name[], float min = 0, float max = 100) {
//	imgui::Begin("Debug Variables");
//	imgui::SliderFloat(name, &val, min, max);
//	imgui::End();
//}
//
//float DebugSliderFloat(char name[], float fallback = 0, float min = 0, float max = 100) {
//	DebugSliderFloat(fallback, name, min, max); return fallback;
//}
//
//void DebugSliderDouble(double& val, char name[], double min = 0, double max = 100) {
//	imgui::Begin("Debug Variables");
//	float v = val;
//	imgui::SliderFloat(name, &v, min, max);
//	val = v;
//	imgui::End();
//}
//
//double DebugSliderDouble(char name[], double fallback = 0, double min = 0, double max = 100) {
//	DebugSliderDouble(fallback, name, min, max); return fallback;
//}


// Platform-Depedent: Windows
#ifdef SFML_SYSTEM_WINDOWS

/*
const string getEnvironmentVariable(string name) {
	char buffer[2048];
	int size;
	size = GetEnvironmentVariable(name.c_str(), buffer, 2048);
	return size == 0 ? ""s : string(buffer);
}

void startProcess(string command, string workingDir = "") {
	LogMessage() << "Calling external runtime: " << command << dlog;

	if (workingDir == "") {
		size_t pos = command.find_last_of('\\');
		if (pos == string::npos)
			workingDir = "";
		else
			workingDir = command.substr(0, pos + 1);
	}

	mlog << "Runtime working directory: " << workingDir << dlog;

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Start the child process.
	if (!CreateProcess(NULL,                  // No module name (use command line)
		const_cast<LPTSTR>(command.c_str()), // Command line
		NULL,                                // Process handle not inheritable
		NULL,                                // Thread handle not inheritable
		FALSE,                               // Set handle inheritance to FALSE
		DETACHED_PROCESS,                    // Detach console if possible
		NULL,                                // Use parent's environment block
		workingDir == "" ? NULL : workingDir.c_str(),
		&si,                                 // Pointer to STARTUPINFO structure
		&pi)                                 // Pointer to PROCESS_INFORMATION structure
		) {
		mlog << Log::Error << "CreateProcess failed (" << (int)GetLastError() << ")." << dlog;
		return;
	}

	// Close process and thread handles.
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

const float getHighDpiScaleFactor() {
	UINT dpi = 96;
	DPI_AWARENESS dpiAwareness = GetAwarenessFromDpiAwarenessContext(GetThreadDpiAwarenessContext());
	switch (dpiAwareness) {
		// Scale the window to the system DPI
	case DPI_AWARENESS_SYSTEM_AWARE:
		dpi = GetDpiForSystem();
		break;

		// Scale the window to the monitor DPI
	case DPI_AWARENESS_PER_MONITOR_AWARE:
		dpi = GetDpiForWindow(win.getSystemHandle());
		break;
	}

	return dpi / 96.0f;
}
*/

////////////////////////////////////////////////////////////
// Inform the Nvidia/AMD driver that this SFML application
// could benefit from using the more powerful discrete GPU
////////////////////////////////////////////////////////////
#ifdef USE_DISCRETE_GPU
extern "C"
{
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}
#endif // USE_DISCRETE_GPU

extern int main(int argc, char* argv[]);

////////////////////////////////////////////////////////////
// Windows specific: we define the WinMain entry point,
// so that developers can use the standard main function
// even in a Win32 Application project, and thus keep a
// portable code
////////////////////////////////////////////////////////////
#ifdef USE_DEFAULT_WINMAIN_ENTRYPOINT
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, INT) {
	return main(__argc, __argv);
}
#endif // USE_DEFAULT_WINMAIN_ENTRYPOINT

#endif // SFML_SYSTEM_WINDOWS


#include "App.hpp"


App* app;
