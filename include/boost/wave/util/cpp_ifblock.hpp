/*=============================================================================
    Wave: A Standard compliant C++ preprocessor library

    Copyright (c) 2001-2004 Hartmut Kaiser
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#if !defined(CPP_IFBLOCK_HPP_D4676B36_00C5_41F4_BC9F_9CBBAE3B8006_INCLUDED)
#define CPP_IFBLOCK_HPP_D4676B36_00C5_41F4_BC9F_9CBBAE3B8006_INCLUDED

#include <stack>

///////////////////////////////////////////////////////////////////////////////
namespace boost {
namespace wave {
namespace util {

///////////////////////////////////////////////////////////////////////////////
// the class if_blocks handles recursive conditional compilation contexts
class if_block
{
public:
    if_block() :
        status(true), some_part_status(true), 
        enclosing_status(true), is_in_else(false)
    {
    }
    if_block(bool status_, bool enclosing_status_) : 
        status(status_), 
        some_part_status(status_),
        enclosing_status(enclosing_status_),
        is_in_else(false)
    {
    }

    void set_status(bool status_) 
    { 
        status = status_; 
        if (status_) 
            some_part_status = true; 
    }
    bool get_status() const { return status; }
    bool get_some_part_status() const { return some_part_status; }
    bool get_enclosing_status() const { return enclosing_status; }
    bool get_in_else() const { return is_in_else; }
    void set_in_else() { is_in_else = true; }

private:
   bool status;             // Current block is true
   bool some_part_status;   // One of the preceeding or current #if/#elif was true
   bool enclosing_status;   // Enclosing #if block is true
   bool is_in_else;         // Inside the #else part
};

///////////////////////////////////////////////////////////////////////////////
// stack of conditional compilation contexts
class if_block_stack 
:   private std::stack<if_block> 
{
public:
    typedef std::stack<if_block>::size_type size_type;
    
    void enter_if_block(bool new_status)
    {
    // If enclosing block is false, then this block is also false
        bool enclosing_status = get_status();
        this->push (value_type (new_status && enclosing_status, enclosing_status));
    }
    bool enter_elif_block(bool new_status)
     {   
        if (!is_inside_ifpart())
            return false;       // #elif without matching #if

        if (get_enclosing_status()) {
            if (get_status()) {
            // entered a (false) #elif block from a true block
                this->top().set_status(false);
            } 
            else if (new_status && !this->top().get_some_part_status()) {
            // Entered true #elif block and no previous block was true
                this->top().set_status(new_status);
            }
        }
        return true;
    }
    bool enter_else_block()
    {
        if (!is_inside_ifpart())
            return false;       // #else without matching #if

        if (get_enclosing_status()) {
            if (!this->top().get_some_part_status()) {
            // Entered (true) #else block and no previous block was true
                this->top().set_status(true);
            } 
            else if (get_status()) {
            // Entered (false) else block from true block
                this->top().set_status(false);
            }

        // Set else flag
            this->top().set_in_else();
        }
        return true;
    }
    bool exit_if_block()
    {
        if (0 == this->size())
            return false;   // #endif without matching #if

        this->pop();
        return true;
    }

// return, wether the top (innermost) condition is true or false
    bool get_status() const
    { 
        return 0 == this->size() || this->top().get_status(); 
    }

    size_type get_if_block_depth() const { return this->size(); }
    
protected:
    bool get_enclosing_status() const
    {
       return 0 == this->size() || this->top().get_enclosing_status();
    }

    bool is_inside_ifpart() const
    {
       return 0 != this->size() && !this->top().get_in_else();
    }
    bool is_inside_elsepart() const
    {
       return 0 != this->size() && this->top().get_in_else();
    }
};

///////////////////////////////////////////////////////////////////////////////
}   // namespace util
}   // namespace wave
}   // namespace boost

#endif // !defined(CPP_IFBLOCK_HPP_D4676B36_00C5_41F4_BC9F_9CBBAE3B8006_INCLUDED)
