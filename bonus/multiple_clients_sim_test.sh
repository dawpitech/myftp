#!/usr/bin/env bash

# Function to perform FTP operations
ftp_operations() {
  ftp -inv 127.0.0.1 1234 <<EOF
bye
EOF
}

# Run the ftp_operations function 100 times in parallel
for i in {1..100}; do
  ftp_operations &
done

# Wait for all background jobs to complete
wait

echo "All FTP operations completed."
