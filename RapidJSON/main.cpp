#include "Monster.h"
#include <iostream>
#include <fstream>
#include <sstream>

// NuGet
// 외부 라이브러리, 특별 기능 등을 사용하기 위해
// 설치/ 관리 해주는 도구
// => NuGet을 통해 RapidJSON 설치

// rapidjson을 위한 헤더들은 NuGet을 통해 프로젝트에 통합되어 있으므로 < > 기호 사용
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

// RapidJSON
// 텐센트에서 개발한 매우 빠른 속도의 JSON 라이브러리
// 문자열로 되어 있는 JSON String을 DOM (문서 객체 모델)로 해석(Parsing),
// 해당 DOM을 통해 값을 접근, 수정 가능
// DOM을 다시 JSON 문자열로 변환 가능

using namespace rapidjson;

bool SaveToJson(const char* filename, std::vector<Monster>& monsters)
{
	StringBuffer sb;
	PrettyWriter<StringBuffer> writer(sb);
	writer.StartObject();
	writer.Key("monsters");
	writer.StartArray();
	for (auto monster : monsters)
	{
		writer.StartObject();
		{
			writer.Key("name"); writer.String(monster.GetName().c_str());
			writer.Key("status");
			writer.StartObject();
			{
				Status status = monster.GetStatus();
				writer.Key("level"); writer.Int(status.mLevel);
				writer.Key("hp"); writer.Int(status.mHP);
				writer.Key("mp"); writer.Int(status.mMP);
			}
			writer.EndObject();
			writer.Key("items");
			writer.StartArray();
			{
				for (auto item : monster.GetDropItems())
				{
					writer.StartObject();
					writer.Key("name"); writer.String(item.mName.c_str());
					writer.Key("gold"); writer.Int(item.mGold);
					writer.EndObject();
				}
			}
			writer.EndArray();
		}
		writer.EndObject();
	}
	writer.EndArray();
	writer.EndObject();
	std::ofstream ofs;
	ofs.exceptions(std::ofstream::badbit | std::ofstream::failbit);
	try
	{
		ofs.open(filename);
		ofs << sb.GetString();
		ofs.close();
	}
	catch (std::ofstream::failure e)
	{
		std::cerr << "파일 저장 중에 예외가 발생했습니다.\n";
		std::cerr << e.what();
		ofs.close();
		return false;
	}
	return true;
}

bool LoadFromJson(const char* filename, std::vector<Monster>& monsters)
{
	std::ifstream ifs;		// JSON 파일을 읽어오는 스트림
	ifs.exceptions(std::ifstream::badbit | std::ifstream::failbit);
	std::stringstream ss;	// 문자열을 읽어오는 스트림
	try
	{
		ifs.open(filename);
		ss << ifs.rdbuf();		// rdbuf = 서로 다른 두 스트림을 연결
		ifs.close();
	}
	catch (std::ifstream::failure e)
	{
		std::cerr << "파일을 읽는 도중에 예외가 발생했습니다.\n";
		std::cerr << e.what();
		ifs.close();
		return false;
	}
	Document d;
	d.Parse(ss.str().c_str());		// 읽어온 문자열을 사용해 Document(DOM 객체) 로 해석(Parse)
									// 멤버함수 Parse는 인자로 c-style 문자열을 받음
	for (auto& elem : d["monsters"].GetArray())
	{
		Monster monster;
		monster.SetName(elem["name"].GetString());
		Status status;
		status.mLevel = elem["status"].GetObject()["level"].GetInt();
		status.mHP = elem["status"].GetObject()["hp"].GetInt();
		status.mMP = elem["status"].GetObject()["mp"].GetInt();
		monster.SetStatus(status);
		for (auto& elem2 : elem["items"].GetArray())
		{
			Item item;
			item.mName = elem2["name"].GetString();
			item.mGold = elem2["gold"].GetInt();
			monster.AddDropItem(item);
		}
		monsters.push_back(monster);
	}
	return true;
}

int main()
{
	std::vector<Monster> monsters;
	Monster monster;
	monster.SetName("슬라임");
	monster.SetStatus(Status{ 1,1,1 });
	monster.AddDropItem(Item{ "끈적한 젤리", 1 });
	monsters.push_back(monster);
	monster.GetDropItems().clear();
	monster.SetName("늑대인간");
	monster.SetStatus(Status{ 5,5,5 });
	monster.AddDropItem(Item{ "발톱", 2 });
	monster.AddDropItem(Item{ "늑대가죽", 5 });
	monsters.push_back(monster);
	monster.GetDropItems().clear();
	monster.SetName("악마");
	monster.SetStatus(Status{ 10,10,10 });
	monster.AddDropItem(Item{ "날개", 10 });
	monster.AddDropItem(Item{ "손톱", 5 });
	monsters.push_back(monster);
	SaveToJson("Data/monsters.json", monsters);
	monsters.clear();
	LoadFromJson("Data/monsters.json", monsters);
}