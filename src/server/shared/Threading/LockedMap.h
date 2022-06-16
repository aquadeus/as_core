////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/* written for use instead not locked std::map */

#ifndef LOCKEDMAP_H
#define LOCKEDMAP_H

#include <map>
#include <functional>
#include <assert.h>
#include <ace/Guard_T.h>
#include <functional>
#include <mutex>

#include "Common.h"

namespace ACE_Based
{
    template <class Key, class T, class Compare = std::less<Key>, class Allocator = std::allocator<std::pair<const Key,T> > >
    class LockedMap
    {
        public:

        typedef   ACE_RW_Thread_Mutex         LockType;
        typedef   ACE_Read_Guard<LockType>     ReadGuard;
        typedef   ACE_Write_Guard<LockType>    WriteGuard;

        typedef typename std::map<Key, T, Compare, Allocator>::iterator               iterator;
        typedef typename std::map<Key, T, Compare, Allocator>::const_iterator         const_iterator;
        typedef typename std::map<Key, T, Compare, Allocator>::reverse_iterator       reverse_iterator;
        typedef typename std::map<Key, T, Compare, Allocator>::const_reverse_iterator const_reverse_iterator;
        typedef typename std::map<Key, T, Compare, Allocator>::allocator_type         allocator_type;
        typedef typename std::map<Key, T, Compare, Allocator>::value_type             value_type;
        typedef typename std::map<Key, T, Compare, Allocator>::size_type              size_type;
        typedef typename std::map<Key, T, Compare, Allocator>::key_compare            key_compare;
        typedef typename std::map<Key, T, Compare, Allocator>::value_compare          value_compare;

        public:
            // Constructors
            explicit LockedMap(const Compare& comp = Compare(), const Allocator& alloc = Allocator()) : m_storage(comp, alloc)
            {}

            template <class InputIterator> LockedMap(InputIterator first, InputIterator last, const Compare& comp = Compare(), const Allocator& alloc = Allocator())
                : m_storage(first, last, comp, alloc)
            {}

            LockedMap(const LockedMap<Key, T, Compare, Allocator> & x ) : m_storage( x.m_storage )
            {}

            // Destructor
            virtual ~LockedMap(void)
            {
                WriteGuard Guard(GetLock());
            }

            // Copy
            LockedMap<Key, T, Compare, Allocator>& operator= (const LockedMap<Key,T,Compare,Allocator>& x)
            {
                WriteGuard Guard(GetLock());
                ReadGuard GuardX(x.GetLock());
                m_storage = x.m_storage;
                return *this;
            }

            // Iterators
            iterator begin()
            {
                ReadGuard Guard(GetLock());
                return m_storage.begin();
            }

            iterator end()
            {
                ReadGuard Guard(GetLock());
                return m_storage.end();
            }

            const_iterator begin() const
            {
                ReadGuard Guard(GetLock());
                return m_storage.begin();
            }

            const_iterator end() const
            {
                ReadGuard Guard(GetLock());
                return m_storage.end();
            }

            reverse_iterator rbegin()
            {
                ReadGuard Guard(GetLock());
                return m_storage.rbegin();
            }

            reverse_iterator rend()
            {
                ReadGuard Guard(GetLock());
                return m_storage.rend();
            }

            const_reverse_iterator rbegin() const
            {
                ReadGuard Guard(GetLock());
                return m_storage.rbegin();
            }

            const_reverse_iterator rend() const
            {
                ReadGuard Guard(GetLock());
                return m_storage.rend();
            }

            // Capacity
            size_type size(void) const
            {
                ReadGuard Guard(GetLock());
                return m_storage.size();
            }

            size_type max_size(void) const
            {
                ReadGuard Guard(GetLock());
                return m_storage.max_size();
            }

            bool empty(void) const
            {
                ReadGuard Guard(GetLock());
                return m_storage.empty();
            }

            // Access
            T& operator[](const Key& x)
            {
                if (find(x) == end())
                {
                    WriteGuard Guard(GetLock());
                    return m_storage[x];
                }
                else
                {
                    ReadGuard Guard(GetLock());
                    return m_storage[x];
                }
            }

            const T& operator[](const Key& x) const
            {
                ReadGuard Guard(GetLock());
                return m_storage[x];
            }

            // Modifiers
            std::pair<iterator, bool> insert(const value_type& x)
            {
                WriteGuard Guard(GetLock());
                return m_storage.insert(x);
            }

            iterator insert(iterator position, const value_type& x)
            {
                WriteGuard Guard(GetLock());
                return m_storage.insert(position, x);
            }

            template <class InputIterator> void insert(InputIterator first, InputIterator last)
            {
                WriteGuard Guard(GetLock());
                m_storage.insert(first, last);
            }

            void erase(iterator pos)
            {
                WriteGuard Guard(GetLock());
                m_storage.erase(pos);
            }

            size_type erase(const Key& x)
            {
                WriteGuard Guard(GetLock());
                return m_storage.erase(x);
            }

            void erase(iterator begin, iterator end)
            {
                WriteGuard Guard(GetLock());
                m_storage.erase(begin, end);
            }

            void swap(LockedMap<Key, T, Compare, Allocator>& x)
            {
                WriteGuard Guard(GetLock());
                WriteGuard GuardX(x.GetLock());
                m_storage.swap(x.storage);
            }

            void clear(void) 
            {
                WriteGuard Guard(GetLock());
                m_storage.clear();
            }

            // Observers
            key_compare key_comp(void) const
            {
                ReadGuard Guard(GetLock());
                return m_storage.key_comp();
            }

            value_compare value_comp(void) const
            {
                ReadGuard Guard(GetLock());
                return m_storage.value_comp();
            }

            // Operations
            const_iterator find(const Key& x) const
            {
                ReadGuard Guard(GetLock());
                return m_storage.find(x);
            }

            iterator find(const Key& x)
            {
                ReadGuard Guard(GetLock());
                return m_storage.find(x);
            }

            size_type count(const Key& x) const
            {
                ReadGuard Guard(GetLock());
                return m_storage.count(x);
            }

            const_iterator lower_bound(const Key& x) const
            {
                ReadGuard Guard(GetLock());
                return m_storage.lower_bound(x);
            }

            iterator lower_bound(const Key& x)
            {
                ReadGuard Guard(GetLock());
                return m_storage.lower_bound(x);
            }

            const_iterator upper_bound(const Key& x) const
            {
                ReadGuard Guard(GetLock());
                return m_storage.upper_bound(x);
            }

            iterator upper_bound(const Key& x)
            {
                ReadGuard Guard(GetLock());
                return m_storage.upper_bound(x);
            }

            std::pair<const_iterator,const_iterator> equal_range(const Key& x) const
            {
                ReadGuard Guard(GetLock());
                return m_storage.equal_range(x);
            }

            std::pair<iterator,iterator> equal_range(const Key& x)
            {
                ReadGuard Guard(GetLock());
                return m_storage.equal_range(x);
            }

            // Allocator
            allocator_type get_allocator(void) const
            {
                ReadGuard Guard(GetLock());
                return m_storage.get_allocator();
            }

            void safe_foreach(bool p_AutoIncrement, bool p_WriteLock, std::function<void(iterator&)> p_Predicate)
            {
                safe_foreach(p_AutoIncrement, p_Predicate, p_WriteLock);
            }

            void safe_foreach(bool p_AutoIncrement, std::function<void(iterator&)> p_Predicate, bool p_WriteLock = false)
            {
                if (p_WriteLock)
                    i_lock.acquire_write();
                else
                    i_lock.acquire_read();

                for (iterator l_Itr = m_storage.begin(); l_Itr != m_storage.end();)
                {
                    p_Predicate(l_Itr);

                    if (p_AutoIncrement)
                        l_Itr++;
                }

                i_lock.release();
            }

            void safe_foreach(bool p_AutoIncrement, std::function<const void(const_iterator&)> p_Predicate) const
            {
                i_lock.acquire_read();

                for (const_iterator l_Itr = m_storage.begin(); l_Itr != m_storage.end();)
                {
                    p_Predicate(l_Itr);

                    if (p_AutoIncrement)
                        l_Itr++;
                }

                i_lock.release();
            }

            LockType&       GetLock() { return i_lock; }
            LockType&       GetLock() const { return i_lock; }

            // may be used _ONLY_ with external locking!
            std::map<Key, T>&  getSource()
            {
                return m_storage;
            }

        protected:
            mutable LockType           i_lock;
            std::map<Key, T, Compare, Allocator>     m_storage;
    };

    template <class Key, class T, class Compare = std::less<Key>, class Allocator = std::allocator<std::pair<const Key,T> > >
    class RecursiveLockedMap
    {
        public:

        typedef   std::recursive_mutex         LockType;
        typedef   std::lock_guard<LockType>    RecursiveGuard;

        typedef typename std::map<Key, T, Compare, Allocator>::iterator               iterator;
        typedef typename std::map<Key, T, Compare, Allocator>::const_iterator         const_iterator;
        typedef typename std::map<Key, T, Compare, Allocator>::reverse_iterator       reverse_iterator;
        typedef typename std::map<Key, T, Compare, Allocator>::const_reverse_iterator const_reverse_iterator;
        typedef typename std::map<Key, T, Compare, Allocator>::allocator_type         allocator_type;
        typedef typename std::map<Key, T, Compare, Allocator>::value_type             value_type;
        typedef typename std::map<Key, T, Compare, Allocator>::size_type              size_type;
        typedef typename std::map<Key, T, Compare, Allocator>::key_compare            key_compare;
        typedef typename std::map<Key, T, Compare, Allocator>::value_compare          value_compare;

        public:
            // Constructors
            explicit RecursiveLockedMap(const Compare& comp = Compare(), const Allocator& alloc = Allocator()) : m_storage(comp, alloc)
            {}

            template <class InputIterator> RecursiveLockedMap(InputIterator first, InputIterator last, const Compare& comp = Compare(), const Allocator& alloc = Allocator())
                : m_storage(first, last, comp, alloc)
            {}

            RecursiveLockedMap(const RecursiveLockedMap<Key, T, Compare, Allocator> & x ) : m_storage( x.m_storage )
            {}

            // Destructor
            virtual ~RecursiveLockedMap(void)
            {
                RecursiveGuard Guard(GetLock());
            }

            // Copy
            RecursiveLockedMap<Key, T, Compare, Allocator>& operator= (const RecursiveLockedMap<Key,T,Compare,Allocator>& x)
            {
                RecursiveGuard Guard(GetLock());
                m_storage = x.m_storage;
                return *this;
            }

            // Iterators
            iterator begin()
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.begin();
            }

            iterator end()
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.end();
            }

            const_iterator begin() const
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.begin();
            }

            const_iterator end() const
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.end();
            }

            reverse_iterator rbegin()
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.rbegin();
            }

            reverse_iterator rend()
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.rend();
            }

            const_reverse_iterator rbegin() const
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.rbegin();
            }

            const_reverse_iterator rend() const
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.rend();
            }

            // Capacity
            size_type size(void) const
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.size();
            }

            size_type max_size(void) const
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.max_size();
            }

            bool empty(void) const
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.empty();
            }

            // Access
            T& operator[](const Key& x)
            {
                if (find(x) == end())
                {
                    RecursiveGuard Guard(GetLock());
                    return m_storage[x];
                }
                else
                {
                    RecursiveGuard Guard(GetLock());
                    return m_storage[x];
                }
            }

            const T& operator[](const Key& x) const
            {
                RecursiveGuard Guard(GetLock());
                return m_storage[x];
            }

            // Modifiers
            std::pair<iterator, bool> insert(const value_type& x)
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.insert(x);
            }

            iterator insert(iterator position, const value_type& x)
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.insert(position, x);
            }

            template <class InputIterator> void insert(InputIterator first, InputIterator last)
            {
                RecursiveGuard Guard(GetLock());
                m_storage.insert(first, last);
            }

            void erase(iterator pos)
            {
                RecursiveGuard Guard(GetLock());
                m_storage.erase(pos);
            }

            size_type erase(const Key& x)
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.erase(x);
            }

            void erase(iterator begin, iterator end)
            {
                RecursiveGuard Guard(GetLock());
                m_storage.erase(begin, end);
            }

            void swap(LockedMap<Key, T, Compare, Allocator>& x)
            {
                RecursiveGuard Guard(GetLock());
                m_storage.swap(x.storage);
            }

            void clear(void) 
            {
                RecursiveGuard Guard(GetLock());
                m_storage.clear();
            }

            // Observers
            key_compare key_comp(void) const
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.key_comp();
            }

            value_compare value_comp(void) const
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.value_comp();
            }

            // Operations
            const_iterator find(const Key& x) const
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.find(x);
            }

            iterator find(const Key& x)
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.find(x);
            }

            size_type count(const Key& x) const
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.count(x);
            }

            const_iterator lower_bound(const Key& x) const
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.lower_bound(x);
            }

            iterator lower_bound(const Key& x)
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.lower_bound(x);
            }

            const_iterator upper_bound(const Key& x) const
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.upper_bound(x);
            }

            iterator upper_bound(const Key& x)
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.upper_bound(x);
            }

            std::pair<const_iterator,const_iterator> equal_range(const Key& x) const
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.equal_range(x);
            }

            std::pair<iterator,iterator> equal_range(const Key& x)
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.equal_range(x);
            }

            // Allocator
            allocator_type get_allocator(void) const
            {
                RecursiveGuard Guard(GetLock());
                return m_storage.get_allocator();
            }

            void safe_foreach(bool p_AutoIncrement, std::function<void(iterator*)> p_Predicate)
            {
                RecursiveGuard Guard(GetLock());

                for (iterator l_Itr = m_storage.begin(); l_Itr != m_storage.end();)
                {
                    p_Predicate(&l_Itr);

                    if (p_AutoIncrement)
                        l_Itr++;
                }
            }

            void safe_foreach(bool p_AutoIncrement, std::function<const void(const_iterator*)> p_Predicate) const
            {
                RecursiveGuard Guard(GetLock());

                for (const_iterator l_Itr = m_storage.begin(); l_Itr != m_storage.end();)
                {
                    p_Predicate(&l_Itr);

                    if (p_AutoIncrement)
                        l_Itr++;
                }
            }

            LockType&       GetLock() { return i_lock; }
            LockType&       GetLock() const { return i_lock; }

            // may be used _ONLY_ with external locking!
            std::map<Key, T>&  getSource()
            {
                return m_storage;
            }

        protected:
            mutable LockType           i_lock;
            std::map<Key, T, Compare, Allocator>     m_storage;
    };

    template <class Key, class T, class Compare = std::less<Key>, class Allocator = std::allocator<std::pair<const Key,T> > >
    class LockedMultiMap
    {
        public:

        typedef   ACE_RW_Thread_Mutex          LockType;
        typedef   ACE_Read_Guard<LockType>     ReadGuard;
        typedef   ACE_Write_Guard<LockType>    WriteGuard;

        typedef typename std::multimap<Key, T, Compare, Allocator>::iterator               iterator;
        typedef typename std::multimap<Key, T, Compare, Allocator>::const_iterator         const_iterator;
        typedef typename std::multimap<Key, T, Compare, Allocator>::reverse_iterator       reverse_iterator;
        typedef typename std::multimap<Key, T, Compare, Allocator>::const_reverse_iterator const_reverse_iterator;
        typedef typename std::multimap<Key, T, Compare, Allocator>::allocator_type         allocator_type;
        typedef typename std::multimap<Key, T, Compare, Allocator>::value_type             value_type;
        typedef typename std::multimap<Key, T, Compare, Allocator>::size_type              size_type;
        typedef typename std::multimap<Key, T, Compare, Allocator>::key_compare            key_compare;
        typedef typename std::multimap<Key, T, Compare, Allocator>::value_compare          value_compare;

        public:
            // Constructors
            explicit LockedMultiMap(const Compare& comp = Compare(), const Allocator& alloc = Allocator()) : m_storage(comp, alloc)
            {}

            template <class InputIterator> LockedMultiMap(InputIterator first, InputIterator last, const Compare& comp = Compare(), const Allocator& alloc = Allocator())
                : m_storage(first, last, comp, alloc)
            {}

            LockedMultiMap(const LockedMap<Key, T, Compare, Allocator> & x ) : m_storage( x.m_storage )
            {}

            // Destructor
            virtual ~LockedMultiMap(void)
            {
                WriteGuard Guard(GetLock());
            }

            // Copy
            LockedMultiMap<Key, T, Compare, Allocator>& operator= (const LockedMultiMap<Key,T,Compare,Allocator>& x)
            {
                WriteGuard Guard(GetLock());
                ReadGuard GuardX(x.GetLock());
                m_storage = x.m_storage;
                return *this;
            }

            // Iterators
            iterator begin()
            {
                ReadGuard Guard(GetLock());
                return m_storage.begin();
            }

            iterator end()
            {
                ReadGuard Guard(GetLock());
                return m_storage.end();
            }

            const_iterator begin() const
            {
                ReadGuard Guard(GetLock());
                return m_storage.begin();
            }

            const_iterator end() const
            {
                ReadGuard Guard(GetLock());
                return m_storage.end();
            }

            reverse_iterator rbegin()
            {
                ReadGuard Guard(GetLock());
                return m_storage.rbegin();
            }

            reverse_iterator rend()
            {
                ReadGuard Guard(GetLock());
                return m_storage.rend();
            }

            const_reverse_iterator rbegin() const
            {
                ReadGuard Guard(GetLock());
                return m_storage.rbegin();
            }

            const_reverse_iterator rend() const
            {
                ReadGuard Guard(GetLock());
                return m_storage.rend();
            }

            // Capacity
            size_type size(void) const
            {
                ReadGuard Guard(GetLock());
                return m_storage.size(); 
            }

            size_type max_size(void) const
            {
                ReadGuard Guard(GetLock());
                return m_storage.max_size();
            }

            bool empty(void) const
            {
                ReadGuard Guard(GetLock());
                return m_storage.empty();
            }

            // Modifiers
            std::pair<iterator, bool> insert(const value_type& x)
            {
                WriteGuard Guard(GetLock());
                return m_storage.insert(x);
            }

            iterator insert(iterator position, const value_type& x)
            {
                WriteGuard Guard(GetLock());
                return m_storage.insert(position, x);
            }

            template <class InputIterator> void insert(InputIterator first, InputIterator last)
            {
                WriteGuard Guard(GetLock());
                m_storage.insert(first, last);
            }

            void erase(iterator pos)
            {
                WriteGuard Guard(GetLock());
                m_storage.erase(pos);
            }

            size_type erase(const Key& x)
            {
                WriteGuard Guard(GetLock());
                return m_storage.erase(x);
            }

            void erase(iterator begin, iterator end)
            {
                WriteGuard Guard(GetLock());
                m_storage.erase(begin, end);
            }

            void swap(LockedMap<Key, T, Compare, Allocator>& x)
            {
                WriteGuard Guard(GetLock());
                WriteGuard GuardX(x.GetLock());
                m_storage.swap(x.storage);
            }

            void clear(void)
            {
                WriteGuard Guard(GetLock());
                m_storage.clear();
            }

            // Observers
            key_compare key_comp(void) const
            {
                ReadGuard Guard(GetLock());
                return m_storage.key_comp();
            }

            value_compare value_comp(void) const
            {
                ReadGuard Guard(GetLock());
                return m_storage.value_comp();
            }

            // Operations
            const_iterator find(const Key& x) const
            {
                ReadGuard Guard(GetLock());
                return m_storage.find(x);
            }

            iterator find(const Key& x)
            {
                ReadGuard Guard(GetLock());
                return m_storage.find(x);
            }

            size_type count(const Key& x) const
            {
                ReadGuard Guard(GetLock());
                return m_storage.count(x);
            }

            const_iterator lower_bound(const Key& x) const
            {
                ReadGuard Guard(GetLock());
                return m_storage.lower_bound(x);
            }

            iterator lower_bound(const Key& x)
            {
                ReadGuard Guard(GetLock());
                return m_storage.lower_bound(x);
            }

            const_iterator upper_bound(const Key& x) const
            {
                ReadGuard Guard(GetLock());
                return m_storage.upper_bound(x);
            }

            iterator upper_bound(const Key& x)
            {
                ReadGuard Guard(GetLock());
                return m_storage.upper_bound(x);
            }

            std::pair<const_iterator,const_iterator> equal_range(const Key& x) const
            {
                ReadGuard Guard(GetLock());
                return m_storage.equal_range(x);
            }

            std::pair<iterator,iterator> equal_range(const Key& x)
            {
                ReadGuard Guard(GetLock());
                return m_storage.equal_range(x);
            }

            // Allocator
            allocator_type get_allocator(void) const
            {
                ReadGuard Guard(GetLock());
                return m_storage.get_allocator();
            }

            LockType&       GetLock() { return i_lock; }
            LockType&       GetLock() const { return i_lock; }

            // may be used _ONLY_ with external locking!
            std::multimap<Key, T>&  getSource()
            {
                return m_storage;
            }

        protected:
            mutable LockType           i_lock;
            std::multimap<Key, T, Compare, Allocator>     m_storage;
    };
}
#endif
