#if !defined(_XBOX) && !defined(X360)

#ifndef RSA_CRYPT_HPP
#define RSA_CRYPT_HPP

#include "Platform.h"
#include "Export.h"

class RAK_DLL_EXPORT RSACrypt
{
	uint32_t *p, p_inv, *q, q_inv, *qInv, *dP, *dQ, factor_limbs;
	uint32_t e, *modulus, mod_inv, mod_limbs;

	void cleanup();

public:
	RSACrypt();
	~RSACrypt();

public:
	bool setPrivateKey(const uint32_t *p, const uint32_t *q, int halfFactorLimbs);
	bool setPublicKey(const uint32_t *modulus, int mod_limbs, uint32_t e);

public:
	// Bitsize is limbs * 32
	// Private key size is limbs/2 words
	bool generatePrivateKey(uint32_t limbs); // limbs must be a multiple of 2

public:
	uint32_t getFactorLimbs();
	void getPrivateP(uint32_t *p); // p buffer has factor_limbs
	void getPrivateQ(uint32_t *q); // q buffer has factor_limbs

	uint32_t getModLimbs();
	void getPublicModulus(uint32_t *modulus); // modulus buffer has mod_limbs
	uint32_t getPublicExponent();

public:
	bool encrypt(uint32_t *ct, const uint32_t *pt); // pt limbs = mod_limbs
	bool decrypt(uint32_t *pt, const uint32_t *ct); // ct limbs = mod_limbs
};

#endif // RSA_CRYPT_HPP

#endif
