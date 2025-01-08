#pragma once

using UUID = uint64_t;

static UUID uuid()
{
	static std::random_device randomDevice;
	static std::mt19937_64 engine(randomDevice());
	static std::uniform_int_distribution<UUID> uniformDistribution(std::numeric_limits<UUID>::min(), std::numeric_limits<UUID>::max());

	return uniformDistribution(engine);
}

static std::vector<std::string> splitString(std::string str, const char* delims)
{
	std::vector<std::string> parts;

	size_t prev = 0, pos;
	while ((pos = str.find_first_of(delims, prev)) != std::string::npos) {
		if (pos > prev)
			parts.push_back(str.substr(prev, pos - prev));
		prev = pos + 1;
	}
	if (prev < str.size())
		parts.push_back(str.substr(prev, std::string::npos));

	return parts;
}

static std::string readFile(const std::filesystem::path& filePath)
{
    // open file
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return "";
    }

    // get file size
    file.seekg(0, std::ios::end);
    const size_t size = file.tellg();    
    if (size < 1) {
        return "";
    }

    // read file content
    std::string content(size, ' ');
    file.seekg(0, std::ios::beg);   
    file.read(&content[0], size);
    return content;
}

template<typename ... T>
static constexpr std::string strPrintf(const std::string_view fmt, const T ... args)
{
	const size_t size = static_cast<size_t>(std::snprintf(nullptr, 0, fmt.data(), args ...));
	std::string str(size, '\0');
	std::snprintf(str.data(), size+1, fmt.data(), args ...);
	return str;
}

static uint32_t vecToRGBA(const glm::vec4& color)
{
	const uint8_t r = static_cast<uint8_t>(color.r * 255.0f);
	const uint8_t g = static_cast<uint8_t>(color.g * 255.0f);
	const uint8_t b = static_cast<uint8_t>(color.b * 255.0f);
	const uint8_t a = static_cast<uint8_t>(color.a * 255.0f);

	return (a << 24) | (b << 16) | (g << 8) | r;
}
