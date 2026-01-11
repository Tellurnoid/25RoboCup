struct Vector {
  float x;
  float y;
};

Vector makeV(float deg, float speed) {  //角度と速さからベクトルを作る
  Vector v;
  float rad = deg * M_PI / 180.0;
  v.x = cos(rad) * speed;
  v.y = sin(rad) * speed;
  return v;
}

float getCom(Vector v, float deg) {
  float rad = deg * PI / 180.0f;
  return v.x * cos(rad) + v.y * sin(rad);
}


Vector addV(Vector a, Vector b) {  //ベクトルの足し算
  Vector result;
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  return result;
}

Vector normalizeV(Vector v) { // 正規化（単位ベクトル化）
  float len = sqrt(v.x * v.x + v.y * v.y);
  if (len == 0) return {0, 0};
  return { v.x / len, v.y / len };
}

Vector lenV(Vector v, int len_v) {  // 正規化（単位ベクトル化）
  float len = sqrt(v.x * v.x + v.y * v.y);
  if (len == 0) return {0, 0};
  return { v.x / len * len_v, v.y / len * len_v };
}

float inRange(float x) {
  while (x > 180) x -= 360;
  while (x < -180) x += 360;
  return x;
}

struct Derivative {
  unsigned long last_time = 0;
  float last_value = 0;
  float value_d = 0;
};