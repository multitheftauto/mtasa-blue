//////////////////////////////////////////////////////////////////////////
//
// simplemap.h - defines a table indexing primitive.
//
// Author: Wraith, 2k5-2k6
// Public domain. No warranty.
// 
// Abstract:	This map works on static data: it does not contain items,
//				only indexes them. Used for identifier->tag field conversion.
//
// (internal)


#ifndef _SIMPLEMAP_H_
#define _SIMPLEMAP_H_

namespace tools
{
	
template <class K, class V>
struct vpair{
	K key;
	V value;
	typedef typename K key_type;
	typedef typename V value_type;

	vpair( const K& k = K(), const V& v = V() ): key(k), value(v) {}
};

// generic comparator
template<class K>
struct cmp_default
{
	bool operator()(const K& left, const K& right)  const 
	{ return left == right; }
};

// asciiz string comparator
struct cmp_sz
{
	bool operator()(const char* p, const char* q) const
	{ 
		for( ; *p && *q && *p == *q; ++p, ++q );
		return !*p && !*q;
	}
};

#pragma warning(disable:4786)

// designed specifically to work on static data
template< class K, class V, class Pred = cmp_defalt<K> >
class simplemap
{
public:
	typedef typename vpair<K,V> pair_type;
	typedef typename Pred predicate_type;

	simplemap( const pair_type* b, const pair_type* e, predicate_type pr = predicate_type() ): m_begin(b), m_end(e), m_cmp( pr ) {}
	~simplemap() {}

	V operator [] (const  K& k ) const 
	{
		for( const pair_type* p = m_begin; p!= m_end; ++p )
			if( m_cmp(p->key, k) )
				return p->value;
		return pair_type::value_type();
	}
private:
	const pair_type* m_begin;
	const pair_type* m_end;
	predicate_type m_cmp;
};

}

// Important: wear protective gloves while using these macros.
#define SIZE_OF_ARRAY(x) (sizeof(x)/sizeof(x[0]))
#define END_OF_ARRAY(x) (x + SIZE_OF_ARRAY(x))


#endif