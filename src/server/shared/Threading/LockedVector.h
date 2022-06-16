////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

/* based on LockedQueue class from MaNGOS */
/* written for use instead not locked std::list && std::vector */

#ifndef LOCKEDVECTOR_H
#define LOCKEDVECTOR_H

#include "Common.h"
#include <functional>

namespace ACE_Based
{
    template <class T, class Allocator = std::allocator<T> >
    class LockedVector
    {
        public:

        typedef   ACE_RW_Thread_Mutex          LockType;
        typedef   ACE_Read_Guard<LockType>     ReadGuard;
        typedef   ACE_Write_Guard<LockType>    WriteGuard;

        typedef typename std::vector<T, Allocator>::iterator               iterator;
        typedef typename std::vector<T, Allocator>::const_iterator         const_iterator;
        typedef typename std::vector<T, Allocator>::reverse_iterator       reverse_iterator;
        typedef typename std::vector<T, Allocator>::const_reverse_iterator const_reverse_iterator;
        typedef typename std::vector<T, Allocator>::allocator_type         allocator_type;
        typedef typename std::vector<T, Allocator>::value_type             value_type;
        typedef typename std::vector<T, Allocator>::size_type              size_type;
        typedef typename std::vector<T, Allocator>::difference_type        difference_type;

        public:
            //Constructors
            explicit LockedVector(const Allocator& alloc = Allocator()) : m_storage(alloc)
            {}

            explicit LockedVector(size_type n, const T& value = T(), const Allocator& alloc = Allocator())
                : m_storage(n,value,alloc)
            {}

            virtual ~LockedVector(void)
            {
                WriteGuard Guard(GetLock());
            }

            void reserve(size_type idx)
            {
                WriteGuard Guard(GetLock());
                m_storage.reserve(idx);
            }

            // Methods
            void push_back(const T& item)
            {
                WriteGuard Guard(GetLock());
                m_storage.push_back(item);
            }

            void insert(iterator pos, size_type n, const T& u )
            {
                WriteGuard Guard(GetLock());
                m_storage.insert( pos, n, u );
            }

            template <class InputIterator>
            void insert(iterator pos, InputIterator begin, InputIterator end)
            {
                WriteGuard Guard(GetLock());
                m_storage.insert(pos, begin, end);
            }

            void pop_back()
            {
                WriteGuard Guard(GetLock());
                m_storage.pop_back();
            }

            void erase(size_type pos)
            {
                WriteGuard Guard(GetLock());
                m_storage.erase(m_storage.begin() + pos);
            }

            iterator erase(iterator itr)
            {
                WriteGuard Guard(GetLock());
                return m_storage.erase(itr);
            }

            void remove(const T& item)
            {
                erase(item);
            }

            void erase(const T& item)
            {
                WriteGuard Guard(GetLock());
                for (size_type i = 0; i < m_storage.size();)
                {
                    if (item == m_storage[i])
                        m_storage.erase(m_storage.begin() + i);
                    else
                        ++i;
                }
            }

            T* find(const T& item)
            {
                ReadGuard Guard(GetLock());
                for (size_type i = 0; i < m_storage.size(); ++i)
                {
                    if (item == m_storage[i])
                        return &m_storage[i];
                }
                return NULL;
            }

            const T* find(const T& item) const
            {
                ReadGuard Guard(GetLock());
                for (size_type i = 0; i < m_storage.size(); ++i)
                {
                    if (item == m_storage[i])
                        return &m_storage[i];
                }
                return NULL;
            }

            bool exist(std::function<bool(T)> p_Predicat) const
            {
                ReadGuard Guard(GetLock());
                for (size_type i = 0; i < m_storage.size(); ++i)
                {
                    if (p_Predicat(m_storage[i]))
                        return true;
                }

                return false;
            }

            void foreach_until(std::function<bool(T)> p_Predicat) const
            {
                ReadGuard Guard(GetLock());
                for (size_type i = 0; i < m_storage.size(); ++i)
                {
                    if (p_Predicat(m_storage[i]))
                        return;
                }
            }

            iterator find_itr(std::function<bool(T)> p_Predicat)
            {
                ReadGuard Guard(GetLock());
                
                for (auto itr = m_storage.begin(); itr != m_storage.end();  itr++)
                {
                    if (p_Predicat(*itr))
                        return itr;
                }

                return m_storage.end();
            }

            void clear()
            {
                WriteGuard Guard(GetLock());
                m_storage.clear();
            }

            T& operator[](size_type idx)
            {
                ReadGuard Guard(GetLock());
                return m_storage[idx];
            }

            const T& operator[](size_type idx) const
            {
                ReadGuard Guard(GetLock());
                return m_storage[idx];
            }

            T& at(size_type idx)
            {
                ReadGuard Guard(GetLock());
                return m_storage.at(idx);
            }

            T& front()
            {
                ReadGuard Guard(GetLock());
                return m_storage.front();
            }

            T& back()
            {
                ReadGuard Guard(GetLock());
                return m_storage.back();
            }

            const T& front() const
            {
                ReadGuard Guard(GetLock());
                return m_storage.front();
            }

            const T& back() const
            {
                ReadGuard Guard(GetLock());
                return m_storage.back();
            }

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

            bool empty() const
            {
                ReadGuard Guard(GetLock());
                return    m_storage.empty();
            }

            size_type size() const
            {
                ReadGuard Guard(GetLock());
                return    m_storage.size();
            }

            LockedVector& operator=(const std::vector<T> &v)
            {
                clear();
                WriteGuard Guard(GetLock());
                for (typename std::vector<T>::const_iterator i = v.begin(); i != v.end(); ++i)
                {
                    this->push_back(*i);
                }
                return *this;
            }

            LockedVector(const std::vector<T> &v)
            {
                WriteGuard Guard(GetLock());
                for (typename std::vector<T>::const_iterator i = v.begin(); i != v.end(); ++i)
                {
                    this->push_back(*i);
                }
            }

            LockedVector& operator=(const std::list<T> &v)
            {
                clear();
                WriteGuard Guard(GetLock());
                for (typename std::list<T>::const_iterator i = v.begin(); i != v.end(); ++i)
                {
                    this->push_back(*i);
                }
                return *this;
            }

            LockedVector(const std::list<T> &v)
            {
                WriteGuard Guard(GetLock());
                for (typename std::list<T>::const_iterator i = v.begin(); i != v.end(); ++i)
                {
                    this->push_back(*i);
                }
            }

            LockedVector& operator=(const LockedVector<T> &v)
            {
                WriteGuard Guard(GetLock());
                ReadGuard GuardX(v.GetLock());
                m_storage = v.m_storage;
                return *this;
            }

            LockedVector(const LockedVector<T> &v)
            {
                WriteGuard Guard(GetLock());
                ReadGuard GuardX(v.GetLock());
                m_storage = v.m_storage;
            }

            void swap(LockedVector<T, Allocator>& x)
            {
                WriteGuard Guard(GetLock());
                WriteGuard GuardX(x.GetLock());
                m_storage.swap(x.m_storage);
            }

            void resize(size_type num, T def = T())
            {
                WriteGuard Guard(GetLock());
                m_storage.resize(num, def);
            }

            //Allocator
            allocator_type get_allocator() const
            {
                ReadGuard Guard(GetLock());
                return m_storage.get_allocator();
            }

            // Sort template
            template <typename C>
            void sort(C& compare)
            {
                iterator _begin = begin();
                iterator _end   = end();
                WriteGuard Guard(GetLock());
                std::stable_sort(_begin,_end,compare);
            }

            void foreach(std::function<void(T)> p_Predicate) const
            {
                i_lock.acquire_read();

                for (const_iterator l_Itr = m_storage.begin(); l_Itr != m_storage.end(); l_Itr++)
                    p_Predicate(*l_Itr);

                i_lock.release();
            }

            void foreach(bool p_AutoIncrement, std::function<void(const_iterator&)> p_Predicate) const
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

            void foreach(bool p_AutoIncrement, std::function<void(iterator&)> p_Predicate)
            {
                i_lock.acquire_read();

                for (iterator l_Itr = m_storage.begin(); l_Itr != m_storage.end();)
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
            std::vector<T>&  getSource()
            {
                return m_storage;
            }

        protected:
            mutable LockType           i_lock;
            std::vector<T, Allocator>  m_storage;
    };
}
#endif