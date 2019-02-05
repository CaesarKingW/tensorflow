
# This file is part of the CLBlast project. The project is licensed under Apache Version 2.0. This file follows the
# PEP8 Python style guide and uses a max-width of 120 characters per line.
#
# Author(s):
#   Cedric Nugteren <www.cedricnugteren.nl>


# Short-hands for data-types
D_HALF = "half"
D_FLOAT = "float"
D_DOUBLE = "double"
D_FLOAT2 = "float2"
D_DOUBLE2 = "double2"
D_HALF_OPENCL = "cl_half"
D_FLOAT2_OPENCL = "cl_float2"
D_DOUBLE2_OPENCL = "cl_double2"


class DataType:
    """Class holding data-type and precision information"""

    def __init__(self, precision_name, name, template, scalars, buffer_type):
        self.precision_name = precision_name
        self.name = name
        self.template = template
        self.alpha_cpp = scalars[0]
        self.beta_cpp = scalars[1]
        self.alpha_cl = scalars[2]
        self.beta_cl = scalars[3]
        self.buffer_type = buffer_type

    def use_alpha(self):
        """Outputs the name of the data-type (alpha/beta), possibly transforming into the right type"""
        if self.alpha_cpp in [D_FLOAT2, D_DOUBLE2]:
            return self.alpha_cpp + "{alpha.s[0], alpha.s[1]}"
        return "alpha"

    def use_beta(self):
        """As above, but for beta instead of alpha"""
        if self.beta_cpp in [D_FLOAT2, D_DOUBLE2]:
            return self.beta_cpp + "{beta.s[0], beta.s[1]}"
        return "beta"

    def use_alpha_opencl(self):
        """As above, but the transformation is in the opposite direction"""
        if self.alpha_cpp in [D_FLOAT2, D_DOUBLE2]:
            return self.alpha_cl + "{{alpha.real(), alpha.imag()}}"
        return "alpha"

    def use_beta_opencl(self):
        """As above, but for beta instead of alpha"""
        if self.beta_cpp in [D_FLOAT2, D_DOUBLE2]:
            return self.beta_cl + "{{beta.real(), beta.imag()}}"
        return "beta"

    def test_template(self):
        """Returns the template as used in the correctness/performance tests"""
        if self.buffer_type != self.beta_cpp:
            return "<" + self.buffer_type + "," + self.beta_cpp + ">, " + self.buffer_type + ", " + self.beta_cpp
        return "<" + self.buffer_type + ">, " + self.buffer_type + ", " + self.beta_cpp

    def is_complex(self, scalar):
        """Current scalar is complex"""
        return ((scalar == "alpha" and self.alpha_cpp in [D_FLOAT2, D_DOUBLE2]) or
                (scalar == "beta" and self.beta_cpp in [D_FLOAT2, D_DOUBLE2]))


# Regular data-types
H = DataType("H", "H", D_HALF, [D_HALF] * 2 + [D_HALF_OPENCL] * 2, D_HALF)  # half (16)
S = DataType("S", "S", D_FLOAT, [D_FLOAT] * 4, D_FLOAT)  # single (32)
D = DataType("D", "D", D_DOUBLE, [D_DOUBLE] * 4, D_DOUBLE)  # double (64)
C = DataType("C", "C", D_FLOAT2, [D_FLOAT2] * 2 + [D_FLOAT2_OPENCL] * 2, D_FLOAT2)  # single-complex (3232)
Z = DataType("Z", "Z", D_DOUBLE2, [D_DOUBLE2] * 2 + [D_DOUBLE2_OPENCL] * 2, D_DOUBLE2)  # double-complex (6464)

# Special cases
Sc = DataType("C", "Sc", D_FLOAT2, [D_FLOAT2] * 4, D_FLOAT2)  # As C, but with real output
Dz = DataType("Z", "Dz", D_DOUBLE2, [D_DOUBLE2] * 4, D_DOUBLE2)  # As Z, but with real output
iH = DataType("H", "iH", D_HALF, [D_HALF] * 4, D_HALF)  # As H, but with integer output
iS = DataType("S", "iS", D_FLOAT, [D_FLOAT] * 4, D_FLOAT)  # As S, but with integer output
iD = DataType("D", "iD", D_DOUBLE, [D_DOUBLE] * 4, D_DOUBLE)  # As D, but with integer output
iC = DataType("C", "iC", D_FLOAT2, [D_FLOAT2] * 2 + [D_FLOAT2_OPENCL] * 2, D_FLOAT2)  # As C, but with integer output
iZ = DataType("Z", "iZ", D_DOUBLE2, [D_DOUBLE2] * 2 + [D_DOUBLE2_OPENCL] * 2, D_DOUBLE2)  # As Z, but with int output
Css = DataType("C", "C", D_FLOAT, [D_FLOAT, D_FLOAT, D_FLOAT, D_FLOAT], D_FLOAT2)  # As C, but with constants from S
Zdd = DataType("Z", "Z", D_DOUBLE, [D_DOUBLE] * 4, D_DOUBLE2)  # As Z, but with constants from D
Ccs = DataType("C", "C", D_FLOAT2 + "," + D_FLOAT, [D_FLOAT2, D_FLOAT, D_FLOAT2_OPENCL, D_FLOAT], D_FLOAT2)  # As C, but with one constant from S
Zzd = DataType("Z", "Z", D_DOUBLE2 + "," + D_DOUBLE, [D_DOUBLE2, D_DOUBLE, D_DOUBLE2_OPENCL, D_DOUBLE], D_DOUBLE2)  # As Z, but with one constant from D

# C++ template data-types
T = DataType("T", "typename T", "T", ["T", "T", "T", "T"], "T")  # regular routine
Tc = DataType("Tc", "typename T", "std::complex<T>,T", ["T", "T", "T", "T"], "std::complex<T>")  # for herk
TU = DataType("TU", "typename T, typename U", "T,U", ["T", "U", "T", "U"], "T")  # for her2k
