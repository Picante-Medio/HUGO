#pragma once

#include <cstddef>
#include <cstdlib>
#include <tuple>
#include <vector>

#include "ComponentEvent.h"
#include "EcsTypes.h"
#include "Mutable.h"

namespace HUGO {

	template<typename ...Ts>
	class ComponentTuple {

		std::tuple<Ts&...> pack;
		
		std::tuple<std::shared_ptr<ComponentEvent<Ts>>...>& updateEvents;

	public:
		ComponentTuple(Ts&... components, std::tuple<std::shared_ptr<ComponentEvent<Ts>>...>& updateEvents) 
			: pack(components...), updateEvents(updateEvents)
		{}

		// Antiguo get
		template<typename T>
		T& getDeprecated() {
			return std::get<T&>(pack);
		}

		template<typename T>
		const T& getConst() const {
			return std::get<T&>(pack);
		}

		template<typename T>
		Mutable<T> getMutable() {
			return Mutable<T>(std::get<std::shared_ptr<ComponentEvent<T>>>(updateEvents), std::get<T&>(pack));
		}
	};

	template<typename ...Ts>
	class Query {
		using TupleList = std::vector<ComponentTuple<Ts...>>;
		TupleList componentPacks;

		std::tuple<std::shared_ptr<ComponentEvent<Ts>>...> updateEvents;

	public:
		Query(std::shared_ptr<ComponentEvent<Ts>>... events) : componentPacks(), updateEvents(events...) {}

		void push_back(Ts&... components) { 
			componentPacks.emplace_back(components..., updateEvents);
		}

		typename TupleList::iterator begin() { return componentPacks.begin(); }
		typename TupleList::iterator end() { return componentPacks.end(); }

		typename TupleList::const_iterator cbegin() const { return componentPacks.cbegin(); }
		typename TupleList::const_iterator cend() const { return componentPacks.cend(); }

		ComponentTuple<Ts...>& operator[](size_t index) { return componentPacks[index]; }
		const ComponentTuple<Ts...>& at(size_t index) const { return componentPacks.at(index); }
        size_t size() const { return componentPacks.size(); } 

	};
};
