/*****************************************************************************
*
*  PROJECT:		Multi Theft Auto v1.0
*  LICENSE:		See LICENSE in the top level directory
*  FILE:		sdk/CMatrix4.h
*  PURPOSE:		4x4 matrix class
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/

#ifndef __CMATRIX4_H
#define __CMATRIX4_H

#include "CVector.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

class CMatrix4
{
public:
	CMatrix4 ( void )
	{
		Identity ();
	}

	void rotX(float angle) {
		// Assuming the angle is in radians. (?)
		float c = cos(angle);
		float s = sin(angle);
		m_cell[0] = 1.0;
		m_cell[4] = 0.0;
		m_cell[8] = 0.0;
		m_cell[1] = 0.0;
		m_cell[5] = c;
		m_cell[9] = s;
		m_cell[2] = 0.0;
		m_cell[6] = -s;
		m_cell[10] = c;
	}


	void rotY(float angle) {
		// Assuming the angle is in radians. (?)
		float c = cos(angle);
		float s = sin(angle);
		m_cell[0] = c;
		m_cell[4] = 0.0;
		m_cell[8] = -s;
		m_cell[1] = 0.0;
		m_cell[5] = 1;
		m_cell[9] = 0.0;
		m_cell[2] = s;
		m_cell[6] = 0.0;
		m_cell[10] = c;
	}


	void rotZ(float angle) {
		// Assuming the angle is in radians. (?)
		float c = cos(angle);
		float s = sin(angle);
		m_cell[0] = c;
		m_cell[4] = s;
		m_cell[8] = 0.0;
		m_cell[1] = -s;
		m_cell[5] = c;
		m_cell[9] = 0.0;
		m_cell[2] = 0.0;
		m_cell[6] = 0.0;
		m_cell[10] = 1.0;
	}

	void Identity ( void )
	{
					m_cell[1] =		m_cell[2] =	m_cell[3] =
		m_cell[4] =					m_cell[6] =	m_cell[7] =
		m_cell[8] =	m_cell[9] =					m_cell[11] = 
		m_cell[12]=	m_cell[13]=		m_cell[14]=	0;

        m_cell[0] = m_cell[5] = m_cell[10] = m_cell[15] = 1.0f;
	}

	void Translate ( CVector vec )
	{
		m_cell[3] += vec.fX;
		m_cell[7] += vec.fY;
		m_cell[11]+= vec.fZ;
	}

	const CMatrix4& operator *= ( const CMatrix4& m )
	{
		float c[16];
		unsigned int idx;
		for (unsigned int i = 0; i < 4; i++)
		{
			for (unsigned int j = 0; j < 4; j++)
			{
				idx = (i * 4) + j;
				c[idx]  = m_cell[(i*4)]   * m.m_cell[j];
				c[idx] += m_cell[(i*4)+1] * m.m_cell[j+4];
				c[idx] += m_cell[(i*4)+2] * m.m_cell[j+8];
				c[idx] += m_cell[(i*4)+3] * m.m_cell[j+12];
			}
		}
		memcpy(m_cell, c, 16 * sizeof(float));
		return *this;
	}

	CVector Multiply ( const CVector& v )
	{
		CVector ret;

		ret.fX = v.fX * m_cell[0] + v.fY * m_cell[1] + v.fZ * m_cell[2] + m_cell[3];
		ret.fY = v.fX * m_cell[4] + v.fY * m_cell[5] + v.fZ * m_cell[6] + m_cell[7];
		ret.fZ = v.fX * m_cell[8] + v.fY * m_cell[9] + v.fZ * m_cell[10] + m_cell[11];

		return ret;
	}

	CMatrix4 operator * ( const CMatrix4& m ) const
	{
		CMatrix4 c(*this);
		c *= m;
		return c;
	}

	float & operator [] ( unsigned int i )
	{
		return m_cell[i];
	}

	float operator [] ( unsigned int i ) const
	{
		return m_cell[i];
	}

private:
	float m_cell[16];
};

#endif
