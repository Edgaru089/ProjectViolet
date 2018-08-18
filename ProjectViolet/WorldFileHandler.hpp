#pragma once

#include "Main.hpp"
#include "IO.hpp"
#include "TerrainManager.hpp"

class WorldFileHandler {
public:

	static bool loadFromFile(const string& filename, TerrainManager& terrain, EntityManager& entity) {
		mlog << "[WorldFileHandler] Loading World to File: " << filename << dlog;

		terrain.clearChunks();
		entity.entities.clear();

		mlog << "[WorldFileHandler] Loading Terrain..." << dlog;
		ifstream tin(filename + ".terrain");
		if (!tin) {
			mlog << Log::Error << "[WorldFileHandler] Unable to open file." << dlog;
			logicIO.lastError = "File Open Failed.";
			return false;
		}
		int chunkcnt;
		int sx, sy;
		tin >> sx >> sy >> chunkcnt;
		terrain.setChunkCount(Vector2i(sx, sy));
		for (int i = 1; i <= chunkcnt; i++) {
			int x, y;
			tin >> x >> y;
			auto c = terrain.loadEmptyChunk(Vector2i(x, y));
			string line;
			for (int i = 0; i < chunkSize; i++)
				for (int j = 0; j < chunkSize; j++) {
					do {
						getline(tin, line);
					} while (line == "");
					string blockid;
					int k = 0;
					while (line[k] != ',') {
						blockid.push_back(line[k]);
						k++;
					}
					k++;
					auto b = (c->blocks[i][j] = blockAllocator.allocate(blockid));
					if (b != nullptr) {
						b->setInChunkPosition(Vector2i(i, j));
						b->setChunkId(Vector2i(x, y));
						deserializeDataset(line.substr(k), b->getDataset());
					}
				}
		}
		terrain._updateLighting();
		tin.close();

		mlog << "[WorldFileHandler] Loading Entities..." << dlog;
		ifstream ein(filename + ".entity");
		if (!ein) {
			mlog << Log::Error << "[WorldFileHandler] Unable to open file." << dlog;
			logicIO.lastError = "File Open Failed.";
			return false;
		}
		int cnt;
		Uuid playerUuid;
		ein >> cnt;
		ein >> playerUuid.sc1 >> playerUuid.sc2 >> playerUuid.sc3 >> playerUuid.sc4 >> playerUuid.sc5_high2 >> playerUuid.sc5_low4;
		string line;
		for (int i = 1; i <= cnt; i++) {
			do {
				getline(ein, line);
			} while (line == "");
			istringstream is(line);
			Uuid uid;
			string eid;
			Vector2d pos, vec;
			is >> uid.sc1 >> uid.sc2 >> uid.sc3 >> uid.sc4 >> uid.sc5_high2 >> uid.sc5_low4;
			is >> eid >> pos.x >> pos.y >> vec.x >> vec.y;
			auto e = entityAllocator.allocate(eid);
			if (e != nullptr) {
				e->setPosition(pos);
				e->setVelocity(vec);
				e->setUuid(uid);
				e->alive = true;
				deserializeDataset(line.substr(line.find('{')), e->getDataset());
				entity.insert(uid, e);
			}
		}
		shared_ptr<PlayerEntity> player;
		if ((player = dynamic_pointer_cast<PlayerEntity>(entity.getEntity(playerUuid))) != nullptr)
			localPlayer = player;

		mlog << "[WorldFileHandler] World Loaded." << dlog;
		return true;
	}


	static bool saveToFile(const string& filename, TerrainManager& terrain, EntityManager& entity) {
		mlog << "[WorldFileHandler] Saving World to File: " << filename << dlog;

		mlog << "[WorldFileHandler] Saving Terrain..." << dlog;
		ofstream tout(filename + ".terrain");
		if (!tout) {
			mlog << Log::Error << "[WorldFileHandler] Unable to open file." << dlog;
			logicIO.lastError = "File Open Failed.";
			return false;
		}
		tout << terrain.chunkCount.x << ' ' << terrain.chunkCount.y << ' ' << terrain.chunks.size() << '\n';
		for (auto& i : terrain.chunks) {
			Vector2i cc = i.first;
			shared_ptr<Chunk> c = i.second;
			tout << cc.x << " " << cc.y << '\n';
			for (int i = 0; i < chunkSize; i++) {
				for (int j = 0; j < chunkSize; j++) {
					if (c->blocks[i][j] != nullptr)
						tout << c->blocks[i][j]->getBlockId() << ", " << serializeDataset(c->blocks[i][j]->getDataset()) << '\n';
					else
						tout << "null, {}" << '\n';
				}
			}
			tout << '\n';
		}
		tout.close();

		mlog << "[WorldFileHandler] Saving Entities..." << dlog;
		ofstream eout(filename + ".entity");
		if (!eout) {
			mlog << Log::Error << "[WorldFileHandler] Unable to open file." << dlog;
			logicIO.lastError = "File Open Failed.";
			return false;
		}
		eout << setprecision(16);
		eout << entity.entities.size() << '\n';
		eout << localPlayer->getUuid().sc1 << ' ' << localPlayer->getUuid().sc2 << ' '
			<< localPlayer->getUuid().sc3 << ' ' << localPlayer->getUuid().sc4 << ' '
			<< localPlayer->getUuid().sc5_high2 << ' ' << localPlayer->getUuid().sc5_low4 << '\n';
		for (auto& i : entity.entities) {
			eout << i.first.sc1 << ' ' << i.first.sc2 << ' ' << i.first.sc3 << ' ' << i.first.sc4 << ' ' << i.first.sc5_high2 << ' ' << i.first.sc5_low4 << ' ';
			eout << i.second->getEntityId() << ' ' << i.second->getPosition().x << ' ' << i.second->getPosition().y << ' ' << i.second->getVelocity().x << ' ' << i.second->getVelocity().y << ' ';
			eout << serializeDataset(i.second->getDataset()) << '\n';
		}
		eout.close();

		mlog << "[WorldFileHandler] World Saved." << dlog;
		return true;
	}

	static string serializeDataset(Dataset& dataset) {
		string ans = "";

		ans.append("{");

		for (auto& i : dataset.getDatasets()) {
			ans.append("[");
			ans.append(i.first);
			ans.append(", ");
			if (i.second.getType() == Data::Bool)
				ans.append("bool");
			else if (i.second.getType() == Data::Integer)
				ans.append("int");
			else if (i.second.getType() == Data::String)
				ans.append("string");
			else if (i.second.getType() == Data::Uuid)
				ans.append("uuid");
			ans.append(", $");
			if (i.second.getType() == Data::Bool)
				ans.append(to_string((int)i.second.getDataBool()));
			else if (i.second.getType() == Data::Integer)
				ans.append(to_string(i.second.getDataInt()));
			else if (i.second.getType() == Data::String) {
				ans.push_back('\"');
				ans.append(i.second.getDataString());
				ans.push_back('\"');
			}
			else if (i.second.getType() == Data::Uuid) {
				Uuid id = i.second.getDataUuid();
				ans.append(to_string((long long)id.sc1));
				ans.push_back(' ');
				ans.append(to_string((long long)id.sc2));
				ans.push_back(' ');
				ans.append(to_string((long long)id.sc3));
				ans.push_back(' ');
				ans.append(to_string((long long)id.sc4));
				ans.push_back(' ');
				ans.append(to_string((long long)id.sc5_high2));
				ans.push_back(' ');
				ans.append(to_string((long long)id.sc5_low4));
			}
			ans.append("]");
		}

		ans.append("}");
		return ans;
	}

	static void deserializeDataset(const string& str, Dataset& dataset) {
		dataset.getDatasets().clear();
		int i = 0;
		while (str[i] != '{') i++;

		while (str[i] != '}') {
			string id, type, word;

			while (str[i] != '[') {
				if (str[i] == '}')
					return;
				i++;
			}
			while (!(isalnum(str[i]) || str[i] == '_'))	i++;
			while (isalnum(str[i]) || str[i] == '_')
				id.push_back(str[i++]);

			while (!(isalnum(str[i]) || str[i] == '_'))	i++;
			while (isalnum(str[i]) || str[i] == '_')
				type.push_back(str[i++]);

			while (str[i] != '$') i++;
			i++;
			if (str[i] == '\"')i++;

			while (!(str[i] == ']' || (str[i] == '\"'&&str[i + 1] == ']')))
				word += str[i++];
			i++;
			if (str[i] == ']')
				i++;

			Data d;
			if (type == "bool")
				d.setData((bool)atoi(word.c_str()));
			else if (type == "int")
				d.setData((int)atoi(word.c_str()));
			else if (type == "uuid") {
				Uuid id;
				istringstream is(word);
				is >> id.sc1 >> id.sc2 >> id.sc3 >> id.sc4 >> id.sc5_high2 >> id.sc5_low4;
				d.setData(id);
			}
			else if (type == "string")
				d.setData(word);

			dataset.getDatasets().insert(make_pair(id, d));
		}
	}

};


