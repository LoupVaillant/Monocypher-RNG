// Monocypher version __git__
//
// This file is dual-licensed.  Choose whichever licence you want from
// the two licences listed below.
//
// The first licence is a regular 2-clause BSD licence.  The second licence
// is the CC-0 from Creative Commons. It is intended to release Monocypher
// to the public domain.  The BSD licence serves as a fallback option.
//
// SPDX-License-Identifier: BSD-2-Clause OR CC0-1.0
//
// ------------------------------------------------------------------------
//
// Copyright (c) 2019, Loup Vaillant
// All rights reserved.
//
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the
//    distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// ------------------------------------------------------------------------
//
// Written in 2017-2019 by Loup Vaillant
//
// To the extent possible under law, the author(s) have dedicated all copyright
// and related neighboring rights to this software to the public domain
// worldwide.  This software is distributed without any warranty.
//
// You should have received a copy of the CC0 Public Domain Dedication along
// with this software.  If not, see
// <https://creativecommons.org/publicdomain/zero/1.0/>

#include "monocypher-rng.h"
#include "monocypher.h"

#define FOR(i, start, end) for (size_t i = (start); i < (end); i++)
typedef uint8_t u8;

// avoid memcpy dependency (the compiler will likely use memcpy anyway)
static void copy(u8 *out, u8 *in, size_t size)
{
    FOR (i, 0, size) {
        out[i] = in[i];
    }
}

static void rng_refill(crypto_rng_ctx *ctx)
{
    u8 zero[8] = {0};
    crypto_chacha20(ctx->pool, 0, 512, ctx->pool, zero);
    ctx->idx = 32;
}

void crypto_rng_init(crypto_rng_ctx *ctx, u8 random_seed[32])
{
    copy(ctx->pool, random_seed, 32);
    rng_refill(ctx);
    crypto_wipe(random_seed, 32);
}

void crypto_rng_read(crypto_rng_ctx *ctx, u8 *buf, size_t size)
{
    size_t remainder = 512 - ctx->idx;
    size_t size1     = size > remainder ? remainder : size;

    // Use any remaining random bytes
    copy(buf, ctx->pool + ctx->idx, size1);
    ctx->idx += size1;
    buf      += size1;
    size     -= size1;

    // Fill the buffer block by block
    static const size_t pool_size = 512 - 32; // keep 32 bytes to rekey
    while (size > pool_size) {
        rng_refill(ctx);
        copy(buf, ctx->pool, pool_size);
        size -= pool_size;
        buf  += pool_size;
    }

    // Fill the last bytes, if any
    if (size > 0) {
        rng_refill(ctx);
        copy(buf, ctx->pool + ctx->idx, size);
        ctx->idx += size;
    }
}

void crypto_rng_fork(crypto_rng_ctx *ctx, crypto_rng_ctx *child_ctx)
{
    u8 child_seed[32]; // wiped by crypto_rng_init;
    crypto_rng_read(ctx, child_seed, 32);
    crypto_rng_init(child_ctx, child_seed);
}
