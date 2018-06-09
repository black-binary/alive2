#pragma once

// Copyright (c) 2018-present The Alive2 Authors.
// Distributed under the MIT license that can be found in the LICENSE file.

#include <cstdint>
#include <ostream>
#include <utility>

typedef struct _Z3_app* Z3_app;
typedef struct _Z3_ast* Z3_ast;
typedef struct _Z3_sort* Z3_sort;
typedef struct _Z3_func_decl* Z3_func_decl;

namespace smt {

class expr {
  uintptr_t ptr;

  expr(Z3_ast ast);
  bool isZ3Ast() const;
  Z3_ast ast() const;
  Z3_ast operator()() const { return ast(); }
  void incRef();
  void decRef();

  Z3_sort sort() const;
  Z3_app isApp() const;

  static Z3_ast mkTrue();
  static Z3_ast mkFalse();
  static expr mkUInt(uint64_t n, Z3_sort sort);
  static expr mkInt(int64_t n, Z3_sort sort);
  static expr mkConst(Z3_func_decl decl);

public:
  expr() : ptr(0) {}

  expr(expr &&other) : ptr(0) {
    std::swap(ptr, other.ptr);
  }

  expr(const expr &other);
  expr(bool val) : expr(val ? mkTrue() : mkFalse()) {}
  ~expr();

  void operator=(expr &&other);
  void operator=(const expr &other);

  static expr mkUInt(uint64_t n, unsigned bits);
  static expr mkInt(int64_t n, unsigned bits);
  static expr mkVar(const char *name, unsigned bits);
  static expr mkBoolVar(const char *name);

  static expr IntMin(unsigned bits);
  static expr IntMax(unsigned bits);

  // structural equivalence
  bool eq(const expr &rhs) const;

  bool isValid() const { return ptr != 0; }

  bool isConst() const;
  bool isTrue() const;
  bool isFalse() const;
  bool isZero() const;
  bool isAllOnes() const;

  unsigned bits() const;
  bool isUInt(uint64_t &n) const;
  bool isInt(int64_t &n) const;

  bool isNot(expr &neg) const;

  expr operator+(const expr &rhs) const;
  expr operator-(const expr &rhs) const;
  expr operator*(const expr &rhs) const;
  expr sdiv(const expr &rhs) const;
  expr udiv(const expr &rhs) const;

  expr add_no_soverflow(const expr &rhs) const;
  expr add_no_uoverflow(const expr &rhs) const;
  expr sub_no_soverflow(const expr &rhs) const;
  expr sub_no_uoverflow(const expr &rhs) const;
  expr mul_no_soverflow(const expr &rhs) const;
  expr mul_no_uoverflow(const expr &rhs) const;
  expr sdiv_exact(const expr &rhs) const;
  expr udiv_exact(const expr &rhs) const;

  expr operator<<(const expr &rhs) const;
  expr ashr(const expr &rhs) const;
  expr lshr(const expr &rhs) const;

  expr shl_no_soverflow(const expr &rhs) const;
  expr shl_no_uoverflow(const expr &rhs) const;
  expr ashr_exact(const expr &rhs) const;
  expr lshr_exact(const expr &rhs) const;

  expr operator&(const expr &rhs) const;
  expr operator|(const expr &rhs) const;
  expr operator^(const expr &rhs) const;

  expr operator!() const;
  expr operator~() const;

  expr operator==(const expr &rhs) const;
  expr operator!=(const expr &rhs) const;

  expr operator&&(const expr &rhs) const;
  expr operator||(const expr &rhs) const;
  // the following are boolean only:
  void operator&=(const expr &rhs);
  void operator|=(const expr &rhs);

  expr implies(const expr &rhs) const;
  expr notImplies(const expr &rhs) const;

  expr ule(const expr &rhs) const;
  expr ult(const expr &rhs) const;
  expr uge(const expr &rhs) const;
  expr ugt(const expr &rhs) const;
  expr sle(const expr &rhs) const;
  expr slt(const expr &rhs) const;
  expr sge(const expr &rhs) const;
  expr sgt(const expr &rhs) const;

  expr sext(unsigned amount) const;
  expr zext(unsigned amount) const;

  expr concat(const expr &rhs) const;
  expr extract(unsigned high, unsigned low) const;

  static expr mkIf(const expr &cond, const expr &then, const expr &els);

  friend std::ostream &operator<<(std::ostream &os, const expr &e);

  // for container use only
  bool operator<(const expr &rhs) const;
  unsigned id() const;
  unsigned hash() const;


  template <typename... Exprs>
  static bool allValid(const expr &e, Exprs&&... exprs) {
    return e.isValid() && allValid(exprs...);
  }
  static bool allValid(const expr &e) { return e.isValid(); }
  static bool allValid() { return true; }

  friend class Solver;
  friend class Model;
};

}
