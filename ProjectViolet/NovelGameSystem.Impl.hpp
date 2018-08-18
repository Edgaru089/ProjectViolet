#pragma once

#include "NovelGameSystem.hpp"
#include "IO.hpp"
#include "AssetManager.hpp"


////////////////////////////////////////
void NovelGameParagraph::loadFromScriptFile(const string& filename) {
	valid = false;
	ifstream fin(filename);
	string str;
	if (fin.bad()) {
		mlog << Log::Error << "[NovelGameParagraph] File \"" << filename << "\" open failed" << dlog;
		return;
	}
	// UTF-8 Signature (EF BB BF)
	if (fin.get() == 0xEF)
		fin.ignore(2);
	else
		fin.unget();
	while (!getline(fin, str).eof()) {
		if (fin.bad()) {
			mlog << Log::Error << "[NovelGameParagraph] File \"" << filename << "\": Bad IO stream" << dlog;
			return;
		}
		if (str == "")
			continue;
		size_t pos = str.find('#');
		if (pos == string::npos)
			commands.push_back(make_pair(str, ""));
		else
			commands.push_back(make_pair(str.substr(0, pos), str.substr(pos + 1)));
	}
	valid = true;
}


////////////////////////////////////////
void NovelGameParagraph::start() {
	if (!valid)
		return;
	running = true;

	nextCommand = 0;
	wantUpdateClick = false;
	runUntilPause();
}


////////////////////////////////////////
void NovelGameParagraph::click() { wantUpdateClick = true; }


////////////////////////////////////////
void NovelGameParagraph::runUntilPause() {
	while (running) {
		if (nextCommand >= commands.size()) {
			running = false;
			break;
		}

		auto comm = toupper(commands[nextCommand].first);
		auto& param = commands[nextCommand].second;
		if (comm == "TEXT" || comm == "TX") {
			text = u8ToSfString(StringParser::replaceSubString(param, { { "\\n", "\n" } }));
			stringCharLastAppearTime = milliseconds(0);
			stringTruncatedCnt = 0;
		}
		else if (comm == "TEXTPAUSE" || comm == "TP" || comm == "") {
			text = u8ToSfString(StringParser::replaceSubString(param, { { "\\n", "\n" } }));
			stringCharLastAppearTime = milliseconds(0);
			stringTruncatedCnt = 0;
			nextCommand++;
			break;
		}
		else if (comm == "TITLE" || comm == "TI")
			title = u8ToSfString(param);
		else if (comm == "SPEAKER" || comm == "SP" || comm == "IMAGE" || comm == "IM") {
			if (param == "")
				speaker = nullptr;
			else {
				if (speaker == nullptr)
					speaker = make_shared<Texture>();
				speaker->loadFromFile(assetManager.getAssetFilename(param));
			}
		}
		else if (comm == "SPEAKERFILE" || comm == "IMAGEFILE") {
			if (param == "")
				speaker = nullptr;
			else {
				if (speaker == nullptr)
					speaker = make_shared<Texture>();
				speaker->loadFromFile(param);
			}
		}
		else if (comm == "PAUSE") {
			nextCommand++;
			break;
		}

		nextCommand++;
	}
}


////////////////////////////////////////
void NovelGameParagraph::updateLogic() {
	if (!running)
		return;

	if (wantUpdateClick) {
		wantUpdateClick = false;
		runUntilPause();
		stringCharLastAppearTime = milliseconds(0);
		stringTruncatedCnt = 0;
	}

	stringCharLastAppearTime += logicIO.deltaTime;
	if (stringTruncatedCnt < text.getSize() &&
		stringCharLastAppearTime >= stringCharAppearTime) {
		stringCharLastAppearTime = milliseconds(0);
		stringTruncatedCnt++;
	}
}


////////////////////////////////////////
void NovelGameParagraph::runImGui() {
	if (!running)
		return;
	if (font != nullptr)
		imgui::PushFont(font);
	imgui::PushStyleVar(ImGuiStyleVar_WindowRounding, 5.0f);
	imgui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(15.0f, 15.0f));
	imgui::PushStyleColor(ImGuiCol_WindowBg, Color(0, 0, 0, 128));

	imgui::SetNextWindowPos(ImVec2(imgui::GetIO().DisplaySize.x / 2.0f, imgui::GetIO().DisplaySize.y - 10.0f),
							ImGuiCond_Always, ImVec2(.5f, 1.0f));
	imgui::SetNextWindowSize(ImVec2(imgui::GetIO().DisplaySize.x - 20.0f, dialogueHeight), ImGuiCond_Always);
	imgui::Begin("##NovelgameMain", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);

	imgui::TextUnformatted(reinterpret_cast<const char*>(text.substring(0, stringTruncatedCnt).toUtf8().c_str()));

	if (imgui::IsWindowHovered() && logicIO.mouseState[Mouse::Left] == LogicIO::JustPressed) {
		click();
	}

	imgui::End();

	if (title != "") {
		imgui::PopStyleVar();
		imgui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10.0f, 10.0f));
		imgui::SetNextWindowPos(ImVec2(10.0f, imgui::GetIO().DisplaySize.y - 10.0f - dialogueHeight - 5.0f),
								ImGuiCond_Always, ImVec2(.0f, 1.0f));
		imgui::Begin("##NovelgameTitle", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoBringToFrontOnFocus);
		imgui::TextUnformatted(reinterpret_cast<const char*>(title.toUtf8().c_str()));
		imgui::End();
	}

	imgui::PopStyleVar(2);
	imgui::PopStyleColor();

	if (speaker != nullptr) {
		ImVec2 size = imgui::GetIO().DisplaySize;
		imgui::PushStyleVar(ImGuiStyleVar_WindowRounding, .0f);
		imgui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, .0f);
		imgui::SetNextWindowBgAlpha(.0f);
		imgui::SetNextWindowPos(size, ImGuiCond_Always, ImVec2(1.0f, 1.0f));
		imgui::SetNextWindowSize(speaker->getSize());
		imgui::Begin("##SpeakerOverlay", nullptr,
					 ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoResize);
		imgui::GetWindowDrawList()->AddImage((ImTextureID)speaker->getNativeHandle(),
											 ImVec2(size.x - speaker->getSize().x, size.y - speaker->getSize().y),
											 size);
		imgui::End();
		imgui::PopStyleVar(2);
	}
	if (font != nullptr)
		imgui::PopFont();
}


////////////////////////////////////////
void NovelGameSystem::preInitalaize() {
	mlog << "[NovelGameSystem] Loading Novelgame Font..." << dlog;

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
	static ImVector<ImWchar> range;
	range.clear();
	builder.BuildRanges(&range);

	ImFontConfig config;
	config.PixelSnapH = 1;
	config.RasterizerMultiply = 1.2f;
	config.OversampleH = 3;
	config.OversampleV = 1.5;

	font = imgui::GetIO().Fonts->AddFontFromFileTTF(
		assetManager.getAssetFilename("novelgame_font").c_str(),
		/*"simsun.ttc",*/
		28.0f,
		&config,
		range.Data
	);

	//font = imgui::GetIO().Fonts->AddFontFromFileTTF(
	//	assetManager.getAssetFilename("novelgame_font").c_str(),
	//	28.0f,
	//	&config,
	//	imgui::GetIO().Fonts->GetGlyphRangesChineseSimplifiedCommon()
	//);

	mlog << "[NovelGameSystem] Loading Novelgame Scripts..." << dlog;
	for (auto& i : assetManager.getAssetMapper()) {
		if (i.second.type == "NOVELGAME_SCRIPT") {
			const string& id = i.second.strid, filename = i.second.filename;
			size_t pos = id.find('_');
			if (pos != string::npos)
				loadScriptFromFile(filename, id.substr(pos + 1));
			else
				loadScriptFromFile(filename, id);
		}
	}
}


////////////////////////////////////////
void NovelGameSystem::loadScriptFromFile(const string& filename, const string& id) {
	mlog << "[NovelGameSystem] Loading paragraph \"" << id << "\" from file \"" << filename << "\"" << dlog;
	if (paragraphs.find(id) != paragraphs.end())
		mlog << Log::Warning << "[NovelGameSystem] Warning: Paragraph with id \"" << id << "\" already exists, skipping" << dlog;
	auto i = paragraphs.insert(make_pair(id, NovelGameParagraph())).first;
	auto& para = i->second;
	para.loadFromScriptFile(filename);
	if (!para.valid) {
		mlog << Log::Error << "[NovelGameSystem] Paragraph returned error upon loading" << dlog;
		paragraphs.erase(i);
	}
	para.strid = id;
	para.font = font;
	mlog << "[NovelGameSystem] Loaded " << para.commands.size() << " commands" << dlog;
}


////////////////////////////////////////
void NovelGameSystem::start(const string& id) {
	auto i = paragraphs.find(id);
	if (i == paragraphs.end()) {
		mlog << Log::Error << "[NovelGameSystem] When starting: Paragraph with id \"" << id << "\" not found" << dlog;
		return;
	}
	i->second.start();
	current = &(i->second);
}


////////////////////////////////////////
void NovelGameSystem::updateLogic() {
	if (current != nullptr) {
		current->updateLogic();
		if (!current->running)
			current = nullptr;
	}
}


////////////////////////////////////////
void NovelGameSystem::runImGui() {
	if (current != nullptr)
		current->runImGui();
}


////////////////////////////////////////
string NovelGameSystem::getCurrentParagraphId() {
	if (current == nullptr)
		return "";
	else
		return current->strid;
}

