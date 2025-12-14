// struct Vector {
//   float x;
//   float y;
// }; // hファイルで一回実行されるから、ここでは書く必要ないらしい

Vector fromPolar(float deg, float speed) {  //角度と速さからベクトルを作る
  Vector v;
  float rad = deg * M_PI / 180.0;
  v.x = cos(rad) * speed;
  v.y = sin(rad) * speed;
  return v;
}

Vector add(Vector a, Vector b) {  //ベクトルの足し算
  Vector result;
  result.x = a.x + b.x;
  result.y = a.y + b.y;
  return result;
}

// 正規化（単位ベクトル化）
Vector normalize(Vector v) {
  float len = sqrt(v.x * v.x + v.y * v.y);
  if (len == 0) return {0, 0};
  return { v.x / len, v.y / len };
}

// 正規化（単位ベクトル化）
Vector lenV(Vector v, int len_v) {
  float len = sqrt(v.x * v.x + v.y * v.y);
  if (len == 0) return {0, 0};
  return { v.x / len * len_v, v.y / len * len_v };
}
