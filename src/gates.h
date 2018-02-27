#pragma once
#include <tuple>
namespace gates {
    //Gates are used to perform logical operations with Events.
    //Gates are designed to be modular. They can be nested inside of each other and all pass their results on through their bool operator(). 
    //Events are designed to be used as sources for gates. Their operator() returns the state of their Event.

    //    L-> in   in <-R
    //          /&/ <-operation
    // result-> out

    template<typename Op, typename... Srcs>
    struct gate {
        gate(Op pOp, Srcs... pSrcs) :op(pOp), srcs(pSrcs...) {}
        std::tuple<Srcs...> srcs;
        Op op;
        template<size_t I, typename Dummy=float>// Dummy enables template specialization in class scope
        struct evaluator {
            static bool next(std::tuple<Srcs...>& pSrcs){
                return Op::eval(std::get<I>(pSrcs)(), evaluator<I - 1>::next(pSrcs));
            }
        };

        template<typename Dummy>
        struct evaluator<0, Dummy> {
            static bool next(std::tuple<Srcs...>& pSrcs) {
                return std::get<0>(pSrcs)();
            }
        };
        bool operator()() {
            return evaluator<sizeof...(Srcs)-1>::next(srcs);
        }
    };
    template<typename Op, typename... Srcs>
    inline bool operator==(gate<Op, Srcs...> const & l, gate<Op, Srcs...> const& r) {
        return l.srcs == r.srcs && l.op == r.op;
    }
    struct and_op {//both
        and_op(){}
        static inline bool eval(bool lhs, bool rhs) {
            return lhs && rhs;
        }
    };
    struct nand_op {
        nand_op() {}
        static inline bool eval(bool lhs, bool rhs) {
            return !lhs && !rhs;
        }
    };
    struct or_op {//either or both
        or_op() {}
        static inline bool eval(bool lhs, bool rhs) {
            return lhs || rhs;
        }
    };
    struct xor_op {//either one but not both
        xor_op() {}
        static inline bool eval(bool lhs, bool rhs) {
            return !lhs != !rhs;
        }
    };
    struct nor_op {//none
        nor_op() {}
        static inline bool eval(bool lhs, bool rhs) {
            return !lhs && !rhs;
        }
    };
    struct xnor_op {
        xnor_op() {}
        static inline bool eval(bool lhs, bool rhs) {
            return !lhs == !rhs;
        }
    };


    template<typename Source>
    struct toggle_gate {
        toggle_gate(Source pSource, bool pStartAs = false) 
            :src(std::forward<Source>(pSource)), state(pStartAs) {}
        Source src;
        bool state;

        bool operator()() {
            if (src()) {//toggle if src() == true
                src();
                state = !state;
            }
            return state;
        }
    };
    template<typename Source>
    inline bool operator==(toggle_gate<Source> const & l, toggle_gate<Source> const& r) {
        return l.src == r.src && l.state == r.state;
    }
    template<typename Source>
    struct clicker_gate {//fires once when the input changes
        clicker_gate(Source pSource, bool pStartWith = false) :src(std::forward<Source>(pSource)), last_state(pStartWith) {}
        Source src;
        bool last_state;
        bool operator()() {
            if (src() != last_state) {//fire if src() != last_state
                last_state = !last_state;
                return true;
            }
            return false;
        }
    };
    template<typename Source>
    inline bool operator==(clicker_gate<Source> const & l, clicker_gate<Source> const& r) {
        return l.src == r.src && l.last_state == r.last_state;
    }
    template<typename On, typename Off>
    struct switch_gate {//has internal state and two inputs each either turning the state true or false
        switch_gate() {}
        switch_gate(On pOnSource, Off pOffSource, bool pStartWith = false) :on_src(std::forward<On>(pOnSource)), off_src(std::forward<Off>(pOffSource)), state(pStartWith) {}
        On on_src;
        Off off_src;
        bool state;
        bool operator()() {
            //on_src can switch state on and off_src can switch state off
            bool on = on_src();
            bool off = off_src();
            if (on != off) {
                state = on;
            }
            return state;
        }
    };
    template<typename On, typename Off>
    inline bool operator==(switch_gate<On, Off> const & l, switch_gate<On, Off> const& r) {
        return l.on_src == r.on_src && l.off_src == r.off_src && l.state == r.state;
    }
    template<typename Source>
    struct not_gate {//has internal state and two inputs each either turning the state true or false
        not_gate(Source pSource) :src(std::forward<Source>(pSource)) {}
        Source src;
        bool operator()() {
            return !src();
        }
    };
    template<typename Source>
    inline bool operator==(not_gate<Source> const & l, not_gate<Source> const& r) {
        return l.src == r.src;
    }


}
