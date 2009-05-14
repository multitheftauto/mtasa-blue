#if !defined(_XBOX) && !defined(X360)

#include "BigInt.h"
#include <ctype.h>
#include <string.h>

#include "RakAlloca.h"
#include "RakMemoryOverride.h"
#include "Rand.h"

#if defined(_MSC_VER) && !defined(_DEBUG)
#include <intrin.h>
#endif

namespace big
{
	static const char Bits256[] = {
		0, 1, 2, 2, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4,
		5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
		8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8
	};

	// returns the degree of the base 2 monic polynomial
	// (the number of bits used to represent the number)
	// eg, 0 0 0 0 1 0 1 1 ... => 28 out of 32 used
	uint32_t Degree(uint32_t v)
	{
#if defined(_MSC_VER) && !defined(_DEBUG)
		unsigned long index;
		return _BitScanReverse(&index, v) ? (index + 1) : 0;
#else
		uint32_t r, t = v >> 16;

		if (t)	r = (r = t >> 8) ? 24 + Bits256[r] : 16 + Bits256[t];
		else 	r = (r = v >> 8) ? 8 + Bits256[r] : Bits256[v];

		return r;
#endif
	}

	// returns the number of limbs that are actually used
	int LimbDegree(const uint32_t *n, int limbs)
	{
		while (limbs--)
			if (n[limbs])
				return limbs + 1;

		return 0;
	}

	// return bits used
	uint32_t Degree(const uint32_t *n, int limbs)
	{
		uint32_t limb_degree = LimbDegree(n, limbs);
		if (!limb_degree) return 0;
		--limb_degree;

		uint32_t msl_degree = Degree(n[limb_degree]);
		
		return msl_degree + limb_degree*32;
	}

	void Set(uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs)
	{
		int min = lhs_limbs < rhs_limbs ? lhs_limbs : rhs_limbs;

		memcpy(lhs, rhs, min*4);
		memset(&lhs[min], 0, (lhs_limbs - min)*4);
	}
	void Set(uint32_t *lhs, int limbs, const uint32_t *rhs)
	{
		memcpy(lhs, rhs, limbs*4);
	}
	void Set32(uint32_t *lhs, int lhs_limbs, const uint32_t rhs)
	{
		*lhs = rhs;
		memset(&lhs[1], 0, (lhs_limbs - 1)*4);
	}

#if defined(BIG_ENDIAN)

	// Flip the byte order as needed to make 'n' big-endian for sharing over a network
	void ToLittleEndian(uint32_t *n, int limbs)
	{
		for (int ii = 0; ii < limbs; ++ii)
		{
			swapLE(n[ii]);
		}
	}

	// Flip the byte order as needed to make big-endian 'n' use the local byte order
	void FromLittleEndian(uint32_t *n, int limbs)
	{
		// Same operation as ToBigEndian()
		ToLittleEndian(n, limbs);
	}

#endif // BIG_ENDIAN

	bool Less(int limbs, const uint32_t *lhs, const uint32_t *rhs)
	{
		for (int ii = limbs-1; ii >= 0; --ii)
			if (lhs[ii] != rhs[ii])
				return lhs[ii] < rhs[ii];

		return false;
	}
	bool Greater(int limbs, const uint32_t *lhs, const uint32_t *rhs)
	{
		for (int ii = limbs-1; ii >= 0; --ii)
			if (lhs[ii] != rhs[ii])
				return lhs[ii] > rhs[ii];

		return false;
	}
	bool Equal(int limbs, const uint32_t *lhs, const uint32_t *rhs)
	{
		return 0 == memcmp(lhs, rhs, limbs*4);
	}

	bool Less(const uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs)
	{
		if (lhs_limbs > rhs_limbs)
			do if (lhs[--lhs_limbs] != 0) return false; while (lhs_limbs > rhs_limbs);
		else if (lhs_limbs < rhs_limbs)
			do if (rhs[--rhs_limbs] != 0) return true; while (lhs_limbs < rhs_limbs);
	
		while (lhs_limbs--) if (lhs[lhs_limbs] != rhs[lhs_limbs]) return lhs[lhs_limbs] < rhs[lhs_limbs];
		return false; // equal
	}
	bool Greater(const uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs)
	{
		if (lhs_limbs > rhs_limbs)
			do if (lhs[--lhs_limbs] != 0) return true; while (lhs_limbs > rhs_limbs);
		else if (lhs_limbs < rhs_limbs)
			do if (rhs[--rhs_limbs] != 0) return false; while (lhs_limbs < rhs_limbs);
	
		while (lhs_limbs--) if (lhs[lhs_limbs] != rhs[lhs_limbs]) return lhs[lhs_limbs] > rhs[lhs_limbs];
		return false; // equal
	}
	bool Equal(const uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs)
	{
		if (lhs_limbs > rhs_limbs)
			do if (lhs[--lhs_limbs] != 0) return false; while (lhs_limbs > rhs_limbs);
		else if (lhs_limbs < rhs_limbs)
			do if (rhs[--rhs_limbs] != 0) return false; while (lhs_limbs < rhs_limbs);

		while (lhs_limbs--) if (lhs[lhs_limbs] != rhs[lhs_limbs]) return false;
		return true; // equal
	}

	bool Greater32(const uint32_t *lhs, int lhs_limbs, uint32_t rhs)
	{
		if (*lhs > rhs) return true;
		while (--lhs_limbs)
			if (*++lhs) return true;
		return false;
	}
	bool Equal32(const uint32_t *lhs, int lhs_limbs, uint32_t rhs)
	{
		if (*lhs != rhs) return false;
		while (--lhs_limbs)
			if (*++lhs) return false;
		return true; // equal
	}

	// out = in >>> shift
	// Precondition: 0 <= shift < 31
	void ShiftRight(int limbs, uint32_t *out, const uint32_t *in, int shift)
	{
		if (!shift)
		{
			Set(out, limbs, in);
			return;
		}

		uint32_t carry = 0;

		for (int ii = limbs - 1; ii >= 0; --ii)
		{
			uint32_t r = in[ii];

			out[ii] = (r >> shift) | carry;

			carry = r << (32 - shift);
		}
	}

	// {out, carry} = in <<< shift
	// Precondition: 0 <= shift < 31
	uint32_t ShiftLeft(int limbs, uint32_t *out, const uint32_t *in, int shift)
	{
		if (!shift)
		{
			Set(out, limbs, in);
			return 0;
		}

		uint32_t carry = 0;

		for (int ii = 0; ii < limbs; ++ii)
		{
			uint32_t r = in[ii];

			out[ii] = (r << shift) | carry;

			carry = r >> (32 - shift);
		}

		return carry;
	}

	// lhs += rhs, return carry out
	// precondition: lhs_limbs >= rhs_limbs
	uint32_t Add(uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs)
	{
		int ii;
		uint64_t r = (uint64_t)lhs[0] + rhs[0];
		lhs[0] = (uint32_t)r;

		for (ii = 1; ii < rhs_limbs; ++ii)
		{
			r = ((uint64_t)lhs[ii] + rhs[ii]) + (uint32_t)(r >> 32);
			lhs[ii] = (uint32_t)r;
		}

		for (; ii < lhs_limbs && (uint32_t)(r >>= 32) != 0; ++ii)
		{
			r += lhs[ii];
			lhs[ii] = (uint32_t)r;
		}

		return (uint32_t)(r >> 32);
	}

	// out = lhs + rhs, return carry out
	// precondition: lhs_limbs >= rhs_limbs
	uint32_t Add(uint32_t *out, const uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs)
	{
		int ii;
		uint64_t r = (uint64_t)lhs[0] + rhs[0];
		out[0] = (uint32_t)r;

		for (ii = 1; ii < rhs_limbs; ++ii)
		{
			r = ((uint64_t)lhs[ii] + rhs[ii]) + (uint32_t)(r >> 32);
			out[ii] = (uint32_t)r;
		}

		for (; ii < lhs_limbs && (uint32_t)(r >>= 32) != 0; ++ii)
		{
			r += lhs[ii];
			out[ii] = (uint32_t)r;
		}

		return (uint32_t)(r >> 32);
	}

	// lhs += rhs, return carry out
	// precondition: lhs_limbs > 0
	uint32_t Add32(uint32_t *lhs, int lhs_limbs, uint32_t rhs)
	{
		uint32_t n = lhs[0];
		uint32_t r = n + rhs;
		lhs[0] = r;

		if (r >= n)
			return 0;

		for (int ii = 1; ii < lhs_limbs; ++ii)
			if (++lhs[ii])
				return 0;

		return 1;
	}

	// lhs -= rhs, return borrow out
	// precondition: lhs_limbs >= rhs_limbs
	int32_t Subtract(uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs)
	{
		int ii;
		int64_t r = (int64_t)lhs[0] - rhs[0];
		lhs[0] = (uint32_t)r;

		for (ii = 1; ii < rhs_limbs; ++ii)
		{
			r = ((int64_t)lhs[ii] - rhs[ii]) + (int32_t)(r >> 32);
			lhs[ii] = (uint32_t)r;
		}

		for (; ii < lhs_limbs && (int32_t)(r >>= 32) != 0; ++ii)
		{
			r += lhs[ii];
			lhs[ii] = (uint32_t)r;
		}

		return (int32_t)(r >> 32);
	}

	// out = lhs - rhs, return borrow out
	// precondition: lhs_limbs >= rhs_limbs
	int32_t Subtract(uint32_t *out, const uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs)
	{
		int ii;
		int64_t r = (int64_t)lhs[0] - rhs[0];
		out[0] = (uint32_t)r;

		for (ii = 1; ii < rhs_limbs; ++ii)
		{
			r = ((int64_t)lhs[ii] - rhs[ii]) + (int32_t)(r >> 32);
			out[ii] = (uint32_t)r;
		}

		for (; ii < lhs_limbs && (int32_t)(r >>= 32) != 0; ++ii)
		{
			r += lhs[ii];
			out[ii] = (uint32_t)r;
		}

		return (int32_t)(r >> 32);
	}

	// lhs -= rhs, return borrow out
	// precondition: lhs_limbs > 0, result limbs = lhs_limbs
	int32_t Subtract32(uint32_t *lhs, int lhs_limbs, uint32_t rhs)
	{
		uint32_t n = lhs[0];
		uint32_t r = n - rhs;
		lhs[0] = r;

		if (r <= n)
			return 0;

		for (int ii = 1; ii < lhs_limbs; ++ii)
			if (lhs[ii]--)
				return 0;

		return -1;
	}

	// lhs = -rhs
	void Negate(int limbs, uint32_t *lhs, const uint32_t *rhs)
	{
		// Propagate negations until carries stop
		while (limbs-- > 0 && !(*lhs++ = -(int32_t)(*rhs++)));

		// Then just invert the remaining words
		while (limbs-- > 0) *lhs++ = ~(*rhs++);
	}

	// n = ~n, only invert bits up to the MSB, but none above that
	void BitNot(uint32_t *n, int limbs)
	{
		limbs = LimbDegree(n, limbs);
		if (limbs)
		{
			uint32_t high = n[--limbs];
			uint32_t high_degree = 32 - Degree(high);

			n[limbs] = ((uint32_t)(~high << high_degree) >> high_degree);
			while (limbs--) n[limbs] = ~n[limbs];
		}
	}

	// n = ~n, invert all bits, even ones above MSB
	void LimbNot(uint32_t *n, int limbs)
	{
		while (limbs--) *n++ = ~(*n);
	}

	// lhs ^= rhs
	void Xor(int limbs, uint32_t *lhs, const uint32_t *rhs)
	{
		while (limbs--) *lhs++ ^= *rhs++;
	}

	// Return the carry out from A += B << S
    uint32_t AddLeftShift32(
    	int limbs,		// Number of limbs in parameter A and B
    	uint32_t *A,			// Large number
    	const uint32_t *B,	// Large number
    	uint32_t S)			// 32-bit number
	{
		uint64_t sum = 0;
		uint32_t last = 0;

		while (limbs--)
		{
			uint32_t b = *B++;

			sum = (uint64_t)((b << S) | (last >> (32 - S))) + *A + (uint32_t)(sum >> 32);

			last = b;
			*A++ = (uint32_t)sum;
		}

		return (uint32_t)(sum >> 32) + (last >> (32 - S));
	}

	// Return the carry out from result = A * B
    uint32_t Multiply32(
    	int limbs,		// Number of limbs in parameter A, result
    	uint32_t *result,	// Large number
    	const uint32_t *A,	// Large number
    	uint32_t B)			// 32-bit number
	{
		uint64_t p = (uint64_t)A[0] * B;
		result[0] = (uint32_t)p;

		while (--limbs)
		{
			p = (uint64_t)*(++A) * B + (uint32_t)(p >> 32);
			*(++result) = (uint32_t)p;
		}

		return (uint32_t)(p >> 32);
	}

	// Return the carry out from X = X * M + A
    uint32_t MultiplyAdd32(
    	int limbs,	// Number of limbs in parameter A and B
    	uint32_t *X,		// Large number
    	uint32_t M,		// Large number
    	uint32_t A)		// 32-bit number
	{
		uint64_t p = (uint64_t)X[0] * M + A;
		X[0] = (uint32_t)p;

		while (--limbs)
		{
			p = (uint64_t)*(++X) * M + (uint32_t)(p >> 32);
			*X = (uint32_t)p;
		}

		return (uint32_t)(p >> 32);
	}

	// Return the carry out from A += B * M
    uint32_t AddMultiply32(
    	int limbs,		// Number of limbs in parameter A and B
    	uint32_t *A,			// Large number
    	const uint32_t *B,	// Large number
    	uint32_t M)			// 32-bit number
	{
		// This function is roughly 85% of the cost of exponentiation
#if defined(ASSEMBLY_INTEL_SYNTAX) && !defined (_XBOX) && !defined(X360)
		ASSEMBLY_BLOCK // VS.NET, x86, 32-bit words
		{
			mov esi, [B]
			mov edi, [A]
			mov eax, [esi]
			mul [M]					; (edx,eax) = [M]*[esi]
			add eax, [edi]			; (edx,eax) += [edi]
			adc edx, 0
			; (edx,eax) = [B]*[M] + [A]

			mov [edi], eax
			; [A] = eax

			mov ecx, [limbs]
			sub ecx, 1
			jz loop_done
loop_head:
				lea esi, [esi + 4]	; ++B
				mov eax, [esi]		; eax = [B]
				mov ebx, edx		; ebx = last carry
				lea edi, [edi + 4]	; ++A
				mul [M]				; (edx,eax) = [M]*[esi]
				add eax, [edi]		; (edx,eax) += [edi]
				adc edx, 0
				add eax, ebx		; (edx,eax) += ebx
				adc edx, 0
				; (edx,eax) = [esi]*[M] + [edi] + (ebx=last carry)

				mov [edi], eax
				; [A] = eax

			sub ecx, 1
			jnz loop_head
loop_done:
			mov [M], edx	; Use [M] to copy the carry into C++ land
		}

		return M;
#else
		// Unrolled first loop
		uint64_t p = B[0] * (uint64_t)M + A[0];
		A[0] = (uint32_t)p;

		while (--limbs)
		{
			p = (*(++B) * (uint64_t)M + *(++A)) + (uint32_t)(p >> 32);
			A[0] = (uint32_t)p;
		}

		return (uint32_t)(p >> 32);
#endif
	}

	// product = x * y
	void SimpleMultiply(
		int limbs,		// Number of limbs in parameters x, y
		uint32_t *product,	// Large number; buffer size = limbs*2
		const uint32_t *x,	// Large number
		const uint32_t *y)	// Large number
	{
		// Roughly 25% of the cost of exponentiation
		product[limbs] = Multiply32(limbs, product, x, y[0]);

		uint32_t ctr = limbs;
		while (--ctr)
		{
			++product;
			product[limbs] = AddMultiply32(limbs, product, x, (++y)[0]);
		}
	}

	// product = low half of x * y product
	void SimpleMultiplyLowHalf(
		int limbs,		// Number of limbs in parameters x, y
		uint32_t *product,	// Large number; buffer size = limbs
		const uint32_t *x,	// Large number
		const uint32_t *y)	// Large number
	{
		Multiply32(limbs, product, x, y[0]);

		while (--limbs)
		{
			++product;
			++y;
			AddMultiply32(limbs, product, x, y[0]);
		}
	}

	// product = x ^ 2
	void SimpleSquare(
		int limbs,		// Number of limbs in parameter x
		uint32_t *product,	// Large number; buffer size = limbs*2
		const uint32_t *x)	// Large number
	{
		// Seems about 15% faster than SimpleMultiply() in practice
		uint32_t *cross_product = (uint32_t*)alloca(limbs*2*4);

		// Calculate square-less and repeat-less cross products
		cross_product[limbs] = Multiply32(limbs - 1, cross_product + 1, x + 1, x[0]);
		for (int ii = 1; ii < limbs - 1; ++ii)
		{
			cross_product[limbs + ii] = AddMultiply32(limbs - ii - 1, cross_product + ii*2 + 1, x + ii + 1, x[ii]);
		}

		// Calculate square products
		for (int ii = 0; ii < limbs; ++ii)
		{
			uint32_t xi = x[ii];
			uint64_t si = (uint64_t)xi * xi;
			product[ii*2] = (uint32_t)si;
			product[ii*2+1] = (uint32_t)(si >> 32);
		}

		// Multiply the cross product by 2 and add it to the square products
		product[limbs*2 - 1] += AddLeftShift32(limbs*2 - 2, product + 1, cross_product + 1, 1);
	}

	// product = xy
	// memory space for product may not overlap with x,y
    void Multiply(
    	int limbs,		// Number of limbs in x,y
    	uint32_t *product,	// Product; buffer size = limbs*2
    	const uint32_t *x,	// Large number; buffer size = limbs
    	const uint32_t *y)	// Large number; buffer size = limbs
	{
		// Stop recursing under 640 bits or odd limb count
		if (limbs < 30 || (limbs & 1))
		{
			SimpleMultiply(limbs, product, x, y);
			return;
		}

		// Compute high and low products
		Multiply(limbs/2, product, x, y);
		Multiply(limbs/2, product + limbs, x + limbs/2, y + limbs/2);

		// Compute (x1 + x2), xc = carry out
		uint32_t *xsum = (uint32_t*)alloca((limbs/2)*4);
		uint32_t xcarry = Add(xsum, x, limbs/2, x + limbs/2, limbs/2);

		// Compute (y1 + y2), yc = carry out
		uint32_t *ysum = (uint32_t*)alloca((limbs/2)*4);
		uint32_t ycarry = Add(ysum, y, limbs/2, y + limbs/2, limbs/2);

		// Compute (x1 + x2) * (y1 + y2)
		uint32_t *cross_product = (uint32_t*)alloca(limbs*4);
		Multiply(limbs/2, cross_product, xsum, ysum);

		// Subtract out the high and low products
		int32_t cross_carry = Subtract(cross_product, limbs, product, limbs); 
		cross_carry += Subtract(cross_product, limbs, product + limbs, limbs);

		// Fix the extra high carry bits of the result
		if (ycarry) cross_carry += Add(cross_product + limbs/2, limbs/2, xsum, limbs/2);
		if (xcarry) cross_carry += Add(cross_product + limbs/2, limbs/2, ysum, limbs/2);
		cross_carry += (xcarry & ycarry);

		// Add the cross product into the result
		cross_carry += Add(product + limbs/2, limbs*3/2, cross_product, limbs);

		// Add in the fixed high carry bits
		if (cross_carry) Add32(product + limbs*3/2, limbs/2, cross_carry);
	}

	// product = x^2
	// memory space for product may not overlap with x
    void Square(
    	int limbs,		// Number of limbs in x
    	uint32_t *product,	// Product; buffer size = limbs*2
    	const uint32_t *x)	// Large number; buffer size = limbs
	{
		// Stop recursing under 1280 bits or odd limb count
		if (limbs < 40 || (limbs & 1))
		{
			SimpleSquare(limbs, product, x);
			return;
		}

		// Compute high and low squares
		Square(limbs/2, product, x);
		Square(limbs/2, product + limbs, x + limbs/2);

		// Generate the cross product
		uint32_t *cross_product = (uint32_t*)alloca(limbs*4);
		Multiply(limbs/2, cross_product, x, x + limbs/2);

		// Multiply the cross product by 2 and add it to the result
		uint32_t cross_carry = AddLeftShift32(limbs, product + limbs/2, cross_product, 1);

		// Roll the carry out up to the highest limb
		if (cross_carry) Add32(product + limbs*3/2, limbs/2, cross_carry);
	}

	// Returns the remainder of N / divisor for a 32-bit divisor
    uint32_t Modulus32(
    	int limbs,		// Number of limbs in parameter N
    	const uint32_t *N,	// Large number, buffer size = limbs
    	uint32_t divisor)	// 32-bit number
	{
		uint32_t remainder = N[limbs-1] < divisor ? N[limbs-1] : 0;
		uint32_t counter = N[limbs-1] < divisor ? limbs-1 : limbs;
	
		while (counter--) remainder = (uint32_t)((((uint64_t)remainder << 32) | N[counter]) % divisor);
	
		return remainder;
	}

	/*
	 * 'A' is overwritten with the quotient of the operation
	 * Returns the remainder of 'A' / divisor for a 32-bit divisor
	 * 
	 * Does not check for divide-by-zero
	 */
    uint32_t Divide32(
    	int limbs,		// Number of limbs in parameter A
    	uint32_t *A,			// Large number, buffer size = limbs
    	uint32_t divisor)	// 32-bit number
	{
		uint64_t r = 0;
		for (int ii = limbs-1; ii >= 0; --ii)
		{
			uint64_t n = (r << 32) | A[ii];
			A[ii] = (uint32_t)(n / divisor);
			r = n % divisor;
		}
	
		return (uint32_t)r;
	}

	// returns (n ^ -1) Mod 2^32
	uint32_t MulInverse32(uint32_t n)
	{
		// {u1, g1} = 2^32 / n
		uint32_t hb = (~(n - 1) >> 31);
		uint32_t u1 = -(int32_t)(0xFFFFFFFF / n + hb);
		uint32_t g1 = ((-(int32_t)hb) & (0xFFFFFFFF % n + 1)) - n;

		if (!g1) {
			if (n != 1) return 0;
			else return 1;
		}

		uint32_t q, u = 1, g = n;

		for (;;) {
			q = g / g1;
			g %= g1;

			if (!g) {
				if (g1 != 1) return 0;
				else return u1;
			}

			u -= q*u1;
			q = g1 / g;
			g1 %= g;

			if (!g1) {
				if (g != 1) return 0;
				else return u;
			}

			u1 -= q*u;
		}
	}

	/*
	 * Computes multiplicative inverse of given number
	 * Such that: result * u = 1
	 * Using Extended Euclid's Algorithm (GCDe)
	 * 
	 * This is not always possible, so it will return false iff not possible.
	 */
	bool MulInverse(
		int limbs,		// Limbs in u and result
		const uint32_t *u,	// Large number, buffer size = limbs
		uint32_t *result)	// Large number, buffer size = limbs
	{
		uint32_t *u1 = (uint32_t*)alloca(limbs*4);
		uint32_t *u3 = (uint32_t*)alloca(limbs*4);
		uint32_t *v1 = (uint32_t*)alloca(limbs*4);
		uint32_t *v3 = (uint32_t*)alloca(limbs*4);
		uint32_t *t1 = (uint32_t*)alloca(limbs*4);
		uint32_t *t3 = (uint32_t*)alloca(limbs*4);
		uint32_t *q = (uint32_t*)alloca((limbs+1)*4);
		uint32_t *w = (uint32_t*)alloca((limbs+1)*4);

		// Unrolled first iteration
		{
			Set32(u1, limbs, 0);
			Set32(v1, limbs, 1);
			Set(v3, limbs, u);
		}

		// Unrolled second iteration
		if (!LimbDegree(v3, limbs))
			return false;

		// {q, t3} <- R / v3
		Set32(w, limbs, 0);
		w[limbs] = 1;
		Divide(w, limbs+1, v3, limbs, q, t3);

		SimpleMultiplyLowHalf(limbs, t1, q, v1);
		Add(t1, limbs, u1, limbs);

		for (;;)
		{
			if (!LimbDegree(t3, limbs))
			{
				Set(result, limbs, v1);
				return Equal32(v3, limbs, 1);
			}

			Divide(v3, limbs, t3, limbs, q, u3);
			SimpleMultiplyLowHalf(limbs, u1, q, t1);
			Add(u1, limbs, v1, limbs);

			if (!LimbDegree(u3, limbs))
			{
				Negate(limbs, result, t1);
				return Equal32(t3, limbs, 1);
			}

			Divide(t3, limbs, u3, limbs, q, v3);
			SimpleMultiplyLowHalf(limbs, v1, q, u1);
			Add(v1, limbs, t1, limbs);

			if (!LimbDegree(v3, limbs))
			{
				Set(result, limbs, u1);
				return Equal32(u3, limbs, 1);
			}

			Divide(u3, limbs, v3, limbs, q, t3);
			SimpleMultiplyLowHalf(limbs, t1, q, v1);
			Add(t1, limbs, u1, limbs);

			if (!LimbDegree(t3, limbs))
			{
				Negate(limbs, result, v1);
				return Equal32(v3, limbs, 1);
			}

			Divide(v3, limbs, t3, limbs, q, u3);
			SimpleMultiplyLowHalf(limbs, u1, q, t1);
			Add(u1, limbs, v1, limbs);

			if (!LimbDegree(u3, limbs))
			{
				Set(result, limbs, t1);
				return Equal32(t3, limbs, 1);
			}

			Divide(t3, limbs, u3, limbs, q, v3);
			SimpleMultiplyLowHalf(limbs, v1, q, u1);
			Add(v1, limbs, t1, limbs);

			if (!LimbDegree(v3, limbs))
			{
				Negate(limbs, result, u1);
				return Equal32(u3, limbs, 1);
			}

			Divide(u3, limbs, v3, limbs, q, t3);
			SimpleMultiplyLowHalf(limbs, t1, q, v1);
			Add(t1, limbs, u1, limbs);
		}
	}

	// {q, r} = u / v
	// q is not u or v
	// Return false on divide by zero
	bool Divide(
		const uint32_t *u,	// numerator, size = u_limbs
		int u_limbs,
		const uint32_t *v,	// denominator, size = v_limbs
		int v_limbs,
		uint32_t *q,			// quotient, size = u_limbs
		uint32_t *r)			// remainder, size = v_limbs
	{
		// calculate v_used and u_used
		int v_used = LimbDegree(v, v_limbs);
		if (!v_used) return false;

		int u_used = LimbDegree(u, u_limbs);

		// if u < v, avoid division
		if (u_used <= v_used && Less(u, u_used, v, v_used))
		{
			// r = u, q = 0
			Set(r, v_limbs, u, u_used);
			Set32(q, u_limbs, 0);
			return true;
		}

		// if v is 32 bits, use faster Divide32 code
		if (v_used == 1)
		{
			// {q, r} = u / v[0]
			Set(q, u_limbs, u);
			Set32(r, v_limbs, Divide32(u_limbs, q, v[0]));
			return true;
		}

		// calculate high zero bits in v's high used limb
		int shift = 32 - Degree(v[v_used - 1]);
		int uu_used = u_used;
		if (shift > 0) uu_used++;

		uint32_t *uu = (uint32_t*)alloca(uu_used*4);
		uint32_t *vv = (uint32_t*)alloca(v_used*4);

		// shift left to fill high MSB of divisor
		if (shift > 0)
		{
			ShiftLeft(v_used, vv, v, shift);
			uu[u_used] = ShiftLeft(u_used, uu, u, shift);
		}
		else
		{
			Set(uu, u_used, u);
			Set(vv, v_used, v);
		}

		int q_high_index = uu_used - v_used;

		if (GreaterOrEqual(uu + q_high_index, v_used, vv, v_used))
		{
			Subtract(uu + q_high_index, v_used, vv, v_used);
			Set32(q + q_high_index, u_used - q_high_index, 1);
		}
		else
		{
			Set32(q + q_high_index, u_used - q_high_index, 0);
		}

		uint32_t *vq_product = (uint32_t*)alloca((v_used+1)*4);

		// for each limb,
		for (int ii = q_high_index - 1; ii >= 0; --ii)
		{
			uint64_t q_full = *(uint64_t*)(uu + ii + v_used - 1) / vv[v_used - 1];
			uint32_t q_low = (uint32_t)q_full;
			uint32_t q_high = (uint32_t)(q_full >> 32);

			vq_product[v_used] = Multiply32(v_used, vq_product, vv, q_low);

			if (q_high) // it must be '1'
				Add(vq_product + 1, v_used, vv, v_used);

			if (Subtract(uu + ii, v_used + 1, vq_product, v_used + 1))
			{
				--q_low;
				if (Add(uu + ii, v_used + 1, vv, v_used) == 0)
				{
					--q_low;
					Add(uu + ii, v_used + 1, vv, v_used);
				}
			}

			q[ii] = q_low;
		}

		memset(r + v_used, 0, (v_limbs - v_used)*4);
		ShiftRight(v_used, r, uu, shift);

		return true;
	}

	// r = u % v
	// Return false on divide by zero
	bool Modulus(
		const uint32_t *u,	// numerator, size = u_limbs
		int u_limbs,
		const uint32_t *v,	// denominator, size = v_limbs
		int v_limbs,
		uint32_t *r)			// remainder, size = v_limbs
	{
		// calculate v_used and u_used
		int v_used = LimbDegree(v, v_limbs);
		if (!v_used) return false;

		int u_used = LimbDegree(u, u_limbs);

		// if u < v, avoid division
		if (u_used <= v_used && Less(u, u_used, v, v_used))
		{
			// r = u, q = 0
			Set(r, v_limbs, u, u_used);
			//Set32(q, u_limbs, 0);
			return true;
		}

		// if v is 32 bits, use faster Divide32 code
		if (v_used == 1)
		{
			// {q, r} = u / v[0]
			//Set(q, u_limbs, u);
			Set32(r, v_limbs, Modulus32(u_limbs, u, v[0]));
			return true;
		}

		// calculate high zero bits in v's high used limb
		int shift = 32 - Degree(v[v_used - 1]);
		int uu_used = u_used;
		if (shift > 0) uu_used++;

		uint32_t *uu = (uint32_t*)alloca(uu_used*4);
		uint32_t *vv = (uint32_t*)alloca(v_used*4);

		// shift left to fill high MSB of divisor
		if (shift > 0)
		{
			ShiftLeft(v_used, vv, v, shift);
			uu[u_used] = ShiftLeft(u_used, uu, u, shift);
		}
		else
		{
			Set(uu, u_used, u);
			Set(vv, v_used, v);
		}

		int q_high_index = uu_used - v_used;

		if (GreaterOrEqual(uu + q_high_index, v_used, vv, v_used))
		{
			Subtract(uu + q_high_index, v_used, vv, v_used);
			//Set32(q + q_high_index, u_used - q_high_index, 1);
		}
		else
		{
			//Set32(q + q_high_index, u_used - q_high_index, 0);
		}

		uint32_t *vq_product = (uint32_t*)alloca((v_used+1)*4);

		// for each limb,
		for (int ii = q_high_index - 1; ii >= 0; --ii)
		{
			uint64_t q_full = *(uint64_t*)(uu + ii + v_used - 1) / vv[v_used - 1];
			uint32_t q_low = (uint32_t)q_full;
			uint32_t q_high = (uint32_t)(q_full >> 32);

			vq_product[v_used] = Multiply32(v_used, vq_product, vv, q_low);

			if (q_high) // it must be '1'
				Add(vq_product + 1, v_used, vv, v_used);

			if (Subtract(uu + ii, v_used + 1, vq_product, v_used + 1))
			{
				//--q_low;
				if (Add(uu + ii, v_used + 1, vv, v_used) == 0)
				{
					//--q_low;
					Add(uu + ii, v_used + 1, vv, v_used);
				}
			}

			//q[ii] = q_low;
		}

		memset(r + v_used, 0, (v_limbs - v_used)*4);
		ShiftRight(v_used, r, uu, shift);

		return true;
	}

	// m_inv ~= 2^(2k)/m
	// Generates m_inv parameter of BarrettModulus()
	// It is limbs in size, chopping off the 2^k bit
	// Only works for m with the high bit set
	void BarrettModulusPrecomp(
		int limbs,		// Number of limbs in m and m_inv
		const uint32_t *m,	// Modulus, size = limbs
		uint32_t *m_inv)		// Large number result, size = limbs
	{
		uint32_t *q = (uint32_t*)alloca((limbs*2+1)*4);

		// q = 2^(2k)
		big::Set32(q, limbs*2, 0);
		q[limbs*2] = 1;

		// q /= m
		big::Divide(q, limbs*2+1, m, limbs, q, m_inv);

		// m_inv = q
		Set(m_inv, limbs, q);
	}

	// r = x mod m
	// Using Barrett's method with precomputed m_inv
	void BarrettModulus(
		int limbs,			// Number of limbs in m and m_inv
		const uint32_t *x,		// Number to reduce, size = limbs*2
		const uint32_t *m,		// Modulus, size = limbs
		const uint32_t *m_inv,	// R/Modulus, precomputed, size = limbs
		uint32_t *result)		// Large number result
	{
		// q2 = x * m_inv
		// Skips the low limbs+1 words and some high limbs too
		// Needs to partially calculate the next 2 words below for carries
		uint32_t *q2 = (uint32_t*)alloca((limbs+3)*4);
		int ii, jj = limbs - 1;

		// derived from the fact that m_inv[limbs] was always 1, so m_inv is the same length as modulus now
		*(uint64_t*)q2 = (uint64_t)m_inv[jj] * x[jj];
		*(uint64_t*)(q2 + 1) = (uint64_t)q2[1] + x[jj];

		for (ii = 1; ii < limbs; ++ii)
			*(uint64_t*)(q2 + ii + 1) = ((uint64_t)q2[ii + 1] + x[jj + ii]) + AddMultiply32(ii + 1, q2, m_inv + jj - ii, x[jj + ii]);

		*(uint64_t*)(q2 + ii + 1) = ((uint64_t)q2[ii + 1] + x[jj + ii]) + AddMultiply32(ii, q2 + 1, m_inv, x[jj + ii]);

		q2 += 2;

		// r2 = (q3 * m2) mod b^(k+1)
		uint32_t *r2 = (uint32_t*)alloca((limbs+1)*4);

		// Skip high words in product, also input limbs are different by 1
		Multiply32(limbs + 1, r2, q2, m[0]);
		for (int ii = 1; ii < limbs; ++ii)
			AddMultiply32(limbs + 1 - ii, r2 + ii, q2, m[ii]);

		// Correct the error of up to two modulii
		uint32_t *r = (uint32_t*)alloca((limbs+1)*4);
		if (Subtract(r, x, limbs+1, r2, limbs+1))
		{
			while (!Subtract(r, limbs+1, m, limbs));
		}
		else
		{
			while (GreaterOrEqual(r, limbs+1, m, limbs))
				Subtract(r, limbs+1, m, limbs);
		}

		Set(result, limbs, r);
	}

	// result = (x * y) (Mod modulus)
	bool MulMod(
		int limbs,			// Number of limbs in x,y,modulus
		const uint32_t *x,		// Large number x
		const uint32_t *y,		// Large number y
		const uint32_t *modulus,	// Large number modulus
		uint32_t *result)		// Large number result
	{
		uint32_t *product = (uint32_t*)alloca(limbs*2*4);

		Multiply(limbs, product, x, y);

		return Modulus(product, limbs * 2, modulus, limbs, result);
	}

	// Convert bigint to string
	/*
	std::string ToStr(const uint32_t *n, int limbs, int base)
	{
		limbs = LimbDegree(n, limbs);
		if (!limbs) return "0";

		std::string out;
		char ch;

		uint32_t *m = (uint32_t*)alloca(limbs*4);
		Set(m, limbs, n, limbs);

		while (limbs)
		{
			uint32_t mod = Divide32(limbs, m, base);
			if (mod <= 9) ch = '0' + mod;
			else ch = 'A' + mod - 10;
			out = ch + out;
			limbs = LimbDegree(m, limbs);
		}

		return out;
	}
	*/

	// Convert string to bigint
	// Return 0 if string contains non-digit characters, else number of limbs used
	int ToInt(uint32_t *lhs, int max_limbs, const char *rhs, uint32_t base)
	{
		if (max_limbs < 2) return 0;

		lhs[0] = 0;
		int used = 1;

		char ch;
		while ((ch = *rhs++))
		{
			uint32_t mod;
			if (ch >= '0' && ch <= '9') mod = ch - '0';
			else mod = toupper(ch) - 'A' + 10;
			if (mod >= base) return 0;

			// lhs *= base
			uint32_t carry = MultiplyAdd32(used, lhs, base, mod);

			// react to running out of room
			if (carry)
			{
				if (used >= max_limbs)
					return 0;

				lhs[used++] = carry;
			}
		}

		if (used < max_limbs)
			Set32(lhs+used, max_limbs-used, 0);

		return used;
	}

	/*
	 * Computes: result = GCD(a, b)  (greatest common divisor)
	 * 
	 * Length of result is the length of the smallest argument
	 */
	void GCD(
		const uint32_t *a,	//	Large number, buffer size = a_limbs
		int a_limbs,	//	Size of a
		const uint32_t *b,	//	Large number, buffer size = b_limbs
		int b_limbs,	//	Size of b
		uint32_t *result)	//	Large number, buffer size = min(a, b)
	{
		int limbs = (a_limbs <= b_limbs) ? a_limbs : b_limbs;

		uint32_t *g = (uint32_t*)alloca(limbs*4);
		uint32_t *g1 = (uint32_t*)alloca(limbs*4);

		if (a_limbs <= b_limbs)
		{
			// g = a, g1 = b (mod a)
			Set(g, limbs, a, a_limbs);
			Modulus(b, b_limbs, a, a_limbs, g1);
		}
		else
		{
			// g = b, g1 = a (mod b)
			Set(g, limbs, b, b_limbs);
			Modulus(a, a_limbs, b, b_limbs, g1);
		}

		for (;;) {
			// g = (g mod g1)
			Modulus(g, limbs, g1, limbs, g);

			if (!LimbDegree(g, limbs)) {
				Set(result, limbs, g1, limbs);
				return;
			}

			// g1 = (g1 mod g)
			Modulus(g1, limbs, g, limbs, g1);

			if (!LimbDegree(g1, limbs)) {
				Set(result, limbs, g, limbs);
				return;
			}
		}
	}

	/*
	 * Computes: result = (1/u) (Mod v)
	 * Such that: result * u (Mod v) = 1
	 * Using Extended Euclid's Algorithm (GCDe)
	 * 
	 * This is not always possible, so it will return false iff not possible.
	 */
	bool InvMod(
		const uint32_t *u,	// Large number, buffer size = u_limbs
		int u_limbs,	// Limbs in u
		const uint32_t *v,	// Large number, buffer size = limbs
		int limbs,		// Limbs in modulus(v) and result
		uint32_t *result)	// Large number, buffer size = limbs
	{
		uint32_t *u1 = (uint32_t*)alloca(limbs*4);
		uint32_t *u3 = (uint32_t*)alloca(limbs*4);
		uint32_t *v1 = (uint32_t*)alloca(limbs*4);
		uint32_t *v3 = (uint32_t*)alloca(limbs*4);
		uint32_t *t1 = (uint32_t*)alloca(limbs*4);
		uint32_t *t3 = (uint32_t*)alloca(limbs*4);
		uint32_t *q = (uint32_t*)alloca((limbs + u_limbs)*4);

		// Unrolled first iteration
		{
			Set32(u1, limbs, 0);
			Set32(v1, limbs, 1);
			Set(u3, limbs, v);

			// v3 = u % v
			Modulus(u, u_limbs, v, limbs, v3);
		}

		for (;;)
		{
			if (!LimbDegree(v3, limbs))
			{
				Subtract(result, v, limbs, u1, limbs);
				return Equal32(u3, limbs, 1);
			}

			Divide(u3, limbs, v3, limbs, q, t3);
			SimpleMultiplyLowHalf(limbs, t1, q, v1);
			Add(t1, limbs, u1, limbs);

			if (!LimbDegree(t3, limbs))
			{
				Set(result, limbs, v1);
				return Equal32(v3, limbs, 1);
			}

			Divide(v3, limbs, t3, limbs, q, u3);
			SimpleMultiplyLowHalf(limbs, u1, q, t1);
			Add(u1, limbs, v1, limbs);

			if (!LimbDegree(u3, limbs))
			{
				Subtract(result, v, limbs, t1, limbs);
				return Equal32(t3, limbs, 1);
			}

			Divide(t3, limbs, u3, limbs, q, v3);
			SimpleMultiplyLowHalf(limbs, v1, q, u1);
			Add(v1, limbs, t1, limbs);

			if (!LimbDegree(v3, limbs))
			{
				Set(result, limbs, u1);
				return Equal32(u3, limbs, 1);
			}

			Divide(u3, limbs, v3, limbs, q, t3);
			SimpleMultiplyLowHalf(limbs, t1, q, v1);
			Add(t1, limbs, u1, limbs);

			if (!LimbDegree(t3, limbs))
			{
				Subtract(result, v, limbs, v1, limbs);
				return Equal32(v3, limbs, 1);
			}

			Divide(v3, limbs, t3, limbs, q, u3);
			SimpleMultiplyLowHalf(limbs, u1, q, t1);
			Add(u1, limbs, v1, limbs);

			if (!LimbDegree(u3, limbs))
			{
				Set(result, limbs, t1);
				return Equal32(t3, limbs, 1);
			}

			Divide(t3, limbs, u3, limbs, q, v3);
			SimpleMultiplyLowHalf(limbs, v1, q, u1);
			Add(v1, limbs, t1, limbs);
		}
	}

	// root = sqrt(square)
	// Based on Newton-Raphson iteration: root_n+1 = (root_n + square/root_n) / 2
	// Doubles number of correct bits each iteration
	// Precondition: The high limb of square is non-zero
	// Returns false if it was unable to determine the root
	bool SquareRoot(
		int limbs,			// Number of limbs in root
		const uint32_t *square,	// Square to root, size = limbs * 2
		uint32_t *root)			// Output root, size = limbs
	{
		uint32_t *q = (uint32_t*)alloca(limbs*2*4);
		uint32_t *r = (uint32_t*)alloca((limbs+1)*4);

		// Take high limbs of square as the initial root guess
		Set(root, limbs, square + limbs);

		int ctr = 64;
		while (ctr--)
		{
			// {q, r} = square / root
			Divide(square, limbs*2, root, limbs, q, r);

			// root = (root + q) / 2, assuming high limbs of q = 0
			Add(q, limbs+1, root, limbs);

			// Round division up to the nearest bit
			// Fixes a problem where root is off by 1
			if (q[0] & 1) Add32(q, limbs+1, 2);

			ShiftRight(limbs+1, q, q, 1);

			// Return success if there was no change
			if (Equal(limbs, q, root))
				return true;

			// Else update root and continue
			Set(root, limbs, q);
		}

		// In practice only takes about 9 iterations, as many as 31
		// Varies slightly as number of limbs increases but not by much
		return false;
	}

	// Calculates mod_inv from low limb of modulus for Mon*()
	uint32_t MonReducePrecomp(uint32_t modulus0)
	{
		// mod_inv = -M ^ -1 (Mod 2^32)
		return MulInverse32(-(int32_t)modulus0);
	}

	// Compute n_residue for Montgomery reduction
	void MonInputResidue(
		const uint32_t *n,		//	Large number, buffer size = n_limbs
		int n_limbs,		//	Number of limbs in n
		const uint32_t *modulus,	//	Large number, buffer size = m_limbs
		int m_limbs,		//	Number of limbs in modulus
		uint32_t *n_residue)		//	Result, buffer size = m_limbs
	{
		// p = n * 2^(k*m)
		uint32_t *p = (uint32_t*)alloca((n_limbs+m_limbs)*4);
		Set(p+m_limbs, n_limbs, n, n_limbs);
		Set32(p, m_limbs, 0);

		// n_residue = p (Mod modulus)
		Modulus(p, n_limbs+m_limbs, modulus, m_limbs, n_residue);
	}

	// result = a * b * r^-1 (Mod modulus) in Montgomery domain
	void MonPro(
		int limbs,				// Number of limbs in each parameter
		const uint32_t *a_residue,	// Large number, buffer size = limbs
		const uint32_t *b_residue,	// Large number, buffer size = limbs
		const uint32_t *modulus,		// Large number, buffer size = limbs
		uint32_t mod_inv,			// MonReducePrecomp() return
		uint32_t *result)			// Large number, buffer size = limbs
	{
		uint32_t *t = (uint32_t*)alloca(limbs*2*4);

		Multiply(limbs, t, a_residue, b_residue);
		MonReduce(limbs, t, modulus, mod_inv, result);
	}

	// result = a^-1 (Mod modulus) in Montgomery domain
	void MonInverse(
		int limbs,				// Number of limbs in each parameter
		const uint32_t *a_residue,	// Large number, buffer size = limbs
		const uint32_t *modulus,		// Large number, buffer size = limbs
		uint32_t mod_inv,			// MonReducePrecomp() return
		uint32_t *result)			// Large number, buffer size = limbs
	{
		Set(result, limbs, a_residue);
		MonFinish(limbs, result, modulus, mod_inv);
		InvMod(result, limbs, modulus, limbs, result);
		MonInputResidue(result, limbs, modulus, limbs, result);
	}

	// result = a * r^-1 (Mod modulus) in Montgomery domain
	// The result may be greater than the modulus, but this is okay since
	// the result is still in the RNS.  MonFinish() corrects this at the end.
	void MonReduce(
		int limbs,			// Number of limbs in modulus
		uint32_t *s,				// Large number, buffer size = limbs*2, gets clobbered
		const uint32_t *modulus,	// Large number, buffer size = limbs
		uint32_t mod_inv,		// MonReducePrecomp() return
		uint32_t *result)		// Large number, buffer size = limbs
	{
		// This function is roughly 60% of the cost of exponentiation
		for (int ii = 0; ii < limbs; ++ii)
		{
			uint32_t q = s[0] * mod_inv;
			s[0] = AddMultiply32(limbs, s, modulus, q);
			++s;
		}

		// Add the saved carries
		if (Add(result, s, limbs, s - limbs, limbs))
		{
			// Reduce the result only when needed
			Subtract(result, limbs, modulus, limbs);
		}
	}

	// result = a * r^-1 (Mod modulus) in Montgomery domain
	void MonFinish(
		int limbs,			// Number of limbs in each parameter
		uint32_t *n,				// Large number, buffer size = limbs
		const uint32_t *modulus,	// Large number, buffer size = limbs
		uint32_t mod_inv)		// MonReducePrecomp() return
	{
		uint32_t *t = (uint32_t*)alloca(limbs*2*4);
		memcpy(t, n, limbs*4);
		memset(t + limbs, 0, limbs*4);

		// Reduce the number
		MonReduce(limbs, t, modulus, mod_inv, n);

		// Fix MonReduce() results greater than the modulus
		if (!Less(limbs, n, modulus))
			Subtract(n, limbs, modulus, limbs);
	}

	// Simple internal version without windowing for small exponents
	static void SimpleMonExpMod(
		const uint32_t *base,	//	Base for exponentiation, buffer size = mod_limbs
		const uint32_t *exponent,//	Exponent, buffer size = exponent_limbs
		int exponent_limbs,	//	Number of limbs in exponent
		const uint32_t *modulus,	//	Modulus, buffer size = mod_limbs
		int mod_limbs,		//	Number of limbs in modulus
		uint32_t mod_inv,		//	MonReducePrecomp() return
		uint32_t *result)		//	Result, buffer size = mod_limbs
	{
		bool set = false;

		uint32_t *temp = (uint32_t*)alloca((mod_limbs*2)*4);

		// Run down exponent bits and use the squaring method
		for (int ii = exponent_limbs - 1; ii >= 0; --ii)
		{
			uint32_t e_i = exponent[ii];

			for (uint32_t mask = 0x80000000; mask; mask >>= 1)
			{
				if (set)
				{
					// result = result^2
					Square(mod_limbs, temp, result);
					MonReduce(mod_limbs, temp, modulus, mod_inv, result);

					if (e_i & mask)
					{
						// result *= base
						Multiply(mod_limbs, temp, result, base);
						MonReduce(mod_limbs, temp, modulus, mod_inv, result);
					}
				}
				else
				{
					if (e_i & mask)
					{
						// result = base
						Set(result, mod_limbs, base, mod_limbs);
						set = true;
					}
				}
			}
		}
	}

	// Precompute a window for ExpMod() and MonExpMod()
	// Requires 2^window_bits multiplies
	uint32_t *PrecomputeWindow(const uint32_t *base, const uint32_t *modulus, int limbs, uint32_t mod_inv, int window_bits)
	{
		uint32_t *temp = (uint32_t*)alloca(limbs*2*4);

		uint32_t *base_squared = (uint32_t*)alloca(limbs*4);
		Square(limbs, temp, base);
		MonReduce(limbs, temp, modulus, mod_inv, base_squared);

		// precomputed window starts with 000001, 000011, 000101, 000111, ...
		uint32_t k = (1 << (window_bits - 1));

		uint32_t *window = RakNet::OP_NEW_ARRAY<uint32_t>(limbs * k, __FILE__, __LINE__ );

		uint32_t *cw = window;
		Set(window, limbs, base);

		while (--k)
		{
			// cw+1 = cw * base^2
			Multiply(limbs, temp, cw, base_squared);
			MonReduce(limbs, temp, modulus, mod_inv, cw + limbs);
			cw += limbs;
		}

		return window;
	};

	// Computes: result = base ^ exponent (Mod modulus)
	// Using Montgomery multiplication with simple squaring method
	// Base parameter must be a Montgomery Residue created with MonInputResidue()
	void MonExpMod(
		const uint32_t *base,	//	Base for exponentiation, buffer size = mod_limbs
		const uint32_t *exponent,//	Exponent, buffer size = exponent_limbs
		int exponent_limbs,	//	Number of limbs in exponent
		const uint32_t *modulus,	//	Modulus, buffer size = mod_limbs
		int mod_limbs,		//	Number of limbs in modulus
		uint32_t mod_inv,		//	MonReducePrecomp() return
		uint32_t *result)		//	Result, buffer size = mod_limbs
	{
		// Calculate the number of window bits to use (decent approximation..)
		int window_bits = Degree(exponent_limbs);

		// If the window bits are too small, might as well just use left-to-right S&M method
		if (window_bits < 4)
		{
			SimpleMonExpMod(base, exponent, exponent_limbs, modulus, mod_limbs, mod_inv, result);
			return;
		}

		// Precompute a window of the size determined above
		uint32_t *window = PrecomputeWindow(base, modulus, mod_limbs, mod_inv, window_bits);

		bool seen_bits = false;
		uint32_t e_bits=0, trailing_zeroes=0, used_bits = 0;

		uint32_t *temp = (uint32_t*)alloca((mod_limbs*2)*4);

		for (int ii = exponent_limbs - 1; ii >= 0; --ii)
		{
			uint32_t e_i = exponent[ii];

			int wordbits = 32;
			while (wordbits--)
			{
				// If we have been accumulating bits,
				if (used_bits)
				{
					// If this new bit is set,
					if (e_i >> 31)
					{
						e_bits <<= 1;
						e_bits |= 1;

						trailing_zeroes = 0;
					}
					else // the new bit is unset
					{
						e_bits <<= 1;

						++trailing_zeroes;
					}

					++used_bits;

					// If we have used up the window bits,
					if (used_bits == (uint32_t) window_bits)
					{
						// Select window index 1011 from "101110"
						uint32_t window_index = e_bits >> (trailing_zeroes + 1);

						if (seen_bits)
						{
							uint32_t ctr = used_bits - trailing_zeroes;
							while (ctr--)
							{
								// result = result^2
								Square(mod_limbs, temp, result);
								MonReduce(mod_limbs, temp, modulus, mod_inv, result);
							}

							// result = result * window[index]
							Multiply(mod_limbs, temp, result, &window[window_index * mod_limbs]);
							MonReduce(mod_limbs, temp, modulus, mod_inv, result);
						}
						else
						{
							// result = window[index]
							Set(result, mod_limbs, &window[window_index * mod_limbs]);
							seen_bits = true;
						}

						while (trailing_zeroes--)
						{
							// result = result^2
							Square(mod_limbs, temp, result);
							MonReduce(mod_limbs, temp, modulus, mod_inv, result);
						}

						used_bits = 0;
					}
				}
				else
				{
					// If this new bit is set,
					if (e_i >> 31)
					{
						used_bits = 1;
						e_bits = 1;
						trailing_zeroes = 0;
					}
					else // the new bit is unset
					{
						// If we have processed any bits yet,
						if (seen_bits)
						{
							// result = result^2
							Square(mod_limbs, temp, result);
							MonReduce(mod_limbs, temp, modulus, mod_inv, result);
						}
					}
				}

				e_i <<= 1;
			}
		}

		if (used_bits)
		{
			// Select window index 1011 from "101110"
			uint32_t window_index = e_bits >> (trailing_zeroes + 1);

			if (seen_bits)
			{
				uint32_t ctr = used_bits - trailing_zeroes;
				while (ctr--)
				{
					// result = result^2
					Square(mod_limbs, temp, result);
					MonReduce(mod_limbs, temp, modulus, mod_inv, result);
				}

				// result = result * window[index]
				Multiply(mod_limbs, temp, result, &window[window_index * mod_limbs]);
				MonReduce(mod_limbs, temp, modulus, mod_inv, result);
			}
			else
			{
				// result = window[index]
				Set(result, mod_limbs, &window[window_index * mod_limbs]);
				//seen_bits = true;
			}

			while (trailing_zeroes--)
			{
				// result = result^2
				Square(mod_limbs, temp, result);
				MonReduce(mod_limbs, temp, modulus, mod_inv, result);
			}

			//e_bits = 0;
		}

		RakNet::OP_DELETE_ARRAY(window, __FILE__, __LINE__);
	}

	// Computes: result = base ^ exponent (Mod modulus)
	// Using Montgomery multiplication with simple squaring method
	void ExpMod(
		const uint32_t *base,	//	Base for exponentiation, buffer size = base_limbs
		int base_limbs,		//	Number of limbs in base
		const uint32_t *exponent,//	Exponent, buffer size = exponent_limbs
		int exponent_limbs,	//	Number of limbs in exponent
		const uint32_t *modulus,	//	Modulus, buffer size = mod_limbs
		int mod_limbs,		//	Number of limbs in modulus
		uint32_t mod_inv,		//	MonReducePrecomp() return
		uint32_t *result)		//	Result, buffer size = mod_limbs
	{
		uint32_t *mon_base = (uint32_t*)alloca(mod_limbs*4);
		MonInputResidue(base, base_limbs, modulus, mod_limbs, mon_base);

		MonExpMod(mon_base, exponent, exponent_limbs, modulus, mod_limbs, mod_inv, result);

		MonFinish(mod_limbs, result, modulus, mod_inv);
	}

	// returns b ^ e (Mod m)
	uint32_t ExpMod(uint32_t b, uint32_t e, uint32_t m)
	{
		// validate arguments
		if (b == 0 || m <= 1) return 0;
		if (e == 0) return 1;

		// find high bit of exponent
		uint32_t mask = 0x80000000;
		while ((e & mask) == 0) mask >>= 1;

		// seen 1 set bit, so result = base so far
		uint32_t r = b;

		while (mask >>= 1)
		{
			// VS.NET does a poor job recognizing that the division
			// is just an IDIV with a 32-bit dividend (not 64-bit) :-(

			// r = r^2 (mod m)
			r = (uint32_t)(((uint64_t)r * r) % m);

			// if exponent bit is set, r = r*b (mod m)
			if (e & mask) r = (uint32_t)(((uint64_t)r * b) % m);
		}

		return r;
	}

	// Rabin-Miller method for finding a strong pseudo-prime
	// Preconditions: High bit and low bit of n = 1
	bool RabinMillerPrimeTest(
		const uint32_t *n,	// Number to check for primality
		int limbs,		// Number of limbs in n
		uint32_t k)			// Confidence level (40 is pretty good)
	{
		// n1 = n - 1
		uint32_t *n1 = (uint32_t *)alloca(limbs*4);
		Set(n1, limbs, n);
		Subtract32(n1, limbs, 1);

		// d = n1
		uint32_t *d = (uint32_t *)alloca(limbs*4);
		Set(d, limbs, n1);

		// remove factors of two from d
		while (!(d[0] & 1))
			ShiftRight(limbs, d, d, 1);

		uint32_t *a = (uint32_t *)alloca(limbs*4);
		uint32_t *t = (uint32_t *)alloca(limbs*4);
		uint32_t *p = (uint32_t *)alloca((limbs*2)*4);
		uint32_t n_inv = MonReducePrecomp(n[0]);

		// iterate k times
		while (k--)
		{
			//do Random::ref()->generate(a, limbs*4);
			do fillBufferMT(a,limbs*4);
			while (GreaterOrEqual(a, limbs, n, limbs));

			// a = a ^ d (Mod n)
			ExpMod(a, limbs, d, limbs, n, limbs, n_inv, a);

			Set(t, limbs, d);
			while (!Equal(limbs, t, n1) &&
				   !Equal32(a, limbs, 1) &&
				   !Equal(limbs, a, n1))
			{
				// a = a^2 (Mod n), non-critical path
				Square(limbs, p, a);
				Modulus(p, limbs*2, n, limbs, a);

				// t <<= 1
				ShiftLeft(limbs, t, t, 1);
			}

			if (!Equal(limbs, a, n1) && !(t[0] & 1)) return false;
		}
		
		return true;
	}

	// Generate a strong pseudo-prime using the Rabin-Miller primality test
	void GenerateStrongPseudoPrime(
		uint32_t *n,			// Output prime
		int limbs)		// Number of limbs in n
	{
		do {
			fillBufferMT(n,limbs*4);
			n[limbs-1] |= 0x80000000;
			n[0] |= 1;
		} while (!RabinMillerPrimeTest(n, limbs, 40)); // 40 iterations
	}
}

#endif
