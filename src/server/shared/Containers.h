////////////////////////////////////////////////////////////////////////////////
//
//  MILLENIUM-STUDIO
//  Copyright 2016 Millenium-studio SARL
//  All Rights Reserved.
//
////////////////////////////////////////////////////////////////////////////////

#ifndef CONTAINERS_H
#define CONTAINERS_H

#include "Common.h"
#include "Util.h"

//! Because circular includes are bad
extern uint32 urand(uint32 min, uint32 max);

namespace JadeCore
{
    template<class T>
    constexpr T* AddressOrSelf(T* ptr)
    {
        return ptr;
    }

    template<class T>
    constexpr T* AddressOrSelf(T& not_ptr)
    {
        return std::addressof(not_ptr);
    }

    namespace Containers
    {
        template<class C>
        constexpr std::size_t Size(C const& container)
        {
            return container.size();
        }

        template<class T, std::size_t size>
        constexpr std::size_t Size(T const(&)[size]) noexcept
        {
            return size;
        }

        template<class T>
        void RandomResizeSet(std::set<T> &t_set, uint32 size)
        {
            size_t set_size = t_set.size();

            while (set_size > size)
            {
                typename std::set<T>::iterator itr = t_set.begin();
                std::advance(itr, urand(0, set_size - 1));
                t_set.erase(itr);
                --set_size;
            }
        }

        template<class T>
        void RandomResizeList(std::list<T> &list, uint32 size)
        {
            size_t list_size = list.size();

            while (list_size > size)
            {
                typename std::list<T>::iterator itr = list.begin();
                std::advance(itr, urand(0, list_size - 1));
                list.erase(itr);
                --list_size;
            }
        }

        template<class T, class Predicate>
        void RandomResizeList(std::list<T> &list, Predicate& predicate, uint32 size)
        {
            //! First use predicate filter
            std::list<T> listCopy;
            for (typename std::list<T>::iterator itr = list.begin(); itr != list.end(); ++itr)
                if (predicate(*itr))
                    listCopy.push_back(*itr);

            if (size)
                RandomResizeList(listCopy, size);

            list = listCopy;
        }

        /* Select a random element from a container. Note: make sure you explicitly empty check the container */
        template <class C> typename C::value_type const& SelectRandomContainerElement(C const& container)
        {
            typename C::const_iterator it = container.begin();
            std::advance(it, urand(0, container.size() - 1));
            return *it;
        }

        // Reorder the elements of the container randomly. @param container Container to reorder
        template<class C>
        void RandomShuffle(C& container)
        {
            std::random_device rd;
            std::mt19937 _m(rd());
            std::shuffle(std::begin(container), std::end(container), _m);
        }

        template <typename T>
        void RandomShuffleList(std::list<T>& list)
        {
            // create a vector of (wrapped) references to elements in the list
            std::vector<std::reference_wrapper<const T>> vec(list.begin(), list.end());

            // shuffle (the references in) the vector
            std::random_device rd;
            std::mt19937 _m(rd());
            std::shuffle(vec.begin(), vec.end(), _m);

            // copy the shuffled sequence into a new list
            std::list<T> shuffled_list{ vec.begin(), vec.end() };

            // swap the old list with the shuffled list
            list.swap(shuffled_list);
        }

        /**
         * Select a random element from a container where each element has a different chance to be selected.
         *
         * @param container Container to select an element from
         * @param weights Chances of each element to be selected, must be in the same order as elements in container.
         *                Caller is responsible for checking that sum of all weights is greater than 0.
         *
         * Note: container cannot be empty
        */
        template<class C>
        auto SelectRandomWeightedContainerElement(C const& container, std::vector<double> weights) -> decltype(std::begin(container))
        {
            auto it = std::begin(container);
            std::advance(it, urandweighted(weights.size(), weights.data()));
            return it;
        }
        /**
         * Select a random element from a container where each element has a different chance to be selected.
         *
         * @param container Container to select an element from
         * @param weightExtractor Function retrieving chance of each element in container, expected to take an element of the container and returning a double
         *
         * Note: container cannot be empty
        */
        template<class C, class Fn>
        auto SelectRandomWeightedContainerElement(C const& container, Fn weightExtractor) -> decltype(std::begin(container))
        {
            std::vector<double> weights;
            weights.reserve(Size(container));
            double weightSum = 0.0;
            for (auto& val : container)
            {
                double weight = weightExtractor(val);
                weights.push_back(weight);
                weightSum += weight;
            }
            if (weightSum <= 0.0)
                weights.assign(Size(container), 1.0);

            return SelectRandomWeightedContainerElement(container, weights);
        }

        /**
         * Returns a pointer to mapped value (or the value itself if map stores pointers)
         */
        template<class M>
        auto MapGetValuePtr(M& map, typename M::key_type const& key) -> decltype(AddressOrSelf(map.find(key)->second))
        {
            if (map.empty())
                return nullptr;

            auto itr = map.find(key);
            return itr != map.end() ? AddressOrSelf(itr->second) : nullptr;
        }
    }
    //! namespace Containers
}
//! namespace JadeCore

#endif //! #ifdef CONTAINERS_H
