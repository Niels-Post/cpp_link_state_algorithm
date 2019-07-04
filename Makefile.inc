#
# Copyright Niels Post 2019.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt)
#

LINK_STATE_DIR := $(dir $(lastword $(MAKEFILE_LIST)))

SEARCH += $(LINK_STATE_DIR)include/

HEADERS += $(LINK_STATE_DIR)include/link_state/calculator.hpp
HEADERS += $(LINK_STATE_DIR)include/link_state/node.hpp
