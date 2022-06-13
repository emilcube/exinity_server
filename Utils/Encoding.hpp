#pragma once
#include <openssl/evp.h>
#include <openssl/hmac.h>

//require openssl v1.1
//doesn't work in others version

#include <string>
#include <memory>
#include <vector>

namespace encoding {

    namespace {
        struct HmacCtx
        {
            HMAC_CTX* ctx;
            HmacCtx() {
                this->ctx = HMAC_CTX_new();
            }
            ~HmacCtx() { HMAC_CTX_free(ctx); }
        };
    }


    inline std::string hmac(const std::string& secret,
        std::string msg,
        std::size_t signed_len)
    {
        static HmacCtx hmac;
        char signed_msg[64];

        HMAC_Init_ex(
            &*hmac.ctx, secret.data(), (int)secret.size(), EVP_sha256(), nullptr);
        HMAC_Update(&*hmac.ctx, (unsigned char*)msg.data(), msg.size());
        HMAC_Final(&*hmac.ctx, (unsigned char*)signed_msg, nullptr);

        return { signed_msg, signed_len };
    }

    namespace {
        constexpr char hexmap[] = { '0',
                                   '1',
                                   '2',
                                   '3',
                                   '4',
                                   '5',
                                   '6',
                                   '7',
                                   '8',
                                   '9',
                                   'a',
                                   'b',
                                   'c',
                                   'd',
                                   'e',
                                   'f' };
    }

    inline std::string string_to_hex(unsigned char* data, std::size_t len)
    {
        std::string s(len * 2, ' ');
        for (std::size_t i = 0; i < len; ++i) {
            s[2 * i] = hexmap[(data[i] & 0xF0) >> 4];
            s[2 * i + 1] = hexmap[data[i] & 0x0F];
        }
        return s;
    }

    inline std::vector<unsigned char> sha256(const std::string str)
    {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, str.c_str(), str.size());
        SHA256_Final(hash, &sha256);
        std::vector<unsigned char> ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        {
            ss.push_back((int)hash[i]);
        }
        return ss;
    }

}