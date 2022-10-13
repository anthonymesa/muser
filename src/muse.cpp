
#include "muse.h"
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
}

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