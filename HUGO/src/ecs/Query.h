#pragma once

#include <cstdlib>
#include <tuple>
#include <vector>

template<typename ...Ts>
class ComponentTuple {
  std::tuple<Ts&...> pack;

public:
  ComponentTuple(Ts&... components) 
    : pack(components...)
  {}

  template<typename T>
  T& get() {
      return std::get<T>(pack);
  }

  template<typename T>
  const T& get() const {
    return std::get<T>(pack);
  }
};

template<typename ...Ts>
class Query {
  using TupleList = std::vector<ComponentTuple<Ts...>>;
  TupleList componentPacks;

public:
  Query() : componentPacks() {}

  void push_back(Ts&... components) { 
    componentPacks.emplace_back(components...);
  }

  typename TupleList::iterator begin() { return componentPacks.begin(); }
  typename TupleList::iterator end() { return componentPacks.end(); }

  typename TupleList::const_iterator cbegin() const { return componentPacks.cbegin(); }
  typename TupleList::const_iterator cend() const { return componentPacks.cend(); }

  ComponentTuple<Ts...>& operator[](size_t index) { return componentPacks[index]; }
  const ComponentTuple<Ts...>& at(size_t index) const { return componentPacks.at(index); }
};
