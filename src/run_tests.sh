#!/bin/bash
function check() {
    if [[ $(echo "$1" | ./progetto_algoritmi $2 | md5sum | head -c 32) = $3 ]]
    then
        echo "pass: $2"
        return 0
    else
        echo "FAIL: $2"
        return 1
    fi
}

# Check the output against hashes of outputs that were verified by hand to be correct
check a input/analysis_edge_cases.v b3f6dcaad3e614ae7e18b99f57b4eca1
check s input/logic_properties.v b00719eaf7da5da8b39af5de253aa9f6
check s input/single_gates.v b6896f9decb5242680eea8aae71276ed
check a input/toposort.v 9e53656763b7576f82716eec337a1679
check s input/toposort.v 957aca9b836113040b5857388209aedd
