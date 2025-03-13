import gzip
import sys

def file_to_gzip_header(input_filename: str, output_header: str, array_name: str = "CompressedData"):
    # Read and gzip the input file
    with open(input_filename, 'rb') as f:
        compressed_data = gzip.compress(f.read())

    cpp_array = ', '.join(f'0x{byte:02X}' for byte in compressed_data)
    cpp_code = f"""
#pragma once

#include <array>
#include <cstddef>

constexpr std::array<unsigned char, {len(compressed_data)}> {array_name} = {{
    {cpp_array}
}};
"""

    # Write to output header file
    with open(output_header, 'w') as f:
        f.write(cpp_code)

    print(f"Generated C++ header file: {output_header}")
    print(f"Array size: {len(compressed_data)} bytes")

if __name__ == "__main__":
    if len(sys.argv) != 3:
        print(f"Usage: {sys.argv[0]} <input_file> <output_header>")
        sys.exit(1)

    print(f"Input file: {sys.argv[1]}")
    print(f"Output header: {sys.argv[2]}")

    file_to_gzip_header(sys.argv[1], sys.argv[2], "TAILWIND_GZ_DATA")

