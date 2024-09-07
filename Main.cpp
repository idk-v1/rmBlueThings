#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_NOSTDIO
#include "stb_image_write.h"

#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;
using rec_dir_it = fs::recursive_directory_iterator;
using dir_ent = fs::directory_entry;

typedef unsigned int rgba;
typedef unsigned long long loong;

static void rmBlue(const dir_ent& entry, rgba color)
{
	std::string savePath, saveName;
	int width, height, bpp;

	rgba* data = (rgba*)stbi_load(entry.path().string().data(), &width, &height, &bpp, 4);
	if (data)
	{
		for (loong i = 0; i < width * height; i++)
			if (data[i] == color)
				data[i] = 0;
		savePath = entry.path().parent_path().string();
		saveName = entry.path().filename().string();
		saveName.erase(saveName.find('.'));
		stbi_write_png((savePath + "/" + saveName + "-b.png").data(), width, height, 4, data, width * 4);
		stbi_image_free(data);
	}
	else
		printf("Error: Unable to open %s\n", entry.path().string().data());
}

int main(int argc, char** argv)
{
	rgba color = 0x000084FF;
	std::string colorStr;
	std::ifstream newColor("newColor");
	if (newColor.is_open())
	{
		newColor >> colorStr;
		color = std::strtoul(colorStr.data(), 0, 16);
		newColor.close();
	}

	rgba r, g, b, a;
	r = color >> 24 & 255;
	g = color >> 16 & 255;
	b = color >>  8 & 255;
	a = color       & 255;
	color = a << 24 | b << 16 | g << 8 | r;

	for (int i = 1; i < argc; i++)
	{
		dir_ent entry = dir_ent(argv[i]);
		if (entry.is_directory())
		{
			for (auto& entry : rec_dir_it(argv[i]))
				if (entry.is_regular_file())
					rmBlue(entry, color);
		}
		else
			rmBlue(entry, color);
	}

	return 0;
}