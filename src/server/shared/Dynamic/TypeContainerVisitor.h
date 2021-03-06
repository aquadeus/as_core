////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef TRINITY_TYPECONTAINERVISITOR_H
#define TRINITY_TYPECONTAINERVISITOR_H

/*
 * @class TypeContainerVisitor is implemented as a visitor pattern.  It is
 * a visitor to the TypeContainerList or TypeContainerMapList.  The visitor has
 * to overload its types as a visit method is called.
 */

#include "Define.h"
#include "Dynamic/TypeContainer.h"

// forward declaration
template<class T, class Y> class TypeContainerVisitor;

// visitor helper
template<class VISITOR, class TYPE_CONTAINER> void VisitorHelper(VISITOR &v, TYPE_CONTAINER &c)
{
    v.Visit(c);
}

// terminate condition for container list
template<class VISITOR> void VisitorHelper(VISITOR &/*v*/, ContainerList<TypeNull> &/*c*/)
{
}

template<class VISITOR, class T> void VisitorHelper(VISITOR &v, ContainerList<T> &c)
{
    v.Visit(c._element);
}

// recursion for container list
template<class VISITOR, class H, class T> void VisitorHelper(VISITOR &v, ContainerList<TypeList<H, T> > &c)
{
    VisitorHelper(v, c._elements);
    VisitorHelper(v, c._TailElements);
}

// terminate condition container map list
template<class VISITOR> void VisitorHelper(VISITOR &/*v*/, ContainerMapList<TypeNull> &/*c*/)
{
}

template<class VISITOR, class T> void VisitorHelper(VISITOR &v, ContainerMapList<T> &c)
{
    v.Visit(c._element);
}

// recursion container map list
template<class VISITOR, class H, class T> void VisitorHelper(VISITOR &v, ContainerMapList<TypeList<H, T> > &c)
{
    VisitorHelper(v, c._elements);
    VisitorHelper(v, c._TailElements);
}

// array list
template<class VISITOR, class T> void VisitorHelper(VISITOR &v, ContainerArrayList<T> &c)
{
    v.Visit(c._element);
}

template<class VISITOR> void VisitorHelper(VISITOR &/*v*/, ContainerArrayList<TypeNull> &/*c*/)
{
}

// recursion
template<class VISITOR, class H, class T> void VisitorHelper(VISITOR &v, ContainerArrayList<TypeList<H, T> > &c)
{
    VisitorHelper(v, c._elements);
    VisitorHelper(v, c._TailElements);
}

// for TypeMapContainer
template<class VISITOR, class OBJECT_TYPES> void VisitorHelper(VISITOR &v, TypeMapContainer<OBJECT_TYPES> &c)
{
    VisitorHelper(v, c.GetElements());
}

// for TypeMapContainer
template<class VISITOR, class OBJECT_TYPES> void VisitorHelperWithNearbies(VISITOR &v, TypeMapContainer<OBJECT_TYPES> &c)
{
    VisitorHelperWithNearbies(v, c.GetElements());
}

// terminate condition container map list
template<class VISITOR> void VisitorHelperWithNearbies(VISITOR &/*v*/, ContainerMapList<TypeNull> &/*c*/)
{
}

template<class VISITOR, class T> void VisitorHelperWithNearbies(VISITOR &v, ContainerMapList<T> &c)
{
    v.VisitWithNearbies(c._element);
}

// recursion container map list
template<class VISITOR, class H, class T> void VisitorHelperWithNearbies(VISITOR &v, ContainerMapList<TypeList<H, T> > &c)
{
    VisitorHelperWithNearbies(v, c._elements);
    VisitorHelperWithNearbies(v, c._nearbyElements);
    VisitorHelperWithNearbies(v, c._TailElements);
}

template<class VISITOR, class TYPE_CONTAINER>
class TypeContainerVisitor
{
    public:
        TypeContainerVisitor(VISITOR &v) : i_visitor(v) {}

        void Visit(TYPE_CONTAINER &c)
        {
            VisitorHelper(i_visitor, c);
        }

        void Visit(const TYPE_CONTAINER &c) const
        {
            VisitorHelper(i_visitor, c);
        }

        void VisitWithNearbies(TYPE_CONTAINER &c)
        {
            VisitorHelperWithNearbies(i_visitor, c);
        }

        void VisitWithNearbies(const TYPE_CONTAINER &c) const
        {
            VisitorHelperWithNearbies(i_visitor, c);
        }

    private:
        VISITOR &i_visitor;
};
#endif

