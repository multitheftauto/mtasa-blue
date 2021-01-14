#ifndef AFFINETRANSFORM_H
#define AFFINETRANSFORM_H

namespace lunasvg {

class Point;
class Rect;

class AffineTransform
{
public:
    AffineTransform();
    AffineTransform(const AffineTransform& transform);
    AffineTransform(double m00, double m10, double m01, double m11, double m02, double m12);

    void setMatrix(double m00, double m10, double m01, double m11, double m02, double m12);
    void makeIdentity();
    AffineTransform inverted() const;

    AffineTransform operator*(const AffineTransform& transform) const;
    AffineTransform& operator*=(const AffineTransform& transform);

    AffineTransform& multiply(const AffineTransform& transform);
    AffineTransform& postmultiply(const AffineTransform& transform);
    AffineTransform& rotate(double radians);
    AffineTransform& rotate(double radians, double cx, double cy);
    AffineTransform& scale(double sx, double sy);
    AffineTransform& shear(double shx, double shy);
    AffineTransform& translate(double cx, double cy);

    static AffineTransform fromRotate(double radians);
    static AffineTransform fromRotate(double radians, double cx, double cy);
    static AffineTransform fromScale(double sx, double sy);
    static AffineTransform fromShear(double shx, double shy);
    static AffineTransform fromTranslate(double tx, double ty);

    void map(double x, double y, double& _x, double& _y) const;
    void map(const double* src, double* dst, int size) const;
    void map(const Point* src, Point* dst, int size) const;
    Point mapPoint(const Point& point) const;
    Rect mapRect(const Rect& rect) const;

    bool isIdentity() const;
    double determinant() const;
    const double* getMatrix() const;

private:
    double m_matrix[2*3];
};

inline const double* AffineTransform::getMatrix() const
{
   return m_matrix;
}

} // namespace lunasvg

#endif //AFFINETRANSFORM_H
