#pragma once

#include "EasyCL/EasyCL.h"

#include "llvm/Support/Casting.h" // for llvm rtti

#include <string>

namespace cocl {
    // These Arg classes store kernel parameter values, which we can use
    // at kernel creation time, and then pass into the kernel at that point
    // we dont create the kernel until the actual launch command (which is after
    // the kernelSetArg commands), so we have all the information available at that
    // time about what kernel arguments we have
    // concretely, it means we can dedupe the underlying cl_mem buffers, for example
    class Arg {
    public:
        enum ArgKind {
            AK_Base,
            AK_Int8Arg,
            AK_Int32Arg,
            AK_UInt32Arg,
            AK_Int64Arg,
            AK_FloatArg,
            AK_NullPtrArg,
            AK_ClmemArg,
            AK_StructArg
        };
        Arg(ArgKind kind=AK_Base) : Kind(kind) {}
        virtual ~Arg() {}
        virtual void inject(easycl::CLKernel *kernel) = 0;
        virtual std::string str() = 0;

    private:
        const ArgKind Kind;

    public:
        ArgKind getKind() const { return Kind; }

        static bool classof(const Arg *arg) {
            return arg->getKind() == AK_Base;
        }
    };
    class Int8Arg : public Arg {
    public:
        Int8Arg(char v) : Arg(AK_Int8Arg), v(v) {}
        void inject(easycl::CLKernel *kernel) {
            kernel->in_char(v);
        }
        virtual std::string str() { return "Int8Arg"; }
        char v;
        static bool classof(const Arg *arg) {
            return arg->getKind() == AK_Int8Arg;
        }
    };
    class Int32Arg : public Arg {
    public:
        Int32Arg(int v) : Arg(AK_Int32Arg), v(v) {}
        void inject(easycl::CLKernel *kernel) {
            kernel->in_int32(v);
        }
        virtual std::string str();
        int v;
        static bool classof(const Arg *arg) {
            return arg->getKind() == AK_Int32Arg;
        }
    };
    class UInt32Arg : public Arg {
    public:
        UInt32Arg(uint32_t v) : Arg(AK_UInt32Arg), v(v) {}
        void inject(easycl::CLKernel *kernel) {
            kernel->in_uint32(v);
        }
        virtual std::string str() { return "UInt32Arg"; }
        uint32_t v;
        static bool classof(const Arg *arg) {
            return arg->getKind() == AK_UInt32Arg;
        }
    };
    class Int64Arg : public Arg {
    public:
        Int64Arg(int64_t v) : Arg(AK_Int64Arg), v(v) {}
        void inject(easycl::CLKernel *kernel) {
            kernel->in_int64(v);
        }
        virtual std::string str();
        int64_t v;
        static bool classof(const Arg *arg) {
            return arg->getKind() == AK_Int64Arg;
        }
    };
    class FloatArg : public Arg {
    public:
        FloatArg(float v) : Arg(AK_FloatArg), v(v) {}
        void inject(easycl::CLKernel *kernel) {
            kernel->in_float(v);
        }
        virtual std::string str() { return "FloatArg"; }
        float v;
        static bool classof(const Arg *arg) {
            return arg->getKind() == AK_FloatArg;
        }
    };
    class NullPtrArg : public Arg {
    public:
        NullPtrArg() : Arg(AK_NullPtrArg) {}
        void inject(easycl::CLKernel *kernel) {
            kernel->in_nullptr();
        }
        virtual std::string str() { return "NullPtrArg"; }
        static bool classof(const Arg *arg) {
            return arg->getKind() == AK_NullPtrArg;
        }
    };
    class ClmemArg : public Arg {
    public:
        ClmemArg(cl_mem v) : Arg(AK_ClmemArg), v(v) {}
        void inject(easycl::CLKernel *kernel) {
            kernel->inout(&v);
        }
        virtual std::string str() { return "ClmemArg"; }
        cl_mem v;
        static bool classof(const Arg *arg) {
            return arg->getKind() == AK_ClmemArg;
        }
    };
    class StructArg : public Arg {
    public:
        StructArg(char *pCpuStruct, int structAllocateSize) :
            Arg(AK_StructArg),
            pCpuStruct(pCpuStruct), structAllocateSize(structAllocateSize)
        {}
        virtual std::string str() { return "StructArg"; }
        char *pCpuStruct;
        int structAllocateSize;
        static bool classof(const Arg *arg) {
            return arg->getKind() == AK_StructArg;
        }
    };
} // namespace cocl
