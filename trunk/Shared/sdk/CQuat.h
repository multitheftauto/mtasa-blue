/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/CQuat.h
*  PURPOSE:		Quaternion class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

class CQuat;

#ifndef __CQUAT_H
#define __CQUAT_H

#include "CMatrix4.h"

#include <math.h>
#include <float.h>

#pragma warning (disable:4244)

#define EPSILON			0.001

#ifndef PI
#define PI (float)3.14159265358979323846264338327950
#endif

#define maximum(a,b) 	(((a)>(b))?(a):(b))

#pragma warning ( disable:4244 )
class CQuat
{
public:
	// SSE optimize this some time!
	// Important documents on optimization:
	//		http://cache-www.intel.com/cd/00/00/29/37/293748_293748.pdf
	//		http://cache-www.intel.com/cd/00/00/29/37/293747_293747.pdf

	CQuat() {
		x = y = z = w = 0;
	};
	CQuat(CMatrix* m) {
		w = sqrt( maximum( 0, 1.0f + m->vRight.fX + m->vFront.fY + m->vUp.fZ ) ) * 0.5f;
		x = sqrt( maximum( 0, 1.0f + m->vRight.fX - m->vFront.fY - m->vUp.fZ ) ) * 0.5f;
		y = sqrt( maximum( 0, 1.0f - m->vRight.fX + m->vFront.fY - m->vUp.fZ ) ) * 0.5f;
		z = sqrt( maximum( 0, 1.0f - m->vRight.fX - m->vFront.fY + m->vUp.fZ ) ) * 0.5f;
		
		x = _copysign( x, m->vUp.fY - m->vFront.fZ );
		y = _copysign( y, m->vRight.fZ - m->vUp.fX );
		z = _copysign( z, m->vFront.fX - m->vRight.fY );
	};

	static void ToMatrix(const CQuat& q, CMatrix& m){
		float xx = q.x * q.x;
		float xy = q.x * q.y;
		float xz = q.x * q.z;
		float xw = q.x * q.w;

		float yy = q.y * q.y;
		float yz = q.y * q.z;
		float yw = q.y * q.w;

		float zz = q.z * q.z;
		float zw = q.z * q.w;

		m.vRight.fX =		1.0f -	2.0f * ( yy + zz );
		m.vRight.fY =				2.0f * ( xy - zw );
		m.vRight.fZ =				2.0f * ( xz + yw );

		m.vFront.fX =			2.0f * ( xy + zw );
		m.vFront.fY =	1.0f -	2.0f * ( xx + zz );
		m.vFront.fZ =			2.0f * ( yz - xw );

		m.vUp.fX =					2.0f * ( xz - yw );
		m.vUp.fY =					2.0f * ( yz + xw );
		m.vUp.fZ =			1.0f -	2.0f * ( xx + yy );
	};

	// Linear interpolation
	static void LERP(const CQuat& qa, const CQuat& qb, CQuat& qm, float t) {
		float cosom, scale0, scale1, s;
		cosom = qa.x * qb.x + qa.y * qb.y + qa.z * qb.z + qa.w * qb.w;
		scale0 = 1.0f - t;
		scale1 = ( cosom >= 0.0f ) ? t : -t;
		qm.x = scale0 * qa.x + scale1 * qb.x;
		qm.y = scale0 * qa.y + scale1 * qb.y;
		qm.z = scale0 * qa.z + scale1 * qb.z;
		qm.w = scale0 * qa.w + scale1 * qb.w;
		s = 1.0f / sqrt( qm.x * qm.x + qm.y * qm.y + qm.z * qm.z + qm.w * qm.w );
		qm.x *= s;
		qm.y *= s;
		qm.z *= s;
		qm.w *= s;
	};

	// Spherical linear interpolation
	static void SLERP(const CQuat& qa, const CQuat& qb, CQuat& qm, float t) {
		// Calculate angle between them.
		float cosHalfTheta = qa.w * qb.w + qa.x * qb.x + qa.y * qb.y + qa.z * qb.z;
		// if qa=qb or qa=-qb then theta = 0 and we can return qa
		if (abs(cosHalfTheta) >= 1.0f){
			qm.w = qa.w;qm.x = qa.x;qm.y = qa.y;qm.z = qa.z;
			return;
		}
		// Calculate temporary values.
		float halfTheta = acos(cosHalfTheta);
		float sinHalfTheta = sqrt(1.0f - cosHalfTheta*cosHalfTheta);
		// if theta = 180 degrees then result is not fully defined
		// we could rotate around any axis normal to qa or qb
		if (fabs(sinHalfTheta) < 0.001f){ // fabs is floating point absolute
			qm.w = (qa.w * 0.5f + qb.w * 0.5f);
			qm.x = (qa.x * 0.5f + qb.x * 0.5f);
			qm.y = (qa.y * 0.5f + qb.y * 0.5f);
			qm.z = (qa.z * 0.5f + qb.z * 0.5f);
			return;
		}
		float ratioA = sin((1.0f - t) * halfTheta) / sinHalfTheta;
		float ratioB = sin(t * halfTheta) / sinHalfTheta; 
		//calculate Quaternion.
		qm.w = (qa.w * ratioA + qb.w * ratioB);
		qm.x = (qa.x * ratioA + qb.x * ratioB);
		qm.y = (qa.y * ratioA + qb.y * ratioB);
		qm.z = (qa.z * ratioA + qb.z * ratioB);
	};
private:
	float x, y, z, w;
};

#endif
