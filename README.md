# Ensuring there are no collisions among SHA256 hashes of simple numeric strings
This code was made to ensure there are no collisions in the SHA256 hashes gotten from numerical strings with leading zeros in the range of 0000 000 000 to 9999 999 999.

# Requirements
To execute, this code will need a POSIX-compliant environment (Linux, macOS, perhaps Cygwin) and approximately up to 524 GB of RAM.

# Dependencies
OpenSSL

# Time to compute
On a memory-optimized current-generation AWS instance on the 64-bit x86 architecture with enough RAM and 64 virtual cores, it takes 200 minutes to complete.

# Outcome
No collisions have been identified.
