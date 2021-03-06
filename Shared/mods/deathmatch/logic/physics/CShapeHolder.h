/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        mods/shared_logic/logic/lua/CShapeHolder.h
 *  PURPOSE:     Tells the lua shape what BP underlying shape type is
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

template <class T, typename std::enable_if<std::is_base_of<btCollisionShape, T>::value>::type* = nullptr>
class CShapeHolder
{
public:
    virtual T* GetBtShape() const = 0;
};
