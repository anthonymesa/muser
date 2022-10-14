
#include "muse.h"
#include "AudioFile.h"
#include <iostream>

Muse::Muse(int _count, std::string _obj_file_path, std::string _tex_file_path) {
    this->name = "model_" + std::to_string(++_count);
    this->model = LoadModel(_obj_file_path.c_str());                 // Load model
    this->model_texture = LoadTexture(_tex_file_path.c_str()); // Load model texture
    this->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = model_texture;  
}

Muse::~Muse() {
    return;
}

void Muse::announce(std::string _text) {
    std::cout << "Muse \"" << this->name << "\" - " << _text << std::endl;
}

void Muse::rasterizeBuffer() {
    // this->audio_buffer.push_back(1);
    std::cout << std::to_string(this->model.meshes[0].vertexCount) << std::endl;
    std::cout << std::to_string(this->model.meshes[0].triangleCount) << std::endl;
    std::cout << std::to_string(this->model.meshes[0].vertexCount / 3) << std::endl;
}

/*
bool MuseHeightMap::rasterize(int _width, MuseMesh &_mesh) {

  this->buffer_width = _width;

  // Copy the mesh pointer, because it will need to be referenced by other
  // functions later.
  this->mesh_reference = &_mesh;

  // If there are no faces to rasterize, exit.

  const int faces_count = this->mesh_reference->faces.size() / 9;

  if (faces_count == 0) {
    std::cout << "museRaster::RasterizeTopologyToBuffer - faces_amt evaluated "
                 "to be 0. No faces to draw, aborting raster process..."
              << std::endl;
    return false;
  }

  // Using threads to split the load of rasterisation.

  const static int threads_count = std::thread::hardware_concurrency();
  std::thread t[threads_count];

  // Create and then wait on rasterisation threads.
  
  for(int i = 0; i < threads_count; i++) {
    t[i] = std::thread(
      rasterizeOnThread, 
      i, 
      faces_count, 
      threads_count,
      this
    );
  }

  for(int i = 0; i < threads_count; i++) {
    t[i].join();
    std::cout << "thread"
              << std::to_string(i)
              << " finished and joined."
              << std::endl;
  }

  return true;
}*/

void Muse::exportImage(std::string _filename) {

}

void Muse::exportAudio(std::string _filename) {

}

std::string Muse::getName() {
    return this->name;
}

Model Muse::getModel() {
    return this->model;
}

Texture2D Muse::getTexture() {
    return this->model_texture;
}

std::vector<unsigned int> Muse::getAudioBuffer() {
    return this->audio_buffer;
}

bool Muse::bufferReady() {
    return !this->audio_buffer.empty();
}