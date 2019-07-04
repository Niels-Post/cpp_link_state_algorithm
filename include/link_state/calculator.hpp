/*
 *
 * Copyright Niels Post 2019.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * https://www.boost.org/LICENSE_1_0.txt)
 *
*/

#ifndef IPASS_LINK_STATE_CALCULATOR_HPP
#define IPASS_LINK_STATE_CALCULATOR_HPP

#include <link_state/node.hpp>

namespace link_state {
    /**
     * \defgroup link_state Link State Algorithm Calculator
     * \brief Abstract implementation of the link_state algorithm
     *
     * Uses a template for adaptibility with various situations.
     * Different datatypes can be used as node identifiers, edge costs.
     * The maximum number of nodes and edges can be changed through the template as well
     */

    /**
     * \addtogroup link_state
     * @{
     */

    /**
     * \brief Link State Calculator. Calculates shortest path from a source node to any node in the network.
     *
     * Calculator assumes node index 0 to always be the source node. Make sure this is correctly set for any use of this module.
     * THe link state algorithm is used to find the shortest route for the current state of the given network graph.
     * @tparam id_type Datatype that is used for node identifiers
     * @tparam cost_type Datatype used for edge costs, calculator automatically calculates the max value for the given datatype based on it's size. Make sure this datatype is large enough to hold summed distances as well.
     * @tparam max_edges Maximum number of edges each node can hold. Keeping this at a minimum saves memory space.
     * @tparam max_nodes Maximum number of nodes in the network graph. Keeping this at a minimum saves memory space.
     */
    template<typename id_type, typename cost_type, size_t max_edges, size_t max_nodes>
    class calculator {
    private:
        std::array<node<id_type, cost_type, max_edges>, max_nodes>
                nodes = {};
        size_t node_count = 0;
    public:
        /// Calculated maximum distance for this cost_type
        cost_type max_distance;

        /**
         * \brief Create a calculator
         *
         * @param source_id Identifier for the source node (index 0)
         */
        explicit calculator(id_type source_id) {
            cost_type temp = 0;
            for (size_t i = 0; i < sizeof(cost_type); i++) {
                temp <<= 8;
                temp |= 0xFF;
            }
            max_distance = temp;
            nodes[0].id = source_id;
            nodes[0].distance = 0;
            node_count++;
        }

        /**
         * \brief Retrieve a node's current state.
         *
         * Note that any routing related information will not be accurate unless setup() and loop() have been called in the current network state.
         * Will return an empty node, if no node exists at the given index yet.
         * @param index Node index
         * @return  The node at the given index.
         */
        node<id_type, cost_type, max_edges> &get_node(const size_t &index) {
            return nodes[index];
        }

        /**
         * \brief Retrieve the number of nodes currently available in the network.
         *
         * @return Number of nodes
         */
        size_t get_node_count() const {
            return node_count;
        }

        /**
         * \brief Retrieve the node index of the node with a given identifier.
         *
         * Returns the current node count if no node with that id exists.
         * @param id Identifier to check for.
         * @return The index of the node, or node_count
         */
        size_t get_index_by_id(const id_type &id) {
            for (size_t i = 0; i < node_count; i++) {
                if (nodes[i].id == id) {
                    return i;
                }
            }
            return node_count;
        }

        /**
         * \brief Replace the node with the given node, or inserts the node if there was no node with that id
         *
         * Will replace the node based on current identifier, any other part of node state won't be checked for.
         * @param node The node to insert/replace
         */
        void insert_replace(node<id_type, cost_type, max_edges> node) {
            size_t existing_node = get_index_by_id(node.id);
            nodes[existing_node] = node;

            if (existing_node == node_count) {
                node_count++;
            }
        }

        /**
         * \brief Remove node that has the given id
         *
         * @param id Identifier for which to remove a node
         * @return True if the remove succeeded, false if the node didn't exist
         */
        bool remove(const id_type &id) {
            size_t node_index = get_index_by_id(id);
            if (node_index != node_count && node_index != 0) {
                node_count--;
                for (size_t i = node_index; i < node_count; i++) {
                    nodes[i] = nodes[i + 1];
                }
                nodes[node_count] = {};
                return true;
            }
            return false;
        }

        /**
         * \brief Get next hop for a given node id. Note that setup and loop need to have been called in the current network state for accurate results.
         *
         * Propagates through previous_nodes until the source node is reached, and returns the last node before that
         * @param id ID to find next hop for
         * @return The id of the next hop
         */
        id_type get_next_hop(const id_type &id) {
            size_t node_index = get_index_by_id(id);
            if (node_index == node_count) {
                return 0;
            }

            auto current_hop = get_node(node_index);
            while (current_hop.previous_node != nodes[0].id) {

                node_index = get_index_by_id(current_hop.previous_node);
                if (current_hop.distance == max_distance) {
                    return 0;
                }
                if (!current_hop.shortest_path_known) {
                    return 0;
                }
                if (current_hop.previous_node == 0) {
                    return 0;
                }
                if (node_index ==
                    node_count) { //Either this node is unreachable, or link state hasn't been run yet since latest change todo auto-run?
                    return 0;
                }


                current_hop = get_node(node_index);

            }
            return current_hop.id;
        }

        /**
         * \brief Setup phase for Link_state routing algorithm
         *
         * Sets all "shortest_path_known"'s to false, except for the source node.
         * Adds the initial distance of all direct neighbours of the source node.
         */
        void setup() {
            node<id_type, cost_type, max_edges> &source_node = nodes[0];
            source_node.shortest_path_known = true;

            for (size_t i = 1; i < node_count; i++) {
                node<id_type, cost_type, max_edges> &current_node = nodes[i];
                current_node.shortest_path_known = false;
                current_node.distance = max_distance;
                for (size_t j = 0; j < current_node.edge_count; j++) {
                    if (current_node.edges[j] == source_node.id && current_node.edge_costs[j] < current_node.distance) {
                        current_node.distance = current_node.edge_costs[j];
                        current_node.previous_node = source_node.id;
                        break;
                    }
                }
            }
        }

        /**
         * \brief Loop phase for Link_state routing algorithm
         *
         * Loops through all nodes until all nodes have shortest_path_known = true, or the remaining nodes are all definitively unreachable.
         * Calculates the minimum distance for each node, and sets previous node.
         */
        void loop() {
            for (size_t definitive_node_count = 1; definitive_node_count < node_count; definitive_node_count++) {
                cost_type min_distance = max_distance;
                size_t min_distance_node = 0;
                for (size_t i = 1; i < node_count; i++) {
                    node<id_type, cost_type, max_edges> &check_node = nodes[i];
                    if (check_node.shortest_path_known) {
                        continue;
                    }

                    if (check_node.distance < min_distance) {
                        min_distance = check_node.distance;
                        min_distance_node = i;
                    }
                }

                if (min_distance == max_distance) { //Fucked, ABORT todo: request routing update
                    return;
                } else {
                }

                node<id_type, cost_type, max_edges> &current_node = nodes[min_distance_node];


                for (size_t edge_id = 0; edge_id < current_node.edge_count; edge_id++) {
                    auto neighbour_index = get_index_by_id(current_node.edges[edge_id]);
                    if (neighbour_index == node_count) {
                        // We don't know this node, todo request routing update, propagate through previous_hops to find next_hop
                        continue;
                    }

                    node<id_type, cost_type, max_edges> &neighbour = nodes[neighbour_index];

                    if (neighbour.shortest_path_known) {
                        continue;
                    }

                    if ((current_node.distance + current_node.edge_costs[edge_id]) < neighbour.distance) {
                        neighbour.distance = (current_node.distance + current_node.edge_costs[edge_id]);
                        neighbour.previous_node = current_node.id;
                    }

                }

                current_node.shortest_path_known = true;
            }

        }


        /**
         * \brief Clean up any unreachable nodes
         *
         * Calls setup and loop first, otherwise we would be removing connected nodes
         */
        void cleanup(bool calculate = false) {
            if (calculate) {
                setup();
                loop();
            }

            for (uint8_t current_node = 0; current_node < node_count; current_node++) {
                if (nodes[current_node].distance == max_distance) {
                    remove(nodes[current_node].id);
                    current_node--;
                }
            }
        }
    };

    /**
     * @}
     */
}
#endif //IPASS_LINK_STATE_CALCULATOR_HPP
