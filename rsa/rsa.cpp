#include "rsa.hpp"

//////
/*
 * Copyright 2019 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef _XF_SECURITY_ASYMMETRIC_CRYPTOGRAPHY_HPP_
#define _XF_SECURITY_ASYMMETRIC_CRYPTOGRAPHY_HPP_

#include <ap_int.h>
// #include "xf_security/modular.hpp"

/*
 * Copyright 2019 Xilinx, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 *
 * @file modular.hpp
 * @brief header file for modular operators.
 * This file is part of Vitis Security Library.
 *
 */

#ifndef _XF_SECURITY_MODULAR_HPP_
#define _XF_SECURITY_MODULAR_HPP_

#include <ap_int.h>
#include <hls_stream.h>

#ifndef __SYNTHESIS__
#include <iostream>
#endif

namespace xf {
namespace security {
namespace internal {

/*
 * @brief Montgomery Production of opA and opB and returns opA * opB * opM^-1 mod R
 * Reference: "Efficient architectures for implementing montgomery modular multiplication and RSA modular exponentiation
 * on reconfigurable logic" by Alan Daly, William Marnane
 *
 * @tparam N bit width of opA, opB and opM
 *
 * @param opA Montgomery representation of A
 * @param opB Montgomery representation of B
 * @param opM modulus
 */
template <int N>
ap_uint<N> monProduct(ap_uint<N> opA, ap_uint<N> opB, ap_uint<N> opM) {
    ap_uint<N + 2> s = 0;
    ap_uint<1> a0 = opA[0];
    for (int i = 0; i < N; i++) {
        ap_uint<1> qa = opB[i];
        ap_uint<1> qm = s[0] ^ (opB[i] & a0);
        ap_uint<N> addA = qa == ap_uint<1>(1) ? opA : ap_uint<N>(0);
        ap_uint<N> addM = qm == ap_uint<1>(1) ? opM : ap_uint<N>(0);
        s += (addA + addM);
        s >>= 1;
    }
    if (s > opM) {
        s -= opM;
    }
    return s;
}

/*
 * @brief Modular Exponentiation
 *
 * @tparam L bit width of base and modulus
 * @tparam N bit width of exponent
 *
 * @param base Base of Modular Exponentiation
 * @param exponent Exponent of Modular Exponentiation
 * @param modulus Modulus of Modular Exponentiation
 * @param rMod 2^(2*L) mod modulus could be pre-calculated.
 */
template <int L, int N>
ap_uint<L> modularExp(ap_uint<L> base, ap_uint<N> exponent, ap_uint<L> modulus, ap_uint<L> rMod) {
    ap_uint<L> P = monProduct<L>(rMod, base, modulus);
    ap_uint<L> R = monProduct<L>(rMod, 1, modulus);
    for (int i = N - 1; i >= 0; i--) {
        R = monProduct<L>(R, R, modulus);
        if (exponent[i] == 1) {
            R = monProduct<L>(R, P, modulus);
        }
    }
    return monProduct<L>(R, 1, modulus);
}

/*
 * @brief modulo operation, returns remainder of dividend against divisor.
 *
 * @tparam L bit width of dividend
 * @tparam N bit width of divisor
 *
 * @param dividend Dividend
 * @param divisor Divisor
 */
template <int L, int N>
ap_uint<N> simpleMod(ap_uint<L> dividend, ap_uint<N> divisor) {
    ap_uint<N> remainder = dividend % divisor;
    return remainder;
}

/**
 * @brief return (opA * opB) mod opM
 *
 * @tparam N bit width of opA, opB and opM
 *
 * @param opA Product input, should be less than opM
 * @param opB Product input, should be less than opM
 * @param opM Modulus, should be larger than 2^(N-1)
 */
template <int N>
ap_uint<N> productMod(ap_uint<N> opA, ap_uint<N> opB, ap_uint<N> opM) {
    ap_uint<N + 1> tmp = 0;
    for (int i = N - 1; i >= 0; i--) {
#pragma HLS PIPELINE II=0
        tmp <<= 1;
        if (tmp > opM) {
            tmp -= opM;
        }
        if (opB[i] == 1) {
            tmp += opA;
            if (tmp > opM) {
                tmp -= opM;
            }
        }
    }
    return tmp;
}

/**
 * @brief return (opA + opB) mod opM
 *
 * @tparam N bit width of opA, opB and opM
 *
 * @param opA Product input, should be less than opM
 * @param opB Product input, should be less than opM
 * @param opM Modulus
 */
template <int N>
ap_uint<N> addMod(ap_uint<N> opA, ap_uint<N> opB, ap_uint<N> opM) {
    ap_uint<N + 1> sum = opA + opB;
    if (sum >= opM) {
        sum -= opM;
    }
    return sum;
}

/**
 * @brief return (opA - opB) mod opM
 *
 * @tparam N bit width of opA, opB and opM
 *
 * @param opA Product input, should be less than opM
 * @param opB Product input, should be less than opM
 * @param opM Modulus
 */
template <int N>
ap_uint<N> subMod(ap_uint<N> opA, ap_uint<N> opB, ap_uint<N> opM) {
    ap_uint<N + 1> sum;
    if (opA >= opB) {
        sum = opA - opB;
    } else {
        sum = opA + opM;
        sum -= opB;
    }
    return sum;
}

/**
 * @brief return montgomery inverse of opA
 * Reference: "The Montgomery Modular Inverse - Revisited" by E Savas, CK Koç
 *
 * @tparam N bit width of opA and opM
 *
 * @param opA Input of modular inverse.
 * @param opM Modulus of modular inverse.
 */
template <int N>
ap_uint<N> monInv(ap_uint<N> opA, ap_uint<N> opM) {
    // calc r = opA^-1 * 2^k and k
    ap_uint<N> u = opM;
    ap_uint<N> v = opA;
    ap_uint<N> s = 1;
    ap_uint<N + 1> r = 0;
    ap_uint<32> k = 0;

    while (v > 0) {
        if (u[0] == 0) {
            u >>= 1;
            s <<= 1;
        } else if (v[0] == 0) {
            v >>= 1;
            r <<= 1;
        } else if (u > v) {
            u -= v;
            u >>= 1;
            r += s;
            s <<= 1;
        } else {
            v -= u;
            v >>= 1;
            s += r;
            r <<= 1;
        }
        k++;
    }

    if (r >= opM) {
        r -= opM;
    }
    r = opM - r;

    k -= N;

    for (int i = 0; i < k; i++) {
        if (r[0] == 1) {
            r += opM;
        }
        r >>= 1;
    }

    return r;
}

/**
 * @brief return modular inverse of opA
 * Reference: "The Montgomery Modular Inverse - Revisited" by E Savas, CK Koç
 *
 * @tparam N bit width of opA and opM, opM should no less than 2^(N-1)
 *
 * @param opA Input of modular inverse.
 * @param opM Modulus of modular inverse.
 */
template <int N>
ap_uint<N> modularInv(ap_uint<N> opA, ap_uint<N> opM) {
    // calc r = opA^-1 * 2^k and k
    ap_uint<N> u = opM;
    ap_uint<N> v = opA;
    ap_uint<N> s = 1;
    ap_uint<N + 1> r = 0;
    ap_uint<32> k = 0;

    while (v > 0) {
        if (u[0] == 0) {
            u >>= 1;
            s <<= 1;
        } else if (v[0] == 0) {
            v >>= 1;
            r <<= 1;
        } else if (u > v) {
            u -= v;
            u >>= 1;
            r += s;
            s <<= 1;
        } else {
            v -= u;
            v >>= 1;
            s += r;
            r <<= 1;
        }
        k++;
    }

    if (r >= opM) {
        r -= opM;
    }
    r = opM - r;

    k -= N;

    for (int i = 0; i < k; i++) {
        if (r[0] == 1) {
            r += opM;
        }
        r >>= 1;
    }

    ap_uint<N> res = monProduct<N>(r.range(N - 1, 0), 1, opM);

    return res;
}

} // namespace internal
} // namespace security
} // namespace xf

#endif
// #include "xf_security/modular.hpp"

namespace xf {
namespace security {

/**
 * @brief RSA encryption/decryption class
 *
 * @tparam N BitWdith of modulus of key.
 * @tparam L BitWdith of exponents of RSA encryption and decryption
 */
template <int N, int L>
class rsa {
   public:
    ap_uint<L> exponent;
    ap_uint<N> modulus;
    ap_uint<N> rMod;

    /**
     * @brief Update key before use it to encrypt message
     *
     * @param inputModulus Modulus in RSA public key.
     * @param inputExponent Exponent in RSA public key or private key.
     */
    void updateKey(ap_uint<N> inputModulus, ap_uint<L> inputExponent) {
        modulus = inputModulus;
        exponent = inputExponent;

        ap_uint<N + 1> tmp = 0;
        tmp[N] = 1;
        tmp %= inputModulus;

        rMod = xf::security::internal::productMod<N>(tmp, tmp, inputModulus);
    }

    /**
     * @brief Update key before use it to encrypt message
     *
     * @param inputModulus Modulus in RSA public key.
     * @param inputExponent Exponent in RSA public key or private key.
     * @param inputRMod 2^(2 * N) mod modulus, pre-calculated by user
     */
    void updateKey(ap_uint<N> inputModulus, ap_uint<L> inputExponent, ap_uint<N> inputRMod) {
        modulus = inputModulus;
        exponent = inputExponent;
        rMod = inputRMod;
    }

    /**
     * @brief Encrypt message and get result. It does not include any padding scheme
     *
     * @param message Message to be encrypted/decrypted
     * @param result Generated encrypted/decrypted result.
     */
    void process(ap_uint<N> message, ap_uint<N>& result) {
        result = xf::security::internal::modularExp<N, L>(message, exponent, modulus, rMod);
    }
};

} // namespace security
} // namespace xf
#endif

//////
inline void rsa_test(ap_uint<512> message, ap_uint<512> modulus, ap_uint<512> exponent, ap_uint<512>& result) {
    xf::security::rsa<512, 512> processor;
    processor.updateKey(modulus, exponent);
    processor.process(message, result);
}

void rsa(hls::stream<AXIS_DATA>& input,
		hls::stream<AXIS_DATA >& output) {
#pragma HLS INTERFACE ap_ctrl_none port=return // removes handshake
#pragma HLS interface axis register port = input
#pragma HLS interface axis register port = output
#pragma HLS AGGREGATE variable=input bit
#pragma HLS AGGREGATE variable=output bit

	static const ap_uint<512> modulus = "0x00C87C2049F275B962773F15F01DFF98B9313432836D64CDB9BEBD2326FA0025386C079CAE1A02DD1B1609F0312C0D53B9C08D10F6D72C30442B87DC8B2650FC63";
	static const ap_uint<512> exponent = "0x010001";

	AXIS_DATA outWord;
	input.read(outWord);
	ap_uint<512> message = outWord.tdata;
	ap_uint<512> result2;
	rsa_test(message, modulus, exponent, result2);
	outWord.tdata = result2;
	output.write(outWord);
}
void rsa_decrypt(hls::stream<AXIS_DATA>& input,
		hls::stream<AXIS_DATA >& output) {
#pragma HLS INTERFACE ap_ctrl_none port=return // removes handshake
#pragma HLS interface axis register port = input
#pragma HLS interface axis register port = output
#pragma HLS AGGREGATE variable=input bit
#pragma HLS AGGREGATE variable=output bit

	static const ap_uint<512> modulus = "0x00C87C2049F275B962773F15F01DFF98B9313432836D64CDB9BEBD2326FA0025386C079CAE1A02DD1B1609F0312C0D53B9C08D10F6D72C30442B87DC8B2650FC63";
	static const ap_uint<512> exponent2 = ("0x7E1EF8010CF25A294373004E09D11D3683699ECD67281EF356B68D429BCB339DE9998B43079F7B6A5DE98B5610FE0FEB93A5453E9F390A18BC90720E89DD4C21");

	AXIS_DATA currWord;
	input.read(currWord);

	if (currWord.tlast == 1) {
		ap_uint<512> message = currWord.tdata;
		ap_uint<512> result2;
		rsa_test(message, modulus, exponent2, result2);
		currWord.tdata = result2;
	}
	output.write(currWord);

}
