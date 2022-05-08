#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>
#include <sstream>

/*
Testing Rationale
------------------

I decided to go with a bottom-up testing strategy for this assignment too.
I tested the constructors, functions and iterators in the order given in the spec,
but with some slight changes to this general order as I needed some modifers and accesors for more
comprehensive construction tests.

The approach I used is as follows:
Test cases are split and categorized by [stable.names] titles from the project spec.
Testing each sub-function in sections, broken up into sections of non-const/
const or empty/non-empty graph inputs, or a combination of them where applicable. Following this
there are sections for each function for exceptions and potential edge cases where applicable.

Example Test format:

TEST_CASE("category_description, function_name()", [categoryTag]) {
   SECTION(non-const function test) {
      ...
   }
      SECTION(const function test) {
      ...
   }

   SECTION(const function test) {
      ...
   }
}

WHY I used this approach. This approach ensured that:
-----------------------------------------------------

1. The above testing format helped me verify and rectify the const-ness of all functions.
2. With the bottom-up testing approach, functions that have been thoroughly tested previously are
used in following tests, validating the correctness.
3. It's clear to see what's going in each test, in an easy to understand format where each test case
is simple and broken up into sections.
4. The tests are designed to be the least brittle as possible, not being affected by a change in
implementation as I only call the specified funtions/constructors.
5. Since I followed the order of testing (mentioned above) with the above testing strategy, it
ensured that the tests covered the entire implementation.
*/

TEST_CASE("Constructing graphs", "[Constructors]") {
	SECTION("empty constructor") {
		auto empty_graph = gdwg::graph<int, std::string>();
		CHECK(empty_graph.empty());
	}

	SECTION("const empty constructor") {
		auto const empty_graph = gdwg::graph<int, std::string>();
		CHECK(empty_graph.empty());
	}

	SECTION("list constructor") {
		auto graph = gdwg::graph<std::string, std::string>{"leo", "messi", "the", "goat"};
		auto sorted_graph = std::vector<std::string>{"goat", "leo", "messi", "the"};
		CHECK(graph.nodes() == sorted_graph);
	}

	SECTION("const list constructor") {
		auto const graph = gdwg::graph<std::string, std::string>{"leo", "messi", "the", "goat"};
		auto sorted_graph = std::vector<std::string>{"goat", "leo", "messi", "the"};
		CHECK(graph.nodes() == sorted_graph);
	}

	SECTION("input_iterator constructor") {
		auto vec = std::vector<int>{1, 3, 4, 2};
		auto sorted_vec = std::vector<int>{1, 2, 3, 4};
		auto iter_graph = gdwg::graph<int, int>(vec.begin(), vec.end());
		CHECK(iter_graph.nodes() == sorted_vec);
	}

	SECTION("const input_iterator constructor") {
		auto vec = std::vector<int>{1, 3, 4, 2};
		auto sorted_vec = std::vector<int>{1, 2, 3, 4};
		auto const iter_graph = gdwg::graph<int, int>(vec.begin(), vec.end());
		CHECK(iter_graph.nodes() == sorted_vec);
	}

	SECTION("copy constructor") {
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto sorted_graph = std::vector<int>{1, 2, 3, 4};
		auto graph = gdwg::graph<int, int>(list);
		auto graph2 = graph;
		CHECK(graph.nodes() == graph2.nodes());
		CHECK(graph2.nodes() == sorted_graph);
	}

	SECTION("const copy constructor") {
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto sorted_graph = std::vector<int>{1, 2, 3, 4};
		auto const graph = gdwg::graph<int, int>(list);
		auto graph2 = graph;
		CHECK(graph.nodes() == graph2.nodes());
		CHECK(graph2.nodes() == sorted_graph);
	}

	SECTION("copy assignment") {
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto sorted_graph = std::vector<int>{1, 2, 3, 4};
		auto list2 = std::initializer_list<int>{1, 6, 5, 2};
		auto graph = gdwg::graph<int, int>(list);
		auto graph2 = gdwg::graph<int, int>(list2);
		graph2 = graph;
		// CHECK(graph == graph2);
		CHECK(graph2.nodes() == sorted_graph);
	}

	SECTION("copy assignment") {
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto sorted_graph = std::vector<int>{1, 2, 3, 4};
		auto list2 = std::initializer_list<int>{1, 6, 5, 2};
		auto const graph = gdwg::graph<int, int>(list);
		auto graph2 = gdwg::graph<int, int>(list2);
		graph2 = graph;
		// CHECK(graph == graph2);
		CHECK(graph2.nodes() == sorted_graph);
	}

	SECTION("move constructor") {
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto sorted_graph = std::vector<int>{1, 2, 3, 4};
		auto graph = gdwg::graph<int, int>(list);
		auto graphCopy = graph;
		auto graph2 = gdwg::graph<int, int>(std::move(graph));
		// CHECK(graph == graphCopy);
		CHECK(graph2.nodes() == sorted_graph);
		CHECK(graph.empty() == true);
	}

	SECTION("const move constructor") {
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto sorted_graph = std::vector<int>{1, 2, 3, 4};
		auto graph = gdwg::graph<int, int>(list);
		auto const graphCopy = graph;
		auto graph2 = gdwg::graph<int, int>(std::move(graph));
		// CHECK(graph == graphCopy);
		CHECK(graph2.nodes() == sorted_graph);
		CHECK(graph.empty() == true);
	}

	SECTION("move assignment") {
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto sorted_graph = std::vector<int>{1, 2, 3, 4};
		auto graph = gdwg::graph<int, int>(list);
		auto graphCopy = graph;
		auto graph2 = std::move(graph);
		// CHECK(graph == graphCopy);
		CHECK(graph2.nodes() == sorted_graph);
		CHECK(graph.empty() == true);
	}

	SECTION("const move assignment") {
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto sorted_graph = std::vector<int>{1, 2, 3, 4};
		auto graph = gdwg::graph<int, int>(list);
		auto const graphCopy = graph;
		auto graph2 = std::move(graph);
		// CHECK(graph == graphCopy);
		CHECK(graph2.nodes() == sorted_graph);
		CHECK(graph.empty() == true);
	}
}

TEST_CASE("Checking nodes in a graph", "[Accessors]") {
	SECTION("is_node function, is_node()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
		};

		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));
		CHECK(!g.is_node(5));
	}

	SECTION("const is_node function, is_node()") {
		using graph_t = gdwg::graph<int, int>;
		auto const list = std::initializer_list<int>{1, 3, 4, 2};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
		};

		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));
		CHECK(!g.is_node(5));
	}

	SECTION("check non-empty graph, empty()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
		};

		CHECK(!g.empty());
	}

	SECTION("check empty graph, empty()") {
		auto empty_graph = gdwg::graph<int, std::string>();
		CHECK(empty_graph.empty());
	}

	SECTION("check nodes are connected, is_connected()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
		};

		CHECK(g.is_connected(1, 4));
		CHECK(g.is_connected(2, 4));
		CHECK(g.is_connected(1, 1));
		CHECK(!g.is_connected(2, 1));
	}

	SECTION("check some nodes are not connected, is_connected()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
		};

		CHECK_THROWS_WITH(g.is_connected(1, 7),
		                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't "
		                  "exist in the graph");
		CHECK_THROWS_WITH(g.is_connected(7, 1),
		                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't "
		                  "exist in the graph");
	}

	SECTION("check nodes present in empty graph, nodes()") {
		auto g = gdwg::graph<int, std::string>();
		auto empty_list = std::vector<int>{};
		CHECK(g.nodes() == empty_list);
	}

	SECTION("check nodes present in graph, nodes()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto sorted_list = std::vector<int>{1, 2, 3, 4};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
		};

		CHECK(g.nodes() == sorted_list);
	}

	SECTION("check weights present in empty graph, weights()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		};

		auto empty_list = std::vector<int>{};
		CHECK(g.weights(1, 2) == empty_list);
	}

	SECTION("check weights present in graph, weights()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		   {1, 4, 5},
		};

		auto sorted_weight_list_1_4 = std::vector<int>{3, 5};
		auto sorted_weight_list_2_4 = std::vector<int>{2};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
		};

		CHECK(g.weights(1, 4) == sorted_weight_list_1_4);
		CHECK_THROWS_WITH(g.weights(6, 9),
		                  "Cannot call gdwg::graph<N, E>::weights if src or dst node don't exist in "
		                  "the graph");
	}

	SECTION("check if edge is present in graph(), find()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto sorted_list = std::vector<int>{1, 2, 3, 4};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
		};

		CHECK(g.nodes() == sorted_list);
		auto iter = g.begin();
		CHECK(g.find(1, 1, 2) == iter);
		iter++;
		CHECK(g.find(1, 4, 3) == iter);
		iter++;
		iter--;
		CHECK(g.find(1, 4, 3) == iter);
		iter++;
		CHECK(g.find(2, 4, 2) == iter);
		iter++;
		CHECK(g.find(1, 1, 7) == iter);
	}

	SECTION("check if edges is present in graph(), find()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto sorted_list = std::vector<int>{1, 2, 3, 4};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
		};
	}

	SECTION("check directed connections from src, connections()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto connections_1 = std::vector<int>{1, 4};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
		};

		CHECK(g.connections(1) == connections_1);
		CHECK_THROWS_WITH(g.connections(11),
		                  "Cannot call gdwg::graph<N, E>::connections if src doesn't exist in the "
		                  "graph");
	}
}

TEST_CASE("Iterators", "[Iterator Access, Iterators]") {
	SECTION("check begin and dereferencing works simple case, begin(), operator*()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{{1, 1, 1}};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
		};

		auto start_iter = g.begin();

		CHECK((*start_iter).from == 1);
		CHECK((*start_iter).to == 1);
		CHECK((*start_iter).weight == 1);
		CHECK(g.begin() != g.end());
	}

	SECTION("check begin works, begin(), operator*()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
		};

		auto start_iter = g.begin();

		CHECK((*start_iter).from == 1);
		CHECK((*start_iter).to == 1);
		CHECK((*start_iter).weight == 2);
	}

	SECTION("check ++ works, begin(), operator++()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
		};

		auto start_iter = g.begin();

		CHECK((*start_iter).from == 1);
		CHECK((*start_iter).to == 1);
		CHECK((*start_iter).weight == 2);
		CHECK(start_iter != g.end());
		++start_iter;
		CHECK((*start_iter).from == 1);
		CHECK((*start_iter).to == 4);
		CHECK((*start_iter).weight == 3);
		CHECK(start_iter != g.end());
		start_iter++;
		CHECK((*start_iter).from == 2);
		CHECK((*start_iter).to == 4);
		CHECK((*start_iter).weight == 2);
		++start_iter;
		CHECK(start_iter == g.end());
	}

	SECTION("check ++ works in a for loop, operator++()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
		};

		auto flag = 0;
		for (auto iter = g.begin(); iter != g.end(); iter++) {
			if (flag == 2) {
				CHECK((*iter).from == 2);
				CHECK((*iter).to == 4);
				CHECK((*iter).weight == 2);
			}
			flag++;
		}
	}

	SECTION("check -- works, operator--()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
		};

		auto iter = g.begin();
		iter++;
		iter++;
		iter--;
		CHECK((*iter).from == 1);
		CHECK((*iter).to == 4);
		CHECK((*iter).weight == 3);
		CHECK(iter != g.end());

		auto end_iter = std::prev(g.end());
		CHECK((*end_iter).from == 2);
		CHECK((*end_iter).to == 4);
		CHECK((*end_iter).weight == 2);
		CHECK(end_iter != g.end());
		--end_iter;
		CHECK((*end_iter).from == 1);
		CHECK((*end_iter).to == 4);
		CHECK((*end_iter).weight == 3);
		CHECK(end_iter != g.end());
		--end_iter;
		CHECK((*end_iter).from == 1);
		CHECK((*end_iter).to == 1);
		CHECK((*end_iter).weight == 2);
		CHECK(end_iter != g.end());
		CHECK(end_iter == g.begin());
	}
}

TEST_CASE("Modifying the nodes and edges in a graph", "[Modifiers]") {
	SECTION("check inserting nodes works, insert_node()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
		};

		g.insert_node(7);
		CHECK(g.is_node(7));
		CHECK(!g.is_node(8));
	}

	SECTION("check inserting edges works, insert_edge()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
		};

		CHECK(!g.empty());
		CHECK(g.begin() == g.find(1, 1, 2));
	}

	SECTION("check repacing a node, insert_edge()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
		};

		g.replace_node(1, 7);
		CHECK_THROWS_WITH(g.replace_node(6, 9),
		                  "Cannot call gdwg::graph<N, E>::replace_node on a node that doesn't exist");
		CHECK(g.begin() == g.find(2, 4, 2));
	}

	SECTION("check merge_repacing a node, merge_replace()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
		};

		CHECK_THROWS_WITH(g.replace_node(6, 9),
		                  "Cannot call gdwg::graph<N, E>::replace_node on a node that doesn't exist");
	}

	SECTION("check erasing a node works, erase_node()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
		};

		g.erase_node(1);

		CHECK(g.find(1, 1, 2) == g.end());
		CHECK(g.find(1, 4, 3) == g.end());
		CHECK(g.find(2, 4, 2) == g.begin());
	}

	SECTION("check erasing a node works, erase_node()") {
		using graph_t = gdwg::graph<int, int>;
		// one node with no connections
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		   {2, 1, 2},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
		};

		g.erase_node(1);

		CHECK(g.find(1, 1, 2) == g.end());
		CHECK(g.find(1, 4, 3) == g.end());
		CHECK(g.find(2, 4, 2) == g.begin());
		CHECK(g.find(2, 1, 2) == g.end());

		g.erase_node(2);
		g.erase_node(4);
		CHECK(!g.empty());
		g.erase_node(3);
		CHECK(g.empty());
	}

	SECTION("check erasing a weight, erase_edge()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1, 4, 2};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 4, 2},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
		};

		CHECK_THROWS_WITH(g.erase_edge(1, 7, 2),
		                  "Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they don't "
		                  "exist in the graph");

		auto iter = g.find(1, 4, 3);
		CHECK(g.erase_edge(1, 4, 2) == true);
		CHECK(g.begin() == iter);
		CHECK(g.find(1, 4, 3) != g.end());
		CHECK(g.find(2, 4, 2) != g.end());
		CHECK(g.erase_edge(1, 4, 3) == true);
		CHECK(g.find(2, 4, 2) == g.begin());
		CHECK(g.erase_edge(2, 4, 2) == true);
		CHECK(!g.empty());
		CHECK(g.erase_node(4) == true);
		CHECK(g.empty());
	}

	SECTION("check erasing a node at an iterator works, erase_node(iterator)") {
		using graph_t = gdwg::graph<int, int>;
		// one node with no connections
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		   {1, 4, 5},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
		};

		auto iter = g.begin();
		iter++;
		g.erase_edge(iter);
	}

	SECTION("check erasing a node at an iterator works, erase_node(iterator)") {
		using graph_t = gdwg::graph<int, int>;
		// one node with no connections
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		   {1, 4, 5},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
		};

		auto iter = g.begin();
		auto iter2 = iter;
		iter2++;
		iter2++;

		// erase first 2 nodes
		g.erase_edge(iter, iter2);
	}

	SECTION("check clearing a graph, clear()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1, 4, 2};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 4, 2},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
		};

		g.clear();
		CHECK(g.empty());
	}
}

TEST_CASE("Checking if graphs are equal", "[Comparisons]") {
	SECTION("check if 2 graphs are equal, operator==()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto g = graph_t(list);
		auto gCopy = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
			gCopy.insert_edge(src, dst, weight);
		};

		CHECK(gCopy == g);
	}

	SECTION("check if 2 unequal length graphs are equal, operator==()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto g = graph_t(list);
		auto gCopy = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
			gCopy.insert_edge(src, dst, weight);
		};

		gCopy.insert_edge(1, 4, 5);
		CHECK(gCopy != g);
	}

	SECTION("check if 2 empty graphs are equal, operator==()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1, 3, 4, 2};
		auto g = graph_t(list);
		auto gCopy = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {1, 4, 3},
		   {2, 4, 2},
		   {1, 1, 2},
		};

		for (const auto& [src, dst, weight] : v) {
			g.insert_edge(src, dst, weight);
			gCopy.insert_edge(src, dst, weight);
		};

		gCopy.insert_edge(1, 4, 5);
		gCopy.clear();
		g.clear();
		CHECK(g.empty() == gCopy.empty());
	}
}

TEST_CASE("Checking if extractor works", "[Extractor]") {
	SECTION("fully formed graph, operator<<()") {
		using graph_t = gdwg::graph<int, int>;
		auto list = std::initializer_list<int>{1, 2, 3, 4, 5, 6};
		auto g = graph_t(list);
		auto const v = std::vector<graph_t::value_type>{
		   {4, 1, -4},
		   {3, 2, 2},
		   {2, 4, 2},
		   {2, 1, 1},
		   {6, 2, 5},
		   {6, 3, 10},
		   {1, 5, -1},
		   {3, 6, -8},
		   {4, 5, 3},
		   {5, 2, 7},
		};

		for (const auto& [x, y, z] : v) {
			g.insert_edge(x, y, z);
		};

		g.insert_node(64);
		auto out = std::ostringstream{};
		out << g;
		auto const expected_output = std::string_view(R"(1 (
  5 | -1
)
2 (
  1 | 1
  4 | 2
)
3 (
  2 | 2
  6 | -8
)
4 (
  1 | -4
  5 | 3
)
5 (
  2 | 7
)
6 (
  2 | 5
  3 | 10
)
64 (
)
)");

		CHECK(out.str() == expected_output);
	}
}
