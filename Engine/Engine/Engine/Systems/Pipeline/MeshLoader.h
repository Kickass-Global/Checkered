#ifndef MESHLOADER_H
#define MESHLOADER_H

#include <fstream>

#include "assimp/Importer.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

namespace Pipeline {

	class MeshLoader {
		void load(std::string filename)
		{
			Assimp::Importer importer;

			const aiScene* = importer.ReadFile

		}
	};

}

#endif // !MESHLOADER_H
