/*=============================================================================
    Wave: A Standard compliant C++ preprocessor library

    Definition of the lexer iterator 
    
    Copyright (c) 2001-2004 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#if !defined(CPP_LEX_ITERATOR_HPP_AF0C37E3_CBD8_4F33_A225_51CF576FA61F_INCLUDED)
#define CPP_LEX_ITERATOR_HPP_AF0C37E3_CBD8_4F33_A225_51CF576FA61F_INCLUDED

#include <string>
#include <iostream>

#include <boost/assert.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/spirit/iterator/multi_pass.hpp>

#include <boost/wave/util/file_position.hpp>
#include <boost/wave/util/functor_input.hpp>
#include <boost/wave/cpplexer/cpp_lex_interface.hpp>

#include <boost/wave/language_support.hpp>

///////////////////////////////////////////////////////////////////////////////
namespace boost {
namespace wave {
namespace cpplexer {
namespace impl {

///////////////////////////////////////////////////////////////////////////////
//  
//  lex_iterator_functor_shim
//
///////////////////////////////////////////////////////////////////////////////

template <typename TokenT> 
class lex_iterator_functor_shim 
{
public:
    template <typename IteratorT>
    lex_iterator_functor_shim(IteratorT const &first, IteratorT const &last, 
            typename TokenT::position_t const &pos, 
            boost::wave::language_support language)
    :   functor_ptr(lex_input_interface<TokenT>
            ::new_lexer(first, last, pos, language)) 
    {}

// interface to the boost::spirit::multi_pass_policies::functor_input policy
    typedef TokenT result_type;

    static result_type const eof;
    
    result_type operator()() 
    { 
        BOOST_ASSERT(0 != functor_ptr.get());
        return functor_ptr->get(); 
    }
    void set_position(typename TokenT::position_t const &pos)
    {
        BOOST_ASSERT(0 != functor_ptr.get());
        functor_ptr->set_position(pos);
    }
    
private:
    boost::shared_ptr<lex_input_interface<TokenT> > functor_ptr;
};

///////////////////////////////////////////////////////////////////////////////
//  eof token
template <typename LexT>
typename lex_iterator_functor_shim<LexT>::result_type const
    lex_iterator_functor_shim<LexT>::eof;

///////////////////////////////////////////////////////////////////////////////
}   // namespace impl

///////////////////////////////////////////////////////////////////////////////
//  
//  lex_iterator
//
//      A generic C++ lexer interface class, which allows to plug in different
//      lexer implementations (template parameter LexT). The following 
//      requirement apply:
//
//          - the lexer type should have a function implemented, which returnes
//            the next lexed token from the input stream:
//                typename LexT::token_t get();
//          - at the end of the input stream this function should return the
//            eof token equivalent
//          - the lexer should implement a constructor taking two iterators
//            pointing to the beginning and the end of the input stream and
//            a third parameter containing the name of the parsed input file 
//
///////////////////////////////////////////////////////////////////////////////

template <typename TokenT>
class lex_iterator 
:   public boost::spirit::multi_pass<
        impl::lex_iterator_functor_shim<TokenT>,
        boost::wave::util::functor_input
    >
{
    typedef impl::lex_iterator_functor_shim<TokenT> input_policy_t;
    typedef 
        boost::spirit::multi_pass<input_policy_t, 
                boost::wave::util::functor_input>
        base_t;
    typedef lex_iterator<TokenT>                    self_t;
    
public:
    typedef TokenT token_t;
    
    lex_iterator()
    {}
    
    template <typename IteratorT>
    lex_iterator(IteratorT const &first, IteratorT const &last, 
            typename TokenT::position_t const &pos, 
            boost::wave::language_support language)
    :   base_t(input_policy_t(first, last, pos, language))
    {}
};

///////////////////////////////////////////////////////////////////////////////
}   // namespace cpplexer
}   // namespace wave
}   // namespace boost

#endif // !defined(CPP_LEX_ITERATOR_HPP_AF0C37E3_CBD8_4F33_A225_51CF576FA61F_INCLUDED)
