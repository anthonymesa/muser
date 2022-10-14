
#include "muse.h"
#include "AudioFile.h"
#include <iostream>
#include <thread>
#include <cmath>

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

void rasterizeOnThread(int _current_thread, int _faces_count, int _threads_count, Muse _muse) {
  int thread_faces_limit = _faces_count / _threads_count;

  for(int face_index = 0; face_index < thread_faces_limit; face_index++) {
    _muse.executePartialRender(_current_thread, thread_faces_limit, face_index);
  }
}

void Muse::executePartialRender(int _current_thread, int _thread_faces_limit, int _face_index) {
  std::vector<unsigned int> face;

  // Calculate the face index so that the threads don't work on the same
  // indices.
  populateFaceRasterData(
    face, 
    (_current_thread * _thread_faces_limit) + _face_index
  );

  orderFace(face);
  rasterizeFace(face);
}

void Muse::rasterizeBuffer() {

    initMinMaxValues();

    const Mesh raster_mesh = this->model.meshes[0];

    const int faces_count = raster_mesh.triangleCount;

    const static int threads_count = std::thread::hardware_concurrency();
    std::thread t[threads_count];

    // Create and then wait on rasterisation threads.
    
    for(int i = 0; i < threads_count; i++) {
        t[i] = std::thread(rasterizeOnThread, i, faces_count, threads_count, this);
    }

    for(int i = 0; i < threads_count; i++) {
        t[i].join();
        std::cout << "thread" << std::to_string(i) << " finished and joined." << std::endl;
    }
}

float Muse::normalizeVector(std::tuple<float, float, float> vertex) {
  return std::sqrt(std::pow(std::get<0>(vertex), 2) +
                   std::pow(std::get<1>(vertex), 2) +
                   std::pow(std::get<2>(vertex), 2));
}

void Muse::initMinMaxValues() {
    for(int i = 0; i < this->model.meshes[0].vertexCount; i++) {
          // turn into tuple for normalizing distance
        std::tuple<float, float, float> distance{
            this->model.meshes[0].vertices[(i * 3) + 0],
            this->model.meshes[0].vertices[(i * 3) + 1],
            this->model.meshes[0].vertices[(i * 3) + 2],
        };

        float distance_normalized = normalizeVector(distance);

        // set min max accordingly
        if (distance_normalized < this->min_distance_from_origin) {
            this->min_distance_from_origin = distance_normalized;
        }
        if (distance_normalized > this->max_distance_from_origin) {
            this->max_distance_from_origin = distance_normalized;
        }

        // set min max difference accordingly
        this->min_max_distance_difference =
            this->max_distance_from_origin - this->min_distance_from_origin;
    }
}

void Muse::populateFaceRasterData(std::vector<unsigned int> &_face, int _face_index) {
  // first point
  _face.push_back(getFaceX1(_face_index));
  _face.push_back(getFaceY1(_face_index));
  _face.push_back(getFaceZ1(_face_index));
  
  // second point
  _face.push_back(getFaceX2(_face_index));
  _face.push_back(getFaceY2(_face_index));
  _face.push_back(getFaceZ2(_face_index));
  
  // third point
  _face.push_back(getFaceX3(_face_index));
  _face.push_back(getFaceY3(_face_index));
  _face.push_back(getFaceZ3(_face_index));
}

unsigned int Muse::getFaceX1(int _face_index)
{
  return getCoordinateValue(1, 1, _face_index);
}

unsigned int Muse::getFaceY1(int _face_index)
{
  return getCoordinateValue(1, 2, _face_index);
}

unsigned int Muse::getFaceZ1(int _face_index)
{
  return getColorValue(1, _face_index);
}

unsigned int Muse::getFaceX2(int _face_index)
{
  return getCoordinateValue(2, 1, _face_index);
}

unsigned int Muse::getFaceY2(int _face_index)
{
  return getCoordinateValue(2, 2, _face_index);
}

unsigned int Muse::getFaceZ2(int _face_index)
{
  return getColorValue(2, _face_index);
}

unsigned int Muse::getFaceX3(int _face_index)
{
  return getCoordinateValue(3, 1, _face_index);
}

unsigned int Muse::getFaceY3(int _face_index)
{
  return getCoordinateValue(3, 2, _face_index);
}

unsigned int Muse::getFaceZ3(int _face_index)
{
  return getColorValue(3, _face_index);
}

unsigned int Muse::getCoordinateValue(int _point_index, int _dimension_index, int _face_index) {
  return (this->model.meshes[0].texcoords[(_face_index * 6) + ((_point_index - 1) * 2) + (_dimension_index - 1)]) * BUFFER_WIDTH;
}

float Muse::calculateRealMagnitude(float _temp_raw_mag) {
  return (_temp_raw_mag - this->min_distance_from_origin) /
         this->min_max_distance_difference;
}

unsigned int Muse::getColorValue(int _point_index, int _face_index) {
  std::tuple<float, float, float> vertex = {
      this->model.meshes[0].vertices[(_face_index * 9) + ((_point_index - 1) * 3) + 0],
      this->model.meshes[0].vertices[(_face_index * 9) + ((_point_index - 1) * 3) + 1],
      this->model.meshes[0].vertices[(_face_index * 9) + ((_point_index - 1) * 3) + 2]
  };

  float temp_raw_mag = this->normalizeVector(vertex);
  float temp_real_mag = this->calculateRealMagnitude(temp_raw_mag);
  float color_magnitude = 255 * temp_real_mag;

  if((color_magnitude < 0) || (color_magnitude > 255))
  {
    std::cout << "error" << std::endl;
  }

  return (unsigned int) color_magnitude;
}


void  Muse::orderFace(std::vector<unsigned int> &_face_data)
{
  if (_face_data[1] > _face_data[4])
  {
    std::swap(_face_data[0], _face_data[3]);
    std::swap(_face_data[1], _face_data[4]);
    std::swap(_face_data[2], _face_data[5]);
  }

  if (_face_data[4] > _face_data[7])
  {
    std::swap(_face_data[3], _face_data[6]);
    std::swap(_face_data[4], _face_data[7]);
    std::swap(_face_data[5], _face_data[8]);
  }

  if (_face_data[1] > _face_data[4])
  {
    std::swap(_face_data[0], _face_data[3]);
    std::swap(_face_data[1], _face_data[4]);
    std::swap(_face_data[2], _face_data[5]);
  }
}

void Muse::rasterizeFace(std::vector<unsigned int> &face)
{
  Point L = std::make_tuple(face[0], face[1], face[2]);
  Point M = std::make_tuple(face[3], face[4], face[5]);
  Point H = std::make_tuple(face[6], face[7], face[8]);

  // none of the color values should be greater than 255.
  if((std::get<2>(L) > 255) ||
     (std::get<2>(M) > 255) ||
     (std::get<2>(H) > 255)
  ) {
    std::cout << "error" << std::endl;
  }

  this->rasterizeTop(L, M, H);
  RasterizeBottom(L, M, H);
}

void Muse::rasterizeTop(Point &L, Point &M, Point &H)
{
  float m_a = slope(H, L);
  float b_a = yIntercept(m_a, H);

  float m_b = slope(H, M);
  float b_b = yIntercept(m_b, H);

  for (int omega = std::get<1>(M); omega < std::get<1>(H); omega++)
  {
    float A_y = omega;
    float A_x = (!std::isinf(m_a) && m_a !=0) ? 
                xIntercept(omega, b_a, m_a) : std::get<0>(H);
    float A_z = interpolate(A_x, A_y, H, L);

    Point A = std::make_tuple(A_x, A_y, A_z);

    float B_y = omega;
    float B_x = (!std::isinf(m_b) && m_b !=0) ? 
          xIntercept(omega, b_b, m_b) : std::get<0>(M);
    float B_z = interpolate(B_x, B_y, H, M);

    Point B = std::make_tuple(B_x, B_y, B_z);

    rasterizeLine(omega, A, B);
  }
}

float Muse::slope(Point &_u, Point &_v)
{
  return (std::get<1>(_u) - std::get<1>(_v)) / 
         (std::get<0>(_u) - std::get<0>(_v));
}

float Muse::yIntercept(float _m, Point &_u)
{
  return std::get<1>(_u) - (_m * std::get<0>(_u));
}

float Muse::xIntercept(int &_y, float &_b, float &_m)
{
  return (_y - _b) / _m;
}

float Muse::interpolate(
  float _curr_x, 
  float _curr_y, 
  Point &_a_bound, 
  Point &_b_bound
) {
  float color_difference = (std::get<2>(_b_bound) - std::get<2>(_a_bound));
  float progress = this->distance2d(
                           _curr_x, 
                           _curr_y, 
                           std::get<0>(_a_bound), 
                           std::get<1>(_a_bound)
                         );
  float whole = this->distance2d(
                        std::get<0>(_b_bound), 
                        std::get<1>(_b_bound),
                        std::get<0>(_a_bound),
                        std::get<1>(_a_bound)
                      );
  float ratio = progress / whole;
  return std::get<2>(_a_bound) + (color_difference * ratio);
}

float Muse::distance2d(float _x1, float _y1, float _x2, float _y2)
{
  float square_x = std::pow((_x1 - _x2), 2);
  float square_y = std::pow((_y1 - _y2), 2);
  return std::sqrt(square_x + square_y);
}

void Muse::rasterizeLine(int omega, Point &A, Point &B)
{
  if (std::get<0>(A) > std::get<0>(B))
  {
    std::swap(A, B);
  }

  for (int i = std::get<0>(A); i < (std::get<0>(A) + 1); i++)
  {
    float T_x = i;
    float T_y = omega;
    float T_z = this->interpolate(T_x, T_y, A, B);

    this->audio_buffer.at((T_y * BUFFER_WIDTH) + T_x) = T_z;
  }
}

/*
bool Muse::rasterize(int _width, MuseMesh &_mesh) {

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