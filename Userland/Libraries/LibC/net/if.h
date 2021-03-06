/*
 * Copyright (c) 2018-2021, Andreas Kling <kling@serenityos.org>
 *
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

#include <Kernel/API/POSIX/net/if.h>

__BEGIN_DECLS

unsigned int if_nametoindex(char const* ifname);
char* if_indextoname(unsigned int ifindex, char* ifname);

__END_DECLS
