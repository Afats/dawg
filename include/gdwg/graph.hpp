#ifndef GDWG_GRAPH_HPP
#define GDWG_GRAPH_HPP

// z5232937

#include <algorithm>
#include <cassert>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <set>
#include <tuple>
#include <utility>

// reference:
/* I played around here to get an optimal ds: https://techiedelight.com/compiler/
future reference:
alt. ds: std::unordered_map<std::shared_ptr<N>, std::set<incoming_links, comparator>>
incoming_links = <std::pair<std::shared_ptr<N>, std::set<shared_ptr<W>>
alt. ds: std::map<std::pair<std::shared_ptr<N>, std::shared_ptr<N>>, std::set<std::shared_ptr<E>>>
links_; alt. ds: multimap?? */

namespace gdwg {
	template<typename N, typename E>
	class graph {
	public:
		struct value_type {
			N from;
			N to;
			E weight;
		};

		class iterator;

		struct comparator {
			using is_transparent = void;
			auto operator()(const std::shared_ptr<N>& left_node,
			                const std::shared_ptr<N>& right_node) const -> bool {
				return *left_node < *right_node;
			}

			// transparent comparisons
			auto operator()(const std::shared_ptr<N>& left_node, const N& right_val) const -> bool {
				return *left_node < right_val;
			}
			auto operator()(const N& left_val, const std::shared_ptr<N>& right_node) const -> bool {
				return left_val < *right_node;
			}
		};

		struct setComparator {
			using is_transparent = void;
			auto operator()(const std::shared_ptr<E>& left_node,
			                const std::shared_ptr<E>& right_node) const -> bool {
				return *left_node < *right_node;
			}

			// transparent comparisons
			auto operator()(const std::shared_ptr<E>& left_node, const E& right_val) const -> bool {
				return *left_node < right_val;
			}
			auto operator()(const E& left_val, const std::shared_ptr<E>& right_node) const -> bool {
				return left_val < *right_node;
			}
		};

		// ----------   constructors -----------
		graph() = default;

		~graph() = default;

		graph(std::initializer_list<N> il) {
			nodes_ = decltype(nodes_)();
			edges_ = decltype(edges_)();

			for (auto iter : il) {
				insert_node(iter);
			}
		}
		template<typename InputIt>
		graph(InputIt first, InputIt last) {
			for (auto iter = first; iter != last; ++iter) {
				insert_node(*iter);
			}
		}

		// move constructor
		graph(graph&& other) noexcept {
			nodes_ = std::move(other.nodes_);
			edges_ = std::move(other.edges_);
		}

		// move-assigned and destroyed
		auto operator=(graph&& other) noexcept -> graph& {
			std::swap(nodes_, other.nodes_);
			std::swap(edges_, other.edges_);
			//
			other.clear();
			return *this;
		}

		// copy constructor
		graph(graph const& other) {
			// clear the data up
			nodes_ = decltype(nodes_)();
			edges_ = decltype(edges_)();

			for (auto node : other.nodes()) {
				insert_node(node);
			}

			for (auto const& src_nodes : other.edges_) {
				for (auto const& dst_node : src_nodes.second) {
					auto new_src = std::make_shared<N>(*src_nodes.first);
					auto new_dest = std::make_shared<N>(*dst_node.first);

					// loop through set of weights
					for (auto const& weight : dst_node.second) {
						auto new_weight = std::make_shared<E>(*weight);
						edges_[new_src][new_dest].insert(new_weight);
					}
				}
			}
		}

		// copy assignment
		auto operator=(graph const& other) -> graph& {
			// if (this == other) {
			//  return *this;
			// }
			auto temp_graph = other;
			std::swap(nodes_, temp_graph.nodes_);
			std::swap(edges_, temp_graph.edges_);

			return *this;
		}

		// ----------   modifiers -----------

		// add node if one doesnt exist in graph
		auto insert_node(N const& value) noexcept -> bool {
			if (!is_node(value)) {
				auto node = std::make_shared<N>(value);
				// add to nodes set
				nodes_.insert(std::move(node));
				return true;
			}
			return false;
		}

		// creates edge if it doesnt exist already
		auto insert_edge(N const& src, N const& dst, E const& weight) -> bool {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::insert_edge when either src "
				                         "or dst node does not exist");
			}

			if (!edge_exists(src, dst, weight)) {
				auto new_src = std::make_shared<N>(src);
				auto new_dest = std::make_shared<N>(dst);
				auto new_weight = std::make_shared<E>(weight);
				edges_[new_src][new_dest].insert(new_weight);
				return true;
			}

			return false;
		}

		// replaces node with a node that doesn't exist yet in same spot
		auto replace_node(N const& old_data, N const& new_data) -> bool {
			if (is_node(new_data)) {
				return false;
			}
			if (!is_node(old_data)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::replace_node on a node that "
				                         "doesn't exist");
			}

			auto old_d = std::make_shared<N>(old_data);
			auto new_d = std::make_shared<N>(new_data);
			auto set_handle = nodes_.extract(old_d);
			set_handle.value() = new_d;
			nodes_.insert(move(set_handle));

			for (auto outer_key : edges_) {
				// replace inner key
				for (auto& inner_key : outer_key.second) {
					if (*inner_key.first == old_data) {
						auto inner_handle = outer_key.second.extract(old_d);
						inner_handle.key() = new_d;
						outer_key.second.insert(move(inner_handle));
					}
				}

				// replace outer key
				if (*outer_key.first == old_data) {
					auto outer_handle = edges_.extract(old_d);
					outer_handle.key() = new_d;
					edges_.insert(std::move(outer_handle));
				}

				return true;
			}

			// replace outer map key
			return true;
		}

		// all incoming/outgoing node edges of old are now new
		auto merge_replace_node(N const& old_data, N const& new_data) -> void {
			if (is_node(old_data) || is_node(new_data))
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::merge_replace_node on old or "
				                         "new data if they don't exist in the graph");
		}

		// erase all nodes of value, remove all associated edges to/from it
		auto erase_node(N const& value) noexcept -> bool {
			if (!is_node(value)) {
				return false;
			}

			auto src = std::make_shared<N>(value);
			nodes_.erase(src);
			edges_.erase(src);

			for (auto iter = nodes_.begin(); iter != nodes_.end(); iter++) {
				auto outer_node = edges_.find(*iter);
				if (outer_node != edges_.end()) {
					outer_node->second.erase(src);
				}
			}
			return true;
		}

		// erase edge of given weight
		auto erase_edge(N const& src, N const& dst, E const& weight) -> bool {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::erase_edge on src or dst if "
				                         "they don't exist in the graph");
			}

			if (!is_connected(src, dst)) {
				return false;
			}

			auto wt = std::make_shared<E>(weight);
			auto src_n = std::make_shared<N>(src);
			auto dst_n = std::make_shared<N>(dst);

			auto iter = find(src, dst, weight);
			if (iter != end()) {
				if (edges_[src_n][dst_n].size() > 1) {
					edges_[src_n][dst_n].erase(wt);
					return true;
				}

				// erase dst link to src if no edges remaining
				if (edges_[src_n][dst_n].size() == 1) {
					edges_[src_n][dst_n].erase(wt);
					edges_[src_n].erase(dst_n);
				}

				// no dst nodes left, delete node from map
				// set will still have keep the deleted node
				if (edges_[src_n].size() == 0) {
					erase_node(src);
				}

				return true;
			}
			return false;
		}

		auto erase_edge(iterator i) -> iterator;

		// erase all nodes from graph
		auto clear() noexcept -> void {
			nodes_.clear();
			edges_.clear();
		}

		auto erase_edge(iterator i, iterator s) -> iterator;

		// ----------   accessors -----------

		// check if node of value exists
		[[nodiscard]] auto is_node(N const& value) const -> bool {
			auto found = nodes_.find(value);
			if (found == nodes_.end()) {
				return false;
			}
			return true;
		}

		// check if no nodes in graph
		[[nodiscard]] auto empty() const noexcept -> bool {
			return (nodes_.empty());
		}

		// check if src->dest is in graph
		[[nodiscard]] auto is_connected(N const& src, N const& dst) const -> bool {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::is_connected if src or dst "
				                         "node don't exist in the graph");
			}

			if (edges_.find(src) == edges_.end()) {
				return false;
			}
			auto const& iter = edges_.find(src)->second;

			if (iter.find(dst) != iter.end()) {
				return true;
			}

			return false;
		}

		// nodes as a vector in ascending order
		[[nodiscard]] auto nodes() const -> std::vector<N> {
			auto res_vec = std::vector<N>();
			for (auto& node : nodes_) {
				res_vec.push_back(*node);
			}

			return res_vec;
		}

		// asc. order of weights from src to dst
		[[nodiscard]] auto weights(N const& src, N const& dst) const -> std::vector<E> {
			if (!is_node(src) || !is_node(dst)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::weights if src or dst node "
				                         "don't exist in the graph");
			}
			auto res_vector = std::vector<E>();

			auto outer_key = edges_.find(src);
			if (outer_key != edges_.end()) {
				auto inner_key = outer_key->second.find(dst);
				if (inner_key != outer_key->second.end()) {
					// weights of outer_key->inner key
					for (auto& set_val : inner_key->second) {
						res_vector.push_back(*set_val);
					}
				}
			}

			return res_vector;
		}

		// return iterator to connection
		[[nodiscard]] auto find(N const& src, N const& dst, E const& weight) const noexcept
		   -> iterator {
			for (auto graph_iter = begin(); graph_iter != end(); graph_iter++) {
				if ((*graph_iter).from == src && (*graph_iter).to == dst
				    && (*graph_iter).weight == weight) {
					return graph_iter;
				}
			}
			return end();
		}

		// asc. sequence of nodes directly connected to src
		[[nodiscard]] auto connections(N const& src) const -> std::vector<N> {
			if (!is_node(src)) {
				throw std::runtime_error("Cannot call gdwg::graph<N, E>::connections if src doesn't "
				                         "exist in the graph");
			}

			auto res_vector = std::vector<N>();

			auto outer_key = edges_.find(src);

			if (outer_key != edges_.end()) {
				for (auto& inner_key : outer_key->second) {
					res_vector.push_back(*inner_key.first);
				}
			}

			return res_vector;
		}

		// ----------   iterator access -----------

		// returns an iterator pointing to the first element in the container
		// return end if no elements or empty edges
		[[nodiscard]] auto begin() const noexcept -> iterator {
			// first src node's first dst node

			auto srcmap = edges_.begin();
			auto dstmap = srcmap->second.begin();
			auto setlist = dstmap->second.begin();

			auto inner_iter = edges_.empty() ? decltype(setlist){} : setlist;
			return iterator(edges_, srcmap, dstmap, inner_iter);
		}

		[[nodiscard]] auto end() const noexcept -> iterator {
			return iterator(edges_, edges_.end(), {}, {});
		}

		// ----------   comparisons -----------

		// compare 2 graphs
		[[nodiscard]] auto operator==(graph const& other) const noexcept -> bool {
			auto flag = 0;
			// nodes are equal, compare maps
			if (nodes() != other.nodes()) {
				return false;
			}

			auto iter1 = begin();
			auto iter2 = other.begin();
			while (iter1 != end() || iter2 != other.end()) {
				if (((*iter1).from == (*iter2).from) && ((*iter1).to == (*iter2).to)
				    && ((*iter1).weight == (*iter2).weight))
				{
					flag = 1;
				}
				else {
					flag = 0;
					break;
				}

				iter1++;
				iter2++;
			}

			return flag;
		}

		//----------   extractor -----------

		// print the graph out
		friend auto operator<<(std::ostream& os, graph const& g) noexcept -> std::ostream& {
			for (auto const& src_node : g.nodes()) {
				os << src_node << " (\n";
				if (!g.connections(src_node).empty()) {
					for (auto const& dst_node : g.connections(src_node)) {
						// loop through set of weights
						for (auto const& weight : g.weights(src_node, dst_node)) {
							os << "  " << dst_node << " | " << weight << "\n";
						}
					}
					os << ")\n";
				}

				// node has no connections
				else
					os << "";
			}
			os << ")\n";

			return os;
		}

	private:
		// sorted set of all nodes in graph
		std::set<std::shared_ptr<N>, comparator> nodes_;
		// map <src, map < dest, weight, comparator >, comparator>
		std::map<std::shared_ptr<N>,
		         std::map<std::shared_ptr<N>, std::set<std::shared_ptr<E>, setComparator>, comparator>,
		         comparator>
		   edges_;

		auto edge_exists(N const& src, N const& dst, E const& weight) const noexcept -> bool {
			// iterate through set of pairs
			for (auto const& src_nodes : edges_) {
				if (*src_nodes.first == src) {
					for (auto const& dst_node : src_nodes.second) {
						if (*dst_node.first == dst) {
							auto found = dst_node.second.find(weight);
							if (found != dst_node.second.end())
								return true;
						}
					}
				}
			}
			return false;
		}
	};

	// ----------   iterator -----------

	template<typename N, typename E>
	class graph<N, E>::iterator {
		using outer = typename std::map<
		   std::shared_ptr<N>,
		   std::map<std::shared_ptr<N>, std::set<std::shared_ptr<E>, setComparator>, comparator>,
		   comparator>::const_iterator;
		using middle =
		   typename std::map<std::shared_ptr<N>, std::set<std::shared_ptr<E>, setComparator>, comparator>::const_iterator;
		using inner = typename std::set<std::shared_ptr<E>, setComparator>::const_iterator;

	public:
		using value_type = graph<N, E>::value_type;
		using reference = value_type;
		using pointer = void;
		using difference_type = std::ptrdiff_t;
		using iterator_category = std::bidirectional_iterator_tag;

		// Iterator constructor
		iterator() = default;

		// Iterator source
		auto operator*() noexcept -> reference {
			value_type graph_val;
			graph_val.from = *outer_iter->first;
			graph_val.to = *middle_iter->first;
			graph_val.weight = **inner_iter;
			return graph_val;
		}

		// Iterator traversal

		// pre increment
		auto operator++() noexcept -> iterator& {
			// make setlist point to start of next set
			auto set_end = middle_iter->second.end();
			// inner iter is not at end of inner key's set
			if (inner_iter != set_end) {
				++inner_iter;
				// inner is still not at end of inner key's set
				if (inner_iter != set_end) {
					return *this;
				}
			}

			++middle_iter;

			// middle iter is not at end of inner keys
			if (middle_iter != outer_iter->second.end()) {
				inner_iter = middle_iter->second.begin();
				return *this;
			}

			++outer_iter;

			// outer_iter is not at outer end of outer keys
			if (outer_iter != edges_->end()) {
				middle_iter = outer_iter->second.begin();
				inner_iter = middle_iter->second.begin();
				return *this;
			}

			// outer is at end
			middle_iter = middle();
			inner_iter = inner();

			return *this;
		};

		// post increment
		auto operator++(int) noexcept -> iterator {
			auto temp_val = *this;
			++*this;
			return temp_val;
		}

		// pre-decremennt
		auto operator--() noexcept -> iterator& {
			// at end of ds
			if (inner_iter == inner()) {
				outer_iter = std::prev(edges_->end());
				middle_iter = std::prev(outer_iter->second.end());
				inner_iter = std::prev(middle_iter->second.end());
				return *this;
			}

			// .begin()
			if (outer_iter == edges_->begin() && middle_iter == outer_iter->second.begin()
			    && inner_iter == middle_iter->second.begin())
			{
				middle_iter = middle();
				inner_iter = inner();
				return *this;
			}

			// begin of any other start inner key
			if (middle_iter == outer_iter->second.begin() && inner_iter == middle_iter->second.begin())
			{
				--outer_iter;
				middle_iter = std::prev(outer_iter->second.end());
				inner_iter = std::prev(middle_iter->second.end());
				return *this;
			}

			// begin of an inner key that has previous inner keys
			if (inner_iter == middle_iter->second.begin()) {
				--middle_iter;
				inner_iter = std::prev(middle_iter->second.end());
				return *this;
			}

			// normal case
			--inner_iter;
			return *this;
		}

		// post-decrement
		auto operator--(int) noexcept -> iterator {
			auto temp_val = *this;
			--*this;
			return temp_val;
		}

		// Iterator comparison
		auto operator==(iterator const& other) const noexcept -> bool = default;

	private:
		friend class graph;
		explicit iterator(
		   std::map<std::shared_ptr<N>,
		            std::map<std::shared_ptr<N>, std::set<std::shared_ptr<E>, setComparator>, comparator>,
		            comparator> const& graph_pointer,
		   outer outer_iter,
		   middle middle_iter,
		   inner inner_iter)
		: edges_(&graph_pointer)
		, outer_iter(outer_iter)
		, middle_iter(middle_iter)
		, inner_iter(inner_iter) {}

		std::map<std::shared_ptr<N>,
		         std::map<std::shared_ptr<N>, std::set<std::shared_ptr<E>, setComparator>, comparator>,
		         comparator> const* edges_ = nullptr;
		outer outer_iter{};
		middle middle_iter{};
		inner inner_iter{};
	};

	// erase edge pointed by i
	template<typename N, typename E>
	auto graph<N, E>::erase_edge(iterator i) -> iterator {
		if (i == end())
			return end();
		auto res = i;
		res++;

		auto const& src = (*i).from;
		auto const& dst = (*i).to;
		auto const& weight = (*i).weight;

		if (erase_edge(src, dst, weight)) {
			return res;
		}
		return end();
	}

	template<typename N, typename E>
	auto graph<N, E>::erase_edge(iterator i, iterator s) -> iterator {
		auto res = i;

		while (res != s) {
			res = erase_edge(res);
		}
		return res;
	}

} // namespace gdwg

#endif // GDWG_GRAPH_HPP
