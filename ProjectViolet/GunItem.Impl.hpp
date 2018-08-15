#pragma once

#include "GunItem.hpp"
#include "BulletEntity.hpp"
#include "TextSystem.hpp"


////////////////////////////////////////
void GunItem::_onShoot() {
	BulletEntity::shoot(bulletDamage(), bulletSpeed(), bulletKnockbackFactor(), localPlayer->getEyePosition(),
		gameIO.degreeAngle + (rand01() - 0.5)*bulletUnaccuracyDegree());
}


////////////////////////////////////////
void GunItem::updateLogic() {
	shootCooldownMill() += logicIO.deltaTime.asMilliseconds();
	if (shootCooldownMill() > shootInterval().asMilliseconds())
		shootCooldownMill() = shootInterval().asMilliseconds();
	if (isShooting()) {
		if (shootCooldownMill() >= shootInterval().asMilliseconds()) {
			if (roundsLeft() > 0) {
				shootCooldownMill() = 0;
				roundsLeft()--;
				_onShoot();
			}
		}
	}
	if (!isReloading() && isInHand()) {
		if (logicIO.keyboardState[Keyboard::R] == LogicIO::JustPressed) {
			bool ok = false;
			for (int i = 0; i < 4; i++)
				for (int j = 0; j < 9; j++) {
					auto& slot = localPlayer->getDataset();
					if (slot[to_string(i) + to_string(j) + "item_name"].getDataString() == magazineItemName()) {
						slot[to_string(i) + to_string(j) + "count"].getDataInt()--;
						if (slot[to_string(i) + to_string(j) + "count"].getDataInt() <= 0)
							slot[to_string(i) + to_string(j) + "item_name"].getDataString() = "";
						ok = true;
						break;
					}
				}
			if (ok) {
				roundsLeft() = 0;
				reloadedTimeMill() = 0;
				isReloading() = true;
			}
		}
	}
	if (isReloading()) {
		reloadedTimeMill() += logicIO.deltaTime.asMilliseconds();
		if (reloadedTimeMill() >= reloadTime().asMilliseconds()) {
			reloadedTimeMill() = reloadTime().asMilliseconds();
			isReloading() = false;
			roundsLeft() = roundsPerMagazine();
		}
	}
}


////////////////////////////////////////
void GunItem::_pushExtraImguiItemsToDashboard() {
	if (isReloading()) {
		imgui::Text(text.get("gunhud.reloading"),
			reloadedTimeMill() * 100 / reloadTime().asMilliseconds());
		imgui::PushStyleColor(ImGuiCol_PlotHistogram, Color(240, 240, 240));
		imgui::ProgressBar((float)(reloadedTimeMill()) / reloadTime().asMilliseconds(),
			ImVec2(-1, 12), "");
		imgui::PopStyleColor();
	}
	else {
		imgui::Text(text.get("gunhud.ammo"), roundsLeft(), roundsPerMagazine());
		imgui::PushStyleColor(ImGuiCol_PlotHistogram, Color(240 - 240 * (float)roundsLeft() / roundsPerMagazine(),
															0,
															240 * (float)roundsLeft() / roundsPerMagazine()));
		imgui::ProgressBar((float)roundsLeft() / roundsPerMagazine(), ImVec2(-1, 12), "");
		imgui::PopStyleColor();
	}
	// Display load progress bar
	imgui::PushStyleColor(ImGuiCol_PlotHistogram, Color(192, 192, 192));
	imgui::ProgressBar(roundsLeft() != 0 ?
		min(1.0f, (float)shootCooldownMill() / shootInterval().asMilliseconds()) :
		.0f,
		ImVec2(-1, 4), "");
	imgui::PopStyleColor();
}


////////////////////////////////////////
bool GunItem::_onRightPressed() {
	isShooting() = true;

	return true;
}


////////////////////////////////////////
void GunItem::_onRightReleased() {
	isShooting() = false;
}

