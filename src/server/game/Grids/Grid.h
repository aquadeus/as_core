////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef TRINITY_GRID_H
#define TRINITY_GRID_H

/*
  @class Grid
  Grid is a logical segment of the game world represented inside TrinIty.
  Grid is bind at compile time to a particular type of object which
  we call it the object of interested.  There are many types of loader,
  specially, dynamic loader, static loader, or on-demand loader.  There's
  a subtle difference between dynamic loader and on-demand loader but
  this is implementation specific to the loader class.  From the
  Grid's perspective, the loader meets its API requirement is suffice.
*/

#include "Define.h"
#include "TypeContainer.h"
#include "TypeContainerVisitor.h"

// forward declaration
template<class A, class T, class O> class GridLoader;

template
<
class ACTIVE_OBJECT,
class WORLD_OBJECT_TYPES,
class GRID_OBJECT_TYPES
>
class Grid
{
    // allows the GridLoader to access its internals
    template<class A, class T, class O> friend class GridLoader;
    public:

        /** destructor to clean up its resources. This includes unloading the
        grid if it has not been unload.
        */
        ~Grid() {}

        /** an object of interested enters the grid
         */
        template<class SPECIFIC_OBJECT> void AddWorldObject(SPECIFIC_OBJECT *obj)
        {
            i_objects.template insert<SPECIFIC_OBJECT>(obj);
            ASSERT(obj->IsInGrid());
        }

        template<class SPECIFIC_OBJECT> void AddNearbyWorldObject(SPECIFIC_OBJECT* p_WorldObject)
        {
            i_objects.template insert_nearby<SPECIFIC_OBJECT>(p_WorldObject);
        }

        template<class SPECIFIC_OBJECT> void RemoveNearbyWorldObject(SPECIFIC_OBJECT* p_WorldObject)
        {
            i_objects.template remove_nearby<SPECIFIC_OBJECT>(p_WorldObject);
        }

        /** an object of interested exits the grid
         */
        //Actually an unlink is enough, no need to go through the container
        //template<class SPECIFIC_OBJECT> void RemoveWorldObject(SPECIFIC_OBJECT *obj)
        //{
        //    ASSERT(obj->GetGridRef().isValid());
        //    i_objects.template remove<SPECIFIC_OBJECT>(obj);
        //    ASSERT(!obj->GetGridRef().isValid());
        //}

        /** Refreshes/update the grid. This required for remote grids.
         */
        //void RefreshGrid(void) { /* TBI */}

        /** Locks a grid.  Any object enters must wait until the grid is unlock.
         */
        //void LockGrid(void) { /* TBI */ }

        /** Unlocks the grid.
         */
        //void UnlockGrid(void) { /* TBI */ }

        // Visit grid objects
        template<class T>
        void Visit(TypeContainerVisitor<T, TypeMapContainer<GRID_OBJECT_TYPES> > &visitor)
        {
            visitor.Visit(i_container);
        }

        // Visit world objects
        template<class T>
        void Visit(TypeContainerVisitor<T, TypeMapContainer<WORLD_OBJECT_TYPES> > &visitor)
        {
            visitor.Visit(i_objects);
        }

        // Visit grid objects
        template<class T>
        void VisitWithNearbies(TypeContainerVisitor<T, TypeMapContainer<GRID_OBJECT_TYPES> > &visitor)
        {
            visitor.VisitWithNearbies(i_container);
        }

        // Visit world objects
        template<class T>
        void VisitWithNearbies(TypeContainerVisitor<T, TypeMapContainer<WORLD_OBJECT_TYPES> > &visitor)
        {
            visitor.VisitWithNearbies(i_objects);
        }

        /** Returns the number of object within the grid.
         */
        //unsigned int ActiveObjectsInGrid(void) const { return i_objects.template Count<ACTIVE_OBJECT>(); }
        template<class T>
        uint32 GetWorldObjectCountInGrid() const
        {
            return i_objects.template Count<T>();
        }

        /** Inserts a container type object into the grid.
         */
        template<class SPECIFIC_OBJECT> void AddGridObject(SPECIFIC_OBJECT *obj)
        {
            i_container.template insert<SPECIFIC_OBJECT>(obj);
            ASSERT(obj->IsInGrid());
        }

        template<class SPECIFIC_OBJECT> void AddNearbyGridObject(SPECIFIC_OBJECT* p_WorldObject)
        {
            i_container.template insert_nearby<SPECIFIC_OBJECT>(p_WorldObject);
        }

        template<class SPECIFIC_OBJECT> void RemoveNearbyGridObject(SPECIFIC_OBJECT* p_WorldObject)
        {
            i_container.template remove_nearby<SPECIFIC_OBJECT>(p_WorldObject);
        }

        /** Removes a containter type object from the grid
         */
        //template<class SPECIFIC_OBJECT> void RemoveGridObject(SPECIFIC_OBJECT *obj)
        //{
        //    ASSERT(obj->GetGridRef().isValid());
        //    i_container.template remove<SPECIFIC_OBJECT>(obj);
        //    ASSERT(!obj->GetGridRef().isValid());
        //}

        /*bool NoWorldObjectInGrid() const
        {
            return i_objects.GetElements().isEmpty();
        }

        bool NoGridObjectInGrid() const
        {
            return i_container.GetElements().isEmpty();
        }*/
    private:

        TypeMapContainer<GRID_OBJECT_TYPES> i_container;
        TypeMapContainer<WORLD_OBJECT_TYPES> i_objects;
        //typedef std::set<void*> ActiveGridObjects;
        //ActiveGridObjects m_activeGridObjects;
};
#endif

