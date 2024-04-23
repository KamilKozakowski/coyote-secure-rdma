#include "eddsa.hpp"


//#define OG
#define BE

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
 * @file eddsa.hpp
 * @brief header file for Edwards-curve Digital Signature Algorithm related function.
 * Now it support curve ed25519.
 * This file is part of Vitis Security Library.
 */

#ifndef _XF_SECURITY_EDDSA_HPP_
#define _XF_SECURITY_EDDSA_HPP_

#include <ap_int.h>
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
 * @brief Montgomery Production of opA and opB and returns opA * opB * R^-1 mod opM
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
MON_PRODUCT_MOD:
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
ap_uint<256> productMod(const ap_uint<256> opA,const ap_uint<256> opB,const ap_uint<256> opM) {
#pragma HLS inline off
    ap_uint<256 + 1> tmp = 0;
PRODUCT_MOD:
    for (int i = 256 - 1; i >= 0; i--) {

#pragma HLS PERFORMANCE target_ti=1000 //KAMIL
       tmp <<= 1;
        if (tmp >= opM) {
            tmp -= opM;
        }
        if (opB[i] == 1) {
            tmp += opA;
            if (tmp >= opM) {
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
 * @param opA Input of modular inverse. opA should be non-zero, might need extra checking
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

INV_MOD_I:
    while (v > 0) {
#pragma HLS loop_tripcount max = 256
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

INV_MOD_II:
    for (int i = 0; i < k; i++) {
#pragma HLS loop_tripcount max = 256
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

#include "sha512.hpp"

namespace xf {
namespace security {

/**
 * @brief Edwards-curve Digital Signature Algorithm on curve ed25519.
 * It take RFC 8032 "Edwards-Curve Digital Signature Algorithm (EdDSA)" as reference.
 * This class provide signing and verifying functions.
 */
class eddsaEd25519 {
   public:
    /// ed25519 related curve parameters.
    const int b = 256;
    const int c = 3;
    const int n = 254;
    const int a = -1;
    const ap_uint<256> Bx = ap_uint<256>("0x216936D3CD6E53FEC0A4E231FDD6DC5C692CC7609525A7B2C9562D608F25D51A");
    const ap_uint<256> By = ap_uint<256>("0x6666666666666666666666666666666666666666666666666666666666666658");
    const ap_uint<256> L = ap_uint<256>("0x1000000000000000000000000000000014DEF9DEA2F79CD65812631A5CF5D3ED");
    const ap_uint<256> p = ap_uint<256>("0x7FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFED");
    const ap_uint<256> d = ap_uint<256>("0x52036CEE2B6FFE738CC740797779E89800700A4D4141D8AB75EB4DCA135978A3");
    const ap_uint<256> p_5_d8 =
        ap_uint<256>("0x0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFD"); // (p - 5)/8
    const ap_uint<256> sqrt_n1 =
        ap_uint<256>("0x2B8324804FC1DF0B2B4D00993DFBD7A72F431806AD2FE478C4EE1B274A0EA0B0"); // sqrt(-1)
    const ap_uint<256> rMod = ap_uint<256>("0x05A4");                                       // 2 ^ 512 mod p
    ap_uint<64> head[8];

    eddsaEd25519() {
#pragma HLS inline
    }

    /**
     * @brief Compress a point (x, y) on curve to its compressed form
     *
     * @param x X coordinate of point.
     * @param y Y coordinate of point.
     * @param res compressed point representation.
     */
    void compress(ap_uint<256> x, ap_uint<256> y, ap_uint<256>& res) {
        ap_uint<256> tmp = y;
        tmp[255] = x[0];
        res = tmp;
    }

    /**
     * @brief Calculate square root of u/v.
     *
     * @param u Input u of u/v to calculate square root.
     * @param v Input u of u/v to calculate square root.
     * @param sqrt_a Square root of u/v.
     */
    bool modularSqrt(ap_uint<256> u, ap_uint<256> v, ap_uint<256>& sqrt_a) {
        ap_uint<256> uv = xf::security::internal::productMod(u, v, p);
        ap_uint<256> v2 = xf::security::internal::productMod(v, v, p);
        ap_uint<256> v4 = xf::security::internal::productMod(v2, v2, p);
        ap_uint<256> uv3 = xf::security::internal::productMod(uv, v2, p);
        ap_uint<256> uv7 = xf::security::internal::productMod(uv3, v4, p);
        ap_uint<256> tmp = xf::security::internal::modularExp<256, 256>(uv7, p_5_d8, p, rMod);
        tmp = xf::security::internal::productMod(uv3, tmp, p);
        ap_uint<256> tmp_2 = xf::security::internal::productMod(tmp, tmp, p);
        tmp_2 = xf::security::internal::productMod(tmp_2, v, p);
        if (tmp_2 == u) {
            sqrt_a = tmp;
            return true;
        } else if (xf::security::internal::addMod<256>(tmp_2, u, p) == 0) {
            sqrt_a = xf::security::internal::productMod(tmp, sqrt_n1, p);
            return true;
        } else {
            return false;
        }
    }

    /**
     * @brief Decompress a point (Px, Py) from its compressed representation.
     *
     * @param P compressed point representation.
     * @param Px X coordinate of the point.
     * @param Py Y coordinate of the point.
     */
    bool decompress(ap_uint<256> P, ap_uint<256>& Px, ap_uint<256>& Py) {
        Py = P.range(254, 0);
        ap_uint<256> y2 = xf::security::internal::productMod(Py, Py, p);
        ap_uint<256> u = xf::security::internal::subMod<256>(y2, 1, p);
        ap_uint<256> v = xf::security::internal::productMod(y2, d, p);
        v = xf::security::internal::addMod<256>(v, 1, p);
        ap_uint<256> sqrt_x;
        bool valid = modularSqrt(u, v, sqrt_x);

        if (P[255] == sqrt_x[0]) {
            Px = sqrt_x;
        } else {
            Px = p - sqrt_x;
        }
        return valid;
    }

   private:
    void writeWholeMsg(ap_uint<8> headLength,
                       ap_uint<128> msgLength,
#ifdef OG
                       hls::stream<ap_uint<64> >& msgStrm,
#else
                       const ap_uint<512>& msg,
#endif
#ifdef OG
                       hls::stream<ap_uint<64> >& wholeMsgStrm,
#else
                       hls::stream<ap_uint<512> >& wholeMsgStrm,
#endif
                       hls::stream<ap_uint<128> >& wholeLenStrm
#ifdef OG
					   ,
                       hls::stream<bool>& endWholeLenStrm
#endif
    ) {
        ap_uint<128> wholeLen = msgLength + headLength * 8;
#if !defined(__SYNTHESIS__)
        std::cout<<"X head length "<<std::dec<<headLength<<" \n";
        std::cout<<"WHOLE LEN "<<std::dec<< wholeLen<<" \n";
        std::cout<<"msgLength LEN "<<std::dec<< msgLength<<" \n";
#endif
#ifdef OG
        wholeLenStrm.write(wholeLen);
        endWholeLenStrm.write(false);
        endWholeLenStrm.write(true);
        for (int i = 0; i < headLength; i++) {
#pragma HLS pipeline II = 1
#if !defined(__SYNTHESIS__)
        	std::cout<<"head[i] "<<std::dec<<i<<" "<<std::hex<<head[i]<<std::endl;
            wholeMsgStrm.write(head[i]);
#endif
        }
        for (ap_uint<128> i = 0; i < msgLength; i += 8) {
#pragma HLS pipeline II = 1
        	ap_uint<64> q;
        	q= msgStrm.read();
#if !defined(__SYNTHESIS__)
        	std::cout<<"words "<<std::dec<<i<<" "<<std::hex<<q<<" ";
#endif
            wholeMsgStrm.write(q);
        }
#if !defined(__SYNTHESIS__)
        std::cout<<std::endl;
#endif
#else

        wholeLenStrm.write(wholeLen);

        if (headLength == 4) {
        	ap_uint<512> zzz = 0;
            for (int i = 0; i < headLength; i++) {
    #pragma HLS pipeline II = 1

            	ap_uint<64> z;
                zzz(63+64*(i+4),64*(i+4)) = head[3-i];
            }
            zzz(255,0) = msg(511,256);
            wholeMsgStrm.write(zzz);
#if !defined(__SYNTHESIS__)
            std::cout<<"zzz "<<std::hex<<zzz<<std::endl;
#endif
            zzz(511,256) = msg(255,0);
            zzz(255,0) = 0;
            wholeMsgStrm.write(zzz);
#if !defined(__SYNTHESIS__)
            std::cout<<"zzz "<<std::hex<<zzz<<std::endl;
#endif
        } else if (headLength == 8) {
        	ap_uint<512> zzz = 0;
            for (int i = 0; i < headLength; i++) {
    #pragma HLS pipeline II = 1
                zzz(63+64*i,64*i) = head[7-i];
            }
            wholeMsgStrm.write(zzz);
#if !defined(__SYNTHESIS__)
        	std::cout<<"zzz "<<std::hex<<zzz<<std::endl;
#endif
            zzz = msg;
#if !defined(__SYNTHESIS__)
            std::cout<<"zzz "<<std::hex<<zzz<<std::endl;
#endif
            wholeMsgStrm.write(zzz);
        }
#endif
    }

    void writeWholeMsg(ap_uint<8> headLength,
#ifdef OG
                       hls::stream<ap_uint<64> >& wholeMsgStrm,
#else
                       hls::stream<ap_uint<512> >& wholeMsgStrm,
#endif
                       hls::stream<ap_uint<128> >& wholeLenStrm
#ifdef OG
					   	   ,
                       hls::stream<bool>& endWholeLenStrm
#endif
					   ) {
        ap_uint<128> wholeLen = headLength * 8;
#if !defined(__SYNTHESIS__)
        std::cout<<"WHOLE LEN "<<std::dec<< wholeLen<<" \n";
#endif
        wholeLenStrm.write(wholeLen);
#ifdef OG
        endWholeLenStrm.write(false);
        endWholeLenStrm.write(true);
        for (int i = 0; i < headLength; i++) {
#pragma HLS pipeline II = 1
            wholeMsgStrm.write(head[i]);
        }
#else
#if !defined(__SYNTHESIS__)
        std::cout<<"WHOLE LEN"<<wholeLen<<" \n\n";
#endif
        if (headLength == 4) {
        	ap_uint<512> zzz = 0;
            for (int i = 0; i < headLength; i++) {
    #pragma HLS pipeline II = 1
            	ap_uint<64> z;
                zzz(63+64*(i+4),64*(i+4)) = head[3-i];
            }
            wholeMsgStrm.write(zzz);
#if !defined(__SYNTHESIS__)
        	std::cout<<"zzz "<<std::hex<<zzz<<std::endl;
#endif
        } else if (headLength == 8) {
        	ap_uint<512> zzz = 0;
            for (int i = 0; i < headLength; i++) {
    #pragma HLS pipeline II = 1
                zzz(63+64*i,64*i) = head[7-i];
            }
            wholeMsgStrm.write(zzz);
#if !defined(__SYNTHESIS__)
        	std::cout<<"zzz "<<std::hex<<zzz<<std::endl;
#endif
        }
#endif
    }

    void wrapperSha512(ap_uint<8> headLength,
                       ap_uint<128> msgLength,
#ifdef OG
                       hls::stream<ap_uint<64> >& msgStrm,
#else
                       const ap_uint<512>& msg,
#endif
                       hls::stream<ap_uint<512> >& digestStrm
#ifdef OG
					   ,
                       hls::stream<bool>& endDigestStrm
#endif
    ) { //
#pragma HLS dataflow

///// TODO !!!!
#ifdef OG
        hls::stream<ap_uint<64> > wholeMsgStrm("wrapper64Strm");
#else
        hls::stream<ap_uint<512> > wholeMsgStrm("wrapper64Strm");
#endif
#pragma HLS stream variable = wholeMsgStrm depth = 16

        hls::stream<ap_uint<128> > wholeLenStrm;
#pragma HLS stream variable = wholeLenStrm depth = 1

#ifdef OG
        hls::stream<bool> endWholeLenStrm;
#pragma HLS stream variable = endWholeLenStrm depth = 2
#endif

        /// q::
#ifdef OG
        writeWholeMsg(headLength, msgLength, msgStrm, wholeMsgStrm, wholeLenStrm, endWholeLenStrm);
#else
        writeWholeMsg(headLength, msgLength, msg, wholeMsgStrm, wholeLenStrm);
#endif
#ifdef OG
        xf::security::sha512<64>(wholeMsgStrm, wholeLenStrm, endWholeLenStrm, digestStrm, endDigestStrm);
#else
        ap_uint<128> len = wholeLenStrm.read();
        q::xf::security::sha512<512>(wholeMsgStrm, len, digestStrm);
#endif
    }

    void wrapperSha512(ap_uint<8> headLength,
                       hls::stream<ap_uint<512> >& digestStrm
#ifdef OG
					   ,
                       hls::stream<bool>& endDigestStrm
#endif
    ) { //
#pragma HLS dataflow
#ifdef OG
        hls::stream<ap_uint<64> > wholeMsgStrm("wrapper64Strm");
#else
        hls::stream<ap_uint<512> > wholeMsgStrm("wrapper64Strm");
#endif
#pragma HLS stream variable = wholeMsgStrm depth = 16
        hls::stream<ap_uint<128> > wholeLenStrm;
#pragma HLS stream variable = wholeLenStrm depth = 1

#ifdef OG
        hls::stream<bool> endWholeLenStrm;
#pragma HLS stream variable = endWholeLenStrm depth = 2

        writeWholeMsg(headLength, wholeMsgStrm, wholeLenStrm, endWholeLenStrm);
        xf::security::sha512<64>(wholeMsgStrm, wholeLenStrm, endWholeLenStrm, digestStrm, endDigestStrm);
#else
        writeWholeMsg(headLength, wholeMsgStrm, wholeLenStrm);
        ap_uint<128> len = wholeLenStrm.read();
        q::xf::security::sha512<512>(wholeMsgStrm, len, digestStrm);
#endif
    }

    ap_uint<512> hashWithHead(ap_uint<8> headLength, ap_uint<128> msgLength,
#ifdef OG
    		hls::stream<ap_uint<64> >& msgStrm
#else
    		const ap_uint<512>& msg
#endif
    ) {
            hls::stream<ap_uint<512> > digestStrm;
    #pragma HLS stream variable = digestStrm depth = 1


#ifdef OG
            hls::stream<bool> endDigestStrm;
#pragma HLS stream variable = endDigestStrm depth = 2
            wrapperSha512(headLength, msgLength, msgStrm, digestStrm, endDigestStrm);
            endDigestStrm.read();
            endDigestStrm.read();
#else
            wrapperSha512(headLength, msgLength, msg, digestStrm);
#endif
            ap_uint<512> r= digestStrm.read();
#if !defined(__SYNTHESIS__)
            std::cout<<"RRR "<<std::hex<<r<<std::endl;
#endif
            return r;
    }

    ap_uint<512> hashWithHead(ap_uint<8> headLength) {
        hls::stream<ap_uint<512> > digestStrm;
#pragma HLS stream variable = digestStrm depth = 1
#ifdef OG
        hls::stream<bool> endDigestStrm;
#pragma HLS stream variable = endDigestStrm depth = 2
        wrapperSha512(headLength, digestStrm, endDigestStrm);
        endDigestStrm.read();
        endDigestStrm.read();
#else
        wrapperSha512(headLength, digestStrm);
#endif
        ap_uint<512> r= digestStrm.read();
#if !defined(__SYNTHESIS__)
        std::cout<<"RRR "<<std::hex<<r<<std::endl;
#endif
        return r;
    }

   public:
    /**
     * @brief perform point addition in ed25519, (x3, y3) = (x1, y1) + (x2, y2)
     *
     * @param x1 X coordinate of point 1.
     * @param y1 Y coordinate of point 1.
     * @param x2 X coordinate of point 2.
     * @param y2 Y coordinate of point 2.
     * @param x3 X coordinate of point 3.
     * @param y3 Y coordinate of point 3.
     */
    void pointAdd(
        ap_uint<256> x1, ap_uint<256> y1, ap_uint<256> x2, ap_uint<256> y2, ap_uint<256>& x3, ap_uint<256>& y3) {
        //
        ap_uint<256> x1y2 = xf::security::internal::productMod(x1, y2, p);
        ap_uint<256> x2y1 = xf::security::internal::productMod(x2, y1, p);
        ap_uint<256> x1x2 = xf::security::internal::productMod(x1, x2, p);
        ap_uint<256> y1y2 = xf::security::internal::productMod(y1, y2, p);
        ap_uint<256> xu = xf::security::internal::addMod<256>(x1y2, x2y1, p);
        ap_uint<256> yu = xf::security::internal::addMod<256>(y1y2, x1x2, p);
        ap_uint<256> dx1x2y1y2 = xf::security::internal::productMod(x1x2, y1y2, p);
        dx1x2y1y2 = xf::security::internal::productMod(dx1x2y1y2, d, p);
        ap_uint<256> xv = xf::security::internal::addMod<256>(1, dx1x2y1y2, p);
        ap_uint<256> yv = xf::security::internal::subMod<256>(1, dx1x2y1y2, p);
        ap_uint<256> xvInv = xf::security::internal::modularInv<255>(xv, p);
        ap_uint<256> yvInv = xf::security::internal::modularInv<255>(yv, p);

        x3 = xf::security::internal::productMod(xu, xvInv, p);
        y3 = xf::security::internal::productMod(yu, yvInv, p);
    }

    /**
     * @brief perform point multiply scalar in ed25519, (resX, resY) = (x, y) * mag
     *
     * @param x X coordinate of point to be multiplied.
     * @param y Y coordinate of point to be multiplied.
     * @param mag scalar operand of this multiplication.
     * @param resX X coordinate of result.
     * @param resY Y coordinate of result.
     */
    void pointMul(ap_uint<256> x, ap_uint<256> y, ap_uint<256> mag, ap_uint<256>& resX, ap_uint<256>& resY) {
        //
        ap_uint<256> tmpX = x;
        ap_uint<256> tmpY = y;
        ap_uint<256> tmpResX = 0;
        ap_uint<256> tmpResY = 1;
        for (int i = 0; i < 256; i++) {
//#pragma HLS PERFORMANCE target_ti=10 KAMIL
            if (mag[i] == 1) {
                pointAdd(tmpResX, tmpResY, tmpX, tmpY, tmpResX, tmpResY);
                pointAdd(tmpX, tmpY, tmpX, tmpY, tmpX, tmpY);
            } else {
            pointAdd(tmpX, tmpY, tmpX, tmpY, tmpX, tmpY);
            }
        }
        resX = tmpResX;
        resY = tmpResY;
    }

    /**
     * @brief Generate public key and digest value of privateKey hash value from privateKey.
     *
     * @param privateKey Private Key.
     * @param publicKey Public Key.
     * @param privateKeyHash Digest value of private key.
     */
    void generatePublicKey(ap_uint<256> privateKey, ap_uint<256>& publicKey, ap_uint<512>& privateKeyHash) {
        for (int i = 0; i < 4; i++) {
#pragma HLS pipeline II = 1
            head[i] = privateKey.range(i * 64 + 63, i * 64);
        }

        ap_uint<512> hash_private = hashWithHead(4);
        privateKeyHash = hash_private;
        ap_uint<256> secret_scalar = hash_private.range(255, 0);
        secret_scalar.range(2, 0) = 0;
        secret_scalar[255] = 0;
        secret_scalar[254] = 1;
        ap_uint<256> x, y;
        pointMul(Bx, By, secret_scalar, x, y);

        ap_uint<256> tmpRes = y;
        tmpRes[255] = x[0];

        publicKey = tmpRes;
    }

    /**
     * @brief signing function
     *
     * @param msgStrm Stream to input messages to be signed, each message should be input throught this stream twice.
     * @param lenStrm Stream to input length of input messages.
     * @param endLenStrm Stream of end flag of lenStrm.
     * @param publicKey Public Key.
     * @param privateKeyHash Digest value of private key.
     * @param signatureStrm Stream to output signature.
     * @param endSignatureStrm Stream of end flag of signatureStrm.
     */
    void sign(
#ifdef OG
    		  hls::stream<ap_uint<64> >& msgStrm,
#else
    		  ap_uint<512> msg,
#endif

#ifdef OG
              hls::stream<ap_uint<128> >& lenStrm,
              hls::stream<bool>& endLenStrm,
#endif
              ap_uint<256> publicKey,
              ap_uint<512> privateKeyHash,
#ifdef OG
              hls::stream<ap_uint<512> >& signatureStrm,
              hls::stream<bool>& endSignatureStrm
#else
              hls::stream<ap_uint<512> >& signatureStrm
#endif
) {

#ifdef OG
        while (!endLenStrm.read()) {
            ap_uint<128> msgLen = lenStrm.read();
#if !defined(__SYNTHESIS__)
            std::cout<<"msgLen "<<msgLen<<std::endl;
#endif
#endif
            // step 2
            for (int i = 0; i < 4; i++) {
#pragma HLS pipeline
                head[i] = privateKeyHash.range((i + 4) * 64 + 63, (i + 4) * 64);
            }
#ifdef OG
            ap_uint<512> r = hashWithHead(4, msgLen, msgStrm);
#else
            ap_uint<512> r = hashWithHead(4, 64, msg);
#endif
            ap_uint<256> r_mod_L = xf::security::internal::simpleMod<512, 256>(r, L);

            // step 3
            ap_uint<256> Rx, Ry;
            pointMul(Bx, By, r_mod_L, Rx, Ry);
            ap_uint<256> R_compress;
            compress(Rx, Ry, R_compress);

            // step 4
            for (int i = 0; i < 4; i++) {
#pragma HLS pipeline II = 1
                head[i] = R_compress.range(i * 64 + 63, i * 64);
            }
            for (int i = 0; i < 4; i++) {
#pragma HLS pipeline II = 1
                head[i + 4] = publicKey.range(i * 64 + 63, i * 64);
            }
#ifdef OG
            ap_uint<512> kHash = hashWithHead(8, msgLen, msgStrm);
#else

            ap_uint<512> kHash = hashWithHead(8, 64, msg);
#endif
            ap_uint<256> kHash_mod_L = xf::security::internal::simpleMod<512, 256>(kHash, L);

            ap_uint<256> secret_scalar = privateKeyHash.range(255, 0);
            secret_scalar.range(2, 0) = 0;
            secret_scalar[255] = 0;
            secret_scalar[254] = 1;

            ap_uint<256> s_mod_L = xf::security::internal::simpleMod<256, 256>(secret_scalar, L);

            ap_uint<256> signature_S = xf::security::internal::productMod(s_mod_L, kHash_mod_L, L);
            signature_S = xf::security::internal::addMod(signature_S, r_mod_L, L);

            ap_uint<512> signature;
            signature.range(255, 0) = R_compress;
            signature.range(511, 256) = signature_S;
            signatureStrm.write(signature);
#ifdef OG
            endSignatureStrm.write(false);
        }
        endSignatureStrm.write(true);
#endif
    }

    /**
     * @brief verifying function
     *
     * @param msgStrm Stream to input messages to be signed.
     * @param lenStrm Stream to input length of input messages.
     * @param signatureStrm Stream to input signatures.
     * @param endSignatureStrm Stream of end flag of signatures.
     * @param publicKeyStrm Stream to input public key.
     * @param ifValidStrm Stream to output if message signature is valid.
     * @param endIfValidStrm Stream of end flag of ifValidStrm.
     */

    void verify(
#ifdef OG
    		  hls::stream<ap_uint<64> >& msgStrm,
#else
    		    const ap_uint<512>& msg,
#endif
#ifdef OG
                hls::stream<ap_uint<128> >& lenStrm,
                hls::stream<ap_uint<512> >& signatureStrm,
                hls::stream<bool>& endSignatureStrm,
                hls::stream<ap_uint<256> >& publicKeyStrm,
                hls::stream<bool>& ifValidStrm,
                hls::stream<bool>& endIfValidStrm
#else
                const ap_uint<128>& msgLength,
                const ap_uint<512>& signature,
                const ap_uint<256>& publicKey,
                hls::stream<bool>& ifValidStrm
#endif
                ) {
#ifdef OG
        while (!endSignatureStrm.read()) {
            bool valid = true;

            ap_uint<128> msgLength = lenStrm.read();
            ap_uint<512> signature = signatureStrm.read();
#else
            bool valid = true;
#endif
            ap_uint<256> sig_S = signature.range(511, 256);
            ap_uint<256> sig_R = signature.range(255, 0);
            // check sig_S is valid
            if (sig_S >= L) {
                valid = false;
            }
            // check sig_R is valid
            ap_uint<256> Rx, Ry;
            if (!decompress(sig_R, Rx, Ry)) {
                valid = false;
            }
            // check if publicKey is valid
            ap_uint<256> Ax, Ay;
#ifdef OG
            ap_uint<256> publicKey = publicKeyStrm.read();
#endif
            if (!decompress(publicKey, Ax, Ay)) {
                valid = false;
            }

            // compute kHash
            for (int i = 0; i < 4; i++) {
#pragma HLS pipeline II = 1
                head[i] = sig_R.range(i * 64 + 63, i * 64);
            }
            for (int i = 0; i < 4; i++) {
#pragma HLS pipeline II = 1
                head[i + 4] = publicKey.range(i * 64 + 63, i * 64);
            }
#ifdef OG
            ap_uint<512> kHash = hashWithHead(8, msgLength, msgStrm);
#else
            ap_uint<512> kHash = hashWithHead(8, msgLength, msg);
#endif
            ap_uint<256> kHash_mod_L = xf::security::internal::simpleMod<512, 256>(kHash, L);

            //
            ap_uint<256> s_mod_L = xf::security::internal::simpleMod<256, 256>(sig_S, L);
            ap_uint<256> leftX, leftY, rightX, rightY;

            // left of equation.
            pointMul(Bx, By, s_mod_L, leftX, leftY);
            pointMul(leftX, leftY, 8, leftX, leftY);
            // right of equation.
            ap_uint<256> tmpX, tmpY;
            pointMul(Rx, Ry, 8, tmpX, tmpY);
            pointMul(Ax, Ay, kHash_mod_L, rightX, rightY);
            pointMul(rightX, rightY, 8, rightX, rightY);
            pointAdd(rightX, rightY, tmpX, tmpY, rightX, rightY);
            if (leftX != rightX || leftY != rightY) {
                valid = false;
            }
            ifValidStrm.write(valid);
#ifdef OG
            endIfValidStrm.write(false);
        }
        endIfValidStrm.write(true);
#endif
    }

};
} // namespace security
} // namespace xf

#endif

void eddsa(hls::stream<AXIS_DATA>& input,
		hls::stream<AXIS_DATA >& output) {
#pragma HLS INTERFACE ap_ctrl_none port=return // removes handshake
#pragma HLS interface axis register port = input
#pragma HLS interface axis register port = output
#pragma HLS AGGREGATE variable=input bit
#pragma HLS AGGREGATE variable=output bit

	AXIS_DATA outWord;
	input.read(outWord);
	ap_uint<512> message = outWord.tdata;
	ap_uint<512> result2;



	xf::security::eddsaEd25519 processor;
	ap_uint<256> publicKey;
	ap_uint<512> privateKeyHash;
	ap_uint<256> tmp = ap_uint<256>("0x4ccd089b28ff96da9db6c346ec114e0f5b8a319f35aba624da8cf6ed4fb8a6fb");
	ap_uint<256> privateKey;
	for (int i = 0; i < 32; i++) {
	    int j = 31 - i;
	    privateKey.range(i * 8 + 7, i * 8) = tmp.range(j * 8 + 7, j * 8);
	}

	hls::stream<ap_uint<512> > signatureStrm;
	#pragma HLS stream variable = signatureStrm depth = 4

//	processor.generatePublicKey(privateKey, publicKey, privateKeyHash);
//	std::cout<<"privk "<<std::hex<<privateKey<<std::endl;
//	std::cout<<"pubk "<<std::hex<<publicKey<<std::endl;
//	std::cout<<"pubkh "<<std::hex<<privateKeyHash<<std::endl;
	privateKey="0xFBA6B84FEDF68CDA24A6AB359F318A5B0F4E11EC46C3B69DDA96FF289B08CD4C";
	publicKey="0x0C66F42AF155CDC08C96C42ECF2C989CBC7E1B4DA70AB7925A8943E8C317403D";
	privateKeyHash="0x1D917204FA6BCEF960810BB0172E2C8E31DA48B3DE63CC25F2CADA9182846645112E502EB0249A255E1C827F3B6B6C7F0A79F4CA8575A91528D58258D79EBD6E";

#ifdef OG

    hls::stream<ap_uint<64> > msgStrm;
    hls::stream<ap_uint<128> > lenStrm;
    hls::stream<bool> endLenStrm;
    hls::stream<bool> endSignatureStrm;
    endLenStrm.write(false);
    endLenStrm.write(true);
	msgStrm.write(message(511,448));
	msgStrm.write(message(447,384));
	msgStrm.write(message(383,320));
	msgStrm.write(message(319,256));
	msgStrm.write(message(255,192));
	msgStrm.write(message(191,128));
	msgStrm.write(message(127,64));
	msgStrm.write(message(63,0));
	msgStrm.write(message(511,448));
	msgStrm.write(message(447,384));
	msgStrm.write(message(383,320));
	msgStrm.write(message(319,256));
	msgStrm.write(message(255,192));
	msgStrm.write(message(191,128));
	msgStrm.write(message(127,64));
	msgStrm.write(message(63,0));
	lenStrm.write(64);
    processor.sign(msgStrm, lenStrm, endLenStrm, publicKey, privateKeyHash, signatureStrm, endSignatureStrm);
    endSignatureStrm.read();
    ap_uint<512> sig = signatureStrm.read();
    endSignatureStrm.read();
#else

    processor.sign(message, publicKey, privateKeyHash, signatureStrm);
    ap_uint<512> sig = signatureStrm.read();
#endif
	outWord.tdata = sig;
	output.write(outWord);
}


void sha512_eddsa_verify(hls::stream<AXIS_DATA>& input,
		hls::stream<ap_uint<1> >& output) {
	   // output: hash digest of input data

// Interface definitions
#pragma HLS INTERFACE ap_ctrl_none port=return // removes handshake
#pragma HLS interface axis register port = input
#pragma HLS interface axis register port = output
#pragma HLS AGGREGATE variable=input bit
#pragma HLS AGGREGATE variable=output bit


	hls::stream<AXIS_DATA> output2;
	#pragma HLS stream variable = output2 depth = 32
	#pragma HLS resource variable = output2 core = FIFO_LUTRAM
	AXIS_DATA signature;

	AXI::xf::security::sha512x<512>(input, output2, signature);

	ap_uint<1> result = 0;

	AXIS_DATA hash;
	output2.read(hash);


	hls::stream<bool> ifValidStrm1("ifValidStrm");
#pragma HLS stream variable = ifValidStrm1 depth = 4
#pragma HLS resource variable = ifValidStrm1 core = FIFO_LUTRAM

	ap_uint<256> publicKey = ap_uint<256>("0x0C66F42AF155CDC08C96C42ECF2C989CBC7E1B4DA70AB7925A8943E8C317403D");
	ap_uint<512> result2;
	ap_uint<512> message = hash.tdata;
//	std::cout<<"MESSAGE "<<std::hex<<message<<std::endl;
	ap_uint<512> sig = signature.tdata;
//	std::cout<<"SIG "<<std::hex<<sig<<std::endl;

	ap_uint<128> l = 64;

	xf::security::eddsaEd25519 processor;
	processor.verify(message,
					 l, sig, publicKey, ifValidStrm1);
	bool valid = ifValidStrm1.read();
	if (!valid) {
		result = 1;
	}
	output.write(result);

}
void sha512(hls::stream<AXIS_DATA>& input,
			hls::stream<AXIS_DATA >& output) {
	   // output: hash digest of input data

// Interface definitions
#pragma HLS INTERFACE ap_ctrl_none port=return // removes handshake
#pragma HLS interface axis register port = input
#pragma HLS interface axis register port = output
#pragma HLS AGGREGATE variable=input bit
#pragma HLS AGGREGATE variable=output bit
	AXIS_DATA last;
    AXI::xf::security::sha512x<512>(input, output, last);
}
