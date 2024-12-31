#pragma once

std::string readFile(const std::string& fileName)
{
    // open file
    std::ifstream file(fileName);
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