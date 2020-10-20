#include "StdInc.h"

ListItem_c::ListItem_c(void) {
    ((void(__thiscall *)(ListItem_c*))0x004A8DB0)(this);
}

ListItem_c::~ListItem_c(void) {
    ((void(__thiscall *)(ListItem_c*))0x004A8DC0)(this);
}

List_c::List_c(void) {
    ((void(__thiscall *)(List_c*))0x004A8DD0)(this);
}

List_c::~List_c(void) {
    ((void(__thiscall *)(List_c*))0x004A8DE0)(this);
}

void List_c::AddItem(ListItem_c * pItem) {
    assert(pItem);
    return ((void(__thiscall *)(List_c *, ListItem_c *))0x004A8DF0)(this, pItem);
}

void List_c::RemoveItem(ListItem_c * pItem) {
    assert(pItem);
    return ((void(__thiscall *)(List_c *, ListItem_c *))0x004A8E30)(this, pItem);
}

ListItem_c * List_c::GetHead(void) {
    return first;
}

ListItem_c * List_c::RemoveHead(void) {
    return ((ListItem_c *(__thiscall *)(List_c *))0x004A8E70)(this);
}

ListItem_c * List_c::GetNext(ListItem_c * pItem) {
    assert(pItem);
    return pItem->next;
}

ListItem_c * List_c::GetPrev(ListItem_c * pItem) {
    assert(pItem);
    return ((ListItem_c *(__thiscall *)(List_c *, ListItem_c *))0x004A9000)(this, pItem);
}

ListItem_c * List_c::GetItemOffset(bool bFromHead, int iOffset) {
    return ((ListItem_c *(__thiscall *)(List_c *, bool, int))0x004A9010)(this, bFromHead, iOffset);
}
