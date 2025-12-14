#ifndef FUNCTIONS_H
#define FUNCTIONS_H //functionは一番最初に実行させたいからhファイルで

// #include <Arduino.h>  // sin, cos, M_PI を使うため（ESP32/Arduino向け）

// 2次元ベクトル構造体
struct Vector {
  float x;
  float y;
};

// 角度[deg]と速さからベクトルを作成
Vector fromPolar(float deg, float speed);

// 2つのベクトルを足し算
Vector add(Vector a, Vector b);

Vector normalize(Vector v);

Vector lenV(Vector v, int len);

#endif
