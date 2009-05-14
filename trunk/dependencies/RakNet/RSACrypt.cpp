#if !defined(_XBOX) && !defined(X360)

#include "RSACrypt.h"
//#include "Random.hpp"
#include "Rand.h"
#include "BigInt.h"
#include "RakAlloca.h"
#include "RakMemoryOverride.h"
using namespace big;


RSACrypt::RSACrypt()
{
	p = 0;
	q = 0;
	qInv = 0;
	dP = 0;
	dQ = 0;
	modulus = 0;

	p_inv = 0;
	q_inv = 0;
	mod_inv = 0;
	mod_limbs = 0;
	factor_limbs = 0;
	e = 0;
}

RSACrypt::~RSACrypt()
{
	cleanup();
}

void RSACrypt::cleanup()
{
	if (p) RakNet::OP_DELETE_ARRAY(p, __FILE__, __LINE__);
	p = 0;
	if (q)RakNet::OP_DELETE_ARRAY(q, __FILE__, __LINE__);
	q = 0;
	if (qInv) RakNet::OP_DELETE_ARRAY(qInv, __FILE__, __LINE__);
	qInv = 0;
	if (dQ) RakNet::OP_DELETE_ARRAY(dQ, __FILE__, __LINE__);
	dQ = 0;
	if (dP) RakNet::OP_DELETE_ARRAY(dQ, __FILE__, __LINE__);
	dP = 0;
	if (modulus) RakNet::OP_DELETE_ARRAY(modulus, __FILE__, __LINE__);
	modulus = 0;

	p_inv = 0;
	q_inv = 0;
	mod_inv = 0;
	mod_limbs = 0;
	factor_limbs = 0;
	e = 0;
}

bool RSACrypt::setPrivateKey(const uint32_t *pi, const uint32_t *qi, int halfFactorLimbs)
{
	cleanup();

	factor_limbs = halfFactorLimbs;
	mod_limbs = factor_limbs * 2;

	p = RakNet::OP_NEW_ARRAY<uint32_t>(factor_limbs, __FILE__, __LINE__ );
	q = RakNet::OP_NEW_ARRAY<uint32_t>(factor_limbs, __FILE__, __LINE__ );
	dP = RakNet::OP_NEW_ARRAY<uint32_t>(factor_limbs, __FILE__, __LINE__ );
	dQ = RakNet::OP_NEW_ARRAY<uint32_t>(factor_limbs, __FILE__, __LINE__ );
	qInv = RakNet::OP_NEW_ARRAY<uint32_t>(factor_limbs, __FILE__, __LINE__ );
	modulus = RakNet::OP_NEW_ARRAY<uint32_t>(mod_limbs, __FILE__, __LINE__ );
	if (!p || !q || !qInv || !dP || !dQ || !modulus) return false;

	// Insure that p > q
	if (Greater(factor_limbs, pi, qi))
	{
		Set(p, factor_limbs, pi);
		Set(q, factor_limbs, qi);
	}
	else
	{
		Set(q, factor_limbs, pi);
		Set(p, factor_limbs, qi);
	}

	// p1 = p-1
	uint32_t *p1 = (uint32_t *)alloca(factor_limbs*4);
	Set(p1, factor_limbs, p);
	Subtract32(p1, factor_limbs, 1);

	// q1 = q-1
	uint32_t *q1 = (uint32_t *)alloca(factor_limbs*4);
	Set(q1, factor_limbs, q);
	Subtract32(q1, factor_limbs, 1);

	// e = first number relatively prime to phi, starting at 65537
	e = 65537-2;

	uint32_t r;
	do {
		e += 2;
		GCD(&e, 1, p1, factor_limbs, &r);
		if (r != 1) continue;
		GCD(&e, 1, q1, factor_limbs, &r);
	} while (r != 1 && e >= 65537);

	if (r != 1) return false;

	// modulus = p * q
	Multiply(factor_limbs, modulus, p, q);

	// dP = (1/e) mod (p-1)
	if (!InvMod(&e, 1, p1, factor_limbs, dP))
		return false;

	// dQ = (1/e) mod (q-1)
	if (!InvMod(&e, 1, q1, factor_limbs, dQ))
		return false;

	// qInv = (1/q) mod p
	if (!InvMod(q, factor_limbs, p, factor_limbs, qInv))
		return false;

	// Prepare for Montgomery multiplication
	p_inv = MonReducePrecomp(p[0]);
	q_inv = MonReducePrecomp(q[0]);
	mod_inv = MonReducePrecomp(modulus[0]);

	return true;
}

bool RSACrypt::setPublicKey(const uint32_t *modulusi, int mod_limbsi, uint32_t ei)
{
	cleanup();

	e = ei;

	mod_limbs = mod_limbsi;

	modulus = RakNet::OP_NEW_ARRAY<uint32_t>(mod_limbs, __FILE__, __LINE__ );
	if (!modulus) return false;

	Set(modulus, mod_limbs, modulusi);

	mod_inv = MonReducePrecomp(modulus[0]);

	return true;
}

bool RSACrypt::generatePrivateKey(uint32_t limbs)
{
	uint32_t *pf = (uint32_t *)alloca(limbs*4);
	GenerateStrongPseudoPrime(pf, limbs/2);

	uint32_t *qf = (uint32_t *)alloca(limbs*4);
	GenerateStrongPseudoPrime(qf, limbs/2);

	return setPrivateKey(pf, qf, limbs/2);
}

uint32_t RSACrypt::getFactorLimbs()
{
	return factor_limbs;
}

void RSACrypt::getPrivateP(uint32_t *po)
{
	Set(po, factor_limbs, p);
}

void RSACrypt::getPrivateQ(uint32_t *qo)
{
	Set(qo, factor_limbs, q);
}

uint32_t RSACrypt::getModLimbs()
{
	return mod_limbs;
}

void RSACrypt::getPublicModulus(uint32_t *moduluso)
{
	Set(moduluso, mod_limbs, modulus);
}

uint32_t RSACrypt::getPublicExponent()
{
	return e;
}

bool RSACrypt::encrypt(uint32_t *ct, const uint32_t *pt)
{
	if (!e) return false;

	ExpMod(pt, mod_limbs, &e, 1, modulus, mod_limbs, mod_inv, ct);

	return true;
}

bool RSACrypt::decrypt(uint32_t *pt, const uint32_t *ct)
{
	if (!e) return false;

	// CRT method

	// s_p = c ^ dP mod p
	uint32_t *s_p = (uint32_t*)alloca(factor_limbs*4);
	ExpMod(ct, mod_limbs, dP, factor_limbs, p, factor_limbs, p_inv, s_p);

	// s_q = c ^ dQ mod q
	uint32_t *s_q = (uint32_t*)alloca(factor_limbs*4);
	ExpMod(ct, mod_limbs, dQ, factor_limbs, q, factor_limbs, q_inv, s_q);

	// Garner's CRT recombination

	// s_p = qInv(s_p - s_q) mod p
	if (Subtract(s_p, factor_limbs, s_q, factor_limbs))
		Add(s_p, factor_limbs, p, factor_limbs);
	MulMod(factor_limbs, qInv, s_p, p, s_p);

	// pt = s_q + s_p*q
	Multiply(factor_limbs, pt, s_p, q);
	Add(pt, mod_limbs, s_q, factor_limbs);

	return true;
}

#endif
