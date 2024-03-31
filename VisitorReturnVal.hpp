// Following visitor pattern from
// https://www.codeproject.com/Tips/1018315/Visitor-with-the-Return-Value
#pragma once

template <typename VisitorImpl, typename VisitablePtr, typename ResultType>
class VisitorReturnVal {
 public:
  ResultType GetValue(VisitablePtr n) {
    VisitorImpl vis;
    n->accept(vis);
    return vis.value;
  }

  void Return(ResultType val) { value = val; }

 private:
  ResultType value;
};