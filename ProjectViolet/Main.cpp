
#pragma warning(disable:4244)
#pragma warning(disable:4018)

#define NOMINMAX

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <thread>

#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include "Main.hpp"
#include "LogSystem.hpp"
#include "App.hpp"

using namespace std;

void threadRendering() {
	static Clock imguiDeltaClock;
	Clock renderMetricsClock;
	ImGui::SFML::Update(win, imguiDeltaClock.restart());
	win.setActive(true);
	renderClock.restart();
	while (isReady) {
		if (!win.isOpen()) {
			sleep(milliseconds(5));
			continue;
		}

		renderLock.lock();

		renderMetricsClock.restart();
		win.setActive(true);
		win.clear();

		logicDataLock.lock();
		app->onRender(win);
		appRenderTime = renderMetricsClock.restart();
		app->runImGui();
		runImGuiTime = renderMetricsClock.restart();
		win.setView(View(FloatRect(0, 0, win.getSize().x, win.getSize().y)));
		ImGui::SFML::Render(win);
		imGuiRenderTime = renderMetricsClock.restart();
		ImGui::SFML::Update(win, imguiDeltaClock.restart());
		imGuiUpdateTime = renderMetricsClock.restart();
		renderThreadTickTime = appRenderTime + runImGuiTime + imGuiRenderTime + imGuiUpdateTime;
		logicDataLock.unlock();

		frameCounter++;
		win.display();

		renderLock.unlock();

		renderTime = renderClock.restart();

		if (frameCounterClock.getElapsedTime() >= seconds(1.0f)) {
			frameCounterClock.restart();
			framePerSecond = frameCounter;
			frameCounter = 0;
			win.setTitle(StringParser::toStringF("%s | Async | TPS: %d, EPS: %d, FPS: %d", projectCode.c_str(), logicTickPerSecond, eventTickPerSecond, framePerSecond));
		}
	}
}

void threadLogicUpdate(int ticksPerSecond) {
	Time tickTime;
	if (ticksPerSecond != 0)
		tickTime = seconds(1.0f / ticksPerSecond);
	else
		tickTime = Time::Zero;
	Clock logicCycleClock;
	Clock logicMetricsClock;
	desktopUpdate.restart();
	while (isReady && win.isOpen()) {
		logicDataLock.lock();
		logicMetricsClock.restart();
		app->updateLogic(win);
		logicThreadTickTime = logicMetricsClock.restart();
		logicDataLock.unlock();

		Time t;
		if (logicTickCounterClock.getElapsedTime() > seconds(1.0f)) {
			logicTickCounterClock.restart();
			logicTickPerSecond = logicTickCounter;
			logicTickCounter = 0;
		}
		logicTickCounter++;
		if ((t = logicCycleClock.getElapsedTime()) < (tickTime == Time::Zero ? renderTime : tickTime))
			sleep((tickTime == Time::Zero ? renderTime : tickTime) - t);
		logicCycleClock.restart();
	}
}

void initRenderWindow(Uint32 style = sf::Style::Default, bool useVSync = true, int framePerSecond = 120) {
	sf::ContextSettings settings;
	VideoMode desk = VideoMode::getDesktopMode();
	settings.antialiasingLevel = 0;
	win.create(style == sf::Style::Fullscreen ? desk : VideoMode(desk.width * 5 / 6, desk.height * 5 / 6), projectCode, style, settings);
	win.clear();
	win.display();
	if (useVSync)
		win.setVerticalSyncEnabled(true);
	else
		win.setFramerateLimit(framePerSecond);
	win.setVisible(true);
}


#ifdef SFML_SYSTEM_WINDOWS
#include <Windows.h>
//Platform-Depedent: Windows
BOOL systemExitEventHandler(DWORD dwCtrlType) {
	if (dwCtrlType == CTRL_C_EVENT)
		mlog << Log::Error << "[Main/EVENT] Control-C Console Exit" << dlog << Log::Info;
	else if (dwCtrlType == CTRL_BREAK_EVENT)
		mlog << Log::Error << "[Main/EVENT] Control-Break Console Exit" << dlog << Log::Info;
	else if (dwCtrlType == CTRL_CLOSE_EVENT)
		mlog << Log::Error << "[Main/EVENT] Control-Close Console Exit" << dlog << Log::Info;
	else if (dwCtrlType == CTRL_LOGOFF_EVENT)
		mlog << Log::Error << "[Main/EVENT] System-Logoff Exit" << dlog << Log::Info;
	else if (dwCtrlType == CTRL_SHUTDOWN_EVENT)
		mlog << Log::Error << "[Main/EVENT] System-Shutdown Exit" << dlog << Log::Info;
	else
		return false;
	logicDataLock.lock();
	isReady = false;
	logicDataLock.unlock();
	waitUntilEqual(isProgramRunning, false);
	return true;
}
#endif // SFML_SYSTEM_WINDOWS

int main(int argc, char* argv[]) {

	isProgramRunning = true;

	cout << projectCode << ' ' << projectSuffix << "\n  Stage " << releaseStage << endl;
	cout << "Version " << majorVersion << "." << minorVersion << "." << patchVersion << " " << releaseStage << ", Complie Time: " << compileTime << endl << endl;

	//system("PAUSE>nul");

	srand(time(NULL));

#ifdef OUTPUT_LOG_TO_STDOUT
	dlog.addOutputStream(clog);
#endif
#ifdef OUTPUT_LOG_TO_FILE
	dlog.addOutputStream(logout);
#endif
	dlog.ignore(LOG_IGNORE_LEVEL);

	Clock loadTimeClock;
	mlog << "[Main] Pre-Initalaizing..." << dlog;

	bool isFullscreen = false;

#ifdef SFML_SYSTEM_WINDOWS
	SetConsoleCtrlHandler((PHANDLER_ROUTINE)systemExitEventHandler, true);
#endif // SFML_SYSTEM_WINDOWS

	mlog << "[Main] Initalaizing ImGui..." << dlog;
	ImGui::SFML::Init();
	ImGui::StyleColorsClassic();
	ImGuiStyle& style = ImGui::GetStyle();
	style.FrameBorderSize = 1.0f;
	style.ScrollbarRounding = 2.0;
	style.WindowRounding = 2.0;

	mlog << "[Main] Initalaizing App..." << dlog;
	app = new App();
	app->initalaizePreWindow();

	mlog << "[Main] Done in " << loadTimeClock.restart().asMilliseconds() << "ms." << dlog;

	mlog << "[Main] Bringing up the window..." << dlog;
	initRenderWindow();

	mlog << Log::Info << "[Main] Post-Initalaizing..." << dlog;

	isReady = true;
	app->initalaizePostWindow(win);

	mlog << "[Main] Done in " << loadTimeClock.restart().asMilliseconds() << "ms." << dlog;

	mlog << "[Main] Starting App..." << dlog;
	app->start(win);
	mlog << "[Main] App Started in " << loadTimeClock.restart().asMilliseconds() << "ms." << dlog;

#ifdef USE_ASYNC_RENDERING
	mlog << Log::Warning << "Async Rendering/Logic Update Enabled. Unstable. Aware." << dlog << Log::Info;
	win.setActive(false);
	thread render(threadRendering);
	thread logic(threadLogicUpdate, 90);
#else
	Clock imguiDeltaClock;
#endif

	//Time eventTickTime = seconds(1.0f / 60);
	Time eventTickTime = microseconds(11111);
	Clock eventCycleClock;
	while (win.isOpen() && isReady) {

		Event event;
		while (win.pollEvent(event)) {

			logicDataLock.lock();

			ImGui::SFML::ProcessEvent(event);

			if ((ImGui::GetIO().WantCaptureKeyboard &&
				(event.type == Event::KeyPressed || event.type == Event::KeyReleased || event.type == Event::TextEntered)) ||
				 (ImGui::GetIO().WantCaptureMouse && (event.type == Event::MouseButtonPressed || event.type ==
													  Event::MouseButtonReleased || event.type == Event::MouseMoved))) {
				logicDataLock.unlock();
				continue;
			}

			app->handleEvent(win, event);
			logicDataLock.unlock();

			if (event.type == Event::Closed) {
				logicDataLock.lock();
				isReady = false;
				win.close();
				logicDataLock.unlock();
				break;
			}
			else if (event.type == Event::Resized) {
				win.setView(View(FloatRect(0, 0, event.size.width, event.size.height)));
			}
			else if (event.type == Event::KeyPressed) {
				if (event.key.code == Keyboard::F11)
					if (!isFullscreen) {
#ifdef USE_ASYNC_RENDERING
						renderLock.lock();
						logicDataLock.lock();
#endif
						initRenderWindow(sf::Style::Fullscreen);
						isFullscreen = true;
						app->onViewportChange(win);
#ifdef USE_ASYNC_RENDERING
						win.setActive(false);
						logicDataLock.unlock();
						renderLock.unlock();
#endif
					}
					else {
#ifdef USE_ASYNC_RENDERING
						renderLock.lock();
						logicDataLock.lock();
#endif
						initRenderWindow();
						isFullscreen = false;
						app->onViewportChange(win);
#ifdef USE_ASYNC_RENDERING
						win.setActive(false);
						logicDataLock.unlock();
						renderLock.unlock();
#endif
					}
			}
		}
#ifndef USE_ASYNC_RENDERING
		win.clear();
		ImGui::SFML::Update(win, imguiDeltaClock.restart());

		app->updateLogic(win);

		app->onRender(win);

		app->runImGui();

		win.setView(View(FloatRect(0, 0, win.getSize().x, win.getSize().y)));

		ImGui::SFML::Render(win);
		win.display();
#endif

		if (eventTickCounterClock.getElapsedTime() >= seconds(1.0f)) {
			eventTickCounterClock.restart();
			eventTickPerSecond = eventTickCounter;
			eventTickCounter = 0;
#ifndef USE_ASYNC_RENDERING
			win.setTitle(StringParser::toStringF("%s | Mono-Thread | FPS: %d", projectCode.c_str(), eventTickPerSecond));
#endif
		}
		eventTickCounter++;

#ifdef USE_ASYNC_RENDERING
		Time t;
		if ((t = eventCycleClock.getElapsedTime()) < (eventTickTime == Time::Zero ? renderTime : eventTickTime))
			sleep((eventTickTime == Time::Zero ? renderTime : eventTickTime) - t);
#endif
		eventCycleClock.restart();

	}
	win.close();
	mlog << "Shutdown In Progress..." << dlog;
	app->onStop();
#ifdef USE_ASYNC_RENDERING
	mlog << "[*] Joining Render Thread..." << dlog;
	render.join();
	mlog << "[*] Joining Logic Update Thread..." << dlog;
	logic.join();
	mlog << "Complete." << dlog;
#endif
	mlog << "Byebye!" << dlog;

	sleep(milliseconds(500));
	isProgramRunning = false;

	return EXIT_SUCCESS;
}


#include "ImplList.hpp"
