sha256_check: sha256_check.c
	clang -lcrypto -pthread -o sha256_check sha256_check.c
