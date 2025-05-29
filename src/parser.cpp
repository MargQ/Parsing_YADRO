// Структура проекта:
// - /src/cpp: C++ программа
// - /scripts: Python и Bash скрипты
// - /build_my_project.sh: скрипт сборки
// - /CMakeLists.txt
// - /README.md

// main.cpp — весь проект в одном файле
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <filesystem>
#include <memory>
#include <algorithm>
#include <cstdint>
#include <stdexcept>
#include <byteswap.h>
struct Point {
    std::string group;
    int x;
    int y;
    std::string file;
};

class IParser {
public:
    virtual std::vector<Point> parse(const std::string& filepath) = 0;
    virtual ~IParser() {}
};

class TxtParser : public IParser {
public:
    std::vector<Point> parse(const std::string& filepath) override {
        std::ifstream file(filepath);
        if (!file) throw std::runtime_error("Cannot open " + filepath);

        std::vector<Point> points;
        std::string line;
        while (std::getline(file, line)) {
            size_t colon = line.find(":");
            size_t comma = line.find(",");
            if (colon == std::string::npos || comma == std::string::npos)
                throw std::runtime_error("Bad format in " + filepath);
            Point p;
            p.group = line.substr(0, colon);
            p.x = std::stoi(line.substr(colon + 1, comma - colon - 1));
            p.y = std::stoi(line.substr(comma + 1));
            p.file = filepath;
            points.push_back(p);
        }
        return points;
    }
};

class BinParser : public IParser {
public:
    std::vector<Point> parse(const std::string& filepath) override {
        std::ifstream file(filepath, std::ios::binary);
        if (!file) throw std::runtime_error("Cannot open " + filepath);

        std::vector<Point> points;
        uint32_t entry;

        while (file.read(reinterpret_cast<char*>(&entry), sizeof(entry))) {
            entry = __builtin_bswap32(entry); 

            uint32_t group = (entry >> 24) & 0xFF;    // старшие 8 бит
            uint32_t x     = (entry >> 12) & 0xFFF;   // средние 12 бит
            uint32_t y     = entry & 0xFFF;           // младшие 12 бит

            points.push_back({std::to_string(group), static_cast<int>(x), static_cast<int>(y), filepath});
        }

        return points;
    }
};

class JsonParser : public IParser {
public:
    std::vector<Point> parse(const std::string& filepath) override {
        std::ifstream file(filepath);
        if (!file) throw std::runtime_error("Cannot open " + filepath);

        std::vector<Point> points;
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

        size_t pos = 0;
        while ((pos = content.find("{", pos)) != std::string::npos) {
            size_t end = content.find("}", pos);
            if (end == std::string::npos) break;
            std::string obj = content.substr(pos, end - pos + 1);

            size_t g1 = obj.find("\"group\"");
            size_t x1 = obj.find("\"x\"");
            size_t y1 = obj.find("\"y\"");
            if (g1 == std::string::npos || x1 == std::string::npos || y1 == std::string::npos) {
                pos = end + 1;
                continue;
            }

            std::string group = extractString(obj, g1);
            int x = extractInt(obj, x1);
            int y = extractInt(obj, y1);
            points.push_back({group, x, y, filepath});
            pos = end + 1;
        }
        return points;
    }

private:
    std::string extractString(const std::string& obj, size_t keypos) {
        size_t quote1 = obj.find('"', keypos + 7);
        size_t quote2 = obj.find('"', quote1 + 1);
        return obj.substr(quote1 + 1, quote2 - quote1 - 1);
    }

    int extractInt(const std::string& obj, size_t keypos) {
        size_t colon = obj.find(':', keypos);
        size_t comma = obj.find(',', colon);
        std::string num = obj.substr(colon + 1, comma - colon - 1);
        return std::stoi(num);
    }
};

class ParserFactory {
public:
    static std::unique_ptr<IParser> createParser(const std::string& filename) {
        auto ext = std::filesystem::path(filename).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        if (ext == ".txt") return std::make_unique<TxtParser>();
        if (ext == ".bin") return std::make_unique<BinParser>();
        if (ext == ".json") return std::make_unique<JsonParser>();
        throw std::runtime_error("Unsupported file extension: " + ext);
    }
};

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <file1> [file2 ...]" << std::endl;
        return 1;
    }

    std::vector<Point> allPoints;

    try {
        for (int i = 1; i < argc; ++i) {
            std::string filename = argv[i];
            auto parser = ParserFactory::createParser(filename);
            auto points = parser->parse(filename);
            allPoints.insert(allPoints.end(), points.begin(), points.end());
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "{\"points\": [";
    for (size_t i = 0; i < allPoints.size(); ++i) {
        const auto& p = allPoints[i];
        std::cout << "{\"file\":\"" << p.file << "\",\"group\":\"" << p.group
                  << "\",\"x\": " << p.x << ", \"y\": " << p.y << "}";
        if (i != allPoints.size() - 1) std::cout << ", ";
    }
    std::cout << "]}" << std::endl;
    return 0;
}

