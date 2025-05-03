#include <private/shaders/shader.hpp>

#include <stdexcept>
#include <fstream>
#include <string>

namespace compwolf
{
	auto shader_code_from_file(const char* path) -> std::vector<uint32_t>
	{
		// file is read in individual bytes to keep little-endian.

		std::ifstream stream(path, std::ios::binary | std::ios::in | std::ios::ate);
		if (!stream.is_open()) throw std::runtime_error("Could not open \"" + std::string(path) + "\".");

		std::vector<uint32_t> data;
		data.resize(stream.tellg() / 4);

		stream.seekg(0);
		stream.read(static_cast<char*>(static_cast<void*>(data.data())), data.size() * 4);

		return data;
	}
}
