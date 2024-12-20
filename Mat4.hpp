//
// Created by gaetz on 24/09/2024.
//

#ifndef MAT4_HPP
#define MAT4_HPP

class Mat4 {
public:
    Mat4();
    Mat4 (float m0_, float m4_, float m8_, float m12_,
          float m1_, float m5_, float m9_, float m13_,
          float m2_, float m6_, float m10_, float m14_,
          float m3_, float m7_, float m11_, float m15_):
            m0 { m0_ }, m4 { m4_ }, m8 { m8_ }, m12 { m12_ },
            m1 { m1_ }, m5 { m5_ }, m9 { m9_ }, m13 { m13_ },
            m2 { m2_ }, m6 { m6_ }, m10 { m10_ }, m14 { m14_ },
            m3 { m3_ }, m7 { m7_ }, m11 { m11_ }, m15 { m15_ } {}

    float m0, m4, m8, m12;  // Matrix first row (4 components)
    float m1, m5, m9, m13;  // Matrix second row (4 components)
    float m2, m6, m10, m14; // Matrix third row (4 components)
    float m3, m7, m11, m15; // Matrix fourth row (4 components)

    static const Mat4 Identity;

    static Mat4 CreateRotationMatrix(float axisX, float axisY, float axisZ, float angle);
    static Mat4 CreateRotationZ(float angle);
    static Mat4 CreateTranslation(float x, float y, float z);
    Mat4 operator*(const Mat4& other) const;
    static Mat4 CreateOrthographicOffCenter(float left, float right, float bottom, float top, float zNearPlane, float zFarPlane);
    static Mat4 CreatePerspectiveFieldOfView(float fieldOfView, float aspectRatio,
                                      float nearPlaneDistance, float farPlaneDistance);
};


#endif //GMATH_MAT4_HPP
