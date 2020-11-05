// Boost.Geometry Index
//
// Copyright (c) 2011-2013 Adam Wulkiewicz, Lodz, Poland.
//
// This file was modified by Oracle on 2019, 2020.
// Modifications copyright (c) 2019-2020 Oracle and/or its affiliates.
// Contributed and/or modified by Adam Wulkiewicz, on behalf of Oracle
//
// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#ifndef BOOST_GEOMETRY_UTIL_TUPLES_HPP
#define BOOST_GEOMETRY_UTIL_TUPLES_HPP

#include <boost/geometry/core/config.hpp>

#include <boost/mpl/if.hpp>
#include <boost/mpl/not.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/type_traits/integral_constant.hpp>
#include <boost/type_traits/is_same.hpp>

#include <utility>

#ifdef BOOST_GEOMETRY_CXX11_TUPLE

#include <tuple>

#endif // BOOST_GEOMETRY_CXX11_TUPLE

namespace boost { namespace geometry { namespace tuples {


using boost::tuples::null_type;


template <int I, typename Tuple>
struct element
    : boost::tuples::element<I, Tuple>
{};

template <typename Tuple>
struct size
    : boost::tuples::length<Tuple>
{};

template <int I, typename HT, typename TT>
inline typename boost::tuples::access_traits
    <
        typename boost::tuples::element<I, boost::tuples::cons<HT, TT> >::type
    >::non_const_type
get(boost::tuples::cons<HT, TT> & tup)
{
    return boost::tuples::get<I>(tup);
}

template <int I, typename HT, typename TT>
inline typename boost::tuples::access_traits
    <
        typename boost::tuples::element<I, boost::tuples::cons<HT, TT> >::type
    >::const_type
get(boost::tuples::cons<HT, TT> const& tup)
{
    return boost::tuples::get<I>(tup);
}


template <int I, typename F, typename S>
struct element<I, std::pair<F, S> >
{};

template <typename F, typename S>
struct element<0, std::pair<F, S> >
{
    typedef F type;
};

template <typename F, typename S>
struct element<1, std::pair<F, S> >
{
    typedef S type;
};

template <typename F, typename S>
struct size<std::pair<F, S> >
    : boost::integral_constant<int, 2>
{};

template <int I, typename Pair>
struct get_pair;

template <typename F, typename S>
struct get_pair<0, std::pair<F, S> >
{
    typedef F type;

    static inline F& apply(std::pair<F, S> & p)
    {
        return p.first;
    }

    static inline F const& apply(std::pair<F, S> const& p)
    {
        return p.first;
    }
};

template <typename F, typename S>
struct get_pair<1, std::pair<F, S> >
{
    typedef S type;

    static inline S& apply(std::pair<F, S> & p)
    {
        return p.second;
    }

    static inline S const& apply(std::pair<F, S> const& p)
    {
        return p.second;
    }
};

template <int I, typename F, typename S>
inline typename get_pair<I, std::pair<F, S> >::type&
get(std::pair<F, S> & p)
{
    return get_pair<I, std::pair<F, S> >::apply(p);
}

template <int I, typename F, typename S>
inline typename get_pair<I, std::pair<F, S> >::type const&
get(std::pair<F, S> const& p)
{
    return get_pair<I, std::pair<F, S> >::apply(p);
}

#ifdef BOOST_GEOMETRY_CXX11_TUPLE

template <int I, typename ...Ts>
struct element<I, std::tuple<Ts...> >
    : std::tuple_element<I, std::tuple<Ts...> >
{};

template <typename ...Ts>
struct size<std::tuple<Ts...> >
    : std::tuple_size<std::tuple<Ts...> >
{};

template <int I, typename ...Ts>
inline typename std::tuple_element<I, std::tuple<Ts...> >::type&
get(std::tuple<Ts...> & tup)
{
    return std::get<I>(tup);
}

template <int I, typename ...Ts>
inline typename std::tuple_element<I, std::tuple<Ts...> >::type const&
get(std::tuple<Ts...> const& tup)
{
    return std::get<I>(tup);
}

#endif // BOOST_GEOMETRY_CXX11_TUPLE


// find_index_if
// Searches for the index of an element for which UnaryPredicate returns true
// If such element is not found the result is N

template
<
    typename Tuple,
    template <typename> class UnaryPred,
    int I = 0,
    int N = size<Tuple>::value
>
struct find_index_if
    : boost::mpl::if_c
        <
            UnaryPred<typename element<I, Tuple>::type>::value,
            boost::integral_constant<int, I>,
            typename find_index_if<Tuple, UnaryPred, I+1, N>::type
        >::type
{};

template
<
    typename Tuple,
    template <typename> class UnaryPred,
    int N
>
struct find_index_if<Tuple, UnaryPred, N, N>
    : boost::integral_constant<int, N>
{};


// find_if
// Searches for an element for which UnaryPredicate returns true
// If such element is not found the result is boost::tuples::null_type

template
<
    typename Tuple,
    template <typename> class UnaryPred,
    int I = 0,
    int N = size<Tuple>::value
>
struct find_if
    : boost::mpl::if_c
        <
            UnaryPred<typename element<I, Tuple>::type>::value,
            element<I, Tuple>,
            find_if<Tuple, UnaryPred, I+1, N>
        >::type
{};

template
<
    typename Tuple,
    template <typename> class UnaryPred,
    int N
>
struct find_if<Tuple, UnaryPred, N, N>
{
    typedef boost::tuples::null_type type;
};


// is_found
// Returns true if a type T (the result of find_if) was found.

template <typename T>
struct is_found
    : boost::mpl::not_<boost::is_same<T, boost::tuples::null_type> >
{};


// is_not_found
// Returns true if a type T (the result of find_if) was not found.

template <typename T>
struct is_not_found
    : boost::is_same<T, boost::tuples::null_type>
{};


// exists_if
// Returns true if search for element meeting UnaryPred can be found.

template <typename Tuple, template <typename> class UnaryPred>
struct exists_if
    : is_found<typename find_if<Tuple, UnaryPred>::type>
{};


// push_back
// A utility used to create a type/object of a Tuple containing
//   all types/objects stored in another Tuple plus additional one.

template <typename Tuple,
          typename T,
          size_t I = 0,
          size_t N = size<Tuple>::value>
struct push_back_bt
{
    typedef
    boost::tuples::cons<
        typename element<I, Tuple>::type,
        typename push_back_bt<Tuple, T, I+1, N>::type
    > type;

    static type apply(Tuple const& tup, T const& t)
    {
        return
        type(
            geometry::tuples::get<I>(tup),
            push_back_bt<Tuple, T, I+1, N>::apply(tup, t)
        );
    }
};

template <typename Tuple, typename T, size_t N>
struct push_back_bt<Tuple, T, N, N>
{
    typedef boost::tuples::cons<T, boost::tuples::null_type> type;

    static type apply(Tuple const&, T const& t)
    {
        return type(t, boost::tuples::null_type());
    }
};

template <typename Tuple, typename T>
struct push_back
    : push_back_bt<Tuple, T>
{};

template <typename F, typename S, typename T>
struct push_back<std::pair<F, S>, T>
{
#ifdef BOOST_GEOMETRY_CXX11_TUPLE
    typedef std::tuple<F, S, T> type;
#else
    typedef boost::tuple<F, S, T> type;
#endif // BOOST_GEOMETRY_CXX11_TUPLE

    static type apply(std::pair<F, S> const& p, T const& t)
    {
        return type(p.first, p.second, t);
    }

#ifdef BOOST_GEOMETRY_CXX11_TUPLE
#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)

    static type apply(std::pair<F, S> && p, T const& t)
    {
        return type(std::move(p.first), std::move(p.second), t);
    }

    static type apply(std::pair<F, S> && p, T && t)
    {
        return type(std::move(p.first), std::move(p.second), std::move(t));
    }

#endif
#endif // BOOST_GEOMETRY_CXX11_TUPLE

};

#ifdef BOOST_GEOMETRY_CXX11_TUPLE

// NOTE: In C++14 std::integer_sequence and std::make_integer_sequence could be used

template <int... Is>
struct int_sequence {};

template <int N, int ...Is>
struct make_int_sequence
{
    typedef typename make_int_sequence<N - 1, N - 1, Is...>::type type;
};

template <int ...Is>
struct make_int_sequence<0, Is...>
{
    typedef int_sequence<Is...> type;
};

template <typename Is, typename Tuple, typename T>
struct push_back_st;

template <int ...Is, typename ...Ts, typename T>
struct push_back_st<int_sequence<Is...>, std::tuple<Ts...>, T>
{
    typedef std::tuple<Ts..., T> type;

    static type apply(std::tuple<Ts...> const& tup, T const& t)
    {
        return type(std::get<Is>(tup)..., t);
    }

#if !defined(BOOST_NO_CXX11_RVALUE_REFERENCES)

    static type apply(std::tuple<Ts...> && tup, T const& t)
    {
        return type(std::move(std::get<Is>(tup))..., t);
    }

    static type apply(std::tuple<Ts...> && tup, T && t)
    {
        return type(std::move(std::get<Is>(tup))..., std::move(t));
    }

#endif
};

template <typename ...Ts, typename T>
struct push_back<std::tuple<Ts...>, T>
    : push_back_st
        <
            typename make_int_sequence<sizeof...(Ts)>::type,
            std::tuple<Ts...>,
            T
        >
{};

#endif // BOOST_GEOMETRY_CXX11_TUPLE


}}} // namespace boost::geometry::tuples

#endif // BOOST_GEOMETRY_UTIL_TUPLES_HPP
