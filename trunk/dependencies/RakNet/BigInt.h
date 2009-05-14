#if !defined(_XBOX) && !defined(X360)

/*
 * BigInts are stored as 32-bit integer arrays.
 * Each integer in the array is referred to as a limb ala GMP.
 * Lower numbered limbs are less significant to the number represented.
 * eg, limb 0 is the least significant limb.
 * Also known as little-endian digit order
 */

#ifndef BIG_INT_HPP
#define BIG_INT_HPP

#include "Platform.h"
//#include <string>

namespace big
{
	// returns the degree of the base 2 monic polynomial
	// (the number of bits used to represent the number)
	// eg, 0 0 0 0 1 0 1 1 ... => 28 out of 32 used
	uint32_t Degree(uint32_t v);

	// returns the number of limbs that are actually used
	int LimbDegree(const uint32_t *n, int limbs);

	// return bits used
	uint32_t Degree(const uint32_t *n, int limbs);

	// lhs = rhs (unequal limbs)
	void Set(uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs);

	// lhs = rhs (equal limbs)
	void Set(uint32_t *lhs, int limbs, const uint32_t *rhs);

	// lhs = rhs (32-bit extension)
	void Set32(uint32_t *lhs, int lhs_limbs, const uint32_t rhs);

#if defined(BIG_ENDIAN)
	// Flip the byte order as needed to make 'n' big-endian for sharing over a network
	void ToLittleEndian(uint32_t *n, int limbs);

	// Flip the byte order as needed to make big-endian 'n' use the local byte order
	void FromLittleEndian(uint32_t *n, int limbs);
#else
	inline void ToLittleEndian(uint32_t *, int) {}
	inline void FromLittleEndian(uint32_t *, int) {}
#endif

	// Comparisons where both operands have the same number of limbs
	bool Less(int limbs, const uint32_t *lhs, const uint32_t *rhs);
	bool Greater(int limbs, const uint32_t *lhs, const uint32_t *rhs);
	bool Equal(int limbs, const uint32_t *lhs, const uint32_t *rhs);

	// lhs < rhs
	bool Less(const uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs);

	// lhs >= rhs
	inline bool GreaterOrEqual(const uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs)
	{
		return !Less(lhs, lhs_limbs, rhs, rhs_limbs);
	}

	// lhs > rhs
	bool Greater(const uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs);

	// lhs <= rhs
	inline bool LessOrEqual(const uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs)
	{
		return !Greater(lhs, lhs_limbs, rhs, rhs_limbs);
	}

	// lhs > rhs
	bool Greater32(const uint32_t *lhs, int lhs_limbs, uint32_t rhs);

	// lhs <= rhs
	inline bool LessOrEqual32(const uint32_t *lhs, int lhs_limbs, uint32_t rhs)
	{
		return !Greater32(lhs, lhs_limbs, rhs);
	}

	// lhs == rhs
	bool Equal(const uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs);

	// lhs == rhs
	bool Equal32(const uint32_t *lhs, int lhs_limbs, uint32_t rhs);

	// out = in >>> shift
	// Precondition: 0 <= shift < 31
	void ShiftRight(int limbs, uint32_t *out, const uint32_t *in, int shift);

	// {out, carry} = in <<< shift
	// Precondition: 0 <= shift < 31
	uint32_t ShiftLeft(int limbs, uint32_t *out, const uint32_t *in, int shift);

	// lhs += rhs, return carry out
	// precondition: lhs_limbs >= rhs_limbs
	uint32_t Add(uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs);

	// out = lhs + rhs, return carry out
	// precondition: lhs_limbs >= rhs_limbs
	uint32_t Add(uint32_t *out, const uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs);

	// lhs += rhs, return carry out
	// precondition: lhs_limbs > 0
	uint32_t Add32(uint32_t *lhs, int lhs_limbs, uint32_t rhs);

	// lhs -= rhs, return borrow out
	// precondition: lhs_limbs >= rhs_limbs
	int32_t Subtract(uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs);

	// out = lhs - rhs, return borrow out
	// precondition: lhs_limbs >= rhs_limbs
	int32_t Subtract(uint32_t *out, const uint32_t *lhs, int lhs_limbs, const uint32_t *rhs, int rhs_limbs);

	// lhs -= rhs, return borrow out
	// precondition: lhs_limbs > 0, result limbs = lhs_limbs
	int32_t Subtract32(uint32_t *lhs, int lhs_limbs, uint32_t rhs);

	// lhs = -rhs
	void Negate(int limbs, uint32_t *lhs, const uint32_t *rhs);

	// n = ~n, only invert bits up to the MSB, but none above that
	void BitNot(uint32_t *n, int limbs);

	// n = ~n, invert all bits, even ones above MSB
	void LimbNot(uint32_t *n, int limbs);

	// lhs ^= rhs
	void Xor(int limbs, uint32_t *lhs, const uint32_t *rhs);

	// Return the carry out from A += B << S
    uint32_t AddLeftShift32(
    	int limbs,		// Number of limbs in parameter A and B
    	uint32_t *A,			// Large number
    	const uint32_t *B,	// Large number
    	uint32_t S);			// 32-bit number

	// Return the carry out from result = A * B
    uint32_t Multiply32(
    	int limbs,		// Number of limbs in parameter A, result
    	uint32_t *result,	// Large number
    	const uint32_t *A,	// Large number
    	uint32_t B);			// 32-bit number

	// Return the carry out from X = X * M + A
    uint32_t MultiplyAdd32(
    	int limbs,	// Number of limbs in parameter A and B
    	uint32_t *X,		// Large number
    	uint32_t M,		// Large number
    	uint32_t A);		// 32-bit number

	// Return the carry out from A += B * M
    uint32_t AddMultiply32(
    	int limbs,		// Number of limbs in parameter A and B
    	uint32_t *A,			// Large number
    	const uint32_t *B,	// Large number
    	uint32_t M);			// 32-bit number

	// product = x * y
	void SimpleMultiply(
		int limbs,		// Number of limbs in parameters x, y
		uint32_t *product,	// Large number; buffer size = limbs*2
		const uint32_t *x,	// Large number
		const uint32_t *y);	// Large number

	// product = x ^ 2
	void SimpleSquare(
		int limbs,		// Number of limbs in parameter x
		uint32_t *product,	// Large number; buffer size = limbs*2
		const uint32_t *x);	// Large number

	// product = xy
	// memory space for product may not overlap with x,y
    void Multiply(
    	int limbs,		// Number of limbs in x,y
    	uint32_t *product,	// Product; buffer size = limbs*2
    	const uint32_t *x,	// Large number; buffer size = limbs
    	const uint32_t *y);	// Large number; buffer size = limbs

	// product = low half of x * y product
	void SimpleMultiplyLowHalf(
		int limbs,		// Number of limbs in parameters x, y
		uint32_t *product,	// Large number; buffer size = limbs
		const uint32_t *x,	// Large number
		const uint32_t *y);	// Large number

	// product = x^2
	// memory space for product may not overlap with x
    void Square(
    	int limbs,		// Number of limbs in x
    	uint32_t *product,	// Product; buffer size = limbs*2
    	const uint32_t *x);	// Large number; buffer size = limbs

	// Returns the remainder of N / divisor for a 32-bit divisor
    uint32_t Modulus32(
    	int limbs,     // Number of limbs in parameter N
    	const uint32_t *N,  // Large number, buffer size = limbs
    	uint32_t divisor);  // 32-bit number

	/*
	 * 'A' is overwritten with the quotient of the operation
	 * Returns the remainder of 'A' / divisor for a 32-bit divisor
	 * 
	 * Does not check for divide-by-zero
	 */
    uint32_t Divide32(
    	int limbs,		// Number of limbs in parameter A
    	uint32_t *A,			// Large number, buffer size = limbs
    	uint32_t divisor);	// 32-bit number

	// returns (n ^ -1) Mod 2^32
	uint32_t MulInverse32(uint32_t n);

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
		uint32_t *result);	// Large number, buffer size = limbs

	// {q, r} = u / v
	// Return false on divide by zero
	bool Divide(
		const uint32_t *u,	// numerator, size = u_limbs
		int u_limbs,
		const uint32_t *v,	// denominator, size = v_limbs
		int v_limbs,
		uint32_t *q,			// quotient, size = u_limbs
		uint32_t *r);		// remainder, size = v_limbs

	// r = u % v
	// Return false on divide by zero
	bool Modulus(
		const uint32_t *u,	// numerator, size = u_limbs
		int u_limbs,
		const uint32_t *v,	// denominator, size = v_limbs
		int v_limbs,
		uint32_t *r);		// remainder, size = v_limbs

	// m_inv ~= 2^(2k)/m
	// Generates m_inv parameter of BarrettModulus()
	// It is limbs in size, chopping off the 2^k bit
	// Only works for m with the high bit set
	void BarrettModulusPrecomp(
		int limbs,		// Number of limbs in m and m_inv
		const uint32_t *m,	// Modulus, size = limbs
		uint32_t *m_inv);	// Large number result, size = limbs

	// r = x mod m
	// Using Barrett's method with precomputed m_inv
	void BarrettModulus(
		int limbs,			// Number of limbs in m and m_inv
		const uint32_t *x,		// Number to reduce, size = limbs*2
		const uint32_t *m,		// Modulus, size = limbs
		const uint32_t *m_inv,	// R/Modulus, precomputed, size = limbs
		uint32_t *result);		// Large number result

	// result = (x * y) (Mod modulus)
	bool MulMod(
		int limbs,			// Number of limbs in x,y,modulus
		const uint32_t *x,		// Large number x
		const uint32_t *y,		// Large number y
		const uint32_t *modulus,	// Large number modulus
		uint32_t *result);		// Large number result

	// Convert string to bigint
	// Return 0 if string contains non-digit characters, else number of limbs used
	int ToInt(uint32_t *lhs, int max_limbs, const char *rhs, uint32_t base = 10);

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
		int limbs,		// Limbs in modulus and result
		uint32_t *result);	// Large number, buffer size = limbs

	/*
	 * Computes: result = GCD(a, b)  (greatest common divisor)
	 * 
	 * Length of result is the length of the smallest argument
	 */
	void GCD(
		const uint32_t *a,	// Large number, buffer size = a_limbs
		int a_limbs,	// Size of a
		const uint32_t *b,	// Large number, buffer size = b_limbs
		int b_limbs,	// Size of b
		uint32_t *result);	// Large number, buffer size = min(a, b)

	// root = sqrt(square)
	// Based on Newton-Raphson iteration: root_n+1 = (root_n + square/root_n) / 2
	// Doubles number of correct bits each iteration
	// Precondition: The high limb of square is non-zero
	// Returns false if it was unable to determine the root
	bool SquareRoot(
		int limbs,			// Number of limbs in root
		const uint32_t *square,	// Square to root, size = limbs * 2
		uint32_t *root);			// Output root, size = limbs

	// Calculates mod_inv from low limb of modulus for Mon*()
	uint32_t MonReducePrecomp(uint32_t modulus0);

	// Compute n_residue for Montgomery reduction
	void MonInputResidue(
		const uint32_t *n,		// Large number, buffer size = n_limbs
		int n_limbs,		// Number of limbs in n
		const uint32_t *modulus,	// Large number, buffer size = m_limbs
		int m_limbs,		// Number of limbs in modulus
		uint32_t *n_residue);	// Result, buffer size = m_limbs

	// result = a * b * r^-1 (Mod modulus) in Montgomery domain
	void MonPro(
		int limbs,				// Number of limbs in each parameter
		const uint32_t *a_residue,	// Large number, buffer size = limbs
		const uint32_t *b_residue,	// Large number, buffer size = limbs
		const uint32_t *modulus,		// Large number, buffer size = limbs
		uint32_t mod_inv,			// MonReducePrecomp() return
		uint32_t *result);			// Large number, buffer size = limbs

	// result = a^-1 (Mod modulus) in Montgomery domain
	void MonInverse(
		int limbs,				// Number of limbs in each parameter
		const uint32_t *a_residue,	// Large number, buffer size = limbs
		const uint32_t *modulus,		// Large number, buffer size = limbs
		uint32_t mod_inv,			// MonReducePrecomp() return
		uint32_t *result);			// Large number, buffer size = limbs

	// result = a * r^-1 (Mod modulus) in Montgomery domain
	// The result may be greater than the modulus, but this is okay since
	// the result is still in the RNS.  MonFinish() corrects this at the end.
	void MonReduce(
		int limbs,			// Number of limbs in each parameter
		uint32_t *s,				// Large number, buffer size = limbs*2, gets clobbered
		const uint32_t *modulus,	// Large number, buffer size = limbs
		uint32_t mod_inv,		// MonReducePrecomp() return
		uint32_t *result);		// Large number, buffer size = limbs

	// result = a * r^-1 (Mod modulus) in Montgomery domain
	void MonFinish(
		int limbs,			// Number of limbs in each parameter
		uint32_t *n,				// Large number, buffer size = limbs
		const uint32_t *modulus,	// Large number, buffer size = limbs
		uint32_t mod_inv);		// MonReducePrecomp() return

	// Computes: result = base ^ exponent (Mod modulus)
	// Using Montgomery multiplication with simple squaring method
	// Base parameter must be a Montgomery Residue created with MonInputResidue()
	void MonExpMod(
		const uint32_t *base,	// Base for exponentiation, buffer size = mod_limbs
		const uint32_t *exponent,// Exponent, buffer size = exponent_limbs
		int exponent_limbs,	// Number of limbs in exponent
		const uint32_t *modulus,	// Modulus, buffer size = mod_limbs
		int mod_limbs,		// Number of limbs in modulus
		uint32_t mod_inv,		// MonReducePrecomp() return
		uint32_t *result);		// Result, buffer size = mod_limbs

	// Computes: result = base ^ exponent (Mod modulus)
	// Using Montgomery multiplication with simple squaring method
	void ExpMod(
		const uint32_t *base,	// Base for exponentiation, buffer size = base_limbs
		int base_limbs,		// Number of limbs in base
		const uint32_t *exponent,// Exponent, buffer size = exponent_limbs
		int exponent_limbs,	// Number of limbs in exponent
		const uint32_t *modulus,	// Modulus, buffer size = mod_limbs
		int mod_limbs,		// Number of limbs in modulus
		uint32_t mod_inv,		// MonReducePrecomp() return
		uint32_t *result);		// Result, buffer size = mod_limbs

	// Computes: result = base ^ exponent (Mod modulus=mod_p*mod_q)
	// Using Montgomery multiplication with Chinese Remainder Theorem
	void ExpCRT(
		const uint32_t *base,	//	Base for exponentiation, buffer size = base_limbs
		int base_limbs,		//	Number of limbs in base
		const uint32_t *exponent,//	Exponent, buffer size = exponent_limbs
		int exponent_limbs,	//	Number of limbs in exponent
		const uint32_t *mod_p,	//	Large number, factorization of modulus, buffer size = p_limbs
		uint32_t p_inv,			//	MonModInv() return
		const uint32_t *mod_q,	//	Large number, factorization of modulus, buffer size = q_limbs
		uint32_t q_inv,			//	MonModInv() return
		const uint32_t *pinvq,	//	Large number, InvMod(p, q) precalculated, buffer size = phi_limbs
		int mod_limbs,		//	Number of limbs in p, q and phi
		uint32_t *result);		//	Result, buffer size = mod_limbs*2

	// Rabin-Miller method for finding a strong pseudo-prime
	// Preconditions: High bit and low bit of n = 1
	bool RabinMillerPrimeTest(
		const uint32_t *n,	// Number to check for primality
		int limbs,		// Number of limbs in n
		uint32_t k);			// Confidence level (40 is pretty good)

	// Generate a strong pseudo-prime using the Rabin-Miller primality test
	void GenerateStrongPseudoPrime(
		uint32_t *n,			// Output prime
		int limbs);		// Number of limbs in n
}

#endif // include guard

#endif
