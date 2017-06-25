/*
 *   Copyright (c) 2007 John Weaver
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 */

#if !defined(__MUNKRES_CPP_MATRIX_BASE_H__)
#define __MUNKRES_CPP_MATRIX_BASE_H__

#include <limits>
#include <cmath>
#include <stdexcept>
#include <iterator>



namespace munkres_cpp
{

template<typename T>
class matrix_base
{
    public:
        // Types.
        using value_type = T;

        // Constants.
        static constexpr value_type zero = value_type (0);
        static constexpr value_type max_val = std::numeric_limits<value_type>::max ();

        // Interface.
        virtual ~matrix_base () = default;
        virtual const value_type & operator () (const size_t, const size_t) const = 0;
        virtual value_type & operator () (const size_t, const size_t) = 0;
        virtual size_t columns () const = 0;
        virtual size_t rows () const = 0;

        // Default implementation.
        virtual void resize (const size_t rows, const size_t columns, const value_type = zero)
        {
            if (rows != this->rows () || columns != this->columns () )
                throw std::logic_error ("Called function with inappropriate default implementation.");
        }

        // Implementation.
        template <typename V = value_type>
        constexpr typename std::enable_if<std::is_integral<V>::value, bool>::type
        is_zero (size_t row, size_t column) const {return operator () (row, column) == 0;}

        template <typename V = value_type>
        constexpr typename std::enable_if<!std::is_integral<V>::value, bool>::type
        is_zero (size_t row, size_t column) const {return FP_ZERO == std::fpclassify (operator () (row, column) );}

        // Implementation of the std::iterator with begin and end functions allow to use STL algorithms.
        template <typename M = matrix_base<value_type> >
        struct iterator : public std::iterator<std::input_iterator_tag, typename M::value_type>
        {
            iterator (M & m, const size_t r, const size_t c) : m {m}, r {r}, c {c} {}
            bool operator == (const iterator & that) {return this->r == that.r && this->c == that.c;}
            bool operator != (const iterator & that) {return ! operator == (that);}
            typename M::value_type & operator * () const {return m (r, c);}
            iterator & operator ++ ()
            {
                r += ++c / m.columns ();
                c  =   c % m.columns ();
                return * this;
            }

            M & m;
            size_t r, c;
        };
        iterator<> begin () {return iterator<> {* this, 0, 0};}
        iterator<> end   () {return iterator<> {* this, rows (), 0};}
};

template<typename T>
constexpr T matrix_base<T>::zero;

template<typename T>
constexpr T matrix_base<T>::max_val;

}// namespace munkres_cpp

#endif /* !defined(__MUNKRES_CPP_MATRIX_BASE_H__) */
