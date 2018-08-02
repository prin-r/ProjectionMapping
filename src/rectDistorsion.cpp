#include "rectDistorsion.h"

void rectDistorsion::setup() {

}
void rectDistorsion::update() {

}

float rectDistorsion::det(const Matrix33f &mat) {
	return mat.determinant();
}

Vec3f rectDistorsion::mulmv(const Matrix33f &mat, const Vec3f &vec) {
	return Vec3f(
		mat.m[0] * vec.x + mat.m[1] * vec.y + mat.m[2] * vec.z,
		mat.m[3] * vec.x + mat.m[4] * vec.y + mat.m[5] * vec.z,
		mat.m[6] * vec.x + mat.m[7] * vec.y + mat.m[8] * vec.z
		);
}

Matrix33f rectDistorsion::adj(const Matrix33f &mat) {
	return Matrix33f(
		mat.m[4] * mat.m[8] - mat.m[5] * mat.m[7], mat.m[2] * mat.m[7] - mat.m[1] * mat.m[8], mat.m[1] * mat.m[5] - mat.m[2] * mat.m[4],
		mat.m[5] * mat.m[6] - mat.m[3] * mat.m[8], mat.m[0] * mat.m[8] - mat.m[2] * mat.m[6], mat.m[2] * mat.m[3] - mat.m[0] * mat.m[5],
		mat.m[3] * mat.m[7] - mat.m[4] * mat.m[6], mat.m[1] * mat.m[6] - mat.m[0] * mat.m[7], mat.m[0] * mat.m[4] - mat.m[1] * mat.m[3]
	);
}

Matrix33f rectDistorsion::multmm(const Matrix33f &mat1, const Matrix33f &mat2) {
	return mat2 * mat1;
}

Matrix33f rectDistorsion::transpose(const Matrix33f &mat) {
	return mat.transposed();
}

Matrix33f rectDistorsion::basisToPoints(const Vec2f &p1, const Vec2f &p2, const Vec2f &p3, const Vec2f &p4) {
	Vec3f r1(p1.x, p2.x, p3.x);
	Vec3f r2(p1.y, p2.y, p3.y);
	Vec3f r3(1, 1, 1);
	Vec3f r4(p4.x, p4.y, 1);

	Matrix33f m1 = Matrix33f(r1, r2, r3);

	float det1 = det(m1);
	Matrix33f adj1 = adj(m1);

	Vec3f adj1xr4 = mulmv(adj1, r4) / det1;
	
	Vec3f rout1(p1.x * adj1xr4.x, p2.x * adj1xr4.y, p3.x * adj1xr4.z);
	Vec3f rout2(p1.y * adj1xr4.x, p2.y * adj1xr4.y, p3.y * adj1xr4.z);

	return Matrix33f(rout1, rout2, adj1xr4);
}

Matrix33f rectDistorsion::general2DProjection(const Vec2f &p1, const Vec2f &p2, const Vec2f &p3, const Vec2f &p4) {
	Matrix33f d = basisToPoints(Vec2f(0, 0), Vec2f(1, 0), Vec2f(1, 1), Vec2f(0, 1));
	Matrix33f s = basisToPoints(p1, p2, p3, p4);

	Matrix33f ds = multmm(d, adj(s));
	ds /= ds.m22;
	return ds.transposed();
}
