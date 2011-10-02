/*****************************************************************************
*
*  PROJECT:     Multi Theft Auto v1.0
*  LICENSE:     See LICENSE in the top level directory
*  FILE:
*  PURPOSE:
*  DEVELOPERS:  D3DOCD
*
*  Multi Theft Auto is available from http://www.multitheftauto.com/
*
*****************************************************************************/


//
//  Ranges of numbers. i.e.   100-4000, 5000-6999, 7000-7010
//  Used track blocks of vertices in a vertex buffer
//
class CRanges
{
public:
    typedef std::map < uint, uint >::iterator IterType;
    std::map < uint, uint > m_StartLastMap;

    void SetRange ( uint uiStart, uint uiLength )
    {
        if ( uiLength < 1 )
            return;
        uint uiLast = uiStart + uiLength - 1;

        // Make a hole
        UnsetRange ( uiStart, uiLength );

        // Insert
        m_StartLastMap [ uiStart ] = uiLast;

        // Maybe join adjacent ranges one day
    }

    void UnsetRange ( uint uiStart, uint uiLength )
    {
        if ( uiLength < 1 )
            return;
        uint uiLast = uiStart + uiLength - 1;
 
        RemoveObscuredRanges ( uiStart, uiLast );

        IterType iterOverlap;
        if ( GetRangeOverlappingPoint ( uiStart, iterOverlap ) )
        {
            uint uiOverlapPrevLast = iterOverlap->second;

            // Modify overlapping range last point
            uint uiNewLast = uiStart - 1;
            iterOverlap->second = uiNewLast;

            if ( uiOverlapPrevLast > uiLast )
            {
                // Need to add range after hole
                uint uiNewStart = uiLast + 1;
                m_StartLastMap[ uiNewStart ] = uiOverlapPrevLast;
            }
        }

        if ( GetRangeOverlappingPoint ( uiLast, iterOverlap ) )
        {
            // Modify overlapping range start point
            uint uiNewStart = uiLast + 1;
            uint uiOldLast = iterOverlap->second;
            m_StartLastMap.erase ( iterOverlap );
            m_StartLastMap[ uiNewStart ] = uiOldLast;
        }

    }

    void RemoveObscuredRanges ( uint uiStart, uint uiLast )
    {
        while ( true )
        {
            IterType iter = m_StartLastMap.lower_bound ( uiStart );
            // iter is on or after start

            if ( iter == m_StartLastMap.end () )
                return;

            // If last of found range is after query last, then range is not obscured
            if ( iter->second > uiLast )
                return;

            // Remove obscured
            m_StartLastMap.erase ( iter );
        }
    }

    bool GetRangeOverlappingPoint ( uint uiPoint, IterType& result )
    {
        IterType iter = m_StartLastMap.lower_bound ( uiPoint );
        // iter is on or after point - So it can't overlap the point

        if ( iter != m_StartLastMap.begin () )
        {
            iter--;
            // iter is now before point

            // If last of found range is after or at query point, then range is overlapping
            if ( iter->second >= uiPoint )
            {
                result = iter;
                return true;
            }
        }
        return false;
    }


    // Returns true if any part of the range already exists in the map
    bool IsRangeSet ( uint uiStart, uint uiLength )
    {
        if ( uiLength < 1 )
            return false;
        uint uiLast = uiStart + uiLength - 1;

        IterType iter = m_StartLastMap.lower_bound ( uiStart );
        // iter is on or after start

        if ( iter != m_StartLastMap.end () )
        {
            // If start of found range is before or at query last, then range is used
            if ( iter->first <= uiLast )
                return true;
        }

        if ( iter != m_StartLastMap.begin () )
        {
            iter--;
            // iter is now before start

            // If last of found range is after or at query start, then range is used
            if ( iter->second >= uiStart )
                return true;
        }

        return false;
    }
};



//
// SAdditionalStreamInfo
//
struct SAdditionalStreamInfo
{
    IDirect3DVertexDeclaration9*    pVertexDeclaration;
    IDirect3DVertexBuffer9*         pStreamData;
    UINT                            Stride;
    CRanges                         ConvertedRanges;
};


struct SCurrentStateInfo
{
private:
    SCurrentStateInfo ( const SCurrentStateInfo& other );
    SCurrentStateInfo& operator= ( const SCurrentStateInfo& other );
public:

    SCurrentStateInfo ( void )
    {
        memset( this, 0, sizeof(*this) );
    }

    ~SCurrentStateInfo ( void )
    {
        SAFE_RELEASE( stream1.pStreamData );
        SAFE_RELEASE( pIndexData );
        SAFE_RELEASE( decl.pVertexDeclaration );
    }

    // Info to DrawIndexPrimitive
    struct
    {
        D3DPRIMITIVETYPE                PrimitiveType;
        INT                             BaseVertexIndex;
        UINT                            MinVertexIndex;
        UINT                            NumVertices;
        UINT                            startIndex;
        UINT                            primCount;
    } args;

    // Render state
    struct
    {
        IDirect3DVertexBuffer9*         pStreamData;
        UINT                            OffsetInBytes;
        UINT                            Stride;
    } stream1;

    IDirect3DIndexBuffer9*         pIndexData;

    struct
    {
        IDirect3DVertexDeclaration9*    pVertexDeclaration;
        D3DVERTEXELEMENT9               elements[MAXD3DDECLLENGTH];
        UINT                            numElements;
        D3DVERTEXBUFFER_DESC            VertexBufferDesc1;
    } decl;
};


//
// CAdditionalVertexStreamManager
//
class CAdditionalVertexStreamManager
{
public:
    ZERO_ON_NEW
                            CAdditionalVertexStreamManager      ( void );
    virtual                 ~CAdditionalVertexStreamManager     ( void );

    void                    OnDeviceCreate                      ( IDirect3DDevice9* pDevice );
    bool                    MaybeSetAdditionalVertexStream      ( D3DPRIMITIVETYPE PrimitiveType,INT BaseVertexIndex,UINT MinVertexIndex,UINT NumVertices,UINT startIndex,UINT primCount );
    void                    MaybeUnsetAdditionalVertexStream    ( void );
    void                    OnVertexBufferDestroy               ( IDirect3DVertexBuffer9* pStreamData1 );
    void                    OnVertexBufferRangeInvalidated      ( IDirect3DVertexBuffer9* pStreamData, uint Offset, uint Size );

    static CAdditionalVertexStreamManager* GetSingleton         ( void );
protected:
    void                    SetAdditionalVertexStream           ( SCurrentStateInfo& renderState );
    bool                    UpdateCurrentStateInfo              ( SCurrentStateInfo& state );
    bool                    UpdateAdditionalStreamContent       ( SCurrentStateInfo& state, SAdditionalStreamInfo* pAdditionalStreamInfo, uint ReadOffsetStart, uint ReadSize, uint WriteOffsetStart, uint WriteSize );
    bool                    CheckCanDoThis                      ( const SCurrentStateInfo& state );
    SAdditionalStreamInfo*  GetAdditionalStreamInfo             ( IDirect3DVertexBuffer9* pStreamData1 );
    SAdditionalStreamInfo*  CreateAdditionalStreamInfo          ( const SCurrentStateInfo& state );


    IDirect3DDevice9*                           m_pDevice;
    IDirect3DVertexDeclaration9*                m_pOldVertexDeclaration;
    std::map < void*, SAdditionalStreamInfo >   m_AdditionalStreamInfoMap;
    static CAdditionalVertexStreamManager*      ms_Singleton;
};
