#include <filesystem>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

namespace fs = std::filesystem;

struct ReflectedField {
    std::string type;
    std::string name;
};

struct ReflectedComponent {
    std::string className;
    std::string headerPath;
    std::vector<ReflectedField> fields;
};

static std::string toFieldType(const std::string& type) {
    if (type == "float") return "FieldType::Float";
    if (type == "int") return "FieldType::Int";
    if (type == "bool") return "FieldType::Bool";
    if (type == "glm::vec3") return "FieldType::Vec3";

    return "FieldType::Unknown";
}

static bool shouldSkipComponent(const std::string& className) {
    return className == "TransformComponent" ||
           className == "CameraComponent" ||
           className == "Component";
}

static ReflectedComponent parseHeader(const fs::path& path) {
    std::ifstream file(path);

    ReflectedComponent component;
    component.headerPath = path.string();

    if (!file.is_open()) {
        std::cerr << "Could not open: " << path << std::endl;
        return component;
    }

    std::string line;
    bool insideComponentClass = false;
    bool inPublicSection = false;
    int braceDepth = 0;

    std::regex classRegex(
        R"((class|struct)\s+(\w+)\s*:\s*public\s+Component)"
    );

    std::regex fieldRegex(
        R"(^\s*(float|int|bool|glm::vec3)\s+(\w+)\s*(=\s*[^;]+)?;)"
    );

    while (std::getline(file, line)) {
        std::smatch match;

        if (!insideComponentClass &&
            std::regex_search(line, match, classRegex)) {
            component.className = match[2].str();
            insideComponentClass = true;
            inPublicSection = false;

            if (line.find("{") != std::string::npos)
                braceDepth = 1;

            continue;
        }

        if (!insideComponentClass)
            continue;

        for (char c : line) {
            if (c == '{') braceDepth++;
            if (c == '}') braceDepth--;
        }

        if (line.find("public:") != std::string::npos) {
            inPublicSection = true;
            continue;
        }

        if (line.find("private:") != std::string::npos ||
            line.find("protected:") != std::string::npos) {
            inPublicSection = false;
            continue;
        }

        if (inPublicSection &&
           braceDepth == 1 &&
           std::regex_search(line, match, fieldRegex)){
           component.fields.push_back({
               match[1].str(),
               match[2].str()
           });
       }
        for (char c : line){
            if (c == '{') braceDepth++;
            if (c == '}') braceDepth--;
        }


        if (braceDepth <= 0 && line.find("};") != std::string::npos)
            break;
    }

    if (shouldSkipComponent(component.className)) {
        component.className.clear();
        component.fields.clear();
    }

    return component;
}

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cerr
            << "Usage: ReflectionGenerator <component_dir> <output_cpp> <project_root>\n";
        return 1;
    }

    fs::path componentDir = argv[1];
    fs::path outputCpp = argv[2];
    fs::path projectRoot = argv[3];

    std::vector<ReflectedComponent> components;

    for (const auto& entry : fs::recursive_directory_iterator(componentDir)) {
        if (!entry.is_regular_file())
            continue;

        if (entry.path().extension() != ".h")
            continue;

        ReflectedComponent component = parseHeader(entry.path());

        if (!component.className.empty()) {
            components.push_back(component);
        }
    }

    std::ofstream out(outputCpp);

    if (!out.is_open()) {
        std::cerr << "Could not create output file: " << outputCpp << std::endl;
        return 1;
    }

    out << "// AUTO-GENERATED FILE. DO NOT EDIT.\n\n";
    out << "#include \"reflection/Reflection.h\"\n";
    out << "#include \"reflection/ReflectionGenerated.h\"\n";
    out << "#include \"scene/Entity.h\"\n\n";

    for (const auto& component : components) {
        fs::path engineSrc = projectRoot / "Engine" / "src";
        fs::path relativePath = fs::relative(component.headerPath, engineSrc);

        out << "#include \"" << relativePath.generic_string() << "\"\n";
    }

    out << "\nvoid RegisterGeneratedReflection()\n";
    out << "{\n";

    for (const auto& component : components) {
        out << "    ReflectionRegistry::registerType<"
            << component.className
            << ">({\n";

        out << "        \"" << component.className << "\",\n";
        out << "        {\n";

        for (const auto& field : component.fields) {
            out << "            {\""
                << field.name
                << "\", "
                << toFieldType(field.type)
                << ", [](Component* component) -> void* {\n"
                << "                return &static_cast<"
                << component.className
                << "*>(component)->"
                << field.name
                << ";\n"
                << "            }},\n";
        }

        out << "        },\n";
        out << "        [](Entity* entity) {\n";
        out << "            entity->addComponent<"
            << component.className
            << ">();\n";
        out << "        }\n";

        out << "    });\n\n";
    }

    out << "}\n";

    std::cout << "Generated reflection for "
              << components.size()
              << " components.\n";

    return 0;
}
