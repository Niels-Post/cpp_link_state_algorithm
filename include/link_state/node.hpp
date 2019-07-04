/*
 *
 * Copyright Niels Post 2019.
 * Distributed under the Boost Software License, Version 1.0.
 * (See accompanying file LICENSE_1_0.txt or copy at
 * https://www.boost.org/LICENSE_1_0.txt)
 *
*/

#ifndef PROJECT_NODE_HPP
#define PROJECT_NODE_HPP

#include <stdint.h>
#include <array>

namespace link_state {

    /**
     * \addtogroup link_state
     * @{
     */

    /**
     * \brief Link_state network node.
     *
     * Stores all link_state related information of a node.
     * It's edges, costs and path information.
     * If shortest_path_known is false, any path-related information shouldn't be trusted, since the link state algorithm hasn't been (successfully) run on this node.
     * All template arguments used should match those of the used link_state::calculator
     * @tparam id_type Datatype for node identifiers
     * @tparam cost_type  Datatype for edge costs,  make sure this is large enough to contain summed distances.
     * @tparam max_edges Maximum number of edges for this node. This should be kept as low as possible, since it saves memory
     */
    template<typename id_type, typename cost_type, size_t max_edges>
    struct node {
        /// Node identifier
        id_type id;
        /// Identifiers of nodes connected to this node
        std::array<id_type, max_edges> edges;
        /// Costs of all connections to this node (make sure the indices match those of node::edges
        std::array<cost_type, max_edges> edge_costs = {1};
        /// Current amount of connections
        uint8_t edge_count = 0;
        /// Current previous node in the shortest path from the source node to this node. In the source node, this will always be 0
        id_type previous_node = 0;
        /// Current distance from the source node to this node
        cost_type distance = 0;
        /// Is the shortest path to this node known (is the node in N'). If this value is false, either the link_state algorithm hasn't been run yet, or this node is unreachable
        bool shortest_path_known = false;

        /**
         * \brief Create a link_state node.
         *
         * @param id Identifier of the link_state node
         */
        node(id_type id) : id(id) {}

        /**
         * \brief Create a default link_state node.
         *
         * Sets the id to zero, and all other values to their defaults
         */
        node() : id(0) {}

        /**
         * \brief Create a link_state node.
         *
         * Automatically counts the number of edges
         * @param id Identifier of the node
         * @param edges All currently known edges
         * @param edgecosts The costs of the currently known edges
         *
         *
         */
        node(id_type id, const std::array<id_type, max_edges> edges,
             const std::array<cost_type, max_edges> edgecosts
        ) : id(id), edges(edges), edge_costs(edgecosts) {
            for (const id_type &edge_id : edges) {
                if (edge_id == 0) {
                    return;
                }
                edge_count++;
            }
        }

        /**
         * \brief Create a link_state node with default edge costs.
         *
         * All edge costs are assumed to be 1. This can be used in networks where edge costs aren't used.
         * In this case the calculator will function as a Distance Vector routing calculator.
         * Automatically counts the number of edges.
         * @param id Identifier of node
         * @param edges The currently known edges
         */
        node(id_type id, const std::array<id_type, max_edges> &edges) : node(id, edges, {0}) {
            edge_costs.fill(1);
        }

    };

    /**
     * @}
     */
}

#endif //PROJECT_NODE_HPP
