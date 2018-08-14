#pragma once

#include "Main.hpp"
#include "Uuid.hpp"


class Data {
public:

	enum DataType {
		Integer,
		String,
		Bool,
		Uuid
	};

	Data() :dataInt(0), dataString(), dataBool(false),type(Integer) {}
	Data(DataType type) :type(type) {}

	template<typename Type>
	Data(DataType type, Type data) : type(type) {
		setData(data);
	}

	void setType(DataType type) { this->type = type; }

	DataType getType() { return type; }


public:

	int& getDataInt() { setType(Integer); return dataInt; }
	void setData(int data) { setType(Integer); dataInt = data; }

	string& getDataString() { setType(String); return dataString; }
	void setData(const string& data) { setType(String); dataString = data; }

	bool& getDataBool() { setType(Bool); return dataBool; }
	void setData(const bool data) { setType(Bool); dataBool = data; }

	::Uuid& getDataUuid() { setType(Uuid); return dataUuid; }
	void setData(const ::Uuid data) { setType(Uuid); dataUuid = data; }

	operator bool() { return dataBool; }
	operator int() { return dataInt; }
	operator string() { return dataString; }
	operator ::Uuid() { return dataUuid; }

private:

	DataType type;

	int dataInt;
	string dataString;
	bool dataBool;
	::Uuid dataUuid;
};

const bool operator == (Data& left, Data& right) {
	if (left.getType() != right.getType())
		return false;
	else {
		if (left.getType() == Data::Bool)
			return left.getDataBool() == right.getDataBool();
		else if (left.getType() == Data::Integer)
			return left.getDataInt() == right.getDataInt();
		else if (left.getType() == Data::String)
			return left.getDataString() == right.getDataString();
		else if (left.getType() == Data::Uuid)
			return left.getDataUuid() == right.getDataUuid();
		else
			return false;
	}
}

const bool operator != (Data& left, Data& right) {
	return !(left == right);
}

Packet& operator << (Packet& packet, Data& data) {
	packet << (Uint8)data.getType();
	if (data.getType() == Data::Integer)
		packet << data.getDataInt();
	else if (data.getType() == Data::String)
		packet << data.getDataString();
	else if (data.getType() == Data::Bool)
		packet << data.getDataBool();
	else if (data.getType() == Data::Uuid)
		packet << data.getDataUuid();
	return packet;
}

Packet& operator >> (Packet& packet, Data& data) {
	Uint8 type;
	packet >> type;
	data.setType((Data::DataType)type);
	if (data.getType() == Data::Integer)
		packet >> data.getDataInt();
	else if (data.getType() == Data::String)
		packet >> data.getDataString();
	else if (data.getType() == Data::Bool)
		packet >> data.getDataBool();
	else if (data.getType() == Data::Uuid)
		packet >> data.getDataUuid();
	return packet;
}


class Dataset {
public:

	void insert(string id, Data& data) { datasets.insert(make_pair(id, data)); }

	template<typename DataType>
	void setData(string id, DataType data) {
		datasets[id].setData(data);
	}

	Data& operator [] (string id) { return datasets[id]; }

	map<string, Data>& getDatasets() { return datasets; }

private:

	map<string, Data> datasets;

};

Packet& operator << (Packet& packet, Dataset& datas) {
	packet << (unsigned long long)datas.getDatasets().size();
	for (auto& i : datas.getDatasets()) {
		packet << i.first << i.second;
	}
	return packet;
}

Packet& operator >> (Packet& packet, Dataset& datas) {
	datas.getDatasets().clear();
	unsigned long long size;
	string id;
	packet >> size;
	for (int i = 1; i <= size; i++) {
		Data data;
		packet >> id >> data;
		datas.insert(id, data);
	}
	return packet;
}

